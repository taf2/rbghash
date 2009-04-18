/** 
 * Copyright (c) 2009 Todd A. Fisher
 * see LICENSE
 */
#include <ruby.h>
#include "ghash.h"

/* ruby 1.9 compat */
#ifndef RSTRING_PTR
#define RSTRING_PTR(str) RSTRING(str)->ptr
#endif

#ifndef RSTRING_LEN
#define RSTRING_LEN(str) RSTRING(str)->len
#endif

#define RB_CALL(func) (VALUE (*)(...))func

static VALUE rb_SparseSet;
static VALUE rb_DenseSet;

/** start sparse **/
static 
void SparseSet_free( GRB::SparseSet *gset )
{
  // free all keys
  for( GRB::SparseSet::iterator it = gset->begin(); it != gset->end(); ++it ) {
    free((void*)*it);
  }
  delete gset;
}

static
VALUE SparseSet_alloc(VALUE klass) 
{
  VALUE object;

  GRB::SparseSet *gset = new GRB::SparseSet();

  object = Data_Wrap_Struct( klass, NULL, SparseSet_free, gset );

  return object;
}

static
VALUE SparseSet_insert( VALUE self, VALUE key )
{
  GRB::SparseSet *gset;
  Data_Get_Struct( self, GRB::SparseSet, gset );

  if( TYPE(key) == T_STRING ) {
    gset->insert(strdup(RSTRING_PTR(key)));
  }
  else {
    rb_raise(rb_eRuntimeError, "Invalid key Type not equal to String");
  }

  return key;
}

static 
VALUE SparseSet_lookup( VALUE self, VALUE key )
{
  GRB::SparseSet *gset;
  Data_Get_Struct( self, GRB::SparseSet, gset );
  const char *pkey = RSTRING_PTR(key);

  GRB::SparseSet::const_iterator it = gset->find(pkey);

  return (it == gset->end()) ? Qfalse : Qtrue;
}

static 
VALUE SparseSet_delete( VALUE self, VALUE key )
{
  GRB::SparseSet *gset;
  Data_Get_Struct( self, GRB::SparseSet, gset );
  const char *pkey = RSTRING_PTR(key);
  GRB::SparseSet::iterator it = gset->find(pkey);

  gset->set_deleted_key(*it);
  gset->erase(it);
  free((void*)*it);
  gset->clear_deleted_key();

  return key;
}

static 
VALUE SparseSet_save( VALUE self, VALUE fname )
{
  GRB::SparseSet *gset;
  Data_Get_Struct( self, GRB::SparseSet, gset );

  FILE *f = fopen(RSTRING_PTR(fname),"wb");
  if( !f ) {
    rb_raise(rb_eRuntimeError,"File not found");
  }

  gset->write_metadata(f);
  gset->write_nopointer_data(f);

  fclose(f);

  return Qnil;
}

static 
VALUE SparseSet_load( VALUE self, VALUE fname )
{
  GRB::SparseSet *gset;
  Data_Get_Struct( self, GRB::SparseSet, gset );

  FILE *f = fopen(RSTRING_PTR(fname),"rb");
  if( !f ) {
    rb_raise(rb_eRuntimeError,"File not found");
  }

  gset->read_metadata(f);
  gset->read_nopointer_data(f);

  fclose(f);

  return Qnil;
}
/** end sparse **/
static 
void DenseSet_free( GRB::DenseSet *gset )
{
  // free all keys
  for( GRB::DenseSet::iterator it = gset->begin(); it != gset->end(); ++it ) {
    free((void*)*it);
  }
  delete gset;
}

static
VALUE DenseSet_alloc(VALUE klass) 
{
  VALUE object;

  GRB::DenseSet *gset = new GRB::DenseSet();

  gset->set_empty_key(NULL);

  object = Data_Wrap_Struct( klass, NULL, DenseSet_free, gset );

  return object;
}

static
VALUE DenseSet_insert( VALUE self, VALUE key )
{
  GRB::DenseSet *gset;
  Data_Get_Struct( self, GRB::DenseSet, gset );

  if( TYPE(key) == T_STRING ) {
    gset->insert(strdup(RSTRING_PTR(key)));
  }
  else {
    rb_raise(rb_eRuntimeError, "Invalid key Type not equal to String");
  }

  return key;
}

static 
VALUE DenseSet_lookup( VALUE self, VALUE key )
{
  GRB::DenseSet *gset;
  Data_Get_Struct( self, GRB::DenseSet, gset );

  GRB::DenseSet::const_iterator it = gset->find(RSTRING_PTR(key));

  return it == gset->end() ? Qfalse : Qtrue;
}

static 
VALUE DenseSet_delete( VALUE self, VALUE key )
{
  GRB::DenseSet *gset;
  Data_Get_Struct( self, GRB::DenseSet, gset );
  const char *pkey = RSTRING_PTR(key);
  GRB::DenseSet::iterator it = gset->find(pkey);

  gset->set_deleted_key(*it);
  gset->erase(it);
  free((void*)*it);
  gset->clear_deleted_key();


  return key;
}

extern "C"
void Init_ghash_core()
{
  rb_SparseSet = rb_define_class( "SparseSet", rb_cObject );

  rb_define_alloc_func( rb_SparseSet, SparseSet_alloc );
  rb_define_method( rb_SparseSet, "insert", RB_CALL(SparseSet_insert), 1 );
  rb_define_method( rb_SparseSet, "[]=", RB_CALL(SparseSet_insert), 2 );
  rb_define_method( rb_SparseSet, "lookup", RB_CALL(SparseSet_lookup), 1 );
  rb_define_method( rb_SparseSet, "key?", RB_CALL(SparseSet_lookup), 1 );
  rb_define_method( rb_SparseSet, "delete", RB_CALL(SparseSet_delete), 1 );
  rb_define_method( rb_SparseSet, "save", RB_CALL(SparseSet_save), 1 );
  rb_define_method( rb_SparseSet, "load", RB_CALL(SparseSet_load), 1 );

  rb_DenseSet = rb_define_class( "DenseSet", rb_cObject );

  rb_define_alloc_func( rb_DenseSet, DenseSet_alloc );
  rb_define_method( rb_DenseSet, "insert", RB_CALL(DenseSet_insert), 1 );
  rb_define_method( rb_DenseSet, "[]=", RB_CALL(DenseSet_insert), 2 );
  rb_define_method( rb_DenseSet, "lookup", RB_CALL(DenseSet_lookup), 1 );
  rb_define_method( rb_DenseSet, "key?", RB_CALL(DenseSet_lookup), 1 );
  rb_define_method( rb_DenseSet, "delete", RB_CALL(DenseSet_delete), 1 );
}

//-----------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	unsigned int h = seed ^ len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}
	
	// Handle the last few bytes of the input array

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
} 
