//
//  AppDelegate.m
//  SpriteTest
//
//  Created by Steven Stewart on 1/1/15.
//  Copyright (c) 2015 Maaz Kamani. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    lastMillies = CACurrentMediaTime() * 1000;
    
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    CGFloat screenScale = [[UIScreen mainScreen] scale];
    CGSize screenSize = CGSizeMake(screenBounds.size.width * screenScale, screenBounds.size.height * screenScale);
    screenWidthInPixels = (float)screenSize.width;
    screenHeightInPixels = (float)screenSize.height;
    
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    
    initView(screenWidthInPixels, screenHeightInPixels);
    
    GLKView *view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds] context:context];
    view.delegate = self;
    
    GLKViewController *controller = [[GLKViewController alloc] init];
    controller.view = view;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    [controller setPreferredFramesPerSecond:60];
    
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = controller;
    [self.window makeKeyAndVisible];
    
    
    //Get texture
    CGImageRef spriteImage = [UIImage imageNamed:@"run_sprite_opt"].CGImage;
    if (!spriteImage) {
        NSLog(@"Failed to load image %@", @"asdf");
        exit(1);
    }
    
    size_t width = CGImageGetWidth(spriteImage);
    size_t height = CGImageGetHeight(spriteImage);
    
    GLubyte * spriteData = (GLubyte *) calloc(width*height*4, sizeof(GLubyte));
    
    CGContextRef spriteContext = CGBitmapContextCreate(spriteData, width, height, 8, width*4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
    
    CGContextDrawImage(spriteContext, CGRectMake(0, 0, width, height), spriteImage);
    CGContextRelease(spriteContext);
    
    
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
    
    free(spriteData);        
    //return texName;
    
    setTextureReference(texName);
    
    return YES;
}
- (void)applicationWillResignActive:(UIApplication *)application
{
    
}
- (void)applicationDidEnterBackground:(UIApplication *)application
{
    
}
- (void)applicationWillEnterForeground:(UIApplication *)application
{
    
}
- (void)applicationDidBecomeActive:(UIApplication *)application
{
    
}
- (void)applicationWillTerminate:(UIApplication *)application
{
    
}

# pragma mark - GlkView Delegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    long currentMillies = CACurrentMediaTime() * 1000;
    long diff = currentMillies - lastMillies;
    lastMillies = currentMillies;
    if(diff > 100) diff = 100;
    
    renderScene((int)diff);
}

@end
