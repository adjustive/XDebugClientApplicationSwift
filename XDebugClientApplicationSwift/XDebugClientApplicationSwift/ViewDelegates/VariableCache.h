//
//  Variable.h
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/6/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifndef Variable_h
#define Variable_h

@interface VariableCache:NSObject

- (id) initWithCPointer:(void*)ptr;
- (id) initRootWithVariablesPointer:(void*)ptr;
- (NSString*) getName;
- (NSString*) getValueDisplay;
- (NSString*) getType;
- (NSString*) getFullValue;
- (int) getNumberOfChildren;
- (VariableCache*) getChildAtIndex: (int) index;
- (void) updateExpandedState: (BOOL)is_expanded withSharedPointer:(void*)shared_ptr;
- (void) addToExpansionArray: (NSMutableArray*)expansion_array;

@end

#endif /* Variable_h */
