//
//  wallheavenapi.cpp
//  FraSaver
//
//  Created by franck on 24/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#include "../include/wallhaven.h"

namespace wallhaven
{
	//- String tools ////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::string strGetLower(const std::string & s) {
		std::string res(s);
		std::transform(res.begin(), res.end(), res.begin(), ::tolower);
		return res;
	}
	
	// trim from start
	std::string strGetLTrim(const std::string &s) {
		std::string res(s);
		res.erase(res.begin(), std::find_if(res.begin(), res.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return res;
	}
	
	// trim from end
	std::string strGetRTrim(const std::string &s) {
		std::string res(s);
		res.erase(std::find_if(res.rbegin(), res.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return res;
	}
	
	// trim from both ends
	std::string strGetTrim(const std::string &s) {
		return strGetLTrim(strGetRTrim(s));
	}
	
	std::string strNoNewline(const std::string & s) {
		std::string res;
		for (auto i : s)
			if (i != '\n')
				res += i;
		
		return res;
	}

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

	CRandomRequestSettings::CRandomRequestSettings()
	:	_q()
	,	_c("111")
	,	_p("100")
	{
	}
	
	CRandomRequestSettings::CRandomRequestSettings(const CRandomRequestSettings & src)
	:	_q(src._q)
	,	_c(src._c)
	,	_p(src._p)
	{
	}
	
	CRandomRequestSettings & CRandomRequestSettings::operator=(const CRandomRequestSettings & src)
	{
		if (this != &src) {
			_q= src._q;
			_c= src._c;
			_p= src._p;
		}
		return (*this);
	}

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	CCollectionRequestSettings::CCollectionRequestSettings()
	:	_userName()
	,	_colId()
	,	_p()
	,	_pageCount(-1) // negative => unknown
	{
	}
	
	CCollectionRequestSettings::CCollectionRequestSettings(const CCollectionRequestSettings & src)
	:	_userName(src._userName)
	,	_colId(src._colId)
	,	_p(src._p)
	,	_pageCount(src._pageCount) // negative => unknown
	{
	}
	
	CCollectionRequestSettings& CCollectionRequestSettings::operator=(const CCollectionRequestSettings & src)
	{
		if (this != &src) {
			_userName=src._userName;
			_colId=src._colId;
			_p=src._p;
			_pageCount=src._pageCount; // negative => unknown
		}
		return (*this);
	}

}