//
//  VariablesOutlineDataSource.swift
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/6/16.
//  Copyright © 2016 elijah. All rights reserved.
//

import Cocoa

class VariablesOutlineDataSource : NSObject,NSOutlineViewDataSource
{
    var a = "Name"
    var b = "Value"
    
    var column1 : NSTableColumn
    var column2 : NSTableColumn
    
    init(column1:NSTableColumn,column2:NSTableColumn) {
        self.column1 = column1
        self.column2 = column2
    }
    
    func outlineView(outlineView: NSOutlineView, numberOfChildrenOfItem item: AnyObject?) -> Int
    {
        if (item == nil)
        {
            return 1
        }
        
        let item_string = item as! String
        if(item_string == a)
        {
            return 4
        }
        return 0
    }
    
    func outlineView(outlineView: NSOutlineView, child index: Int, ofItem item: AnyObject?) -> AnyObject
    {
        if (item == nil)
        {
            return a
        }
        let item_string = item as! String
        if(item_string == a)
        {
            return b
        }
        return a
    }
    
    func outlineView(outlineView: NSOutlineView, isItemExpandable item: AnyObject) -> Bool
    {
        return true;
    }
    
    func outlineView(outlineView: NSOutlineView, objectValueForTableColumn tableColumn: NSTableColumn?, byItem item: AnyObject?) -> AnyObject?
    {
        if (tableColumn == column1)
        {
            return a
        }
        return b
    }
}
