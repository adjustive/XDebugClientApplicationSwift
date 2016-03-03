//
//  MainWindowController.swift
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/30/15.
//  Copyright © 2015 elijah. All rights reserved.
//

import Cocoa

class MainWindowController: NSWindowController,NSWindowDelegate {
    let leftTabView : NSTabView
    let rightTabView : NSTabView
    
    let variablesOutlineScrollView : ScrollViewNoAutoScroll
    let stackTableScrollView : NSScrollView
    let textScrollView : NSScrollView
    
    let variablesOutlineView : NSOutlineView
    let stackTableView : NSTableView
    let textView : CustomTextView
    
    let mainViews : NSView // apart from margin
    let buttonViewArea : NSView
    
    let button_continue: NSButton // continue/run, step over/in/out
    let button_step_over : NSButton
    let button_step_in : NSButton
    let button_step_out : NSButton
    
    let tab_variables : NSTabViewItem
    let tab_stdout : NSTabViewItem
    let tab_stderr : NSTabViewItem
    
    let tab_stack : NSTabViewItem
    let tab_viewarea : NSTabViewItem
    
    let highlightingController : ColoringController
    var lineNumbersView : MarkerLineNumberView?
    
    let stack_datasource : StackTableDataSource
    let variables_datasource : VariablesOutlineDataSourceV2
    
