//
//  XDebugClientDelegate.h
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/29/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "DebuggerActions.h"
#include "StackLine.h"

@interface XDebugClientDelegate : NSObject
{
    
}

+ (XDebugClientDelegate*) getInstance;
- (void) requireDisplayUpdate;
- (void) doSomething;
- (void) startServer;
- (void) stopServer;
- (void) restartServer;
- (void*) getCurrentBreakpointStateSharedPointer:(int) history_index;
- (void) deleteBreakpointSharedPointer: (void*)ptr;
- (BOOL) compareBreakpointSharedPointer: (void*)ptr1 toSharedPointer: (void*)ptr2;
- (bool) sendMessageToSession: (int) session_id message:(DebuggerActions)action withInt: (int)number orWithString: (NSString*)string;
- (void) actionContinue;
- (void) actionStepIn;
- (void) actionStepOut;
- (void) actionStepOver;
- (void) actionStop;
- (NSArray<StackLine*>*) getCurrentStackArray: (void*)stack_ptr;



- (void*) getVariablesPointer: (void*)shared_pointer withStackLevel: (int) stack_level;

- (void*) getStackPointer: (void*)shared_pointer;

- (NSString*) getSourceCodeFilename: (void*)shared_pointer withStackLevel: (int) stack_level;

- (NSString*) getSourceCode: (void*)shared_pointer withFilename:(NSString*)filename;

- (int) getLineNumber: (void*)shared_pointer withStackLevel: (int) stack_level;

- (NSString*) getOutputStream: (void*)shared_pointer ofType:(int)type withStackLevel: (int) stack_level;


@end
