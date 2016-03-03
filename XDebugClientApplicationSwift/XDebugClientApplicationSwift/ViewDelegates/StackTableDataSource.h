//
//  StackTableDataSource.h
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/11/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

@class CustomTextView;
@class VariablesOutlineDataSourceV2;

@interface StackTableDataSource : NSObject <NSTableViewDataSource,NSTableViewDelegate>

@property (weak) NSTableView* tableView;
@property (weak) NSTableColumn* column1;
@property (weak) NSTableColumn* column2;
@property (weak) NSTableColumn* column3;
@property (weak) NSScrollView* scrollview;

@property (strong) CustomTextView* sourceCodeTextView;
@property (weak) VariablesOutlineDataSourceV2* variablesDelegate;

@end
