#import <Cocoa/Cocoa.h>

struct myObjs;

@interface AppDelegate : NSObject <NSApplicationDelegate> {
	struct myObjs *objs;
}

@end
