#include <linux/mutex.h>
#include <drm/drm_drv.h>
#include <drm/drm_prime.h>
#include "../vc4/vc4_drv.h"
#include "v3d_vc4_bind.h"

#include "uapi/drm/vc4_drm.h"

struct mutex bind_lock;

void init_v3d_vc4_bind() {
  mutex_init(&bind_lock);
}

int import_bo_from_vc4(struct drm_device *dev, 
    struct drm_file *file_priv, size_t unaligned_size, u32 *vc4_handle, u32 *handle)
{
  struct drm_vc4_create_bo args = {0};
  int ret, prime_fd;

  if (!is_vc4_enable())
    return -ENODEV;

  mutex_lock(&bind_lock);
  args.size = unaligned_size;
  args.flags = V3D_BIND;
  ret = vc4_create_bo_ioctl(vc4_drm, &args, vc4_drm_file);
  if (ret)
    goto out;
  *vc4_handle = args.handle;
  ret = drm_gem_prime_handle_to_fd(vc4_drm, vc4_drm_file, args.handle,
     DRM_CLOEXEC | DRM_RDWR, &prime_fd);
  if (ret)
    ret = drm_gem_prime_handle_to_fd(vc4_drm, vc4_drm_file, args.handle,
         DRM_CLOEXEC, &prime_fd);
  if (ret)
    goto out;
  
  ret = drm_gem_prime_fd_to_handle(dev, file_priv, prime_fd, handle);
  if (ret)
    DRM_ERROR("Borrow bo from vc4 size:%u, vc4 handle:%u, fd:%d, v3d handle:%u\n", 
      unaligned_size, *vc4_handle, prime_fd, *handle);
out:
  mutex_unlock(&bind_lock);
  return ret; 
}

int free_bo_from_vc4(u32 *handle) {
  int ret;
  /* need releasing handle twice? */
  ret = drm_gem_handle_delete(vc4_drm_file, *handle);
  DRM_DEBUG("release borrowed vc4 handle:%d\n", *handle);
  if (ret)
    DRM_ERROR("Free handle:%u, from vc4, ret:%d\n", *handle, ret);
  *handle = 0;
  return ret;
}