    override init(window: NSWindow?) {
        leftTabView = NSTabView.init();
        rightTabView = NSTabView.init();
        variablesOutlineScrollView = ScrollViewNoAutoScroll.init()
        stackTableScrollView = NSScrollView.init()
        textScrollView = NSScrollView.init()
        
        
        variablesOutlineView = CustomOutlineView.init(frame: CGRectZero)
        stackTableView = NSTableView.init()
        textView = CustomTextView.init()
        
        mainViews = NSView.init()
        buttonViewArea = NSView.init()
        
        button_continue = NSButton.init()
        button_step_over = NSButton.init()
        button_step_in = NSButton.init()
        button_step_out = NSButton.init()
        
        highlightingController = ColoringController.init()
        highlightingController.setTextView(textView);
        textView.delegate = highlightingController;
        
        
        
        tab_variables = NSTabViewItem.init()
        tab_variables.label = "Variables"
        leftTabView.addTabViewItem(tab_variables)
        tab_variables.view?.addSubview(variablesOutlineScrollView)
        
        
        tab_stdout = NSTabViewItem.init()
        tab_stdout.label = "Output"
        leftTabView.addTabViewItem(tab_stdout)
        
        tab_stderr = NSTabViewItem.init()
        tab_stderr.label = "Errors"
        leftTabView.addTabViewItem(tab_stderr)
        
        tab_stack = NSTabViewItem.init();
        tab_stack.label = "Stack"
        tab_stack.view?.addSubview(stackTableScrollView)
        rightTabView.addTabViewItem(tab_stack)
        
        tab_viewarea = NSTabViewItem.init();
        tab_viewarea.label = "Data"
        //tab_stack.view?.addSubview()
        rightTabView.addTabViewItem(tab_viewarea)
        
        stack_datasource = StackTableDataSource.init()
        stack_datasource.tableView = stackTableView;
        stackTableView.setDataSource(stack_datasource)
        
        
        let stack_column = NSTableColumn.init()
        stack_column.title = "Function"
        stackTableView.addTableColumn(stack_column)
        
        let stack_column2 = NSTableColumn.init()
        stack_column2.title = "Filename"
        stackTableView.addTableColumn(stack_column2)
        
        let stack_column3 = NSTableColumn.init()
        stack_column3.title = "Line"
        stackTableView.addTableColumn(stack_column3)
        stack_datasource.column1 = stack_column;
        stack_datasource.column2 = stack_column2;
        stack_datasource.column3 = stack_column3;
        
        let column1 = NSTableColumn.init()
        column1.title = "Name"
        column1.width = 150;
        variablesOutlineView.addTableColumn(column1)
        variablesOutlineView.outlineTableColumn = column1
        
        let column2 = NSTableColumn.init()
        column2.title = "Type"
        column2.width = 50;
        variablesOutlineView.addTableColumn(column2)
        
        let column3 = NSTableColumn.init()
        column3.title = "Value"
        variablesOutlineView.addTableColumn(column3)
        
        
        variables_datasource = VariablesOutlineDataSourceV2.init()
        variablesOutlineView.setDelegate(variables_datasource)
        variablesOutlineView.setDataSource(variables_datasource)
        variablesOutlineView.indentationPerLevel = 20
        variablesOutlineView.indentationMarkerFollowsCell = true
        variables_datasource.outlineView = variablesOutlineView;
        variables_datasource.columnName = column1;
        variables_datasource.columnType = column2;
        variables_datasource.columnValue = column3;
        variables_datasource.scrollview = variablesOutlineScrollView;
        
        stack_datasource.sourceCodeTextView = textView;
        stack_datasource.variablesDelegate = variables_datasource;
        
        let text = "<?php\n                                                include(\"error_handling.php”);\n                                                        \n                                                        class dsafadsfds {\n                                                            \n                                                            function aaasdfdsafds($aaaa,$aaa,$aaa)\n                                                            {\n                                                                return $aaaa;\n                                                            }\n                                                            \n                                                            function aadsfadsfds($aaa,$aaa,$aaa)\n                                                            {\n                                                                $a = “adsfadsfdfs”;\n                                                                $a = 'aadfdsffds';\n                                                                return $aaa;\n                                                            }\n                                                            \n                                                        }\n                                                        \n                                                        ?>";
        
        
        
        textView.setLineNumber(13, forText: text);
        let attributedString = NSAttributedString.init(string: text)
        textView.textStorage?.setAttributedString(attributedString);
        
        
        let preferred_scroll_style = NSScroller.preferredScrollerStyle();
        if (preferred_scroll_style == NSScrollerStyle.Legacy)
        {
            variablesOutlineScrollView.autohidesScrollers = true
            //variablesOutlineScrollView.scrollerStyle = NSScrollerStyle.Legacy;
            variablesOutlineScrollView.hasVerticalScroller = true;
            variablesOutlineScrollView.hasHorizontalScroller = true;
            
            stackTableScrollView.autohidesScrollers = true
            //variablesOutlineScrollView.scrollerStyle = NSScrollerStyle.Legacy;
            stackTableScrollView.hasVerticalScroller = true;
            stackTableScrollView.hasHorizontalScroller = true;
        }
        //variablesOutlineView.setSt
        
        
        super.init(window:window)
        window?.delegate = self
        window?.minSize = CGSizeMake(620,450)
        self.window!.contentView?.addSubview(mainViews)
        
        let contentView = self.window!.contentView
        contentView?.addSubview(mainViews)
        mainViews.addSubview(leftTabView) // outlineScrollView
        mainViews.addSubview(rightTabView)
        mainViews.addSubview(textScrollView)
        mainViews.addSubview(buttonViewArea)
        
        
        button_continue.action = "buttonPress:";
        button_continue.target = self;
        button_step_over.action = "buttonPress:";
        button_step_over.target = self;
        button_step_in.action = "buttonPress:";
        button_step_in.target = self;
        button_step_out.action = "buttonPress:";
        button_step_out.target = self;
        
        stackTableScrollView.documentView = stackTableView
        variablesOutlineScrollView.documentView = variablesOutlineView
        textScrollView.documentView = textView
        
        
        lineNumbersView = MarkerLineNumberView.init(scrollView: textScrollView);
        textScrollView.verticalRulerView = lineNumbersView;
        textScrollView.hasVerticalRuler = true;
        textScrollView.hasHorizontalRuler = false;
        textScrollView.rulersVisible = true;
        //textScrollView.setNeedsDisplay()
        
        textView.maxSize = NSMakeSize(CGFloat(FLT_MAX), CGFloat(FLT_MAX));
        textView.textContainer?.widthTracksTextView = true
        
        buttonViewArea.addSubview(button_continue)
        buttonViewArea.addSubview(button_step_over)
        buttonViewArea.addSubview(button_step_in)
        buttonViewArea.addSubview(button_step_out)
        
        button_continue.title = "Continue"
        button_step_over.title = "Step Over"
        button_step_in.title = "Step In"
        button_step_out.title = "Step Out"

        
        //mainViews.backgroundColor = NSColor.redColor()
        //leftTabView.backgroundColor = NSColor.blueColor()
        //textScrollView.backgroundColor = NSColor.yellowColor()
        //buttonViewArea.backgroundColor = NSColor.greenColor()
        //stackTableScrollView.backgroundColor = NSColor.orangeColor()
        
        self.calculateFrames()
        variablesOutlineView.reloadData()
    }
    
    func buttonPress(button : NSButton)
    {
        let client_delegate = XDebugClientDelegate.getInstance()
        if (button == button_continue)
        {
            client_delegate.actionContinue()
        } else if (button == button_step_in)
        {
            client_delegate.actionStepIn()
        } else if (button == button_step_out)
        {
            client_delegate.actionStepOut()
        } else if (button == button_step_over)
        {
            client_delegate.actionStepOver()
        }
    }

    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func windowDidLoad() {
        super.windowDidLoad()
        
    }
    
