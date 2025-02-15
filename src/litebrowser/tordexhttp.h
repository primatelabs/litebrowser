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

#ifndef LITEBROWSER_TORDEXHTTP_H__
#define LITEBROWSER_TORDEXHTTP_H__

#include "litebrowser/stdafx.h"

#include <stdlib.h>

#include <vector>

namespace tordex
{
	class http_request
	{
		friend class http;
	public:
		typedef std::vector<http_request*>	vector;
	protected:
		HINTERNET			m_hConnection;
		HINTERNET			m_hRequest;
		CRITICAL_SECTION	m_sync;
		http*				m_http;
		BYTE				m_buffer[8192];
		DWORD				m_error;
		ULONG64				m_content_length;
		ULONG64				m_downloaded_length;
		DWORD				m_status;
		std::wstring		m_url;
		LONG				m_refCount;
	public:
		http_request();
		virtual ~http_request();

		virtual void OnFinish(DWORD dwError, LPCWSTR errMsg) = 0;
		virtual void OnData(LPCBYTE data, DWORD len, ULONG64 downloaded, ULONG64 total) = 0;
		virtual void OnHeadersReady(HINTERNET hRequest);

		BOOL	create(LPCWSTR url, HINTERNET hSession);
		void	cancel();
		void	lock();
		void	unlock();
		ULONG64	get_content_length();
		DWORD	get_status_code();
		void	add_ref();
		void	release();

	protected:
		DWORD	onSendRequestComplete();
		DWORD	onHeadersAvailable();
		DWORD	onHandleClosing();
		DWORD	onRequestError(DWORD dwError);
		DWORD	onReadComplete(DWORD len);
		DWORD	readData();
		void	set_parent(http* parent);
	};

	class http
	{
		friend class http_request;

		HINTERNET				m_hSession;
		http_request::vector	m_requests;
		CRITICAL_SECTION		m_sync;
		DWORD					m_maxConnectionsPerServer;
	public:
		http();
		virtual ~http();

		void	set_max_connections_per_server(DWORD max_con);
		BOOL	open(LPCWSTR pwszUserAgent, DWORD dwAccessType, LPCWSTR pwszProxyName, LPCWSTR pwszProxyBypass);
		BOOL	download_file(LPCWSTR url, http_request* request);
		void	stop();
		void	close();

		void lock();
		void unlock();

	private:
		static VOID CALLBACK http_callback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);

	protected:
		void remove_request(http_request* request);
	};

	inline DWORD http_request::get_status_code()
	{
		return m_status;
	}

	inline void http_request::lock()
	{
		EnterCriticalSection(&m_sync);
	}

	inline void http_request::unlock()
	{
		LeaveCriticalSection(&m_sync);
	}

	inline ULONG64	http_request::get_content_length()
	{
		return m_content_length;
	}

	inline void http::lock()
	{
		EnterCriticalSection(&m_sync);
	}

	inline void http::unlock()
	{
		LeaveCriticalSection(&m_sync);
	}

	inline void http::set_max_connections_per_server(DWORD max_con)
	{
		m_maxConnectionsPerServer = max_con;
	}

}

#endif // LITEBROWSER_TORDEXHTTP_H__
