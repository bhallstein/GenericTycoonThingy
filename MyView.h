#import <Cocoa/Cocoa.h>

@interface MyView : NSView {
	NSMutableDictionary *latestEvents;
	NSOpenGLContext *context;
}
-(NSMutableDictionary *)getEvents;
-(void)clearEvents;
-(NSPoint)convertMouseCoords:(NSPoint)point;
@end


@interface MyWindowDelegate : NSObject <NSWindowDelegate> {
	
}
-(bool)windowShouldClose:(id)sender;
@end
