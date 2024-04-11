#ifndef ZSTD_RUBY_H
#define ZSTD_RUBY_H 1

#include <ruby.h>
#include "./libzstd/zstd.h"

static int convert_compression_level(VALUE compression_level_value)
{
  if (NIL_P(compression_level_value)) {
    return ZSTD_CLEVEL_DEFAULT;
  }
  return NUM2INT(compression_level_value);
}

static size_t zstd_compress(ZSTD_CCtx* const ctx, ZSTD_outBuffer* output, ZSTD_inBuffer* input, ZSTD_EndDirective endOp)
{
  return ZSTD_compressStream2(ctx, output, input, endOp);
}

static void set_compress_params(ZSTD_CCtx* const ctx, VALUE level_from_args, VALUE kwargs)
{
  ID kwargs_keys[2];
  kwargs_keys[0] = rb_intern("level");
  kwargs_keys[1] = rb_intern("dict");
  VALUE kwargs_values[2];
  rb_get_kwargs(kwargs, kwargs_keys, 0, 2, kwargs_values);

  int compression_level = ZSTD_CLEVEL_DEFAULT;
  if (kwargs_values[0] != Qundef && kwargs_values[0] != Qnil) {
    compression_level = convert_compression_level(kwargs_values[0]);
  } else if (!NIL_P(level_from_args)) {
    rb_warn("`level` in args is deprecated; use keyword args `level:` instead.");
    compression_level = convert_compression_level(level_from_args);
  }
  ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, compression_level);

  if (kwargs_values[1] != Qundef && kwargs_values[1] != Qnil) {
    char* dict_buffer = RSTRING_PTR(kwargs_values[1]);
    size_t dict_size = RSTRING_LEN(kwargs_values[1]);
    size_t load_dict_ret = ZSTD_CCtx_loadDictionary(ctx, dict_buffer, dict_size);
    if (ZSTD_isError(load_dict_ret)) {
      ZSTD_freeCCtx(ctx);
      rb_raise(rb_eRuntimeError, "%s", "ZSTD_CCtx_loadDictionary failed");
    }
  }
}

static void set_decompress_params(ZSTD_DCtx* const dctx, VALUE kwargs)
{
  ID kwargs_keys[1];
  kwargs_keys[0] = rb_intern("dict");
  VALUE kwargs_values[1];
  rb_get_kwargs(kwargs, kwargs_keys, 0, 1, kwargs_values);

  if (kwargs_values[0] != Qundef && kwargs_values[0] != Qnil) {
    char* dict_buffer = RSTRING_PTR(kwargs_values[0]);
    size_t dict_size = RSTRING_LEN(kwargs_values[0]);
    size_t load_dict_ret = ZSTD_DCtx_loadDictionary(dctx, dict_buffer, dict_size);
    if (ZSTD_isError(load_dict_ret)) {
      ZSTD_freeDCtx(dctx);
      rb_raise(rb_eRuntimeError, "%s", "ZSTD_CCtx_loadDictionary failed");
    }
  }
}

#endif /* ZSTD_RUBY_H */
