//
//  StackTableDataSource.m
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/11/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#import "StackTableDataSource.h"
#import "StackLine.h"
#import "XDebugClientDelegate.h"
#import <Cocoa/Cocoa.h>
#import "CustomTextView.h"

@implementation StackTableDataSource
{
    NSArray<StackLine*>* stackData;
    void* shared_ptr;
    void* stack_pointer;
    NSLock* shared_ptr_lock;
    NSLock* scroll_lock;
    int stack_id;
    int history_id;
    NSLock* guiVarsLock;
    NSString* lastSourceCodeFilename;
    NSLock* sourceCodeLock;
    int last_line_number;
}

- (id) init
{
    self = [super init];
    stackData = nil;
    shared_ptr = 0;
    shared_ptr_lock = [[NSLock alloc] init];
    scroll_lock = [[NSLock alloc] init];
    guiVarsLock= [[NSLock alloc] init];
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
    [guiVarsLock lock];
    int history_level_i = history_id;
    [guiVarsLock unlock];
    [self reloadSourceCodeView];
    [scroll_lock lock];
    //self.scrollview.disableAutoScroll = true;
    [shared_ptr_lock lock];
    void* new_ptr = [[XDebugClientDelegate getInstance] getCurrentBreakpointStateSharedPointer:history_level_i];
    // new design will check the variabled object returned from the shared pointer
    // the above code will be removed and the old shared pointer will always be deleted in the dispatch_after block
    // the variable will/will not be released and the data reloaded based on the variables pointer
    // but the dispatch_after will run anyway and delete the old pointer
    // this design will be copied to StackTableDataSource and to future source code and output stream delegates
    // for variables, there will also need to be checked if the variables object has been set in its new location
    // if it has not been set, then reload and update nothing, and delete the old shared pointer
    
    void* new_stack_ptr = [[XDebugClientDelegate getInstance] getStackPointer:new_ptr];
    __block void* old_ptr = shared_ptr;
    __block NSArray<StackLine*>* oldStack = stackData;
    shared_ptr = new_ptr;
    if (new_stack_ptr != stack_pointer)
    {
        stack_pointer = new_stack_ptr;
        if (stack_pointer)
        {
            stackData = [[XDebugClientDelegate getInstance] getCurrentStackArray:stack_pointer];
        } else {
            stackData = @[];
        }
        // do update here
        [self.tableView reloadData];
    }
    [shared_ptr_lock unlock];
    
    
    // clear old variables here
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [oldStack class]; // does nothing, just retains the variable for 1 second
        [[XDebugClientDelegate getInstance] deleteBreakpointSharedPointer:old_ptr];
    });
    
    //self.scrollview.disableAutoScroll = false;
    [scroll_lock unlock];
    
}

- (void) reloadSourceCodeView
{
    [guiVarsLock lock];
    int stack_level_i = stack_id;
    int history_level_i = history_id;
    [guiVarsLock unlock];
    NSString* sourceCode = nil;
    void* new_ptr = [[XDebugClientDelegate getInstance] getCurrentBreakpointStateSharedPointer:history_level_i];
    NSString* new_filename = [[XDebugClientDelegate getInstance] getSourceCodeFilename:new_ptr withStackLevel:stack_level_i];
    int line_number = [[XDebugClientDelegate getInstance] getLineNumber:new_ptr withStackLevel:stack_level_i];;
    [sourceCodeLock lock];
    if (![new_filename isEqualToString:lastSourceCodeFilename])
    {
        NSString* newSourceCode = [[XDebugClientDelegate getInstance] getSourceCode:new_ptr withFilename:new_filename];
        if (![newSourceCode isEqualToString:@""])
        {
            lastSourceCodeFilename = new_filename;
            sourceCode = newSourceCode;
        } else {
            lastSourceCodeFilename = nil;
            line_number = -1;
            // optional
            // sourceCode = @"";
        }
    }
    //lastSourceCode = sourceCode;
    NSString* sourceCodeForLineNumbers = sourceCode; // reference stored here thread safe
    if (lastSourceCodeFilename && !sourceCode && last_line_number != line_number)
    {
        sourceCodeForLineNumbers = self.sourceCodeTextView.textStorage.string;
    }
    last_line_number = line_number;
    [[XDebugClientDelegate getInstance] deleteBreakpointSharedPointer:new_ptr];
    [sourceCodeLock unlock];
    /* UPDATE OUTSIDE OF THE LOCK */
    if (sourceCode)
    {
        [self.sourceCodeTextView disableHighlighting];
        NSAttributedString* attributedString = [[NSAttributedString alloc] initWithString:sourceCode];
        [self.sourceCodeTextView.textStorage setAttributedString:attributedString];
        //[self.sourceCodeTextView setNeedsDisplay:YES];
        //et attributedString = NSAttributedString.init(string: text)
        //textView.textStorage?.setAttributedString(attributedString);
    }
    if (sourceCodeForLineNumbers && line_number >= 0)
    {
        [self.sourceCodeTextView setLineNumber:line_number forText:sourceCodeForLineNumbers];
    }
}











- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    if (stackData)
    {
        return [stackData count];
    }
    return 0;
}

/* This method is required for the "Cell Based" TableView, and is optional for the "View Based" TableView. If implemented in the latter case, the value will be set to the view at a given row/column if the view responds to -setObjectValue: (such as NSControl and NSTableCellView).
 */
- (nullable id)tableView:(NSTableView *)tableView objectValueForTableColumn:(nullable NSTableColumn *)tableColumn row:(NSInteger)row_index
{
    if (stackData)
    {
        StackLine* row = [stackData objectAtIndex:row_index];
        if (tableColumn == self.column1)
        {
            return row.where;
        } else if (tableColumn == self.column2)
        {
            return row.filename_display;
        } else if (tableColumn == self.column3)
        {
            return row.line_number_display;
        }
    }
    return @"";
}

@end
