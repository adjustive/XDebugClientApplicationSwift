//
//  AppDelegate.swift
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/27/15.
//  Copyright © 2015 elijah. All rights reserved.
//

import Cocoa


@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    var window: NSWindow!
    
    //var window2: NSWindow!
    var windowController: NSWindowController!
    


    func applicationDidFinishLaunching(aNotification: NSNotification) {
        // Insert code here to initialize your application
        let delegate = XDebugClientDelegate.getInstance();
        delegate.doSomething();
        delegate.startServer();
        
        /*
        let delayTime = dispatch_time(DISPATCH_TIME_NOW, Int64(1 * Double(NSEC_PER_SEC)))
        dispatch_after(delayTime, dispatch_get_main_queue()) {
            delegate.restartServer()
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(4 * Double(NSEC_PER_SEC))), dispatch_get_main_queue()) {
            delegate.stopServer()
        }
        */
        
        let screen_size = NSScreen.mainScreen()?.visibleFrame ?? NSMakeRect(0,0,1024,786)
        
        let view = NSView.init(frame: CGRectMake(0, 0, 100, 300));
        
        let frame = CGRectMake(10,10,min(screen_size.size.width*0.9,1400),min(screen_size.height*0.9,1000));
        let window = NSWindow(contentRect: frame, styleMask: NSTitledWindowMask|NSResizableWindowMask|NSMiniaturizableWindowMask|NSClosableWindowMask, backing: NSBackingStoreType.Buffered, `defer`: false);
        //self.window.addChildWindow(window, ordered: NSWindowOrderingMode.Above);
        window.setFrame(frame, display: true);
        window.center();
        window.title = "Debug Client for PHP";
        window.contentView?.addSubview(view);
        let window_controller = MainWindowController.init(window: window);
        self.windowController = window_controller;
        window_controller.showWindow(window);
        //window_controller.showWindow(window);
        //window.makeKeyAndOrderFront(window);
        //window.orderOut(nil);
    }

    func applicationWillTerminate(aNotification: NSNotification) {
        // Insert code here to tear down your application
    }


}

