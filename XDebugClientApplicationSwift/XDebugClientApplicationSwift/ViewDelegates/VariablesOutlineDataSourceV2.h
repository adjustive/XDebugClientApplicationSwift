//
//  VariablesOutlineDataSourceV2.h
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/6/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef VariablesOutlineDataSourceV2_h
#define VariablesOutlineDataSourceV2_h

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

@class ScrollViewNoAutoScroll;
@interface VariablesOutlineDataSourceV2 : NSObject<NSOutlineViewDataSource,NSOutlineViewDelegate>
- (void) doReload;
@property (weak) NSOutlineView* outlineView;
@property (weak) NSTableColumn* columnName;
@property (weak) NSTableColumn* columnType;
@property (weak) NSTableColumn* columnValue;
@property (weak) ScrollViewNoAutoScroll* scrollview;
@end


#endif /* VariablesOutlineDataSourceV2_h */
