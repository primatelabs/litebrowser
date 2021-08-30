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

#include "litebrowser/el_omnibox.h"

#include <Richedit.h>
#include <strsafe.h>

#include "litebrowser/globals.h"

el_omnibox::el_omnibox(const std::shared_ptr<litehtml::document>& doc, HWND parent, cairo_container* container) : litehtml::html_tag(doc), m_edit(parent, container)
{
	m_hWndParent = parent;
	m_haveFocus = FALSE;
}

el_omnibox::~el_omnibox()
{

}

void el_omnibox::update_position()
{
	litehtml::position pos = litehtml::element::get_placement();
	RECT rcPos;
	rcPos.left = pos.left();
	rcPos.right = pos.right();
	rcPos.top = pos.top();
	rcPos.bottom = pos.bottom();
	m_edit.setRect(&rcPos);
}

void el_omnibox::set_url(LPCWSTR url)
{
	m_edit.setText(url);
}

void el_omnibox::draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position* clip)
{
	litehtml::html_tag::draw(hdc, x, y, clip);

	m_edit.draw((cairo_t*)hdc);
}

void el_omnibox::parse_styles(bool is_reparse)
{
	litehtml::html_tag::parse_styles(is_reparse);

	m_edit.setFont((cairo_font*)get_font(), get_color(_t("color"), true));
}

void el_omnibox::set_parent(HWND parent)
{
	m_hWndParent = parent;
	m_edit.set_parent(parent);
}

void el_omnibox::on_click()
{
	if (!m_haveFocus)
	{
		SendMessage(m_hWndParent, WM_OMNIBOX_CLICKED, 0, 0);
	}
}

void el_omnibox::SetFocus()
{
	m_edit.showCaret();
	m_edit.setSelection(0, -1);
	m_haveFocus = TRUE;
}

void el_omnibox::KillFocus()
{
	m_edit.setSelection(0, 0);
	m_edit.hideCaret();
	m_haveFocus = FALSE;
}

std::wstring el_omnibox::get_url()
{
	std::wstring str = m_edit.getText();

	if (!PathIsURL(str.c_str()))
	{
		DWORD sz = (DWORD) str.length() + 32;
		LPWSTR outUrl = new WCHAR[sz];
		HRESULT res = UrlApplyScheme(str.c_str(), outUrl, &sz, URL_APPLY_DEFAULT);
		if (res == E_POINTER)
		{
			delete outUrl;
			LPWSTR outUrl = new WCHAR[sz];
			if (UrlApplyScheme(str.c_str(), outUrl, &sz, URL_APPLY_DEFAULT) == S_OK)
			{
				str = outUrl;
			}
		}
		else if (res == S_OK)
		{
			str = outUrl;
		}
		delete outUrl;
	}

	return str;
}

BOOL el_omnibox::OnLButtonDown(int x, int y)
{
	if (have_focus())
	{
		litehtml::position pos = litehtml::element::get_placement();
		if (m_edit.in_capture() || pos.is_point_inside(x, y))
		{
			m_edit.OnLButtonDown(x, y);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL el_omnibox::OnLButtonUp(int x, int y)
{
	if (have_focus())
	{
		litehtml::position pos = litehtml::element::get_placement();
		if (m_edit.in_capture() || pos.is_point_inside(x, y))
		{
			m_edit.OnLButtonUp(x, y);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL el_omnibox::OnLButtonDblClick(int x, int y)
{
	if (have_focus())
	{
		litehtml::position pos = litehtml::element::get_placement();
		if (pos.is_point_inside(x, y))
		{
			m_edit.OnLButtonDblClick(x, y);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL el_omnibox::OnMouseMove(int x, int y)
{
	if (have_focus())
	{
		litehtml::position pos = litehtml::element::get_placement();
		if (m_edit.in_capture() || pos.is_point_inside(x, y))
		{
			m_edit.OnMouseMove(x, y);
			return TRUE;
		}
	}
	return FALSE;
}
