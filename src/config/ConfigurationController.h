//
//  ConfigurationSheet.h
//  FraSaver
//
//  Created by franck on 17/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol ConfigurationControllerDelegate;

@interface ConfigurationController : NSObject

@property(nonatomic, strong) id<ConfigurationControllerDelegate> delegate;

@property(nonatomic, strong) IBOutlet NSPanel *sheet;

- (instancetype)initWithUserDefaults:(NSUserDefaults *)userDefaults;
- (void)synchronize;
- (NSWindow *)configureSheet;


- (IBAction)dismissConfigSheet:(id)sender;

@end


@protocol ConfigurationControllerDelegate <NSObject>

- (void)configController:(ConfigurationController *)configController dismissConfigSheet:(NSWindow *)sheet;

@end


