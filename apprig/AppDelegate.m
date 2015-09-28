#import "AppDelegate.h"
#import "ConfigurationController.h"
#import "FraSaverView.h"

@interface AppDelegate () <ConfigurationControllerDelegate>

@property (weak) IBOutlet NSWindow *window;
@property(strong) ConfigurationController *configController;
@end

@implementation AppDelegate {

	FraSaverView *fsv;

}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	_configController = [[ConfigurationController alloc] initWithUserDefaults:userDefaults];
	_configController.delegate = self;
	

	NSRect bounds = [_window.contentView bounds];
	fsv = [[FraSaverView alloc] initWithFrame:bounds isPreview:NO];
	fsv.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
	_window.contentView = fsv;
	[fsv startAnimation];
	[self tick:self];


	[self.window makeKeyWindow];
	//[self performSelector:@selector(showPreferences:) withObject:nil afterDelay:0];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*) sender {
	return YES;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
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




- (void)configController:(ConfigurationController *)configController dismissConfigSheet:(NSWindow *)sheet {
	//[self reloadFraView];
	[sheet close];
}

- (IBAction)showPreferences:(id)sender {
	NSWindow *window = [self.configController configureSheet];
	[self.window beginSheet:window completionHandler:nil];
}

- (IBAction)reloadFraView {
	
	// Remove the older webview
	if ([_window.contentView subviews]) {
		fsv = (FraSaverView *)[[self.window.contentView subviews] firstObject];
		[fsv stopAnimation];
		[fsv removeFromSuperview];
		fsv = nil;
	}
	
	// Recreate the subview.
	//NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	NSRect bounds = [self.window.contentView bounds];
	fsv = [[FraSaverView alloc] initWithFrame:bounds isPreview:NO];
	fsv.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
	[self.window.contentView addSubview:fsv];
	[fsv startAnimation];
	[self tick:self];
}

@end