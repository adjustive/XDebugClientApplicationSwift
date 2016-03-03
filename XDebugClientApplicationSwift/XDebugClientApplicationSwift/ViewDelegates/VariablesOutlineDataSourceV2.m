//
//  VariablesOutlineDataSourceV2.m
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/6/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#import "XDebugClientApplicationSwift-Swift.h"
#import "VariablesOutlineDataSourceV2.h"
#import "VariableCache.h"
#import "XDebugClientDelegate.h"
#import "XDebugClientDelegate.h"

@implementation VariablesOutlineDataSourceV2
{
    NSString* a;
    NSString* b;
    NSString* c;
    VariableCache* dataTree;
    void* shared_ptr;
    void* variables_pointer;
    NSLock* shared_ptr_lock;
    NSLock* scroll_lock;
}

- (id) init
{
    self = [super init];
    a = @"Test 1";
    b = @"Test 2";
    c = @"Test 2";
    dataTree = nil;
    shared_ptr = 0;
    shared_ptr_lock = [[NSLock alloc] init];
    scroll_lock = [[NSLock alloc] init];
    variables_pointer = 0;
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(updateDisplayNofitication)
                                                 name:@"update_display"
                                               object:nil];
    return self;
}

- (void) dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void) updateDisplayNofitication
{
    [self doReload];
}

- (void) doReload
{
    [scroll_lock lock];
    self.scrollview.disableAutoScroll = true;
    [shared_ptr_lock lock];
    void* new_ptr = [[XDebugClientDelegate getInstance] getCurrentBreakpointStateSharedPointer:-1];
    // new design will check the variabled object returned from the shared pointer
    // the above code will be removed and the old shared pointer will always be deleted in the dispatch_after block
    // the variable will/will not be released and the data reloaded based on the variables pointer
    // but the dispatch_after will run anyway and delete the old pointer
    // this design will be copied to StackTableDataSource and to future source code and output stream delegates
    // for variables, there will also need to be checked if the variables object has been set in its new location
    // if it has not been set, then reload and update nothing, and delete the old shared pointer
    
    void* new_variables_ptr = [[XDebugClientDelegate getInstance] getVariablesPointer:new_ptr withStackLevel:0];
    __block void* old_ptr = shared_ptr;
    __block VariableCache* oldVariableCache = dataTree;
    shared_ptr = new_ptr;
    if (new_variables_ptr != variables_pointer)
    {
        variables_pointer = new_variables_ptr;
        if (variables_pointer)
        {
            dataTree = [[VariableCache alloc] initRootWithVariablesPointer:variables_pointer]; // does not keep a shared pointer
        } else {
            dataTree = [[VariableCache alloc] init];
        }
        // do update here
        // reload table
        [self.outlineView reloadData];
    }
    NSMutableArray* expansion_states = [[NSMutableArray alloc] initWithCapacity:10];
    [dataTree addToExpansionArray:expansion_states];
    //NSArray<StackLine*>* a = [[XDebugClientDelegate getInstance] getCurrentStack];
    [shared_ptr_lock unlock];
    
    
    // clear old variables here
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [oldVariableCache class]; // does nothing, just retains the variable for 1 second
        [[XDebugClientDelegate getInstance] deleteBreakpointSharedPointer:old_ptr];
    });
    // run outsize the lock as shouldExpandItem uses a lock on the shared pointer
    // alternative would be to get a second shared pointer
    for (VariableCache* item in expansion_states)
    {
        [self.outlineView expandItem:item];
    }
    self.scrollview.disableAutoScroll = false;
    // outlineView expandItem method adjusts the scroll in an unwanted way
    // therefore it is locked with disableAutoScroll and with a mutex on that
    [scroll_lock unlock];
    
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(nullable id)item
{
    VariableCache* v = item;
    if (!item)
    {
        v = dataTree;
    }
    return [v getNumberOfChildren];
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(nullable id)item
{
    VariableCache* v = item;
    if (!item)
    {
        v = dataTree;
    }
    return [v getChildAtIndex:(int)index];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
    VariableCache* v = item;
    if (!item)
    {
        v = dataTree;
    }
    int count = [v getNumberOfChildren];
    return count > 0;
}

- (id) outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
    VariableCache* v = item;
    if (!item)
    {
        v = dataTree;
    }
    if (tableColumn == _columnName)
    {
        return [v getName];
    } else if (tableColumn == _columnType)
    {
        return [v getType];
    } else if (tableColumn == _columnValue)
    {
        return [v getValueDisplay];
    }
    NSLog(@"Error 127 new table column");
    return nil;
    
}

- (void) outlineView:(NSOutlineView *)outlineView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
    
}




/* DELEGATE */
/*
- (BOOL) outlineView:(NSOutlineView *)outlineView shouldShowCellExpansionForTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
    return true;
}

- (void) outlineView:(NSOutlineView *)outlineView willDisplayOutlineCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
    //NSLog(@"outline cell");
}

- (void) outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item

{
    //NSLog(@"regular cell");
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldShowOutlineCellForItem:(id)item {
    return YES;
}

- (BOOL) outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
    return NO;
}
 */

/* USED DELEGATE METHODS */
- (BOOL)outlineView:(NSOutlineView *)outlineView shouldExpandItem:(id)item
{
    [shared_ptr_lock lock];
    if (item)
    {
        VariableCache* variable = item;
        if ([variable isKindOfClass:[VariableCache class]])
        {
            [item updateExpandedState:true withSharedPointer:shared_ptr];
        }
    }
    [shared_ptr_lock unlock];
    return true;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldCollapseItem:(id)item
{
    [shared_ptr_lock lock];
    if (item)
    {
        VariableCache* variable = item;
        if ([variable isKindOfClass:[VariableCache class]])
        {
            [item updateExpandedState:false withSharedPointer:shared_ptr];
        }
    }
    [shared_ptr_lock unlock];
    return true;
}

@end