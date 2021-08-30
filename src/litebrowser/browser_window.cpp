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
#include "litebrowser/html_view_window.h"
#include "litebrowser/toolbar_window.h"

LRESULT BrowserWindow::OnCreate(LPCREATESTRUCT lpcs)
{
	CMessageLoop* loop = app_module.GetMessageLoop();
	ATLASSERT(loop != nullptr);
	loop->AddMessageFilter(this);

	HINSTANCE instance = application.instance();

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

	view_ = new CHTMLViewWnd(application.instance(), &context_, nullptr);
	view_->create(client.left, client.top, client.right - client.left, client.bottom - client.top, m_hWnd);
	::SetFocus(view_->wnd());


	SetMsgHandled(false);
	return 0;
}

void BrowserWindow::OnDestroy()
{
	if (view_) {
		delete view_;
	}
	SetMsgHandled(false);
}

LRESULT BrowserWindow::OnSize(UINT type, CSize extent)
{
	RECT client;
	GetClientRect(&client);
	::SetWindowPos(view_->wnd(), NULL, client.left, client.top, client.right - client.left, client.bottom - client.top, SWP_NOZORDER);
	::UpdateWindow(view_->wnd());
	
	SetMsgHandled(false);
	return 0;
}

void BrowserWindow::OpenURL(LPCWSTR path)
{
	if (view_)	{
		view_->open(path, true);
	}
}


BOOL BrowserWindow::PreTranslateMessage(MSG* pMsg)
{
	return BrowserWindowBase::PreTranslateMessage(pMsg);
}
