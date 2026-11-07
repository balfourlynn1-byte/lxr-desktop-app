#import <Cocoa/Cocoa.h>

@interface Bench : NSObject <NSApplicationDelegate>
@property(strong) NSWindow *window;
@property(strong) NSTextField *status;
@property(strong) NSArray *groups;
@property(strong) NSString *results;
@property(strong) NSString *resources;
@property BOOL busy;
@property(strong) NSMutableArray<NSSlider *> *sliders;
@property(strong) NSMutableArray<NSTextField *> *values;
@property(strong) NSDictionary *reference;
@property(strong) NSArray<NSNumber *> *indices;
@end
@implementation Bench
- (void)applicationDidFinishLaunching:(NSNotification *)note {
    self.resources = NSBundle.mainBundle.resourcePath;
    self.results = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Application Support/JIMBONES/Results"];
    self.groups = @[@"all",@"noclip",@"stepfx",@"layers",@"cpu",@"crunch"];
    NSArray *titles = @[@"Run All Tests",@"Test No Clip",@"Test Step FX",@"Test Metallic/Wavetable",
                        @"Test CPU Diagnostics",@"Test Multi-Voice Crunch",@"Play Baseline Audio",
                        @"Play Experimental Audio",@"View Results"];
    self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0,0,580,630)
        styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable
        backing:NSBackingStoreBuffered defer:NO];
    self.window.title = @"JIMBONES · Intel Mac Test Bench";
    for (NSInteger i=0;i<(NSInteger)titles.count;i++) {
        NSButton *button = [NSButton buttonWithTitle:titles[i] target:self action:@selector(performTest:)];
        button.tag=i;
        button.frame=NSMakeRect(18+(i%2)*277,575-(i/2)*42,268,38);
        [self.window.contentView addSubview:button];
    }
    NSData *data=[NSData dataWithContentsOfFile:[self.resources stringByAppendingPathComponent:@"aggressive-reference.json"]];
    self.reference=[NSJSONSerialization JSONObjectWithData:data options:0 error:NULL][@"controls"];
    self.indices=@[@1,@2,@24,@29,@20,@30,@28,@31];
    self.sliders=[NSMutableArray array]; self.values=[NSMutableArray array];
    NSArray *labels=@[@"Metallic level",@"Wavetable level",@"Wavetable position",@"WaveShape amount",
                      @"Drive",@"Step FX amount",@"Layer blend",@"Velocity to blend"];
    for(NSUInteger i=0;i<labels.count;i++) {
        CGFloat y=367-i*31;
        NSTextField *label=[NSTextField labelWithString:labels[i]];
        label.frame=NSMakeRect(27,y,150,23); [self.window.contentView addSubview:label];
        NSSlider *slider=[NSSlider sliderWithValue:0 minValue:0 maxValue:127 target:self action:@selector(slide:)];
        slider.frame=NSMakeRect(185,y,306,23); slider.tag=i;
        NSTextField *value=[NSTextField labelWithString:@""];
        value.frame=NSMakeRect(510,y,45,23);
        [self.sliders addObject:slider]; [self.values addObject:value];
        [self.window.contentView addSubview:slider]; [self.window.contentView addSubview:value];
    }
    NSButton *approved=[NSButton buttonWithTitle:@"Restore approved custom mix" target:self action:@selector(restore:)];
    approved.tag=1; approved.frame=NSMakeRect(296,407,268,38);
    [self.window.contentView addSubview:approved]; [self restore:nil];
    NSButton *restore=[NSButton buttonWithTitle:@"Restore aggressive preset" target:self action:@selector(restore:)];
    restore.frame=NSMakeRect(19,88,268,38); [self.window.contentView addSubview:restore];
    NSButton *custom=[NSButton buttonWithTitle:@"Render and play my mix" target:self action:@selector(performTest:)];
    custom.tag=9; custom.frame=NSMakeRect(296,88,268,38); [self.window.contentView addSubview:custom];
    self.status=[NSTextField wrappingLabelWithString:@"Ready. Play a comparison or run a fresh production DSP test."];
    self.status.frame=NSMakeRect(23,13,540,60);
    [self.window.contentView addSubview:self.status];
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}
- (void)slide:(NSSlider *)sender {
    self.values[sender.tag].stringValue=[NSString stringWithFormat:@"%ld",(long)sender.integerValue];
}
- (void)restore:(id)sender {
    NSDictionary *settings=self.reference;
    if([sender tag]==1) {
        NSData *data=[NSData dataWithContentsOfFile:[self.resources stringByAppendingPathComponent:@"custom-reference.json"]];
        settings=[NSJSONSerialization JSONObjectWithData:data options:0 error:NULL][@"controls"];
    }
    for(NSSlider *slider in self.sliders) {
        slider.integerValue=[settings[self.indices[slider.tag].stringValue] integerValue];
        [self slide:slider];
    }
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
    NSString *group=index==9?@"custom":self.groups[index];
    NSMutableArray *arguments=[@[[self.resources stringByAppendingPathComponent:@"bench.py"],group,self.results] mutableCopy];
    if(index==9) for(NSSlider *slider in self.sliders) {
        [arguments addObject:[NSString stringWithFormat:@"%@=%ld",self.indices[slider.tag],(long)slider.integerValue]];
    }
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED,0), ^{
        NSTask *task=[NSTask new];
        task.executableURL=[NSURL fileURLWithPath:@"/usr/bin/python3"];
        task.arguments=arguments;
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
        dispatch_async(dispatch_get_main_queue(), ^{
            self.busy=NO; self.status.stringValue=message;
            if(index==9 && ran && task.terminationStatus==0) {
                NSString *output=[NSString stringWithContentsOfFile:log encoding:NSUTF8StringEncoding error:NULL];
                NSString *path=[[output componentsSeparatedByString:@"Results saved in "].lastObject
                                stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceAndNewlineCharacterSet];
                [[NSWorkspace sharedWorkspace] openURL:[NSURL fileURLWithPath:[path stringByAppendingPathComponent:@"My-mix.wav"]]];
            }
        });
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
