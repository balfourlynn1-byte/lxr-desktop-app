#import <Cocoa/Cocoa.h>

@interface Bench : NSObject <NSApplicationDelegate>
@property(strong) NSWindow *window;
@property(strong) NSTextField *status;
@property(strong) NSArray *groups;
@property(strong) NSString *results;
@property(strong) NSString *resources;
@property BOOL busy;
@end
@implementation Bench
- (void)applicationDidFinishLaunching:(NSNotification *)note {
    self.resources = NSBundle.mainBundle.resourcePath;
    self.results = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Application Support/JIMBONES/Results"];
    self.groups = @[@"all",@"noclip",@"stepfx",@"layers",@"cpu",@"crunch"];
    NSArray *titles = @[@"Run All Tests",@"Test No Clip",@"Test Step FX",@"Test Metallic/Wavetable",
                        @"Test CPU Diagnostics",@"Test Multi-Voice Crunch",@"Play Baseline Audio",
                        @"Play Experimental Audio",@"View Results"];
    self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0,0,580,334)
        styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable
        backing:NSBackingStoreBuffered defer:NO];
    self.window.title = @"JIMBONES · Intel Mac Test Bench";
    for (NSInteger i=0;i<(NSInteger)titles.count;i++) {
        NSButton *button = [NSButton buttonWithTitle:titles[i] target:self action:@selector(performTest:)];
        button.tag=i;
        button.frame=NSMakeRect(18+(i%2)*277,275-(i/2)*47,268,38);
        [self.window.contentView addSubview:button];
    }
    self.status=[NSTextField wrappingLabelWithString:@"Ready. Play a comparison or run a fresh production DSP test."];
    self.status.frame=NSMakeRect(23,13,540,60);
    [self.window.contentView addSubview:self.status];
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {return YES;}
- (void)performTest:(NSButton *)sender {
    NSInteger index=sender.tag;
    if(index==6 || index==7) {
        NSString *clip=index==6?@"NoClip.wav":@"Layers-plus-StepFX.wav";
        [[NSWorkspace sharedWorkspace] openURL:[NSURL fileURLWithPath:[self.resources stringByAppendingPathComponent:clip]]];
        return;
    }
    NSError *error=nil;
    if(![[NSFileManager defaultManager] createDirectoryAtPath:self.results withIntermediateDirectories:YES attributes:nil error:&error]) {
        self.status.stringValue=error.localizedDescription; return;
    }
    if(index==8) {
        [[NSWorkspace sharedWorkspace] openURL:[NSURL fileURLWithPath:self.results]];
        return;
    }
    if(self.busy) return;
    self.busy=YES;
    self.status.stringValue=[NSString stringWithFormat:@"Running %@…",sender.title];
    NSString *group=self.groups[index];
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED,0), ^{
        NSTask *task=[NSTask new];
        task.executableURL=[NSURL fileURLWithPath:@"/usr/bin/python3"];
        task.arguments=@[[self.resources stringByAppendingPathComponent:@"bench.py"],group,self.results];
        NSString *log=[self.results stringByAppendingPathComponent:[NSUUID.UUID.UUIDString stringByAppendingString:@".log"]];
        [[NSFileManager defaultManager] createFileAtPath:log contents:nil attributes:nil];
        NSFileHandle *handle=[NSFileHandle fileHandleForWritingAtPath:log];
        task.standardOutput=handle; task.standardError=handle;
        NSError *failure=nil;
        BOOL ran=[task launchAndReturnError:&failure];
        if(ran) [task waitUntilExit];
        [handle closeFile];
        NSString *message=!ran?failure.localizedDescription:task.terminationStatus==0?
          @"PASS. Open View Results for measurements and WAVs.":
          @"FAIL. Open View Results for the log. Python 3 from Apple Command Line Tools is required.";
        dispatch_async(dispatch_get_main_queue(), ^{self.busy=NO; self.status.stringValue=message;});
    });
}
@end
int main(void) {
    @autoreleasepool {
        NSApplication *app=[NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        Bench *delegate=[Bench new]; app.delegate=delegate;
        [app run];
        (void)delegate.window;
    }
    return 0;
}
