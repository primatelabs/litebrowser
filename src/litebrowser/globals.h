// Copyright (c) 2014, tordex
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the {organization} nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef LITEBROWSER_GLOBALS_H__
#define LITEBROWSER_GLOBALS_H__

#include "litebrowser/stdafx.h"

#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>
#include <algorithm>

#include "litebrowser/browser_application.h"
#include "litehtml.h"

extern CAppModule app_module;
extern BrowserApplication application;

// include TxDIB project
#include "litebrowser/TxDIB.h"

// include CAIRO project
#include <cairo.h>
#include <cairo-win32.h>
#pragma comment(lib, "cairo.lib")

// include SIMPLEDIB project
#include "litebrowser/dib.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Msimg32.lib")

#endif // LITEBROWSER_GLOBALS_H__
