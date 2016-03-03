//
//  CustomTextView.h
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/10/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface CustomTextView : NSTextView

- (void) setLineNumber: (int) line forText:(NSString*)text;
- (void) disableHighlighting;

@end
