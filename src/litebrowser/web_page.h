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

#ifndef LITEBROWSER_WEB_PAGE_H__
#define LITEBROWSER_WEB_PAGE_H__

#include "litebrowser/cairo_container.h"
#include "litebrowser/cairo_font.h"
#include "litebrowser/tordexhttp.h"

class CHTMLViewWnd;

class web_page :	public cairo_container
{
	CHTMLViewWnd*				m_parent;
	LONG						m_refCount;
public:
	tordex::http				m_http;
	std::wstring				m_url;
	litehtml::document::ptr		m_doc;
	std::wstring				m_caption;
	std::wstring				m_cursor;
	std::wstring				m_base_path;
	HANDLE						m_hWaitDownload;
	std::wstring				m_waited_file;
	std::wstring				m_hash;
public:
	web_page(CHTMLViewWnd* parent);
	virtual ~web_page();

	void load(LPCWSTR url);
	void on_document_loaded(LPCWSTR file, LPCWSTR encoding, LPCWSTR realUrl);
	void on_image_loaded(LPCWSTR file, LPCWSTR url, bool redraw_only);
	void on_document_error(DWORD dwError, LPCWSTR errMsg);
	void on_waited_finished(DWORD dwError, LPCWSTR file);
	void add_ref();
	void release();
	void get_url(std::wstring& url);

	// litehtml::document_container members
	virtual	void		set_caption(const litehtml::tchar_t* caption);
	virtual	void		set_base_url(const litehtml::tchar_t* base_url);
	virtual void		import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl);
	virtual	void		on_anchor_click(const litehtml::tchar_t* url, const litehtml::element::ptr& el);
	virtual	void		set_cursor(const litehtml::tchar_t* cursor);

	virtual void		make_url(LPCWSTR url, LPCWSTR basepath, std::wstring& out);
	virtual cairo_container::image_ptr	get_image(LPCWSTR url, bool redraw_on_ready);
	virtual void		get_client_rect(litehtml::position& client)  const;
private:
	LPWSTR	load_text_file( LPCWSTR path, bool is_html, LPCWSTR defEncoding = L"UTF-8");
	unsigned char*	load_utf8_file( LPCWSTR path, bool is_html, LPCWSTR defEncoding = L"UTF-8");
	BOOL	download_and_wait(LPCWSTR url);
};

enum web_file_type
{
	web_file_document,
	web_file_image_redraw,
	web_file_image_rerender,
	web_file_waited,
};

class web_file : public tordex::http_request
{
	WCHAR			m_file[MAX_PATH];
	web_page*		m_page;
	web_file_type	m_type;
	HANDLE			m_hFile;
	LPVOID			m_data;
	std::wstring	m_realUrl;
public:
	web_file(web_page* page, web_file_type type, LPVOID data = NULL);
	virtual ~web_file();

	virtual void OnFinish(DWORD dwError, LPCWSTR errMsg);
	virtual void OnData(LPCBYTE data, DWORD len, ULONG64 downloaded, ULONG64 total);
	virtual void OnHeadersReady(HINTERNET hRequest);

};

#endif // LITEBROWSER_WEB_PAGE_H__
