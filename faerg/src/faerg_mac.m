#import <stdlib.h>
#import <stdio.h>
#import "faerg_mac.h"
#import "nfd.h"

static int open_dialog(void);
static int save_dialog(void);

char *load_image(char *path) {
    /* todo: load image with Cocoa and return pixel data */
    open_dialog();
    return NULL;
}

static int open_dialog(void) {
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog("png,jpg", NULL, &outPath);
    if ( result == NFD_OKAY )
    {
        puts("Success!");
        puts(outPath);
        free(outPath);
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
