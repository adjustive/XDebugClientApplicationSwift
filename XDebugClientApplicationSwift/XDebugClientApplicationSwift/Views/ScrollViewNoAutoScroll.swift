//
//  ScrollViewNoAutoScroll.swift
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/9/16.
//  Copyright © 2016 elijah. All rights reserved.
//

import Cocoa

public class ScrollViewNoAutoScroll: NSScrollView {
    
    public var disableAutoScroll = false
    private var lastScroll = NSMakePoint(0,0);
    
    override public func drawRect(dirtyRect: NSRect) {
        super.drawRect(dirtyRect)

        // Drawing code here.
    }
    
    override public func scrollClipView(aClipView: NSClipView, toPoint aPoint: NSPoint) {
        if (!disableAutoScroll)
        {
            lastScroll = aPoint;
            super.scrollClipView(aClipView, toPoint: aPoint);
        } else {
            NSLog("Scroll was stopped");
            let c_height = self.documentView?.frame.size.height ?? 0;
            let frame_height = self.frame.size.height;
            NSLog("last scroll:%f\ndocument height: %f\nframe height %f\n\n",lastScroll.y,c_height,frame_height)
            if (lastScroll.y > c_height - frame_height*0.90)
            {
                super.scrollClipView(aClipView, toPoint: aPoint);
            }
        }
    }
    
    /*
    override public func scrollRectToVisible(aRect: NSRect) -> Bool {
    return false;
    }
    
    override public func scrollToBeginningOfDocument(sender: AnyObject?) {
    NSLog("test");
    }
    override func scrollLineDown(sender: AnyObject?) {
        NSLog("test");
    }
    
    override func scrollLineUp(sender: AnyObject?) {
        NSLog("test");
    }
    
    override func scrollPageDown(sender: AnyObject?) {
        NSLog("test");
    }
    
    override func scrollPageUp(sender: AnyObject?) {
        NSLog("test");
    }
    
    override func scrollPoint(aPoint: NSPoint) {
        NSLog("test");
    }
    
    override func scrollRect(aRect: NSRect, by delta: NSSize) {
        NSLog("test");
    }
    
    override func scrollToEndOfDocument(sender: AnyObject?) {
        NSLog("test");
    }
    
    override func scrollWheel(theEvent: NSEvent) {
        NSLog("test");
    }
*/
    
}
