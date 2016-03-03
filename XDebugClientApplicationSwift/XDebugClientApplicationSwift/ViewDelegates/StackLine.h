//
//  StackLine.h
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/9/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface StackLine : NSObject

@property NSString* filename;
@property NSString* filename_display;
@property NSString* where;
@property int line_number;
@property int level;
@property NSString* display_value;
@property NSString* line_number_display;

@end
