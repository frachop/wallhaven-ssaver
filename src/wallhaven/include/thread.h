//
//  thread.h
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#pragma once
#include <thread>
#include <mutex>

namespace wallhaven
{

	class CThread
	{
	public:
		CThread();
		virtual ~CThread();
		
	public:
		void start();
		void abort();
		bool isAborted() { return !_canContinue; }
		bool canContinue()  { return _canContinue; }
		bool isRunning() { return _running; }
		
	protected:
		virtual void run() = 0;
		void ms_sleep( uint64_t ms);
		
	private:
		void entryPoint();
		
	private:
		std::atomic_bool _running;
		std::atomic_bool _canContinue;
		std::thread      _thread;
	};

}