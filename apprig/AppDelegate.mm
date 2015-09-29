#import "AppDelegate.h"
#import "ConfigurationController.h"
#import "WallhavenSSaverView.h"

@interface AppDelegate () <ConfigurationControllerDelegate>

@property (weak) IBOutlet NSWindow *window;
@property(strong) ConfigurationController *configController;
@end

@implementation AppDelegate {

	WallhavenSSaverView *fsv;

}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
//	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
//	_configController = [[ConfigurationController alloc] initWithUserDefaults:userDefaults];
//	_configController.delegate = self;
	

	NSRect bounds = [_window.contentView bounds];
	fsv = [[WallhavenSSaverView alloc] initWithFrame:bounds isPreview:NO];
	fsv.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
	_window.contentView = fsv;
	[fsv startAnimation];
	[self tick:self];

	[self.window makeKeyWindow];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*) sender {
	return YES;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

-(void)tick:(id)sender
{
	//NSLog(@"%s", __PRETTY_FUNCTION__);
	[fsv animateOneFrame];
	[fsv setNeedsDisplay:YES];

	const NSTimeInterval ti = fsv.animationTimeInterval;
	[NSTimer scheduledTimerWithTimeInterval:ti
									 target:self
								   selector:@selector(tick:)
								   userInfo:nil
									repeats:NO];

}

- (IBAction)showPreferences:(id)sender {

	NSWindow *window = [fsv configureSheetFromRig];
	//NSWindow *window = [self.configController configureSheet];
	[self.window beginSheet:window completionHandler:nil];
}

@end