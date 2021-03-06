/*
 * Copyright (c) 2016 Docker Inc
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdint.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/fail.h>
#include <caml/bigarray.h>
#include <caml/threads.h>
#include <caml/unixsupport.h>
#include <caml/callback.h>

#ifdef WIN32
#include <winsock2.h>
#include <wtypes.h>
#include <winbase.h>
#include <stdio.h>
#include <tchar.h>

#endif

static void named_pipe_not_available()
{
  caml_failwith("Named pipes not available");
}

/* string -> t */
CAMLprim value stub_named_pipe_create(value path) {
  CAMLparam1(path);
  CAMLlocal1(result);
#ifdef WIN32
  char *c_path = strdup(String_val(path));
  HANDLE h = INVALID_HANDLE_VALUE;
  DWORD nOutBufferSize = 4096;
  DWORD nInBufferSize  = 4096;
  DWORD nDefaultTimeOut = 0;
  LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
  caml_release_runtime_system();
  h = CreateNamedPipe(
    c_path,
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_BYTE |
    PIPE_READMODE_BYTE |
    PIPE_WAIT |
    PIPE_REJECT_REMOTE_CLIENTS,
    PIPE_UNLIMITED_INSTANCES,
    nOutBufferSize,
    nInBufferSize,
    nDefaultTimeOut,
    lpSecurityAttributes
  );
  free((void*)c_path);
  caml_acquire_runtime_system();

  if (h == INVALID_HANDLE_VALUE) {
    _tprintf(TEXT("CreateNamedPipe failed, GLE=%ld.\n"), GetLastError());
    caml_failwith("CreateNamedPipe failed");
  }
  result = win_alloc_handle(h);
#else
  named_pipe_not_available();
#endif
  CAMLreturn(result);
}

/* t -> bool */
CAMLprim value stub_named_pipe_connect(value handle) {
  CAMLparam1(handle);
  CAMLlocal1(result);
  result = Val_bool(0);
#ifdef WIN32
  HANDLE h = Handle_val(handle);
  BOOL fConnected = FALSE;
  caml_release_runtime_system();
  fConnected = ConnectNamedPipe(h, NULL)?TRUE:(GetLastError() == ERROR_PIPE_CONNECTED);
  caml_acquire_runtime_system();
  if (fConnected) result = Val_bool(1);
#else
  named_pipe_not_available();
#endif
  CAMLreturn(result);
}

/* t -> unit */
CAMLprim value stub_named_pipe_flush(value handle) {
  CAMLparam1(handle);
#ifdef WIN32
  HANDLE h = Handle_val(handle);
  caml_release_runtime_system();
  FlushFileBuffers(h);
  caml_acquire_runtime_system();
#else
  named_pipe_not_available();
#endif
  CAMLreturn(0);
}

/* t -> unit */
CAMLprim value stub_named_pipe_disconnect(value handle) {
  CAMLparam1(handle);
#ifdef WIN32
  HANDLE h = Handle_val(handle);
  caml_release_runtime_system();
  DisconnectNamedPipe(h);
  caml_acquire_runtime_system();
#else
  named_pipe_not_available();
#endif
  CAMLreturn(0);
}

/* t -> unit */
CAMLprim value stub_named_pipe_destroy(value handle) {
  CAMLparam1(handle);
#ifdef WIN32
  HANDLE h = Handle_val(handle);
  caml_release_runtime_system();
  CloseHandle(h);
  caml_acquire_runtime_system();
#else
  named_pipe_not_available();
#endif
  CAMLreturn(0);
}

/* string -> int -> bool */
CAMLprim value stub_named_pipe_wait(value path, value ms) {
  CAMLparam2(path, ms);
  CAMLlocal1(result);
  result = Val_bool(0);
#ifdef WIN32
  char *c_path = strdup(String_val(path));
  int c_ms = Int_val(ms);
  BOOL c_result = FALSE;
  caml_release_runtime_system();
  c_result = WaitNamedPipe(c_path, c_ms);
  caml_acquire_runtime_system();
  result = Val_bool(c_result);
#else
  named_pipe_not_available();
#endif
  CAMLreturn(result);
}
