//
//  parser.cpp
//  FraSaver
//
//  Created by franck on 15/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#include "parser.h"
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
//#import <cocoa/cocoa.h>
#include <regex>

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
//- return cookie header string

static std::string getPassword( const std::string & userName )
{
	const std::string serviceName = "frachop.debugTest";
	void * readedPassword = nullptr;
	UInt32 passwordLength = 0;

	OSStatus res = SecKeychainFindGenericPassword(
		nullptr,
		static_cast<UInt32>(serviceName.length()),
		serviceName.c_str(),
		static_cast<UInt32>(userName.length()),
		userName.c_str(),
		
		&passwordLength,
		&readedPassword,
		
		nullptr );

	if (res) {
		CFStringRef err = SecCopyErrorMessageString (res, nullptr);
		CFShow(err);
	}

	std::string password;
	if ((res == 0) && (passwordLength > 0))
		password = (const char*) readedPassword;

	SecKeychainItemFreeContent( nullptr, readedPassword );
	
	return password;
}

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

CIds::CIds()
{
}

CIds::~CIds()
{
}

void CIds::start()
{
	CThread::start();
}


std::string CIds::getNextUrl()
{
	std::string result;
	_mutex.lock();
	if (!_fullUrls.empty()) {
		result = _fullUrls.front();
		_fullUrls.pop_front();
	}
	_mutex.unlock();
	return result;
}

static void merge(std::list<std::string> & dst, const std::list<std::string> & with)
{
	for (const auto i: with)
		dst.push_back(i);
}

template<class T>
void shuffle(std::list<T> & l)
{
	std::vector<T> v(l.size());
	int i(0);
	while (!l.empty()) {
		v[i++] = l.front();
		l.pop_front();
	}
	
	std::random_shuffle ( v.begin(), v.end() );
	for (i=0; i<v.size();++i)
		l.push_back(v[i]);
}

void CIds::run()
{
	using namespace wallhaven;
	CSession session;
	session.login("zwarf", "aimotion");
/*
std::map<std::string, std::string> favs= session.userFavorites("Gandalf");
std::cout << "favs" << std::endl;
for (auto i : favs)
	std::cout << "  + " << i.first << "=>" << i.second << std::endl;

CCollectionRequestSettings crs;
crs.setColId("2");
crs.setUserName("Gandalf");
session.updatePageCount(crs);

// http://alpha.wallhaven.cc/user/mattilius258/uploads
	
*/
	CRandomRequestSettings rrs;
	//rrs.set("bear \"blue flower\"");

	std::list<std::string> ids;

	while (canContinue())
	{
		if (ids.size() < 24) {
			
			//NSLog(@"%s - (%zu) load new Ids ... ", __PRETTY_FUNCTION__, ids.size());
			const std::list<std::string> randoms = session.getRandomIds( rrs );
			merge( ids, randoms);
			//NSLog(@"%s - got (%zu) Ids ... ", __PRETTY_FUNCTION__, ids.size());
		}
		
		_mutex.lock();
		const size_t fullUrlCount = _fullUrls.size();
		_mutex.unlock();
		
		if ((!ids.empty()) && (fullUrlCount < 3)) {
			
			std::string i = ids.front();
			ids.pop_front();
			
			//NSLog(@"%s - looking for full url of (%s)", __PRETTY_FUNCTION__, i.c_str());
			const std::string fullUrl = session.getFullImageUrl(i);
			if (!fullUrl.empty()) {
				_mutex.lock();
				_fullUrls.push_back(fullUrl);
				shuffle( _fullUrls );
				//NSLog(@"%s - full url (%s) found (%zu urls)", __PRETTY_FUNCTION__, i.c_str(), _fullUrls.size());
				_mutex.unlock();
			}
			
		}
		ms_sleep(500);
	}
	//NSLog(@"%s - DONE", __PRETTY_FUNCTION__);
}


