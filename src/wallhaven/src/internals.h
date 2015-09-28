//
//  internals.h
//  FraSaver
//
//  Created by franck on 28/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#pragma once
#include "../include/wallhaven.h"

namespace wallhaven {

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

	template<class T>
	std::list<T> shuffle_copy(const std::list<T> & _l)
	{
		std::list<T> l(_l);
		shuffle(l);
		return l;
	}

}