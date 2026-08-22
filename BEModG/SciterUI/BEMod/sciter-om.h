#pragma once

#ifndef __SCITER_OM_H__
#define __SCITER_OM_H__

#include "sciter-x-primitives.h"
#include "sciter-x-value.h"

struct som_passport_t;

typedef UINT64 som_atom_t;

typedef struct som_asset_t {
  struct som_asset_class_t* isa;
} som_asset_t;

typedef struct som_asset_class_t {
  long(*asset_add_ref)(som_asset_t* thing);
  long(*asset_release)(som_asset_t* thing);
  long(*asset_get_interface)(som_asset_t* thing, const char* name, void** out);
  struct som_passport_t* (*asset_get_passport)(som_asset_t* thing);
} som_asset_class_t;

inline struct som_asset_class_t* som_asset_get_class(const struct som_asset_t* pass)
{
#ifdef __cplusplus
  return pass ? pass->isa : nullptr;
#else
  return pass ? pass->isa : NULL;
#endif
}

som_atom_t SCAPI SciterAtomValue(const char* name);



#include "sciter-om-def.h"

#endif
