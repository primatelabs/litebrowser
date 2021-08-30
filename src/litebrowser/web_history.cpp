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
#include "web_history.h"


web_history::web_history()
{
	m_current_item = 0;
}

web_history::~web_history()
{

}

void web_history::url_opened( const std::wstring& url )
{
	if(!m_items.empty())
	{
		if(m_current_item != m_items.size() - 1)
		{
			if(m_current_item > 0 && m_items[m_current_item - 1] == url)
			{
				m_current_item--;
			} else if(m_current_item < m_items.size() - 1 && m_items[m_current_item + 1] == url)
			{
				m_current_item++;
			} else
			{
				m_items.erase(m_items.begin() + m_current_item + 1, m_items.end());
				m_items.push_back(url);
				m_current_item = m_items.size() - 1;
			}
		} else
		{
			if(m_current_item > 0 && m_items[m_current_item - 1] == url)
			{
				m_current_item--;
			} else 
			{
				m_items.push_back(url);
				m_current_item = m_items.size() - 1;
			}
		}
	} else
	{
		m_items.push_back(url);
		m_current_item = m_items.size() - 1;
	}
}

bool web_history::back( std::wstring& url )
{
	if(m_items.empty())	return false;
	
	if(m_current_item > 0)
	{
		url = m_items[m_current_item - 1];
		return true;
	}
	return false;
}

bool web_history::forward( std::wstring& url )
{
	if(m_items.empty())	return false;

	if(m_current_item < m_items.size() - 1)
	{
		url = m_items[m_current_item + 1];
		return true;
	}
	return false;
}
