//
//  XDebugClientDelegate.m
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/29/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#import "XDebugClientDelegate.h"
#import "XDebugClientBridge.hpp"
#import "DebuggerActions.h"
#import "../XDebugClient/Model/Variable.hpp"
#import "../XDebugClient/Model/Stack.hpp"
#import "../XDebugClient/Model/StackLineItem.hpp"
#import "../XDebugClient/StateSharedPointerBridge.hpp"
//#import "XDebugClientApplicationSwift-Swift.h"
#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (nonatomic, strong) NSWindow * __null_unspecified window;
@property (nonatomic, strong) NSWindowController * __null_unspecified windowController;
- (void)applicationDidFinishLaunching:(NSNotification * __nonnull)aNotification;
- (void)applicationWillTerminate:(NSNotification * __nonnull)aNotification;
- (nonnull instancetype)init;
@end

@interface XDebugClientDelegate ()

- (void) newSessionWasRecieved: (int)session_id;
- (void) serverException:(NSString*)string;
- (void) serverExited: (NSString*)e_what;
@end

void requireDisplayUpdate()
{
    [[XDebugClientDelegate getInstance] requireDisplayUpdate];
    //void *objectiveCObject;XDebugClientDelegate* a =(__bridge XDebugClientDelegate*)objectiveCObject;[a updateDisplay];
}

void newSessionNativeCallback(int session_id)
{
    [[XDebugClientDelegate getInstance] newSessionWasRecieved:session_id];
}

void serverException(const char text)
{
    //NSString* textObj = [NSString stringWithCString:&text encoding:NSUTF8StringEncoding];
    //[[XDebugClientDelegate getInstance] serverException:textObj];
}

void serverExited(std::string* e_what)
{
    NSString* e_what_str = nil;
    if (e_what)
    {
        e_what_str = [[NSString alloc] initWithCString:e_what->c_str() encoding:NSUTF8StringEncoding];
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [[XDebugClientDelegate getInstance] serverExited:e_what_str];
    });
}

@interface XDebugClientDelegate ()
@property (atomic) BOOL needs_update;
@property (nonatomic,retain) NSTimer* timer;
@property (atomic) BOOL serverIsRunning;
@end

@implementation XDebugClientDelegate


+ (XDebugClientDelegate*) getInstance
{
    static XDebugClientDelegate *instance;
    @synchronized(self) {
        if(!instance) {
            instance = [[XDebugClientDelegate alloc] init];
        }
    }
    
    return instance;
}

- (id) init
{
    self = [super init];
    self.timer = [NSTimer scheduledTimerWithTimeInterval:0.5f target:self selector:@selector(updateDisplay) userInfo:nil repeats:YES];
    //[self.timer s]
    //[self.timer fire];
    return self;
}

- (void) requireDisplayUpdate
{
    self.needs_update = YES;
}

- (void) updateDisplay
{
    if (self.needs_update)
    {
        self.needs_update = NO;
        NSLog(@"Update Display called in Objective-C Class");
        [[NSNotificationCenter defaultCenter] postNotificationName:@"update_display" object:self];
        //char a[] = "aaa";
        //NSString* b = [[NSString alloc] initWithBytes:a length:3 encoding:NSUTF8StringEncoding];
        //NSLog(@"%@",b);
    }
}

- (XDebugClientBridge*) getCPPObj
{
    return getXDebugClientInstance();
}

- (void) doSomething
{
    getXDebugClientInstance()->doSomething();
}

- (void) newSessionWasRecieved: (int)session_id
{
    
}


- (void) serverException:(NSString*)string
{
    self.serverIsRunning = NO;
}

- (void) serverExited: (NSString*)e_what
{
    self.serverIsRunning = NO;
    if (e_what)
    {
        AppDelegate* delegate = (AppDelegate*)[NSApplication sharedApplication].delegate;
        NSWindow* window = delegate.window;
        NSString* message = [NSString stringWithFormat:@"XDebug server/listener exited with error: \"%@\". Press OK to restart the server.",e_what];
        
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        [alert addButtonWithTitle:@"Cancel"];
        [alert setMessageText:@"Server listener error"];
        [alert setInformativeText:message];
        [alert setAlertStyle:NSWarningAlertStyle];
        [alert beginSheetModalForWindow:window modalDelegate:self didEndSelector:@selector(serverErrorAlertDidEnd:returnCode:contextInfo:) contextInfo:nil];
     }
}

- (void)serverErrorAlertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if (returnCode == NSAlertFirstButtonReturn)
    {
        NSLog(@"Agreed to restart server");
        [self startServer];
    }
    else
    {
        NSLog(@"Declined to restart server");
    }
}

- (void) startServer
{
    if (self.serverIsRunning)
    {
        return;
    }
    [NSThread detachNewThreadSelector:@selector(startServerThread) toTarget:self withObject:nil];
    //[self stopServerAfterDelay];
}

- (void) stopServerAfterDelay
{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 1 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self stopServer];
    });
}

- (void) startServerThread
{
    getXDebugClientInstance()->runServer();
}

