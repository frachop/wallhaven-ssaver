//
//  ConfigurationSheet.m
//  FraSaver
//
//  Created by franck on 17/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import "ConfigurationController.h"
#import "Configuration.h"


static NSString* const kConfigNibName = @"Configuration";

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface ConfigurationController()

@property(nonatomic, strong) Configuration *config;

@end

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation ConfigurationController

- (instancetype)initWithUserDefaults:(NSUserDefaults *)userDefaults
{
	self = [super init];
	if (self) {
		_config = [[Configuration alloc] initWithUserDefaults:userDefaults];
	}
	return self;
}


- (void)synchronize
{
	[_config synchronize];
}

- (IBAction)login:(id)sender {
}


- (IBAction)dismissConfigSheet:(id)sender {
	[self synchronize];
	[_delegate configController:self dismissConfigSheet:_sheet];
}

- (NSWindow *)configureSheet
{
	if (!_sheet)
	{
		NSBundle *thisBundle = [NSBundle bundleForClass:[self class]];
		if (![thisBundle loadNibNamed:kConfigNibName owner:self topLevelObjects:nil])
		{
			NSLog(@"Unable to load configuration sheet");
		}
	}
	
	return self.sheet;
}

@end



