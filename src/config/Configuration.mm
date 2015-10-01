//
//  Configuration.m
//  FraSaver
//
//  Created by franck on 18/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#import "Configuration.h"


@interface Configuration ()
@property(nonatomic, strong) NSUserDefaults *userDefaults;
@property(nonatomic, strong) NSMutableArray *tags;
@end


@implementation Configuration

- (instancetype)initWithUserDefaults:(NSUserDefaults *)userDefaults
{
	self = [super init];
	if (self) {
		_userDefaults = userDefaults;
		NSDictionary * d = [_userDefaults valueForKey:@"randomRequest"];
		
		for (NSString* key in d) {
		
			if ([key compare:@"categories"] == 0) {
			
				NSString* c = [d objectForKey:key];
				_randomRequest.categories() = wallhaven::CCategories( [c UTF8String ] );
			
			} else if ([key compare:@"purities"] == 0) {
			
				NSString* c = [d objectForKey:key];
				_randomRequest.purities() = wallhaven::CPurities( [c UTF8String ] );
				
			} else if ([key compare:@"search"] == 0) {
			
				NSString* c = [d objectForKey:key];
				_randomRequest.set( [c UTF8String ] );
				
			}
		
		}
		
		if (_randomRequest.categories().none())
			_randomRequest.categories().set();
		
		if (_randomRequest.purities().none())
			_randomRequest.set(wallhaven::EPurity::SFW);
		
		_purityNSFW_Enabled = false;
		
		NSLog(@"%@", d);

	if (!_tags)
			_tags = [NSMutableArray array];
		/*
		[_tags addObject:@"architecture"];
		[_tags addObject:@"cars"];
		[_tags addObject:@"anime girls"];
		[_tags addObject:@""];
		*/
	}
	
	return self;
}

- (void)synchronize
{
	NSMutableDictionary * d = [[NSMutableDictionary alloc] init];
	[d setValue:[NSString stringWithUTF8String:_randomRequest.categories().to_string().c_str()] forKey:@"categories"];
	[d setValue:[NSString stringWithUTF8String:_randomRequest.purities().to_string().c_str()] forKey:@"purities"];
	[d setValue:[NSString stringWithUTF8String:_randomRequest.search().c_str()] forKey:@"search"];
	[_userDefaults setValue:d forKey:@"randomRequest"];
	//NSLog(@"%s - %@", __PRETTY_FUNCTION__, d);
	
	[_userDefaults synchronize];
}

- (BOOL) getCategoryGeneral { return _randomRequest.get(wallhaven::ECategory::General); }
- (BOOL) getCategoryAnime   { return _randomRequest.get(wallhaven::ECategory::Anime  ); }
- (BOOL) getCategoryPeople  { return _randomRequest.get(wallhaven::ECategory::People ); }
- (void) setCategoryGeneral:(BOOL)v { _randomRequest.set(wallhaven::ECategory::General, v); }
- (void) setCategoryAnime  :(BOOL)v { _randomRequest.set(wallhaven::ECategory::Anime  , v); }
- (void) setCategoryPeople :(BOOL)v { _randomRequest.set(wallhaven::ECategory::People , v); }

- (BOOL) getPuritySFW     { return _randomRequest.get(wallhaven::EPurity::SFW    ); }
- (BOOL) getPuritySketchy { return _randomRequest.get(wallhaven::EPurity::Sketchy); }
- (BOOL) getPurityNSFW    { return _randomRequest.get(wallhaven::EPurity::NSFW   ); }
- (void) setPuritySFW    :(BOOL)v { _randomRequest.set(wallhaven::EPurity::SFW    , v); }
- (void) setPuritySketchy:(BOOL)v { _randomRequest.set(wallhaven::EPurity::Sketchy, v); }
- (void) setPurityNSFW   :(BOOL)v { _randomRequest.set(wallhaven::EPurity::NSFW   , v); }


@end
