#if 0
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

#ifndef LITEBROWSER_TOOLBAR_WINDOW_H__
#define LITEBROWSER_TOOLBAR_WINDOW_H__

#include "litebrowser/stdafx.h"

#include "litebrowser/cairo_container.h"
#include "litebrowser/dib.h"
#include "litebrowser/el_omnibox.h"

#define TOOLBARWND_CLASS	L"TOOLBAR_WINDOW"

class CBrowserWnd;

class CToolbarWnd : public cairo_container
{
	HWND					m_hWnd;
	HINSTANCE				m_hInst;
	litehtml::context		m_context;
	litehtml::document::ptr	m_doc;
	CBrowserWnd*			m_parent;
	std::shared_ptr<el_omnibox>	m_omnibox;
	litehtml::tstring		m_cursor;
	BOOL					m_inCapture;
public:
	CToolbarWnd(HINSTANCE hInst, CBrowserWnd* parent);
	virtual ~CToolbarWnd(void);

	void create(int x, int y, int width, HWND parent);
	HWND wnd()	{ return m_hWnd; }
	int height()
	{
		if(m_doc)
		{
			return m_doc->height();
		}
		return 0;
	}
	int set_width(int width);
	void on_page_loaded(LPCWSTR url);

	// cairo_container members
	virtual void			make_url(LPCWSTR url, LPCWSTR basepath, std::wstring& out);
	virtual cairo_container::image_ptr get_image(LPCWSTR url, bool redraw_on_ready);

	// litehtml::document_container members
	virtual	void	set_caption(const litehtml::tchar_t* caption);
	virtual	void	set_base_url(const litehtml::tchar_t* base_url);
	virtual	void	link(std::shared_ptr<litehtml::document>& doc, litehtml::element::ptr el);
	virtual void	import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl);
	virtual	void	on_anchor_click(const litehtml::tchar_t* url, const litehtml::element::ptr& el);
	virtual	void	set_cursor(const litehtml::tchar_t* cursor);
	virtual std::shared_ptr<litehtml::element> create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc);

protected:
	virtual void	OnCreate();
	virtual void	OnPaint(simpledib::dib* dib, LPRECT rcDraw);
	virtual void	OnSize(int width, int height);
	virtual void	OnDestroy();
	virtual void	OnMouseMove(int x, int y);
	virtual void	OnLButtonDown(int x, int y);
	virtual void	OnLButtonUp(int x, int y);
	virtual void	OnMouseLeave();
	virtual void	OnOmniboxClicked();

	virtual void	get_client_rect(litehtml::position& client) const;

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void render_toolbar(int width);
	void update_cursor();
};

#endif // LITEBROWSER_TOOLBAR_WINDOW_H__
#endif