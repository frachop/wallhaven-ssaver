//
//  parser.h
//  FraSaver
//
//  Created by franck on 15/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#pragma once
#import "common.h"

class CIds
:	private wallhaven::CThread
{
public:
	CIds();
	~CIds();

	void start();
	void stop () { CThread::abort(); }
	std::string getNextUrl();

protected:
	virtual void run() override;
	
private:
	std::mutex               _mutex;
	std::string              _searchString;
	std::list<std::string  > _fullUrls;
};
