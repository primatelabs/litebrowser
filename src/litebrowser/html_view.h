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

#ifndef LITEBROWSER_HTML_VIEW_H__
#define LITEBROWSER_HTML_VIEW_H__

#include "litebrowser/stdafx.h"

#include "litebrowser/web_history.h"
#include "litebrowser/web_page.h"

#define HTMLVIEWWND_CLASS	L"HTMLVIEW_WINDOW"

#define WM_IMAGE_LOADED		(WM_USER + 1000)
#define WM_PAGE_LOADED		(WM_USER + 1001)

#if 0
class HTMLView : public CScrollWindowImpl<HTMLView> {
	litehtml::context* context_ = nullptr;

	web_history history_;

	web_page* page_ = nullptr;

	web_page* page_next_ = nullptr;

	simpledib::dib dib_;

	void Render(LPRECT region);

public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1)

	HTMLView() = delete;

	explicit HTMLView(litehtml::context* ctx);

	virtual ~HTMLView();

	BEGIN_MSG_MAP(HTMLView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground);
		CHAIN_MSG_MAP(CScrollWindowImpl<HTMLView>);
	END_MSG_MAP()

	BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void DoPaint(CDCHandle dc);
};
#endif

class CHTMLViewWnd : public CScrollWindowImpl<CHTMLViewWnd> {
	HINSTANCE					m_hInst;
	int							m_top;
	int							m_left;
	int							m_max_top;
	int							m_max_left;
	litehtml::context*			m_context;
	web_history					m_history;
	web_page*					m_page;
	web_page*					m_page_next;
	CRITICAL_SECTION			m_sync;
	simpledib::dib				m_dib;
	void*				delegate_;
public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1);

	BEGIN_MSG_MAP(CHTMLViewWnd)
		// MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground);
		CHAIN_MSG_MAP(CScrollWindowImpl<CHTMLViewWnd>);
	END_MSG_MAP()

	BOOL PreTranslateMessage(MSG* pMsg);

	// LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void DoPaint(CDCHandle dc);


	CHTMLViewWnd(HINSTANCE	hInst, litehtml::context* ctx, void* delegate);
	
	virtual ~CHTMLViewWnd(void);

	void				open(LPCWSTR url, bool reload = FALSE);
	HWND				wnd()	{ return m_hWnd;	}
	void				refresh();
	void				back();
	void				forward();

	litehtml::context*	get_html_context()
	{
		return m_context;
	}

	void				set_caption();

	void				lock()
	{
		EnterCriticalSection(&m_sync);
	}

	void				unlock()
	{
		LeaveCriticalSection(&m_sync);
	}

	bool				is_valid_page(bool with_lock = true);
	web_page*			get_page(bool with_lock = true);

	void				render(BOOL calc_time = FALSE, BOOL do_redraw = TRUE, int calc_repeat = 1);
	void				get_client_rect(litehtml::position& client) const;
	void				show_hash(std::wstring& hash);
	void				update_history();

protected:
	
	virtual void		OnPaint(simpledib::dib* dib, LPRECT rcDraw);
	virtual void		OnMouseMove(int x, int y);
	virtual void		OnLButtonDown(int x, int y);
	virtual void		OnLButtonUp(int x, int y);
	virtual void		OnMouseLeave();
	virtual void		OnPageReady();
	
	void				redraw(LPRECT rcDraw, BOOL update);
	void				update_scroll();
	void				update_cursor();
	void				create_dib(int width, int height);
	void				scroll_to(int new_left, int new_top);
	

private:
	// static LRESULT	CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};

#endif // LITEBROWSER_HTML_VIEW_H__
