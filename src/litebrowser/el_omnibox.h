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

#ifndef LITEBROWSER_EL_OMNIBOX_H__
#define LITEBROWSER_EL_OMNIBOX_H__

#include "litebrowser/stdafx.h"

#include "litebrowser/sl_edit.h"

#define WM_OMNIBOX_CLICKED	(WM_USER + 10002)

class el_omnibox : public litehtml::html_tag
{
	CSingleLineEditCtrl m_edit;
	HWND m_hWndParent;
	BOOL m_haveFocus;
public:
	el_omnibox(const std::shared_ptr<litehtml::document>& doc, HWND parent, cairo_container* container);
	~el_omnibox();

	virtual void draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position* clip);
	virtual void parse_styles(bool is_reparse);
	virtual void on_click();

	BOOL have_focus()
	{
		return m_haveFocus;
	}
	void update_position();
	void set_url(LPCWSTR url);
	std::wstring get_url();
	void set_parent(HWND parent);
	void SetFocus();
	void KillFocus();
	void select_all()
	{
		m_edit.setSelection(0, -1);
	}
	BOOL OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		return m_edit.OnKeyDown(wParam, lParam);
	}
	BOOL OnKeyUp(WPARAM wParam, LPARAM lParam)
	{
		return m_edit.OnKeyUp(wParam, lParam);
	}
	BOOL OnChar(WPARAM wParam, LPARAM lParam)
	{
		return m_edit.OnChar(wParam, lParam);
	}
	BOOL OnLButtonDown(int x, int y);
	BOOL OnLButtonUp(int x, int y);
	BOOL OnLButtonDblClick(int x, int y);
	BOOL OnMouseMove(int x, int y);

};

#endif // LITEBROWSER_EL_OMNIBOX_H__
