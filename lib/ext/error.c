/**
 * See Copyright Notice in picrin.h
 */

#include <picrin.h>
#include "../value.h"
#include "../object.h"
#include "../state.h"

#if PIC_USE_ERROR

# define pic_exc(pic) pic_ref(pic, "current-exception-handlers")

PIC_JMPBUF *
pic_prepare_try(pic_state *pic)
{
  struct context *cxt = pic_malloc(pic, sizeof(struct context));

  cxt->pc = NULL;
  cxt->fp = NULL;
  cxt->sp = NULL;
  cxt->irep = NULL;
  cxt->conts = pic_nil_value(pic);
  cxt->prev = pic->cxt;
  pic->cxt = cxt;
  return &cxt->jmp;
}

static pic_value
native_exception_handler(pic_state *pic)
{
  pic_value err;

  pic_get_args(pic, "o", &err);

  pic_call(pic, pic_closure_ref(pic, 0), 1, err);
  PIC_UNREACHABLE();
}

void
pic_enter_try(pic_state *pic)
{
  pic_value cont, handler;
  pic_value var, env;

  pic->cxt->ai = pic->ai;

  /* call/cc */
  cont = pic_make_cont(pic, pic_invalid_value(pic));
  handler = pic_lambda(pic, native_exception_handler, 1, cont);
  /* with-exception-handler */
  var = pic_exc(pic);
  env = pic_make_attr(pic);
  pic_attr_set(pic, env, var, pic_cons(pic, handler, pic_call(pic, var, 0)));
  pic->dyn_env = pic_cons(pic, env, pic->dyn_env);

  pic_leave(pic, pic->cxt->ai);
}

void
pic_exit_try(pic_state *pic)
{
  struct context *cxt = pic->cxt;
  pic_value c, it;
  pic->dyn_env = pic_cdr(pic, pic->dyn_env);
  pic_for_each (c, cxt->conts, it) {
    proc_ptr(pic, c)->env->regs[0] = pic_false_value(pic);
  }
  pic->cxt = cxt->prev;
  pic_free(pic, cxt);
  /* don't rewind ai here */
}

pic_value
pic_abort_try(pic_state *pic)
{
  struct context *cxt = pic->cxt;
  pic_value c, it;
  pic_value err = cxt->sp->regs[1];
  pic_for_each (c, cxt->conts, it) {
    proc_ptr(pic, c)->env->regs[0] = pic_false_value(pic);
  }
  pic->cxt = cxt->prev;
  pic_free(pic, cxt);
  pic_protect(pic, err);
  return err;
}

#endif