- (void) stopServer
{
    getXDebugClientInstance()->stopServer();
}

- (void) restartServer
{
    getXDebugClientInstance()->restartServer();
}

- (bool) sendMessageToSession: (int) session_id message:(DebuggerActions)action withInt: (int)number orWithString: (NSString*)string
{
    bool session_exists = false;
    XDebugClientBridge* bridge = getXDebugClientInstance();
    if (string)
    {
        std::string cppstring = std::string([string UTF8String]);
        session_exists = bridge->acceptMessageWithString(session_id,action,number,cppstring);
    } else {
        session_exists = bridge->acceptMessage(session_id,action,number);
    }
    return session_exists;
}

- (void*) getCurrentBreakpointStateSharedPointer:(int)history_index
{
    void* state = (void*)getXDebugClientInstance()->getBreakpointStateWithLock(111,history_index);
    return state;
}

- (BOOL) compareBreakpointSharedPointer: (void*)ptr1 toSharedPointer: (void*)ptr2
{
    return getXDebugClientInstance()->compareBreakpointSharedPointers(ptr1, ptr2);
}

- (void*) getVariablesPointer: (void*)shared_pointer withStackLevel: (int) stack_level
{
    if (!shared_pointer)
    {
        return 0;
    }
    return StateSharedPointerBridge(shared_pointer).getVariablesPointer(stack_level);
}

- (void*) getStackPointer: (void*)shared_pointer
{
    if (!shared_pointer)
    {
        return 0;
    }
    return StateSharedPointerBridge(shared_pointer).getStackPointer();
}

- (NSString*) getSourceCodeFilename: (void*)shared_pointer withStackLevel: (int) stack_level
{
    if (!shared_pointer)
    {
        return 0;
    }
    
    std::string filename = StateSharedPointerBridge(shared_pointer).getSourceCodeFilename(stack_level);
    NSString* result = [NSString stringWithCString:filename.c_str() encoding:NSUTF8StringEncoding];
    return result;
}

- (NSString*) getSourceCode: (void*)shared_pointer withFilename: (NSString*)filenameObj
{
    if (!shared_pointer)
    {
        return 0;
    }
    const char * filename_char = [filenameObj cStringUsingEncoding:NSUTF8StringEncoding];
    const std::string filename = std::string(filename_char);
    std::string string = StateSharedPointerBridge(shared_pointer).getSourceCode(filename);
    NSString* result = [NSString stringWithCString:string.c_str() encoding:NSUTF8StringEncoding];
    return result;
}

- (int) getLineNumber: (void*)shared_pointer withStackLevel: (int) stack_level
{
    if (!shared_pointer)
    {
        return 0;
    }
    return StateSharedPointerBridge(shared_pointer).getLineNumber(stack_level);
}

- (NSString*) getOutputStream: (void*)shared_pointer ofType:(int)type withStackLevel: (int) stack_level
{
    if (!shared_pointer)
    {
        return 0;
    }
    std::string string = StateSharedPointerBridge(shared_pointer).getOutputStream(type);
    NSString* result = [NSString stringWithCString:string.c_str() encoding:NSUTF8StringEncoding];
    return result;
}


- (void) deleteBreakpointSharedPointer: (void*)ptr
{
    getXDebugClientInstance()->removeSessionLock(ptr);
}

- (void) actionContinue
{
    [self sendMessageToSession:111 message:debugger_action_run withInt:0 orWithString:nil];
}

- (void) actionStepIn
{
    [self sendMessageToSession:111 message:debugger_action_step_in withInt:0 orWithString:nil];
}

- (void) actionStepOut
{
    [self sendMessageToSession:111 message:debugger_action_step_out withInt:0 orWithString:nil];
}

- (void) actionStepOver
{
    [self sendMessageToSession:111 message:debugger_action_step_over withInt:0 orWithString:nil];
}

- (void) actionStop
{
    [self sendMessageToSession:111 message:debugger_action_stop withInt:0 orWithString:nil];
}


- (NSArray<StackLine*>*) getCurrentStackArray: (void*)stack_ptr
{
    Stack* stack = (Stack*)stack_ptr;
    int count = stack->getStackSize();
    NSMutableArray* result = [[NSMutableArray alloc] initWithCapacity:count];
    for (int i = 0;i<count;i++)
    {
        const StackLineItem* item_i = stack->getItemAtIndex(i);
        StackLine* obj_i = [[StackLine alloc] init];
        obj_i.filename = [NSString stringWithCString:item_i->filename.c_str() encoding:NSUTF8StringEncoding];
        obj_i.filename_display = [obj_i.filename stringByReplacingOccurrencesOfString:@"file://" withString:@""];
        obj_i.where = [NSString stringWithCString:item_i->where.c_str() encoding:NSUTF8StringEncoding];
        obj_i.line_number = item_i->line_number;
        obj_i.level = item_i->level;
        obj_i.line_number_display = [NSString stringWithFormat:@"%d",obj_i.line_number];
        [result addObject:obj_i];
    }
    return result;
}

@end
