#import "MyView.h"

@implementation MyView

-(id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        latestEvents = [[NSMutableDictionary alloc] init];
    }
    return self;
}

-(BOOL)acceptsFirstResponder {
	return YES;
}
-(void)mouseUp:(NSEvent*)ev {
	NSPoint point = [self convertMouseCoords:[ev locationInWindow]];
	[latestEvents setObject:[NSValue valueWithPoint:point] forKey:@"mouseup"];
}
-(void)rightMouseUp:(NSEvent *)ev {
	NSPoint point = [self convertMouseCoords:[ev locationInWindow]];
	[latestEvents setObject:[NSValue valueWithPoint:point] forKey:@"rightmouseup"];
}
-(NSPoint)convertMouseCoords:(NSPoint)point {
	point = [self convertPoint:point toView:self];
	point.y -= 1;										// The y-axis is 1-based, for some reason
	point.y = [self bounds].size.height - point.y;		// Oh and it’s also inverted
	return point;
}

-(void)keyDown:(NSEvent *)ev {
	[latestEvents setObject:[ev characters] forKey:@"keydown"];
}

-(bool)windowShouldClose:(id)sender {
	return NO;
}

-(NSMutableDictionary *)getEvents {
	return latestEvents;
}
-(void)clearEvents {
	[latestEvents removeAllObjects];
}

@end


@implementation MyWindowDelegate

-(bool)windowShouldClose:(id)sender {
	[[NSApp delegate] performSelector:@selector(sendQuitEvent)];
	return NO;
}

@end
