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
	//_randomRequest.set(wallhaven::EPurity::NSFW, true);
	NSMutableDictionary * d = [[NSMutableDictionary alloc] init];
	[d setValue:[NSString stringWithUTF8String:_randomRequest.categories().to_string().c_str()] forKey:@"categories"];
	[d setValue:[NSString stringWithUTF8String:_randomRequest.purities().to_string().c_str()] forKey:@"purities"];
	[_userDefaults setValue:d forKey:@"randomRequest"];
	[_userDefaults synchronize];
}



@end
