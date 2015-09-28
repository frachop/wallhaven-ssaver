//
//  imageProvider.cpp
//  FraSaver
//
//  Created by franck on 28/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#include "../include/wallhaven.h"


namespace wallhaven {
	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class CImageProvider::CImpl
	:	public CThread
	{
	public:
		CImpl();
		
	public:
		void start( const CRandomRequestSettings & rndAccess );
		void stop();
		CImageData* getNextImage();

	protected:
		virtual void run() override;
		
	private:
		std::mutex             _mutex;
		std::list<CImageData*> _images;
	};

	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

}

