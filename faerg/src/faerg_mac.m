#import <stdlib.h>
#import <stdio.h>
#import "faerg_mac.h"
#import "nfd.h"
#import <Cocoa/Cocoa.h>

static nfdchar_t* open_dialog(void);
static int save_dialog(void);

char *load_image(void) {
    /* todo: load image with Cocoa and return pixel data */
    nfdchar_t *path = open_dialog();
    if(path != NULL) {
        NSString *pathString = [NSString stringWithUTF8String:path];
        NSURL *url = [NSURL fileURLWithPath:pathString];
        NSImage *image = [[NSImage alloc] initWithContentsOfURL:url];
        
        if(image) {
            printf("image loaded\n");
        } else {
            printf("image failed\n");
        }
        free(path);
    } else {
        printf("path is null\n");
    }
    
    return NULL;
}

static nfdchar_t* open_dialog(void) {
    
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog("png,jpg", NULL, &outPath);
    
    if (result == NFD_OKAY && outPath)
    {
        printf("Success!");
    }
    else if (result == NFD_CANCEL)
    {
        puts("User pressed cancel.");
    }
    else
    {
        printf("Error: %s\n", NFD_GetError() );
    }
    
    return outPath;
}

static int save_dialog(void) {
    nfdchar_t *savePath = NULL;
    nfdresult_t result = NFD_SaveDialog( "png,jpg;pdf", NULL, &savePath );
    if ( result == NFD_OKAY )
    {
        puts("Success!");
        puts(savePath);
        free(savePath);
    }
    else if ( result == NFD_CANCEL )
    {
        puts("User pressed cancel.");
    }
    else
    {
        printf("Error: %s\n", NFD_GetError() );
    }
    
    return 0;
}
