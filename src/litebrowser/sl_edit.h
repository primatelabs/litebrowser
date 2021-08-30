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

#pragma once
#include "TxThread.h"
#include "litebrowser/cairo_container.h"
#include "litebrowser/cairo_font.h"


#define WM_UPDATE_CONTROL	(WM_USER + 2001)
#define WM_EDIT_ACTIONKEY	(WM_USER + 2003)
#define WM_EDIT_CAPTURE		(WM_USER + 2004)

class CSingleLineEditCtrl : public CTxThread
{
private:
	cairo_container*	m_container;
	HWND				m_parent;
	std::wstring		m_text;
	cairo_font*			m_hFont;
	litehtml::web_color	m_textColor;
	int					m_lineHeight;
	int					m_caretPos;
	int					m_leftPos;
	BOOL				m_caretIsCreated;
	int					m_selStart;
	int					m_selEnd;
	BOOL				m_inCapture;
	int					m_startCapture;
	int					m_width;
	int					m_height;
	int					m_caretX;
	BOOL				m_showCaret;
	RECT				m_rcText;

public:
	CSingleLineEditCtrl(HWND parent, cairo_container* container);
	virtual ~CSingleLineEditCtrl(void);

	BOOL	OnKeyDown(WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(WPARAM wParam, LPARAM lParam);
	BOOL	OnChar(WPARAM wParam, LPARAM lParam);
	void	OnLButtonDown(int x, int y);
	void	OnLButtonUp(int x, int y);
	void	OnLButtonDblClick(int x, int y);
	void	OnMouseMove(int x, int y);
	void	setRect(LPRECT rcText);
	void	setText(LPCWSTR text);
	LPCWSTR getText()	{ return m_text.c_str(); }
	void	setFont(cairo_font* font, litehtml::web_color& color);
	void	draw(cairo_t* cr);
	void	setSelection(int start, int end);
	void	replaceSel(LPCWSTR text);
	void	hideCaret();
	void	showCaret();
	void	set_parent(HWND parent);
	BOOL	in_capture()
	{
		return m_inCapture;
	}

	virtual DWORD ThreadProc();
private:
	void	UpdateCarret();
	void	UpdateControl();
	void	delSelection();
	void	createCaret();
	void	destroyCaret();
	void	setCaretPos(int pos);
	void	fillSelRect(cairo_t* cr, LPRECT rcFill);
	int		getCaretPosXY(int x, int y);

	void	drawText(cairo_t* cr, LPCWSTR text, int cbText, LPRECT rcText, litehtml::web_color textColor);
	void	getTextExtentPoint(LPCWSTR text, int cbText, LPSIZE sz);
	void	set_color(cairo_t* cr, litehtml::web_color color)	{ cairo_set_source_rgba(cr, color.red / 255.0, color.green / 255.0, color.blue / 255.0, color.alpha / 255.0); }
};
