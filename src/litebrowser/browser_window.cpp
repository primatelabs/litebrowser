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

#include "litebrowser/browser_window.h"

#include "litebrowser/globals.h"
#include "litebrowser/html_view.h"
#include "litebrowser/toolbar_window.h"

BrowserWindow::BrowserWindow()
{
}

BrowserWindow::~BrowserWindow()
{
	if (view_) {
		delete view_;
	}
}

LRESULT BrowserWindow::OnCreate(LPCREATESTRUCT lpcs)
{
	// Figure out the scaling factors in case we're running in HighDPI mode.
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	{
		HDC hdc = ::GetDC(NULL);
		if (hdc) {
			float dpiX = static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSX));
			float dpiY = static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSY));
			scaleX = dpiX / 96.0f;
			scaleY = dpiY / 96.0f;
			::ReleaseDC(NULL, hdc);
		}
	}

	// Figure out the default font metrics using a typical url.
	HFONT defaultFont = AtlGetDefaultGuiFont();
	const wchar_t* s = L"https://www.nytimes.com/";
	CWindowDC dc(m_hWnd);
	CRect rc(0, 0, 0, 0);
	dc.SelectFont(defaultFont);
	dc.DrawText(s, wcslen(s), &rc, DT_CALCRECT);
	
	// defaultFontHeight is  in "logical" pixels (font metrics return the size
	// in device pixels).
	const int defaultFontHeight = rc.bottom / scaleY;

	HINSTANCE instance = application.instance();

	// create command bar window
	HWND hWndCmdBar = command_bar_.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	command_bar_.AttachMenu(GetMenu());
	// load command bar images
	command_bar_.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	
	// Add the toolbar (which doesn't contain anything useful yet).
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	// Create the address bar.
	CString address;
	address.LoadString(IDS_ADDRESS);
	url_.Create(m_hWnd, CRect(0, 0, 0, defaultFontHeight * 2), NULL, WS_CHILD | WS_VISIBLE, WS_EX_STATICEDGE);
	AddSimpleReBarBand(url_, address.GetBuffer(0), TRUE);
	url_.SetFont(AtlGetDefaultGuiFont());

	CReBarCtrl(m_hWndToolBar).LockBands(TRUE);

	CreateSimpleStatusBar();


#if defined(LITEHTML_UTF8)
	LPSTR css = NULL;
	HRSRC hResource = ::FindResource(instance, L"master.css", L"CSS");
	if (hResource)
	{
		DWORD imageSize = ::SizeofResource(instance, hResource);
		if (imageSize)
		{
			LPCSTR pResourceData = (LPCSTR) ::LockResource(::LoadResource(instance, hResource));
			if (pResourceData)
			{
				css = new CHAR[imageSize + 1];
				lstrcpynA(css, pResourceData, imageSize);
				css[imageSize] = 0;
			}
		}
	}
#else
	LPWSTR css = NULL;
	HRSRC hResource = ::FindResource(instance, L"master.css", L"CSS");
	if (hResource) {
		DWORD imageSize = ::SizeofResource(instance, hResource);
		if (imageSize) {
			LPCSTR pResourceData = (LPCSTR) ::LockResource(::LoadResource(instance, hResource));
			if (pResourceData) {
				css = new WCHAR[imageSize * 3];
				int ret = MultiByteToWideChar(CP_UTF8, 0, pResourceData, imageSize, css, imageSize * 3);
				css[ret] = 0;
			}
		}
	}
#endif

	if (css) {
		context_.load_master_stylesheet(css);
		delete css;
	}

	RECT client;
	GetClientRect(&client);

	view_ = new CHTMLViewWnd(instance, &context_, nullptr);
	m_hWndClient = view_->Create(m_hWnd,
		rcDefault,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL,
		WS_EX_CLIENTEDGE);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);
	
	// Register object for message filtering and idle updates.
	CMessageLoop* loop = app_module.GetMessageLoop();
	ATLASSERT(loop != nullptr);
	loop->AddMessageFilter(this);
	loop->AddIdleHandler(this);

	SetMsgHandled(false);
	return 0;
}

void BrowserWindow::OnDestroy()
{
	SetMsgHandled(false);
}

#if 0
LRESULT BrowserWindow::OnSize(UINT type, CSize extent)
{
	RECT client;
	GetClientRect(&client);
	if (view_) {
		::SetWindowPos(view_->wnd(), NULL, client.left, client.top, client.right - client.left, client.bottom - client.top, SWP_NOZORDER);
		::UpdateWindow(view_->wnd());
	}
	
	SetMsgHandled(false);
	return 0;
}
#endif

void BrowserWindow::OpenURL(LPCWSTR path)
{
	if (view_)	{
		view_->open(path, true);
	}
}


BOOL BrowserWindow::PreTranslateMessage(MSG* pMsg)
{
	if (BrowserWindowBase::PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (pMsg->message == WM_CHAR && url_ == pMsg->hwnd) {
		switch (pMsg->wParam) {
			case VK_RETURN: {
				CString szURL;
				int nLength = url_.GetWindowTextLength();
				url_.GetWindowText(szURL.GetBuffer(nLength), nLength + 1);
				szURL.ReleaseBuffer();
				view_->open(szURL.GetBuffer());
				return TRUE;
			}
		}
		return FALSE;
	}

	return BrowserWindowBase::PreTranslateMessage(pMsg);
}

BOOL BrowserWindow::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}
