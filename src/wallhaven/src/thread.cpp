//
//  thread.cpp
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#include "../include/wallhaven.h"
#include <cassert>
#include <chrono>

namespace wallhaven
{
	
	CThread::CThread()
	:	_running(false)
	,	_canContinue(false)
	,	_thread(std::thread())
	{
	}
	
	
	CThread::~CThread()
	{
		abort();
	}
	
	void CThread::entryPoint()
	{
		_running = true;
		run();
		_running = false;
	}
	
	void CThread::start()
	{
		assert( !isRunning() );
		_canContinue = true;
		_running = false;
		_thread  = std::thread( &CThread::entryPoint, this );
	}
	
	void CThread::abort()
	{
		if ( _thread.joinable() ) {
			_canContinue = false;
			_thread.join();
			_thread = std::thread();
		}
	}
	
	void CThread::ms_sleep(uint64_t ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

}

