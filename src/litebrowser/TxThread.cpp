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

#include "globals.h"
#include "TxThread.h"

CTxThread::CTxThread(void)
{
	m_hThread	= NULL;
	m_hStop		= NULL;
	m_trdID		= NULL;
}

CTxThread::~CTxThread(void)
{
	Stop();
	if(m_hThread) CloseHandle(m_hThread);
	if(m_hStop)   CloseHandle(m_hStop);
}

void CTxThread::Run()
{
	Stop();
	m_hStop	  = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = CreateThread(NULL, 0, sThreadProc, (LPVOID) this, 0, &m_trdID);
}

void CTxThread::Stop()
{
	if(m_hThread)
	{
		if(m_hStop)
		{
			SetEvent(m_hStop);
		}
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	if(m_hStop)   CloseHandle(m_hStop);
	m_hStop = NULL;
}

DWORD WINAPI CTxThread::sThreadProc( LPVOID lpParameter )
{
	CTxThread* pThis = (CTxThread*) lpParameter;
	return pThis->ThreadProc();
}

BOOL CTxThread::WaitForStop( DWORD ms )
{
	if(WaitForSingleObject(m_hStop, ms) != WAIT_TIMEOUT)
	{
		return TRUE;
	}
	return FALSE;
}

void CTxThread::postMessage( UINT Msg, WPARAM wParam, LPARAM lParam )
{
	if(m_hThread)
	{
		PostThreadMessage(m_trdID, Msg, wParam, lParam);
	}
}