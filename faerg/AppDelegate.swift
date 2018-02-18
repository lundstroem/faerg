//
//  AppDelegate.swift
//  faerg
//
//  Created by Harry Lundstrom on 2018-02-18.
//  Copyright © 2018 Harry Lundström. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {



    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }

    @IBAction func newDocument(_ sender: Any?) {

    }
    
    @IBAction func openDocument(_ sender: Any?) {
        browseFile(sender: self)
    }
    
    func browseFile(sender: AnyObject) {
        
        let dialog = NSOpenPanel();
        
        dialog.title                   = "Choose a png or jpg file";
        dialog.showsResizeIndicator    = true;
        dialog.showsHiddenFiles        = false;
        dialog.canChooseDirectories    = true;
        dialog.canCreateDirectories    = true;
        dialog.allowsMultipleSelection = false;
        dialog.allowedFileTypes        = ["png","jpg"];
        
        if (dialog.runModal() == NSApplication.ModalResponse.OK) {
            let result = dialog.url // Pathname of the file
            
            if (result != nil) {
                let path = result!.path
                //filename_field.stringValue = path
                print("path:", path)
            }
        } else {
            // User clicked on "Cancel"
            return
        }
        
    }
}

