//
//  urlProvider.hpp
//  FraSaver
//
//  Created by franck on 28/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#pragma once
#include "../include/wallhaven.h"

namespace wallhaven
{
	class CImageInfo
	{
	public:
		CImageInfo();
		CImageInfo(const std::string & sId, const std::string url);
		CImageInfo(const CImageInfo& src);
		CImageInfo & operator=(const CImageInfo& src);
		bool isValid() const { return !_sId.empty(); }

	public:
		std::string sId() const { return _sId; }
		std::string url() const { return _url; }
		
	private:
		std::string _sId;
		std::string _url;
	};

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class CUrlProvider
	:	public CThread
	{
	public:
		CUrlProvider();
		~CUrlProvider();
		void start( const CRandomRequestSettings & rrs );
		CImageInfo getNextUrl();
		void stop();
		
	protected:
		virtual void run() override;
	
	private:
		CRandomRequestSettings _rrs;
	
	private:
		std::mutex            _mutex;
		std::list<CImageInfo> _urls;
	};


}