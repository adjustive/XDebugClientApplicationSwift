//
//  CustomTextView.m
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/10/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#import "CustomTextView.h"

@interface CustomTextView ()
@end

@implementation CustomTextView
{
    NSRange rangeToHighlight;
    BOOL should_highlight;
    NSLock* rangeLock;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Drawing code here.
}


- (void) disableHighlighting
{
    [rangeLock lock];
    should_highlight = false;
    [rangeLock unlock];
}

- (void) setLineNumber: (int) line forText:(NSString*)text
{
    text = [text stringByReplacingOccurrencesOfString:@"\r\n" withString:@"\n"];
    text = [text stringByReplacingOccurrencesOfString:@"\r" withString:@"\n"];
    // 11 line
    int pos = -1;
    int cur_pos = 0;
    int next_line = 0;
    
    int string_length = (int)text.length;
    if (line == 1)
    {
        next_line = (int)[text rangeOfString:@"\n"].location;
        pos = 0;
    } else {
        NSRange cur_range = NSMakeRange(0, string_length);
        for (int i=2;i<=line;i++)
        {
            cur_range.location = cur_pos;
            cur_range.length = string_length - cur_pos;
            cur_pos = (int)[text rangeOfString:@"\n" options:NSLiteralSearch range:cur_range].location + 1;
            if (i == line)
            {
                pos = cur_pos;
                cur_range.location = cur_pos;
                cur_range.length = string_length - cur_pos;
                next_line =(int)[text rangeOfString:@"\n" options:NSLiteralSearch range:cur_range].location;
            }
        }
    }
    // get line length
    int line_length = string_length - pos;
    if (next_line > 0)
    {
        line_length = next_line - pos + 1;
    }
    
    // update variables
    [rangeLock lock];
    should_highlight = NO;
    if (pos != -1)
    {
        rangeToHighlight = NSMakeRange(pos, line_length);
        should_highlight = YES;
    }
    NSRange range_copy = rangeToHighlight;
    BOOL should_highlight_copy = should_highlight;
    [rangeLock unlock];
    [self setNeedsDisplay:YES];
    
    if (should_highlight_copy)
    {
        //NSRect lineRect = [self getHighlightDimensionsRectForRange:lineRange];
        [self scrollRangeToVisible:range_copy];
    }
}

- (void) drawViewBackgroundInRect:(NSRect)rect
{
    [super drawViewBackgroundInRect:rect];
    //NSRange sel = [self selectedRange];
    //NSString* str = [self string];
    //if (sel.location <= [str length]) {
    BOOL should_highlight_i = NO;
    [rangeLock lock];
    should_highlight_i = should_highlight;
    NSRange lineRange = rangeToHighlight;
    [rangeLock unlock];
    if (!should_highlight_i)
    {
        return;
    }
    //NSMakeRange(640, 97);//[str lineRangeForRange:NSMakeRange(sel.location,0)];
        NSRect lineRect = [self getHighlightDimensionsRectForRange:lineRange];
        NSColor *highlightColor = [NSColor colorWithCalibratedRed:0.522 green:0.718 blue:0.878 alpha:1.00];
        [highlightColor set];
        [NSBezierPath fillRect:lineRect];
    //}
}

// Returns a rectangle suitable for highlighting a background rectangle for the given text range.
- (NSRect) getHighlightDimensionsRectForRange:(NSRange)aRange
{
    NSRange r = aRange;
    NSRange startLineRange = [[self string] lineRangeForRange:NSMakeRange(r.location, 0)];
    NSInteger er = NSMaxRange(r)-1;
    NSString *text = [self string];
    
    if (er >= [text length]) {
        return NSZeroRect;
    }
    if (er < r.location) {
        er = r.location;
    }
    
    NSRange endLineRange = [[self string] lineRangeForRange:NSMakeRange(er, 0)];
    
    NSRange gr = [[self layoutManager] glyphRangeForCharacterRange:NSMakeRange(startLineRange.location, NSMaxRange(endLineRange)-startLineRange.location-1)
                                              actualCharacterRange:NULL];
    NSRect br = [[self layoutManager] boundingRectForGlyphRange:gr inTextContainer:[self textContainer]];
    NSRect b = [self bounds];
    CGFloat h = br.size.height;
    CGFloat w = b.size.width;
    CGFloat y = br.origin.y;
    NSPoint containerOrigin = [self textContainerOrigin];
    NSRect aRect = NSMakeRect(0, y, w, h);
    // Convert from view coordinates to container coordinates
    aRect = NSOffsetRect(aRect, containerOrigin.x, containerOrigin.y);
    return aRect;
}

@end
