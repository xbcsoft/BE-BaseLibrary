#pragma once

#ifndef __SCITER_OM_DEF_H__
#define __SCITER_OM_DEF_H__

#include "sciter-om.h"
#include <stdint.h>


#ifdef SCITER_BUILD
  #define SOM_VALUE tool::value
#else
  #define SOM_VALUE SCITER_VALUE
#endif

UINT64 SCAPI SciterAtomValue(const char* name);
SBOOL  SCAPI SciterAtomNameCB(UINT64 atomv, LPCSTR_RECEIVER* rcv, LPVOID rcv_param);

typedef SBOOL(*som_prop_getter_t)(som_asset_t* thing, SOM_VALUE* p_value);
typedef SBOOL(*som_prop_setter_t)(som_asset_t* thing, SOM_VALUE* p_value);
typedef SBOOL(*som_item_getter_t)(som_asset_t* thing, const SOM_VALUE* p_key, SOM_VALUE* p_value);
typedef SBOOL(*som_item_setter_t)(som_asset_t* thing, const SOM_VALUE* p_key, const SOM_VALUE* p_value);
typedef SBOOL(*som_item_next_t)(som_asset_t* thing, SOM_VALUE* p_idx /*in/out*/, SOM_VALUE* p_value);
typedef SBOOL(*som_any_prop_getter_t)(som_asset_t* thing, UINT64 propSymbol, SOM_VALUE* p_value);
typedef SBOOL(*som_any_prop_setter_t)(som_asset_t* thing, UINT64 propSymbol, const SOM_VALUE* p_value);
typedef SBOOL(*som_method_t)(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result);
typedef void(*som_dispose_t)(som_asset_t* thing);
typedef SBOOL(*som_name_resolver_t)(som_asset_t* thing, som_atom_t propSymbol, UINT* pIndex, SBOOL *pIsMethod);

typedef enum {
  SOM_PROP_ACCSESSOR = 0,
  SOM_PROP_INT32 = 1,
  SOM_PROP_INT64 = 2,
  SOM_PROP_FLOAT = 3,
  SOM_PROP_STRING = 4,
} SOM_PROP_TYPE;

typedef struct som_property_def_t {
  intptr_t      type; // SOM_PROP_TYPE
  som_atom_t    name;
  union {
    struct {
      som_prop_getter_t getter;
      som_prop_setter_t setter;
    } accs;
	int32_t i32;
	int64_t i64;
    double  f64;
    const char* str;
  } u;
#ifdef __cplusplus
  explicit som_property_def_t(const char* n, som_prop_getter_t pg, som_prop_setter_t ps = nullptr) : name(SciterAtomValue(n)), 
    type(SOM_PROP_ACCSESSOR)
  {
    u.accs.getter = pg;
    u.accs.setter = ps;
  }
  explicit som_property_def_t(const char* n, int32_t c) : name(SciterAtomValue(n)),
    type(SOM_PROP_INT32)
  {
    u.i32 = c;
  }
  explicit som_property_def_t(const char* n, int64_t c) : name(SciterAtomValue(n)),
    type(SOM_PROP_INT64)
  {
    u.i64 = c;
  }
  explicit som_property_def_t(const char* n, double c) : name(SciterAtomValue(n)),
    type(SOM_PROP_FLOAT)
  {
    u.f64 = c;
  }
  explicit som_property_def_t(const char* n, const char* c) : name(SciterAtomValue(n)),
    type(SOM_PROP_STRING)
  {
    u.str = c;
  }
#endif
} som_property_def_t;


typedef struct som_method_def_t {
  void*        reserved;
  som_atom_t   name;
  size_t       params;
  som_method_t func;
#ifdef __cplusplus
  som_method_def_t(const char* n, size_t p, som_method_t f) : name(SciterAtomValue(n)), params(p), func(f), reserved(0) {}
#endif
} som_method_def_t;

enum som_passport_flags {
  SOM_SEALED_OBJECT = 0x00,     // not extendable
  SOM_EXTENDABLE_OBJECT = 0x01, // extendable, asset may have new properties added
  SOM_HAS_NAME_RESOLVER = 0x02  // if name_resolver is valid 
};

// definiton of object (the thing) access interface
// this structure should be statically allocated - at least survive last instance of the engine
typedef struct som_passport_t {
  UINT64             flags;
  som_atom_t         name;         // class name
  const som_property_def_t* properties; size_t n_properties; // virtual property thunks
  const som_method_def_t*   methods; size_t n_methods;       // method thunks
  som_item_getter_t  item_getter;  // var item_val = thing[k];
  som_item_setter_t  item_setter;  // thing[k] = item_val;
  som_item_next_t    item_next;    // for(var item in thisThing)
  // any property "inteceptors"
  som_any_prop_getter_t  prop_getter;  // var prop_val = thing.k;
  som_any_prop_setter_t  prop_setter;  // thing.k = prop_val;
  som_name_resolver_t name_resolver;
  void* reserved;
} som_passport_t;

#endif
