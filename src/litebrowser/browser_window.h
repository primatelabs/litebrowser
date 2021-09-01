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

#ifndef LITEBROWSER_BROWSER_WINDOW_H__
#define LITEBROWSER_BROWSER_WINDOW_H__

#include "litebrowser/stdafx.h"

#include "litebrowser/globals.h"
#include "litebrowser/html_view.h"
#include "litebrowser/resource.h"

#include "litehtml.h"

class BrowserWindow;

typedef CWinTraits<WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW> BrowserWindowTraits;
typedef CFrameWindowImpl<BrowserWindow, CWindow, BrowserWindowTraits> BrowserWindowBase;

class BrowserWindow : public BrowserWindowBase, 
	public CUpdateUI<BrowserWindow>, 
	public CMessageFilter,
	public CIdleHandler {
protected:
	litehtml::context context_;

	CHTMLViewWnd* view_ = nullptr;
	
	CCommandBarCtrl command_bar_;

	CEdit url_;

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME);

	BEGIN_UPDATE_UI_MAP(BrowserWindow)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(BrowserWindow)
		MSG_WM_CREATE(OnCreate);
		MSG_WM_DESTROY(OnDestroy);

		CHAIN_MSG_MAP(CUpdateUI<BrowserWindow>);
		CHAIN_MSG_MAP(BrowserWindowBase);
	END_MSG_MAP()

	BrowserWindow();

	virtual ~BrowserWindow();

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	
	void OnDestroy();

	LRESULT OnSize(UINT type, CSize extent);

	void OpenURL(LPCWSTR path);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnIdle();
};

#endif // LITEBROWSER_BROWSER_WINDOW_H__
