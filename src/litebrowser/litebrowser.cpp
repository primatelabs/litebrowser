// Copyright (c) 2014, tordex
// Copyright (c) 2021 Primate Labs Inc.
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

#include "litebrowser/litebrowser.h"

#include "litebrowser/browser_window.h"
#include "litebrowser/cairo_font.h"

BrowserApplication application; 
CAppModule app_module;

#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "shlwapi.lib" )

using namespace Gdiplus;

CRITICAL_SECTION cairo_font::m_sync;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	application.instance(hInstance);

	app_module.Init(NULL, hInstance);
	AtlAxWinInit();

	CMessageLoop message_loop;
	app_module.AddMessageLoop(&message_loop);

	InitializeCriticalSectionAndSpinCount(&cairo_font::m_sync, 1000);

	GdiplusStartupInput gdiplus_startup_input;
	ULONG_PTR gdiplus_token;
	GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, NULL);

	BrowserWindow window;
	if (window.CreateEx() == NULL) {
		return 1;
	}

	// window.CenterWindow();
	window.ShowWindow(nCmdShow);
	window.UpdateWindow();

	// CBrowserWnd wnd(hInstance);
	// wnd.create();

	if(lpCmdLine && lpCmdLine[0]) {
		window.OpenURL(lpCmdLine);
	} else {
		window.OpenURL(L"http://www.dmoz.org/");
	}

	message_loop.Run();

	Gdiplus::GdiplusShutdown(gdiplus_token);

	app_module.Term();

	return 0;
}
