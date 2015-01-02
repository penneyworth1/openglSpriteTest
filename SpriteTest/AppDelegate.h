//
//  AppDelegate.h
//  SpriteTest
//
//  Created by Steven Stewart on 1/1/15.
//  Copyright (c) 2015 Maaz Kamani. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import "core.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate, GLKViewDelegate>
{
    float screenWidthInPixels;
    float screenHeightInPixels;
    float lastMillies;
    
    //testing
    GLuint texName;
}

@property (strong, nonatomic) UIWindow *window;


@end

