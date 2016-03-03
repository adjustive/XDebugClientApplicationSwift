#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

// flex things to avoid warnings
void yystatereset();
void yy_scan_string(const char *);
NSUInteger yylex();

@interface ColoringController : NSObject <NSTextStorageDelegate,NSTextViewDelegate> {
    NSTextView* textview;
    //IBOutlet id window;
    NSMutableArray *words;
    NSTimer *completionTimer;
}

// My methods
- (void) setTextView: (NSTextView*)textView;
- (NSMutableArray*)getWordArray;
- (void)stopCompletionTimer;

// TextView delegate
- (void)awakeFromNib;
// Text storage delegate
- (void)textStorageDidProcessEditing:(NSNotification *)notification;
//TextView delegate
- (BOOL)textView:(NSTextView *)textView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString*)replacementString;
- (NSArray *)control:(NSControl *)control textView:(NSTextView *)textView completions:(NSArray *)the_words forPartialWordRange:(NSRange)charRange indexOfSelectedItem:(NSInteger *)index;
- (NSArray *)textView:(NSTextView *)textView completions:(NSArray *)the_words forPartialWordRange:(NSRange)charRange indexOfSelectedItem:(NSInteger *)index;
@end