    func windowDidResize(notification: NSNotification)
    {
        self.calculateFrames()
    }
    
    func calculateFrames()
    {
        // calculate main view size without margins
        // set as the mainViews object
        var main_size = self.window?.contentView?.bounds ?? CGRectZero
        let margin = CGFloat(10)
        main_size.size.height -= margin*2
        main_size.size.width -= margin*2
        main_size.origin.y = margin
        main_size.origin.x = margin
        mainViews.frame = main_size
        
        let tab_view_adjustment = CGFloat(15)
        
        // Determine widths for areas
        let h_margin = CGFloat(3)
        var left_area = main_size.width*0.5;
        left_area = min(left_area,500);
        let right_area = main_size.width - left_area - h_margin;
        var buttons_area = CGFloat(200);
        var stack_area = right_area - buttons_area - h_margin
        if (stack_area < 0)
        {
            buttons_area = 0
            stack_area = 0
        }
        
        // determine vertial heights
        let v_margin = CGFloat(10)
        let top_area_height = CGFloat(150)
        let bottom_area_height = main_size.height - top_area_height - v_margin
        
        let right_tab_adjustment = CGFloat(10);
        
        // set frames
        leftTabView.frame = CGRectMake(0,-tab_view_adjustment,left_area,main_size.height+tab_view_adjustment)
        textScrollView.frame = CGRectMake(left_area+h_margin,0,right_area,bottom_area_height)
        rightTabView.frame = CGRectMake(left_area+h_margin,main_size.height-top_area_height - right_tab_adjustment,stack_area,top_area_height+right_tab_adjustment)
        buttonViewArea.frame = CGRectMake(main_size.width-buttons_area,main_size.height-top_area_height,buttons_area,top_area_height)
        
        // add buttons
        let buttons_v_margin = CGFloat(10);
        var buttons_frame = CGRectMake(5, top_area_height + 10, buttons_area - 10, 30)
        buttons_frame.origin.y -= buttons_frame.height + buttons_v_margin
        button_continue.frame = buttons_frame
        buttons_frame.origin.y -= buttons_frame.height + buttons_v_margin
        button_step_over.frame = buttons_frame
        buttons_frame.origin.y -= buttons_frame.height + buttons_v_margin
        button_step_in.frame = buttons_frame
        buttons_frame.origin.y -= buttons_frame.height + buttons_v_margin
        button_step_out.frame = buttons_frame
        
        // set the tab frame(s) in case of resize
        var tab_frame_super = leftTabView.frame;
        var tab_frame = tab_variables.view?.frame ?? CGRectZero
        var tab_frame_new = CGRectMake(tab_frame.origin.x,tab_frame.origin.y,tab_frame_super.size.width - tab_frame.origin.x*2,tab_frame_super.size.height - tab_frame.origin.y - 13);
        tab_variables.view?.frame = tab_frame_new;
        
        // set the tab frame(s) in case of resize
        tab_frame_super = rightTabView.frame;
        tab_frame = tab_stack.view?.frame ?? CGRectZero
        tab_frame_new = CGRectMake(tab_frame.origin.x,tab_frame.origin.y,tab_frame_super.size.width - tab_frame.origin.x*2,tab_frame_super.size.height - tab_frame.origin.y - 13);
        tab_stack.view?.frame = tab_frame_new;
        
        // set tab view for variables
        var tab_margin = CGFloat(5)
        var tab_bounds = tab_variables.view?.bounds ?? CGRectZero
        tab_bounds.size.height -=  5;
        tab_bounds.origin.y = 5
        tab_bounds.origin.x = tab_margin
        tab_bounds.size.width -= tab_margin*2
        variablesOutlineScrollView.frame = tab_bounds
        
        
        // set tab view for stack
        tab_margin = CGFloat(5)
        tab_bounds = tab_stack.view?.bounds ?? CGRectZero
        tab_bounds.size.height -=  5;
        tab_bounds.origin.y = 5
        tab_bounds.origin.x = tab_margin
        tab_bounds.size.width -= tab_margin*2
        stackTableScrollView.frame = tab_bounds
        
        //tab_frame.size.height +=  CGFloat(20);
        //tab_variables.view?.frame = tab_frame;
        
        // text view
        let content_size = textScrollView.contentSize
        textView.frame = CGRectMake(0, 0, content_size.width, content_size.height)
        
    }

}
