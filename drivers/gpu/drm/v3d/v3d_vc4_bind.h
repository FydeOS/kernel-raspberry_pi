// SPDX-License-Identifier: GPL-2.0+
/* Copyright (C) 2020 FydeOS team */
/* Author: Yang Tsao (yang@fydeos.io)*/

#ifndef V3D_VC$_BIND_H
#define V3D_VC$_BIND_H
#include <drm/drm_drv.h>
#include "../vc4/vc4_drv.h"
static inline bool is_vc4_enable(void) {
  return vc4_drm != NULL && vc4_drm_file != NULL; 
}

int import_bo_from_vc4(struct drm_device *dev, struct drm_file *file_priv, 
  size_t unaligned_size, u32 *vc4_handle, u32 *handle);

int free_bo_from_vc4(u32 *handle);
void init_v3d_vc4_bind(void);
#endif /* V3D_VC$_BIND_H */
