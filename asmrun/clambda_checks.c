/***********************************************************************/
/*                                                                     */
/*                                OCaml                                */
/*                                                                     */
/*                      Pierre Chambart, OCamlPro                      */
/*                  Mark Shinwell, Jane Street Europe                  */
/*                                                                     */
/*  Copyright 2015 Institut National de Recherche en Informatique et   */
/*  en Automatique.  All rights reserved.  This file is distributed    */
/*  under the terms of the GNU Library General Public License, with    */
/*  the special exception on linking described in file ../LICENSE.     */
/*                                                                     */
/***********************************************************************/

/* Runtime checks to try to catch errors in code generation.
   See flambda_to_clambda.ml for more information. */

#include <assert.h>
#include <stdio.h>

#include <caml/mlvalues.h>

value caml_check_value_is_closure(value v, value v_descr)
{
  const char* descr = String_val(v_descr);
  value orig_v = v;

  if (v == (value) 0) {
    fprintf(stderr, "NULL is not a closure: %s\n",
      descr);
    abort();
  }
  if (!Is_block(v)) {
    fprintf(stderr,
      "Expecting a closure, got a non-boxed value %p: %s\n",
      (void*) v, descr);
    abort();
  }
  if (!(Tag_val(v) == Closure_tag || Tag_val(v) == Infix_tag)) {
    fprintf(stderr,
      "Expecting a closure, got a boxed value with tag %i: %s\n",
      Tag_val(v), descr);
    abort();
  }
  if (Tag_val(v) == Infix_tag) {
    v -= Infix_offset_val(v);
    assert(Tag_val(v) == Closure_tag);
  }
  assert(Wosize_val(v) >= 2);

  return orig_v;
}

value caml_check_field_access(value v, value pos, value v_descr)
{
  const char* descr = String_val(v_descr);
  value orig_v = v;
  if (v == (value) 0) {
    fprintf(stderr, "Access to field %lld of NULL: %s\n",
      (unsigned long long) Long_val(pos), descr);
    abort();
  }
  if (!Is_block(v)) {
    fprintf(stderr,
      "Access to field %lld of non-boxed value %p is illegal: %s\n",
      (unsigned long long) Long_val(pos), (void*) v, descr);
    abort();
  }
  if (Tag_val(v) == Infix_tag) {
    uintnat offset = Infix_offset_val(v);
    v -= offset;
    pos += offset / sizeof(value);
  }
  assert(Long_val(pos) >= 0);
  if (Long_val(pos) >= Wosize_val(v)) {
    fprintf(stderr,
      "Access to field %lld of value %p of size %lld is illegal: %s\n",
      (unsigned long long) Long_val(pos), (void*) v,
      (unsigned long long) Wosize_val(v),
      descr);
    abort();
  }
  return orig_v;
}
