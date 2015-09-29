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

@property (nonatomic) BOOL categoryGeneral;
@property (nonatomic) BOOL categoryAnime;
@property (nonatomic) BOOL categoryPeople;

@property (nonatomic) BOOL puritySFW;
@property (nonatomic) BOOL puritySketchy;
@property (nonatomic) BOOL purityNSFW;

@property (nonatomic) BOOL purityNSFW_Enabled;

@property(nonatomic, strong, readonly) NSMutableArray* tags;
@property(nonatomic, strong, readonly) NSString* userName;

- (instancetype)initWithUserDefaults:(NSUserDefaults *)userDefaults;
- (void)synchronize;

@end
