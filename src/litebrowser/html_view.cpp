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

#include "litebrowser/html_view.h"

#include <strsafe.h>

#include <algorithm>

#include "litebrowser/browser_window.h"

using namespace litehtml;

CHTMLViewWnd::CHTMLViewWnd(HINSTANCE hInst, litehtml::context* ctx, void* delegate)
: CScrollWindowImpl<CHTMLViewWnd>()
, delegate_(delegate)
{
	m_hInst			= hInst;
	m_hWnd			= NULL;
	m_top			= 0;
	m_left			= 0;
	m_max_top		= 0;
	m_max_left		= 0;
	m_context		= ctx;
	m_page			= NULL;
	m_page_next		= NULL;

	InitializeCriticalSection(&m_sync);
}

CHTMLViewWnd::~CHTMLViewWnd(void)
{
	DeleteCriticalSection(&m_sync);
}

#if 0
LRESULT CALLBACK CHTMLViewWnd::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CHTMLViewWnd* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CHTMLViewWnd*)GetProp(hWnd, TEXT("htmlview_this"));
		if(pThis && pThis->m_hWnd != hWnd)
		{
			pThis = NULL;
		}
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch (uMessage)
		{
		case WM_PAGE_LOADED:
			pThis->OnPageReady();
			return 0;
		case WM_IMAGE_LOADED:
			if(wParam) {
				pThis->redraw(NULL, FALSE);
			} else {
				pThis->render();
			}
			break;
		case WM_SETCURSOR:
			pThis->update_cursor();
			break;
		case WM_ERASEBKGND:
			return TRUE;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CHTMLViewWnd*)(lpcs->lpCreateParams);
				SetProp(hWnd, TEXT("htmlview_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
				pThis->OnCreate();
			}
			break;
		case WM_PAINT:
			{
				RECT rcClient;
				GetClientRect(hWnd, &rcClient);
				pThis->create_dib(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				pThis->OnPaint(&pThis->m_dib, &ps.rcPaint);

				BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top,
					ps.rcPaint.right - ps.rcPaint.left,
					ps.rcPaint.bottom - ps.rcPaint.top, pThis->m_dib, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

				EndPaint(hWnd, &ps);
			}
			return 0;
		case WM_KEYDOWN:
			pThis->OnKeyDown((UINT) wParam);
			return 0;
		case WM_MOUSEMOVE:
			{
				TRACKMOUSEEVENT tme;
				ZeroMemory(&tme, sizeof(TRACKMOUSEEVENT));
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags		= TME_QUERY;
				tme.hwndTrack	= hWnd;
				TrackMouseEvent(&tme);
				if(!(tme.dwFlags & TME_LEAVE))
				{
					tme.dwFlags		= TME_LEAVE;
					tme.hwndTrack	= hWnd;
					TrackMouseEvent(&tme);
				}
				pThis->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			return 0;
		case WM_MOUSELEAVE:
			pThis->OnMouseLeave();
			return 0;
		case WM_LBUTTONDOWN:
			pThis->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONUP:
			pThis->OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}
#endif

void CHTMLViewWnd::OnPaint( simpledib::dib* dib, LPRECT rcDraw )
{
	cairo_surface_t* surface = cairo_image_surface_create_for_data((unsigned char*) dib->bits(), CAIRO_FORMAT_ARGB32, dib->width(), dib->height(), dib->width() * 4);
	cairo_t* cr = cairo_create(surface);

	cairo_rectangle(cr, rcDraw->left, rcDraw->top, rcDraw->right - rcDraw->left, rcDraw->bottom - rcDraw->top);
	cairo_clip(cr);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);

	lock();

	web_page* page = get_page(false);

	if(page)
	{

		litehtml::position clip(rcDraw->left, rcDraw->top, rcDraw->right - rcDraw->left, rcDraw->bottom - rcDraw->top);
		page->m_doc->draw((litehtml::uint_ptr) cr, -m_left, -m_top, &clip);

		page->release();
	}

	unlock();

	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}

void CHTMLViewWnd::open( LPCWSTR url, bool reload )
{
	std::wstring hash;
	std::wstring s_url = url;

	std::wstring::size_type hash_pos = s_url.find_first_of(L'#');
	if(hash_pos != std::wstring::npos)
	{
		hash = s_url.substr(hash_pos + 1);
		s_url.erase(hash_pos);
	}

	bool open_hash_only = false;

	lock();

	if(m_page)
	{
		if(m_page->m_url == s_url && !reload)
		{
			open_hash_only = true;
		} else
		{
			m_page->m_http.stop();
		}
	}
	if(!open_hash_only)
	{
		if(m_page_next)
		{
			m_page_next->m_http.stop();
			m_page_next->release();
		}
		m_page_next = new web_page(this);
		m_page_next->m_hash	= hash;
		m_page_next->load(s_url.c_str());
	}
	
	unlock();

	if(open_hash_only)
	{
		show_hash(hash);
		update_scroll();
		redraw(NULL, FALSE);
		update_history();
	}
}

void CHTMLViewWnd::render(BOOL calc_time, BOOL do_redraw, int calc_repeat)
{
	if(!m_hWnd)
	{
		return;
	}

	web_page* page = get_page();

	if(page)
	{
		RECT rcClient;
		GetClientRect(&rcClient);

		int width	= rcClient.right - rcClient.left;
		int height	= rcClient.bottom - rcClient.top;

		if(calc_time)
		{
			if (calc_repeat <= 0) calc_repeat = 1;
			DWORD tic1 = GetTickCount();
			for (int i = 0; i < calc_repeat; i++)
			{
				page->m_doc->render(width);
			}
			DWORD tic2 = GetTickCount();
			WCHAR msg[255];
			StringCchPrintf(msg, 255, L"Render time: %d msec", tic2 - tic1);
			MessageBox(msg, L"litebrowser", MB_ICONINFORMATION | MB_OK);
		} else
		{
			page->m_doc->render(width);
		}

		m_max_top = page->m_doc->height() - height;
		if(m_max_top < 0) m_max_top = 0;

		m_max_left = page->m_doc->width() - width;
		if(m_max_left < 0) m_max_left = 0;

		if(do_redraw)
		{
			update_scroll();
			redraw(NULL, FALSE);
		}

		page->release();
	}
}

LRESULT CHTMLViewWnd::OnImageLoaded(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (wParam) {
		redraw(NULL, FALSE);
	}
	else {
		render();
	}
	return 0;
}

LRESULT CHTMLViewWnd::OnPageLoaded(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	OnPageReady();
	return 0;
}


void CHTMLViewWnd::DoPaint(CDCHandle dc)
{
	RECT rect;
	GetClientRect(&rect);

#if 1

	m_dib.create(rect.right - rect.left, rect.bottom - rect.top);

	render();
	OnPaint(&m_dib, &rect);
#if 1
	dc.BitBlt(rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		m_dib,
		rect.left,
		rect.top,
		SRCCOPY);
#endif



	SaveHBITMAPToFile(m_dib.bmp(), L"C:\\Users\\jfpoole\\dib.bmp");
#endif
}

void CHTMLViewWnd::redraw(LPRECT rcDraw, BOOL update)
{
	if(m_hWnd)
	{
		InvalidateRect(rcDraw, TRUE);
		if(update)
		{
			UpdateWindow();
		}
	}
}

void CHTMLViewWnd::update_scroll()
{
	if(!is_valid_page())
	{
		ShowScrollBar(SB_BOTH, FALSE);
		return;
	}

	if(m_max_top > 0)
	{
		ShowScrollBar(SB_VERT, TRUE);

		RECT rcClient;
		GetClientRect(&rcClient);

		SCROLLINFO si;
		si.cbSize	= sizeof(SCROLLINFO);
		si.fMask	= SIF_ALL;
		si.nMin		= 0;
		si.nMax		= m_max_top + (rcClient.bottom - rcClient.top);
		si.nPos		= m_top;
		si.nPage	= rcClient.bottom - rcClient.top;
		SetScrollInfo(SB_VERT, &si, TRUE);
	} else
	{
		ShowScrollBar(SB_VERT, FALSE);
	}

	if(m_max_left > 0)
	{
		ShowScrollBar(SB_HORZ, TRUE);

		RECT rcClient;
		GetClientRect(&rcClient);

		SCROLLINFO si;
		si.cbSize	= sizeof(SCROLLINFO);
		si.fMask	= SIF_ALL;
		si.nMin		= 0;
		si.nMax		= m_max_left + (rcClient.right - rcClient.left);
		si.nPos		= m_left;
		si.nPage	= rcClient.right - rcClient.left;
		SetScrollInfo(SB_HORZ, &si, TRUE);
	} else
	{
		ShowScrollBar(SB_HORZ, FALSE);
	}
}


void CHTMLViewWnd::refresh()
{
	web_page* page = get_page();

	if(page)
	{
		open(page->m_url.c_str(), true);

		page->release();
	}
}

void CHTMLViewWnd::set_caption()
{
	web_page* page = get_page();

	if(!page) {
		::SetWindowText(GetParent(), L"litebrowser");
	} else {
		::SetWindowText(GetParent(), page->m_caption.c_str());
		page->release();
	}
}

void CHTMLViewWnd::OnMouseMove( int x, int y )
{
	web_page* page = get_page();
	if(page)
	{
		litehtml::position::vector redraw_boxes;
		if(page->m_doc->on_mouse_over(x + m_left, y + m_top, x, y, redraw_boxes))
		{
			for(litehtml::position::vector::iterator box = redraw_boxes.begin(); box != redraw_boxes.end(); box++)
			{
				box->x -= m_left;
				box->y -= m_top;
				RECT rcRedraw;
				rcRedraw.left	= box->left();
				rcRedraw.right	= box->right();
				rcRedraw.top	= box->top();
				rcRedraw.bottom	= box->bottom();
				redraw(&rcRedraw, FALSE);
			}
			UpdateWindow();
			update_cursor();
		}
		page->release();
	}
}

void CHTMLViewWnd::OnMouseLeave()
{
	web_page* page = get_page();

	if(page)
	{
		litehtml::position::vector redraw_boxes;
		if(page->m_doc->on_mouse_leave(redraw_boxes))
		{
			for(litehtml::position::vector::iterator box = redraw_boxes.begin(); box != redraw_boxes.end(); box++)
			{
				box->x -= m_left;
				box->y -= m_top;
				RECT rcRedraw;
				rcRedraw.left	= box->left();
				rcRedraw.right	= box->right();
				rcRedraw.top	= box->top();
				rcRedraw.bottom	= box->bottom();
				redraw(&rcRedraw, FALSE);
			}
			UpdateWindow();
		}

		page->release();
	}
}

void CHTMLViewWnd::OnLButtonDown( int x, int y )
{
	web_page* page = get_page();

	if(page)
	{
		litehtml::position::vector redraw_boxes;
		if(page->m_doc->on_lbutton_down(x + m_left, y + m_top, x, y, redraw_boxes))
		{
			for(litehtml::position::vector::iterator box = redraw_boxes.begin(); box != redraw_boxes.end(); box++)
			{
				box->x -= m_left;
				box->y -= m_top;
				RECT rcRedraw;
				rcRedraw.left	= box->left();
				rcRedraw.right	= box->right();
				rcRedraw.top	= box->top();
				rcRedraw.bottom	= box->bottom();
				redraw(&rcRedraw, FALSE);
			}
			UpdateWindow();
		}

		page->release();
	}
}

void CHTMLViewWnd::OnLButtonUp( int x, int y )
{
	web_page* page = get_page();

	if(page)
	{
		litehtml::position::vector redraw_boxes;
		if(page->m_doc->on_lbutton_up(x + m_left, y + m_top, x, y, redraw_boxes))
		{
			for(litehtml::position::vector::iterator box = redraw_boxes.begin(); box != redraw_boxes.end(); box++)
			{
				box->x -= m_left;
				box->y -= m_top;
				RECT rcRedraw;
				rcRedraw.left	= box->left();
				rcRedraw.right	= box->right();
				rcRedraw.top	= box->top();
				rcRedraw.bottom	= box->bottom();
				redraw(&rcRedraw, FALSE);
			}
			UpdateWindow();
		}

		page->release();
	}
}

void CHTMLViewWnd::back()
{
	std::wstring url;
	if(m_history.back(url))
	{
		open(url.c_str(), false);
	}
}

void CHTMLViewWnd::forward()
{
	std::wstring url;
	if(m_history.forward(url))
	{
		open(url.c_str(), false);
	}
}

void CHTMLViewWnd::update_cursor()
{
	LPCWSTR defArrow = m_page_next ? IDC_APPSTARTING : IDC_ARROW;

	web_page* page = get_page();

	if(!page)
	{
		SetCursor(LoadCursor(NULL, defArrow));
	} else
	{
		if(page->m_cursor == L"pointer")
		{
			SetCursor(LoadCursor(NULL, IDC_HAND));
		} else
		{
			SetCursor(LoadCursor(NULL, defArrow));
		}
		page->release();
	}
}

void CHTMLViewWnd::get_client_rect(litehtml::position& client) const
{
	RECT rcClient;
	GetClientRect(&rcClient);
	client.x = rcClient.left;
	client.y = rcClient.top;
	client.width = rcClient.right - rcClient.left;
	client.height = rcClient.bottom - rcClient.top;
}

bool CHTMLViewWnd::is_valid_page(bool with_lock)
{
	bool ret_val = true;

	if(with_lock)
	{
		lock();
	}

	if(!m_page || m_page && !m_page->m_doc)
	{
		ret_val = false;
	}

	if(with_lock)
	{
		unlock();
	}

	return ret_val;
}

web_page* CHTMLViewWnd::get_page(bool with_lock)
{
	web_page* ret_val = NULL;
	if(with_lock)
	{
		lock();
	}
	if(is_valid_page(false))
	{
		ret_val = m_page;
		ret_val->add_ref();
	}
	if(with_lock)
	{
		unlock();
	}

	return ret_val;
}

void CHTMLViewWnd::OnPageReady()
{
	std::wstring url;
	lock();
	web_page* page = m_page_next;
	unlock();

	std::wstring hash;

	bool is_ok = false;

	lock();

	if(m_page_next)
	{
		if(m_page)
		{
			m_page->release();
		}
		m_page = m_page_next;
		m_page_next = NULL;
		is_ok = true;
		hash = m_page->m_hash;
		url = m_page->m_url;
	}

	unlock();

	if(is_ok)
	{
		render(FALSE, FALSE);
		m_top	= 0;
		m_left	= 0;
		show_hash(hash);
		update_scroll();
		redraw(NULL, FALSE);
		set_caption();
		update_history();

#if 0
		// TODO: Reimplement using delegates
		if (m_parent) {
			m_parent->on_page_loaded(url.c_str());
		}
#endif
	}
}

void CHTMLViewWnd::show_hash(std::wstring& hash)
{
	web_page* page = get_page();
	if(page)
	{
		if(!hash.empty())
		{
			litehtml::tchar_t selector[255];
#ifndef LITEHTML_UTF8
			StringCchPrintf(selector, 255, L"#%s", hash.c_str());
#else
			LPSTR hashA = cairo_font::wchar_to_utf8(hash.c_str());
			StringCchPrintfA(selector, 255, "#%s", hashA);
#endif
			element::ptr el = page->m_doc->root()->select_one(selector);
			if(!el)
			{
#ifndef LITEHTML_UTF8
				StringCchPrintf(selector, 255, L"[name=%s]", hash.c_str());
#else
				StringCchPrintfA(selector, 255, "[name=%s]", hashA);
#endif
				el = page->m_doc->root()->select_one(selector);
			}
			if(el)
			{
				litehtml::position pos = el->get_placement();
				m_top = pos.y;
			}
#ifdef LITEHTML_UTF8
			delete hashA;
#endif
		} else
		{
			m_top = 0;
		}
		if(page->m_hash != hash)
		{
			page->m_hash = hash;
		}
		page->release();
	}
}

void CHTMLViewWnd::update_history()
{
	web_page* page = get_page();

	if(page)
	{
		std::wstring url;
		page->get_url(url);
		
		m_history.url_opened(url);

		page->release();
	}
}

void CHTMLViewWnd::create_dib( int width, int height )
{
	if(m_dib.width() < width || m_dib.height() < height)
	{
		m_dib.destroy();
		m_dib.create(width, height, true);
	}
}

void CHTMLViewWnd::scroll_to( int new_left, int new_top )
{
	litehtml::position client;
	get_client_rect(client);

	bool need_redraw = false;
	if(new_top != m_top)
	{
		if(std::abs(new_top - m_top) < client.height - client.height / 4 )
		{
			RECT rcRedraw;
			if(new_top > m_top)
			{
				int lines_count		= new_top - m_top;
				int rgba_to_scroll	= m_dib.width() * lines_count;
				int rgba_total		= m_dib.width() * client.height;

				memmove( m_dib.bits(), m_dib.bits() + rgba_to_scroll, (rgba_total - rgba_to_scroll) * sizeof(RGBQUAD) );
				rcRedraw.left	= client.left();
				rcRedraw.right	= client.right();
				rcRedraw.top	= client.height - lines_count;
				rcRedraw.bottom	= client.height;
			} else
			{
				int lines_count		= m_top - new_top;
				int rgba_to_scroll	= m_dib.width() * lines_count;
				int rgba_total		= m_dib.width() * client.height;

				memmove( m_dib.bits() + rgba_to_scroll, m_dib.bits(), (rgba_total - rgba_to_scroll) * sizeof(RGBQUAD) );
				rcRedraw.left	= client.left();
				rcRedraw.right	= client.right();
				rcRedraw.top	= client.top();
				rcRedraw.bottom	= lines_count;
			}

			int old_top = m_top;
			m_top  = new_top;
			OnPaint(&m_dib, &rcRedraw);

			litehtml::position::vector fixed_boxes;

			lock();
			web_page* page = get_page(false);
			if(page)
			{
				page->m_doc->get_fixed_boxes(fixed_boxes);
				page->release();
			}
			unlock();

			if(!fixed_boxes.empty())
			{
				RECT rcFixed;
				RECT rcClient;
				rcClient.left	= client.left();
				rcClient.right	= client.right();
				rcClient.top	= client.top();
				rcClient.bottom	= client.bottom();

				for(litehtml::position::vector::iterator iter = fixed_boxes.begin(); iter != fixed_boxes.end(); iter++)
				{
					rcRedraw.left	= iter->left();
					rcRedraw.right	= iter->right();
					rcRedraw.top	= iter->top();
					rcRedraw.bottom	= iter->bottom();
					if(IntersectRect(&rcFixed, &rcRedraw, &rcClient))
					{
						OnPaint(&m_dib, &rcFixed);
					}

					rcRedraw.left	= iter->left();
					rcRedraw.right	= iter->right();
					rcRedraw.top	= iter->top() + (old_top - m_top);
					rcRedraw.bottom	= iter->bottom() + (old_top - m_top);

					if(IntersectRect(&rcFixed, &rcRedraw, &rcClient))
					{
						OnPaint(&m_dib, &rcFixed);
					}
				}
			}

			CDC dc = GetDC();

			BitBlt(dc, client.left(), client.top(),
				client.width,
				client.height, m_dib, 0, 0, SRCCOPY);

			
			ReleaseDC(dc);

		} else
		{
			need_redraw = true;
		}

		m_top  = new_top;
		SetScrollPos(SB_VERT, m_top, TRUE);
	}


	if(new_left != m_left)
	{
		m_left  = new_left;
		SetScrollPos(SB_HORZ, m_left, TRUE);
		need_redraw = true;
	}

	if(need_redraw)
	{
		redraw(NULL, TRUE);
	}
}
