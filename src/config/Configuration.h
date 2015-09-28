//
//  Configuration.h
//  FraSaver
//
//  Created by franck on 18/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "../wallhaven/include/wallhaven.h"

@interface Configuration : NSObject

@property wallhaven::CRandomRequestSettings randomRequest;

@property(nonatomic, strong, readonly) NSMutableArray* tags;
@property(nonatomic, strong, readonly) NSString* userName;

- (instancetype)initWithUserDefaults:(NSUserDefaults *)userDefaults;
- (void)synchronize;

@end