#if PIC_USE_ERROR
static const unsigned char error_rom[] = {
0x03, 0x01, 0x00, 0x04, 0x02, 0x01, 0x0b, 0x00, 0x00, 0x00, 0x02, 0x05,
0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x00, 0x02, 0x00, 0x00,
0x02, 0x01, 0x01, 0x06, 0x02, 0x00, 0x01, 0x02, 0x02, 0x00, 0x04, 0x01,
0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x6c, 0x69,
0x73, 0x74, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00,
0x02, 0x01, 0x02, 0x01, 0x00, 0x04, 0x00, 0x01, 0x0d, 0x00, 0x00, 0x00,
0x02, 0x0e, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x6b, 0x65, 0x2d, 0x70, 0x61,
0x72, 0x61, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x00, 0x06, 0x00, 0x00, 0x04,
0x01, 0x01, 0x01, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x01, 0x00, 0x04,
0x0b, 0x0b, 0x48, 0x00, 0x00, 0x00, 0x02, 0x1a, 0x00, 0x00, 0x00, 0x63,
0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d, 0x65, 0x78, 0x63, 0x65, 0x70,
0x74, 0x69, 0x6f, 0x6e, 0x2d, 0x68, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x72,
0x73, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x72, 0x61, 0x69, 0x73, 0x65,
0x00, 0x02, 0x11, 0x00, 0x00, 0x00, 0x72, 0x61, 0x69, 0x73, 0x65, 0x2d,
0x63, 0x6f, 0x6e, 0x74, 0x69, 0x6e, 0x75, 0x61, 0x62, 0x6c, 0x65, 0x00,
0x02, 0x16, 0x00, 0x00, 0x00, 0x77, 0x69, 0x74, 0x68, 0x2d, 0x65, 0x78,
0x63, 0x65, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x2d, 0x68, 0x61, 0x6e, 0x64,
0x6c, 0x65, 0x72, 0x00, 0x02, 0x11, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x6b,
0x65, 0x2d, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62, 0x6a, 0x65,
0x63, 0x74, 0x00, 0x02, 0x0d, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f,
0x72, 0x2d, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x3f, 0x00, 0x02, 0x16,
0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62, 0x6a,
0x65, 0x63, 0x74, 0x2d, 0x69, 0x72, 0x72, 0x69, 0x74, 0x61, 0x6e, 0x74,
0x73, 0x00, 0x02, 0x14, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72,
0x2d, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x2d, 0x6d, 0x65, 0x73, 0x73,
0x61, 0x67, 0x65, 0x00, 0x02, 0x11, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72,
0x6f, 0x72, 0x2d, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x2d, 0x74, 0x79,
0x70, 0x65, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f,
0x72, 0x00, 0x02, 0x07, 0x00, 0x00, 0x00, 0x64, 0x69, 0x73, 0x70, 0x6c,
0x61, 0x79, 0x00, 0x04, 0x00, 0x00, 0x01, 0x07, 0x00, 0x00, 0x02, 0x00,
0x00, 0x07, 0x00, 0x01, 0x02, 0x00, 0x01, 0x07, 0x00, 0x02, 0x02, 0x00,
0x02, 0x07, 0x00, 0x03, 0x02, 0x00, 0x03, 0x07, 0x00, 0x04, 0x02, 0x00,
0x04, 0x07, 0x00, 0x05, 0x02, 0x00, 0x05, 0x07, 0x00, 0x06, 0x02, 0x00,
0x06, 0x07, 0x00, 0x07, 0x02, 0x00, 0x07, 0x07, 0x00, 0x08, 0x02, 0x00,
0x08, 0x07, 0x00, 0x09, 0x02, 0x00, 0x09, 0x02, 0x01, 0x0a, 0x06, 0x02,
0x0a, 0x01, 0x02, 0x02, 0x00, 0x03, 0x01, 0x01, 0x08, 0x00, 0x00, 0x00,
0x02, 0x1a, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74,
0x2d, 0x65, 0x78, 0x63, 0x65, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x2d, 0x68,
0x61, 0x6e, 0x64, 0x6c, 0x65, 0x72, 0x73, 0x00, 0x06, 0x00, 0x00, 0x02,
0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x04, 0x01, 0x00, 0x0d, 0x00, 0x00,
0x00, 0x02, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01, 0x04, 0x02, 0x00, 0x01,
0x01, 0x02, 0x02, 0x00, 0x03, 0x01, 0x01, 0x08, 0x00, 0x00, 0x00, 0x02,
0x0e, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x6b, 0x65, 0x2d, 0x61, 0x74, 0x74,
0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01,
0x00, 0x01, 0x01, 0x01, 0x00, 0x03, 0x01, 0x01, 0x08, 0x00, 0x00, 0x00,
0x02, 0x1b, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74,
0x2d, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x2d, 0x65, 0x6e, 0x76,
0x69, 0x72, 0x6f, 0x6e, 0x6d, 0x65, 0x6e, 0x74, 0x00, 0x06, 0x00, 0x00,
0x02, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x05, 0x01, 0x00, 0x11, 0x00,
0x00, 0x00, 0x02, 0x00, 0x00, 0x04, 0x01, 0x02, 0x01, 0x04, 0x02, 0x01,
0x01, 0x04, 0x03, 0x00, 0x01, 0x01, 0x03, 0x03, 0x00, 0x05, 0x01, 0x01,
0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x6e,
0x73, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x02,
0x04, 0x03, 0x00, 0x03, 0x01, 0x03, 0x01, 0x00, 0x04, 0x01, 0x01, 0x0c,
0x00, 0x00, 0x00, 0x02, 0x1b, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72,
0x65, 0x6e, 0x74, 0x2d, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x2d,
0x65, 0x6e, 0x76, 0x69, 0x72, 0x6f, 0x6e, 0x6d, 0x65, 0x6e, 0x74, 0x00,
0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02,
0x01, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00,
0x00, 0x00, 0x63, 0x64, 0x72, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00,
0x04, 0x02, 0x05, 0x02, 0x01, 0x02, 0x01, 0x00, 0x04, 0x01, 0x01, 0x0c,
0x00, 0x00, 0x00, 0x02, 0x1a, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72,
0x65, 0x6e, 0x74, 0x2d, 0x65, 0x78, 0x63, 0x65, 0x70, 0x74, 0x69, 0x6f,
0x6e, 0x2d, 0x68, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x72, 0x73, 0x00, 0x06,
0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x01,
0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00,
0x00, 0x63, 0x61, 0x72, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04,
0x02, 0x07, 0x02, 0x01, 0x02, 0x01, 0x00, 0x04, 0x01, 0x00, 0x0d, 0x00,
0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x04, 0x02, 0x0a,
0x02, 0x01, 0x02, 0x01, 0x00, 0x05, 0x01, 0x02, 0x0f, 0x00, 0x00, 0x00,
0x02, 0x05, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x00, 0x01,
0x10, 0x00, 0x00, 0x00, 0x68, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x72, 0x20,
0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x65, 0x64, 0x00, 0x06, 0x00, 0x00,
0x02, 0x01, 0x00, 0x03, 0x02, 0x01, 0x04, 0x03, 0x0b, 0x02, 0x01, 0x03,
0x01, 0x00, 0x04, 0x01, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
0x04, 0x01, 0x07, 0x01, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x02, 0x00,
0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x1b, 0x00, 0x00, 0x00,
0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d, 0x64, 0x79, 0x6e, 0x61,
0x6d, 0x69, 0x63, 0x2d, 0x65, 0x6e, 0x76, 0x69, 0x72, 0x6f, 0x6e, 0x6d,
0x65, 0x6e, 0x74, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02,
0x08, 0x03, 0x01, 0x02, 0x01, 0x00, 0x03, 0x00, 0x00, 0x0a, 0x00, 0x00,
0x00, 0x04, 0x00, 0x01, 0x01, 0x04, 0x01, 0x01, 0x02, 0x01, 0x01, 0x02,
0x00, 0x03, 0x01, 0x01, 0x08, 0x00, 0x00, 0x00, 0x02, 0x1a, 0x00, 0x00,
0x00, 0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d, 0x65, 0x78, 0x63,
0x65, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x2d, 0x68, 0x61, 0x6e, 0x64, 0x6c,
0x65, 0x72, 0x73, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01, 0x01,
0x01, 0x00, 0x04, 0x01, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
0x04, 0x01, 0x01, 0x01, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x02, 0x00,
0x03, 0x01, 0x01, 0x08, 0x00, 0x00, 0x00, 0x02, 0x0e, 0x00, 0x00, 0x00,
0x6d, 0x61, 0x6b, 0x65, 0x2d, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75,
0x74, 0x65, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01, 0x01, 0x01,
0x00, 0x03, 0x01, 0x01, 0x08, 0x00, 0x00, 0x00, 0x02, 0x1b, 0x00, 0x00,
0x00, 0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d, 0x64, 0x79, 0x6e,
0x61, 0x6d, 0x69, 0x63, 0x2d, 0x65, 0x6e, 0x76, 0x69, 0x72, 0x6f, 0x6e,
0x6d, 0x65, 0x6e, 0x74, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01,
0x01, 0x01, 0x00, 0x05, 0x01, 0x00, 0x11, 0x00, 0x00, 0x00, 0x02, 0x00,
0x00, 0x04, 0x01, 0x02, 0x01, 0x04, 0x02, 0x01, 0x01, 0x04, 0x03, 0x00,
0x01, 0x01, 0x03, 0x03, 0x00, 0x05, 0x01, 0x01, 0x10, 0x00, 0x00, 0x00,
0x02, 0x04, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x6e, 0x73, 0x00, 0x06, 0x00,
0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x02, 0x04, 0x03, 0x00, 0x03,
0x01, 0x03, 0x01, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02,
0x1b, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d,
0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x2d, 0x65, 0x6e, 0x76, 0x69,
0x72, 0x6f, 0x6e, 0x6d, 0x65, 0x6e, 0x74, 0x00, 0x06, 0x00, 0x00, 0x02,
0x01, 0x00, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x01, 0x00, 0x04, 0x01,
0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x63, 0x64,
0x72, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x05, 0x02,
0x01, 0x02, 0x01, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02,
0x1a, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d,
0x65, 0x78, 0x63, 0x65, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x2d, 0x68, 0x61,
0x6e, 0x64, 0x6c, 0x65, 0x72, 0x73, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01,
0x00, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x01, 0x00, 0x04, 0x01, 0x01,
0x0c, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x63, 0x61, 0x72,
0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x07, 0x02, 0x01,
0x02, 0x01, 0x00, 0x04, 0x01, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x04, 0x00,
0x00, 0x01, 0x02, 0x01, 0x00, 0x04, 0x02, 0x0a, 0x02, 0x01, 0x02, 0x01,
0x00, 0x04, 0x01, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04,
0x01, 0x06, 0x01, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x02, 0x00, 0x04,
0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x1b, 0x00, 0x00, 0x00, 0x63,
0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d, 0x64, 0x79, 0x6e, 0x61, 0x6d,
0x69, 0x63, 0x2d, 0x65, 0x6e, 0x76, 0x69, 0x72, 0x6f, 0x6e, 0x6d, 0x65,
0x6e, 0x74, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x07,
0x03, 0x01, 0x02, 0x01, 0x00, 0x03, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
0x04, 0x00, 0x01, 0x01, 0x04, 0x01, 0x01, 0x02, 0x01, 0x01, 0x03, 0x00,
0x03, 0x01, 0x01, 0x08, 0x00, 0x00, 0x00, 0x02, 0x1a, 0x00, 0x00, 0x00,
0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d, 0x65, 0x78, 0x63, 0x65,
0x70, 0x74, 0x69, 0x6f, 0x6e, 0x2d, 0x68, 0x61, 0x6e, 0x64, 0x6c, 0x65,
0x72, 0x73, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01, 0x01, 0x01,
0x00, 0x04, 0x01, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04,
0x01, 0x01, 0x01, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x02, 0x00, 0x03,
0x01, 0x01, 0x08, 0x00, 0x00, 0x00, 0x02, 0x0e, 0x00, 0x00, 0x00, 0x6d,
0x61, 0x6b, 0x65, 0x2d, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74,
0x65, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00,
0x03, 0x01, 0x01, 0x08, 0x00, 0x00, 0x00, 0x02, 0x1b, 0x00, 0x00, 0x00,
0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d, 0x64, 0x79, 0x6e, 0x61,
0x6d, 0x69, 0x63, 0x2d, 0x65, 0x6e, 0x76, 0x69, 0x72, 0x6f, 0x6e, 0x6d,
0x65, 0x6e, 0x74, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01, 0x01,
0x01, 0x00, 0x05, 0x01, 0x00, 0x11, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
0x04, 0x01, 0x02, 0x01, 0x04, 0x02, 0x01, 0x01, 0x04, 0x03, 0x00, 0x01,
0x01, 0x03, 0x03, 0x00, 0x05, 0x01, 0x01, 0x10, 0x00, 0x00, 0x00, 0x02,
0x04, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x6e, 0x73, 0x00, 0x06, 0x00, 0x00,
0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x02, 0x04, 0x03, 0x00, 0x03, 0x01,
0x03, 0x01, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x1b,
0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x2d, 0x64,
0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x2d, 0x65, 0x6e, 0x76, 0x69, 0x72,
0x6f, 0x6e, 0x6d, 0x65, 0x6e, 0x74, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01,
0x00, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x01, 0x00, 0x05, 0x01, 0x01,
0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x6e,
0x73, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x07, 0x02,
0x04, 0x03, 0x05, 0x02, 0x01, 0x03, 0x01, 0x00, 0x04, 0x01, 0x01, 0x0c,
0x00, 0x00, 0x00, 0x02, 0x1a, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72,
0x65, 0x6e, 0x74, 0x2d, 0x65, 0x78, 0x63, 0x65, 0x70, 0x74, 0x69, 0x6f,
0x6e, 0x2d, 0x68, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x72, 0x73, 0x00, 0x06,
0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x01,
0x00, 0x03, 0x01, 0x00, 0x09, 0x00, 0x00, 0x00, 0x04, 0x00, 0x09, 0x03,
0x02, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x04, 0x01, 0x00, 0x0d, 0x00,
0x00, 0x00, 0x02, 0x00, 0x00, 0x04, 0x01, 0x05, 0x01, 0x04, 0x02, 0x00,
0x01, 0x01, 0x02, 0x02, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00,
0x02, 0x1b, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74,
0x2d, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x2d, 0x65, 0x6e, 0x76,
0x69, 0x72, 0x6f, 0x6e, 0x6d, 0x65, 0x6e, 0x74, 0x00, 0x06, 0x00, 0x00,
0x02, 0x01, 0x00, 0x04, 0x02, 0x06, 0x03, 0x01, 0x02, 0x01, 0x00, 0x03,
0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x01, 0x04, 0x01,
0x01, 0x02, 0x01, 0x01, 0x04, 0x00, 0x06, 0x01, 0x01, 0x14, 0x00, 0x00,
0x00, 0x02, 0x06, 0x00, 0x00, 0x00, 0x76, 0x65, 0x63, 0x74, 0x6f, 0x72,
0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x02, 0x04,
0x03, 0x00, 0x03, 0x04, 0x04, 0x00, 0x04, 0x01, 0x04, 0x01, 0x00, 0x05,
0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x02, 0x0b, 0x00, 0x00, 0x00, 0x6d,
0x61, 0x6b, 0x65, 0x2d, 0x72, 0x65, 0x63, 0x6f, 0x72, 0x64, 0x00, 0x02,
0x0c, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62,
0x6a, 0x65, 0x63, 0x74, 0x00, 0x06, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01,
0x03, 0x02, 0x01, 0x04, 0x03, 0x00, 0x01, 0x01, 0x03, 0x02, 0x00, 0x04,
0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x00, 0x72,
0x65, 0x63, 0x6f, 0x72, 0x64, 0x3f, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01,
0x00, 0x04, 0x02, 0x00, 0x02, 0x01, 0x02, 0x01, 0x00, 0x04, 0x01, 0x01,
0x1c, 0x00, 0x00, 0x00, 0x02, 0x0b, 0x00, 0x00, 0x00, 0x72, 0x65, 0x63,
0x6f, 0x72, 0x64, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x00, 0x04, 0x00, 0x00,
0x01, 0x08, 0x00, 0x10, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04,
0x02, 0x01, 0x02, 0x01, 0x02, 0x04, 0x00, 0x01, 0x01, 0x0a, 0x01, 0x01,
0x01, 0x01, 0x00, 0x05, 0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x02, 0x03,
0x00, 0x00, 0x00, 0x65, 0x71, 0x3f, 0x00, 0x02, 0x0c, 0x00, 0x00, 0x00,
0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74,
0x00, 0x06, 0x00, 0x00, 0x04, 0x01, 0x02, 0x01, 0x04, 0x02, 0x00, 0x01,
0x03, 0x03, 0x01, 0x01, 0x03, 0x02, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00,
0x00, 0x00, 0x02, 0x0d, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72,
0x2d, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x3f, 0x00, 0x06, 0x00, 0x00,
0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x02, 0x01, 0x02, 0x01, 0x00, 0x06,
0x01, 0x04, 0x27, 0x00, 0x00, 0x00, 0x02, 0x0c, 0x00, 0x00, 0x00, 0x72,
0x65, 0x63, 0x6f, 0x72, 0x64, 0x2d, 0x64, 0x61, 0x74, 0x75, 0x6d, 0x00,
0x02, 0x05, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x00, 0x01,
0x14, 0x00, 0x00, 0x00, 0x72, 0x65, 0x63, 0x6f, 0x72, 0x64, 0x20, 0x74,
0x79, 0x70, 0x65, 0x20, 0x6d, 0x69, 0x73, 0x6d, 0x61, 0x74, 0x63, 0x68,
0x00, 0x02, 0x0c, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d,
0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x00, 0x04, 0x00, 0x00, 0x01, 0x08,
0x00, 0x10, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x01,
0x02, 0x01, 0x02, 0x06, 0x00, 0x01, 0x04, 0x01, 0x01, 0x01, 0x03, 0x02,
0x02, 0x04, 0x03, 0x01, 0x02, 0x03, 0x04, 0x03, 0x01, 0x04, 0x01, 0x00,
0x05, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x00,
0x76, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x2d, 0x72, 0x65, 0x66, 0x00, 0x06,
0x00, 0x00, 0x04, 0x01, 0x02, 0x01, 0x04, 0x02, 0x00, 0x01, 0x0d, 0x03,
0x02, 0x01, 0x03, 0x02, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00,
0x02, 0x0d, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f,
0x62, 0x6a, 0x65, 0x63, 0x74, 0x3f, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01,
0x00, 0x04, 0x02, 0x00, 0x02, 0x01, 0x02, 0x01, 0x00, 0x06, 0x01, 0x04,
0x27, 0x00, 0x00, 0x00, 0x02, 0x0c, 0x00, 0x00, 0x00, 0x72, 0x65, 0x63,
0x6f, 0x72, 0x64, 0x2d, 0x64, 0x61, 0x74, 0x75, 0x6d, 0x00, 0x02, 0x05,
0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x00, 0x01, 0x14, 0x00,
0x00, 0x00, 0x72, 0x65, 0x63, 0x6f, 0x72, 0x64, 0x20, 0x74, 0x79, 0x70,
0x65, 0x20, 0x6d, 0x69, 0x73, 0x6d, 0x61, 0x74, 0x63, 0x68, 0x00, 0x02,
0x0c, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62,
0x6a, 0x65, 0x63, 0x74, 0x00, 0x04, 0x00, 0x00, 0x01, 0x08, 0x00, 0x10,
0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x01, 0x02, 0x01,
0x02, 0x06, 0x00, 0x01, 0x04, 0x01, 0x01, 0x01, 0x03, 0x02, 0x02, 0x04,
0x03, 0x01, 0x02, 0x03, 0x04, 0x03, 0x01, 0x04, 0x01, 0x00, 0x05, 0x00,
0x01, 0x10, 0x00, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x00, 0x76, 0x65,
0x63, 0x74, 0x6f, 0x72, 0x2d, 0x72, 0x65, 0x66, 0x00, 0x06, 0x00, 0x00,
0x04, 0x01, 0x02, 0x01, 0x04, 0x02, 0x00, 0x01, 0x0d, 0x03, 0x01, 0x01,
0x03, 0x02, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x0d,
0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62, 0x6a,
0x65, 0x63, 0x74, 0x3f, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04,
0x02, 0x00, 0x02, 0x01, 0x02, 0x01, 0x00, 0x06, 0x01, 0x04, 0x27, 0x00,
0x00, 0x00, 0x02, 0x0c, 0x00, 0x00, 0x00, 0x72, 0x65, 0x63, 0x6f, 0x72,
0x64, 0x2d, 0x64, 0x61, 0x74, 0x75, 0x6d, 0x00, 0x02, 0x05, 0x00, 0x00,
0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x00, 0x01, 0x14, 0x00, 0x00, 0x00,
0x72, 0x65, 0x63, 0x6f, 0x72, 0x64, 0x20, 0x74, 0x79, 0x70, 0x65, 0x20,
0x6d, 0x69, 0x73, 0x6d, 0x61, 0x74, 0x63, 0x68, 0x00, 0x02, 0x0c, 0x00,
0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62, 0x6a, 0x65,
0x63, 0x74, 0x00, 0x04, 0x00, 0x00, 0x01, 0x08, 0x00, 0x10, 0x00, 0x06,
0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x01, 0x02, 0x01, 0x02, 0x06,
0x00, 0x01, 0x04, 0x01, 0x01, 0x01, 0x03, 0x02, 0x02, 0x04, 0x03, 0x01,
0x02, 0x03, 0x04, 0x03, 0x01, 0x04, 0x01, 0x00, 0x05, 0x00, 0x01, 0x10,
0x00, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x00, 0x76, 0x65, 0x63, 0x74,
0x6f, 0x72, 0x2d, 0x72, 0x65, 0x66, 0x00, 0x06, 0x00, 0x00, 0x04, 0x01,
0x02, 0x01, 0x04, 0x02, 0x00, 0x01, 0x0d, 0x03, 0x00, 0x01, 0x03, 0x02,
0x01, 0x06, 0x01, 0x01, 0x12, 0x00, 0x00, 0x00, 0x02, 0x11, 0x00, 0x00,
0x00, 0x6d, 0x61, 0x6b, 0x65, 0x2d, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d,
0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01,
0x00, 0x0a, 0x02, 0x04, 0x03, 0x00, 0x02, 0x04, 0x04, 0x00, 0x03, 0x01,
0x04, 0x01, 0x00, 0x04, 0x00, 0x01, 0x0d, 0x00, 0x00, 0x00, 0x02, 0x05,
0x00, 0x00, 0x00, 0x72, 0x61, 0x69, 0x73, 0x65, 0x00, 0x06, 0x00, 0x00,
0x04, 0x01, 0x01, 0x01, 0x04, 0x02, 0x00, 0x01, 0x01, 0x02, 0x02, 0x00,
0x03, 0x01, 0x00, 0x09, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x02,
0x01, 0x00, 0x01, 0x01, 0x02, 0x01, 0x03, 0x02, 0x00, 0x08, 0x00, 0x00,
0x00, 0x02, 0x00, 0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x04,
0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x6e,
0x75, 0x6c, 0x6c, 0x3f, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04,
0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00, 0x04, 0x00, 0x02, 0x1e, 0x00,
0x00, 0x00, 0x02, 0x12, 0x00, 0x00, 0x00, 0x63, 0x75, 0x72, 0x72, 0x65,
0x6e, 0x74, 0x2d, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x70, 0x6f, 0x72,
0x74, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x63, 0x61, 0x72, 0x00, 0x04,
0x00, 0x00, 0x01, 0x08, 0x00, 0x0d, 0x00, 0x06, 0x00, 0x00, 0x04, 0x01,
0x01, 0x01, 0x01, 0x01, 0x06, 0x00, 0x01, 0x04, 0x01, 0x01, 0x01, 0x04,
0x02, 0x02, 0x03, 0x01, 0x02, 0x01, 0x00, 0x04, 0x01, 0x00, 0x0d, 0x00,
0x00, 0x00, 0x02, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01, 0x04, 0x02, 0x00,
0x01, 0x01, 0x02, 0x02, 0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00,
0x02, 0x0d, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f,
0x62, 0x6a, 0x65, 0x63, 0x74, 0x3f, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01,
0x00, 0x04, 0x02, 0x02, 0x02, 0x01, 0x02, 0x01, 0x00, 0x05, 0x01, 0x00,
0x23, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x08, 0x00, 0x0d, 0x00,
0x02, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01, 0x01, 0x01, 0x04, 0x00, 0x04,
0x02, 0x04, 0x01, 0x01, 0x01, 0x04, 0x02, 0x03, 0x02, 0x04, 0x03, 0x01,
0x02, 0x01, 0x03, 0x01, 0x00, 0x03, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00,
0x02, 0x00, 0x00, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x04, 0x01,
0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x11, 0x00, 0x00, 0x00, 0x65, 0x72,
0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x2d, 0x74,
0x79, 0x70, 0x65, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02,
0x05, 0x02, 0x01, 0x02, 0x01, 0x00, 0x04, 0x01, 0x01, 0x1c, 0x00, 0x00,
0x00, 0x02, 0x11, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d,
0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x00,
0x04, 0x00, 0x00, 0x01, 0x08, 0x00, 0x10, 0x00, 0x06, 0x00, 0x00, 0x02,
0x01, 0x00, 0x04, 0x02, 0x06, 0x02, 0x01, 0x02, 0x04, 0x00, 0x01, 0x01,
0x0c, 0x01, 0x01, 0x01, 0x01, 0x00, 0x05, 0x01, 0x00, 0x11, 0x00, 0x00,
0x00, 0x04, 0x00, 0x08, 0x02, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00, 0x01,
0x04, 0x03, 0x05, 0x02, 0x01, 0x03, 0x01, 0x00, 0x05, 0x00, 0x01, 0x11,
0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x2d, 0x00, 0x04, 0x00,
0x09, 0x02, 0x04, 0x01, 0x03, 0x01, 0x03, 0x02, 0x00, 0x04, 0x03, 0x06,
0x02, 0x01, 0x03, 0x01, 0x00, 0x05, 0x01, 0x01, 0x10, 0x00, 0x00, 0x00,
0x01, 0x08, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x3a, 0x20,
0x22, 0x00, 0x04, 0x00, 0x06, 0x02, 0x02, 0x01, 0x00, 0x03, 0x02, 0x00,
0x04, 0x03, 0x03, 0x02, 0x01, 0x03, 0x01, 0x00, 0x04, 0x01, 0x01, 0x0c,
0x00, 0x00, 0x00, 0x02, 0x14, 0x00, 0x00, 0x00, 0x65, 0x72, 0x72, 0x6f,
0x72, 0x2d, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x2d, 0x6d, 0x65, 0x73,
0x73, 0x61, 0x67, 0x65, 0x00, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04,
0x02, 0x06, 0x02, 0x01, 0x02, 0x01, 0x00, 0x05, 0x01, 0x00, 0x11, 0x00,
0x00, 0x00, 0x04, 0x00, 0x08, 0x02, 0x02, 0x01, 0x00, 0x04, 0x02, 0x00,
0x01, 0x04, 0x03, 0x05, 0x02, 0x01, 0x03, 0x01, 0x00, 0x04, 0x01, 0x01,
0x0c, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x22, 0x00, 0x04,
0x00, 0x09, 0x02, 0x02, 0x01, 0x00, 0x03, 0x02, 0x00, 0x01, 0x02, 0x01,
0x00, 0x04, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x16, 0x00, 0x00,
0x00, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x2d, 0x6f, 0x62, 0x6a, 0x65, 0x63,
0x74, 0x2d, 0x69, 0x72, 0x72, 0x69, 0x74, 0x61, 0x6e, 0x74, 0x73, 0x00,
0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x02, 0x09, 0x02, 0x01, 0x02,
0x01, 0x00, 0x05, 0x02, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x02, 0x08, 0x00,
0x00, 0x00, 0x66, 0x6f, 0x72, 0x2d, 0x65, 0x61, 0x63, 0x68, 0x00, 0x06,
0x00, 0x00, 0x02, 0x01, 0x00, 0x02, 0x02, 0x01, 0x04, 0x03, 0x00, 0x01,
0x01, 0x03, 0x01, 0x00, 0x05, 0x00, 0x01, 0x11, 0x00, 0x00, 0x00, 0x01,
0x01, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x04, 0x00, 0x0c, 0x02, 0x04, 0x01,
0x07, 0x01, 0x03, 0x02, 0x00, 0x04, 0x03, 0x09, 0x02, 0x01, 0x03, 0x02,
0x00, 0x05, 0x01, 0x01, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
0x00, 0x20, 0x00, 0x04, 0x00, 0x0c, 0x02, 0x02, 0x01, 0x00, 0x03, 0x02,
0x00, 0x04, 0x03, 0x09, 0x02, 0x01, 0x03, 0x01, 0x00, 0x05, 0x00, 0x01,
0x11, 0x00, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x77, 0x72, 0x69,
0x74, 0x65, 0x00, 0x06, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01, 0x04, 0x02,
0x01, 0x02, 0x04, 0x03, 0x0a, 0x02, 0x01, 0x03, 0x01, 0x00, 0x03, 0x00,
0x01, 0x0f, 0x00, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x00, 0x64, 0x69,
0x73, 0x70, 0x6c, 0x61, 0x79, 0x00, 0x04, 0x00, 0x00, 0x01, 0x07, 0x00,
0x00, 0x04, 0x00, 0x02, 0x01, 0x0c, 0x01, 0x01, 0x01, 
};
#endif

void
pic_init_error(pic_state *PIC_UNUSED(pic))
{
#if PIC_USE_ERROR
  pic_call(pic, pic_deserialize(pic, pic_blob_value(pic, error_rom, sizeof error_rom)), 0);
#endif
}
