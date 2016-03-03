//
//  NSViewExtensions.swift
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/6/16.
//  Copyright © 2016 elijah. All rights reserved.
//

import Cocoa

extension NSView {
    var backgroundColor: NSColor? {
        get {
            guard let layer = layer, backgroundColor = layer.backgroundColor else { return nil }
            return NSColor(CGColor: backgroundColor)
        }
        
        set {
            wantsLayer = true
            layer?.backgroundColor = newValue?.CGColor
        }
    }
}