/*
**++
**  FACILITY:  DUI
**
**  MODULE DESCRIPTION:
**
**      Implements MOTIF utility functions
**
**  AUTHORS:
**
**      C. Gaspar
**
**  CREATION DATE:  24-01-1993
**
**--
*/
#include <stdio.h>
#include <Mrm/MrmAppl.h>                   /* Motif Toolkit */
#include <Xm/Xm.h>

#include <dim.h>
#include "dui_util.h"

/* compound strings */


static XmString Active_str;

XmString get_str(text)
char *text;
{

	Active_str = XmStringCreateLtoR ( text, XmSTRING_DEFAULT_CHARSET); 
	return(Active_str);
}


void free_str()
{

	XmStringFree(Active_str);
}


XmString create_str(text)
char *text;
{
XmString str;


	str = XmStringCreate ( text, XmSTRING_DEFAULT_CHARSET); 
	return(str);
}

void delete_str(str)
XmString str;
{

	XmStringFree(str);
}

void set_something(w, resource, value)
    Widget w;
    char *resource, *value;
{
    Arg al[1];
	int free = 0;
DISABLE_AST
	if( (!strcmp(resource,XmNlabelString)) ||
		(!strcmp(resource,XmNmessageString)) ||
		(!strcmp(resource,XmNtextString)) ||
		(!strcmp(resource,XmNlistLabelString)) ||
		(!strcmp(resource,XmNselectionLabelString)) )
	{
		free = 1;
		value = (char *)get_str(value);
	}
    XtSetArg(al[0], resource, value);
    XtSetValues(w, al, 1);
	if(free)
		free_str();
	/*
	printf("Flushing %s for widget %s...\n",resource,w->core.name);
	*/
	/*
	XFlush(XtDisplay(w));
	*/
	/*
	printf("Flushed!\n");
	*/
ENABLE_AST
}

void get_something(w, resource, value)
    Widget w;
    char *resource, *value;
{

    Arg al[1];
	int free = 0;
	XmString str;
	char *cstr;

	if( (!strcmp(resource,XmNlabelString)) ||
		(!strcmp(resource,XmNmessageString)) ||
		(!strcmp(resource,XmNtextString)) ||
		(!strcmp(resource,XmNlistLabelString)) ||
		(!strcmp(resource,XmNselectionLabelString)) )
	{
		free = 1;
	    XtSetArg(al[0], resource, &str);
	}
	else
		XtSetArg(al[0], resource, value);
    XtGetValues(w, al, 1);
	if(free)
	{
		XmStringGetLtoR(str, XmSTRING_DEFAULT_CHARSET, &cstr);
		strcpy(value,cstr);
		XtFree(cstr);
	}
}
/*
void set_something_uid(hid, w, resource, value)
    MrmHierarchy hid;
    Widget w;                          
    char *resource, *value;
{
    Arg al[1];

	XtSetArg(al[0], resource, value);

    MrmFetchSetValues(hid, w, al, 1);
}
*/

static XmString str_table[50];

XmStringTable create_str_table(strs)
char strs[50][256];
{
int i;

	for(i=0;strs[i][0];i++)
	{
		str_table[i] = XmStringCreate ( strs[i], XmSTRING_DEFAULT_CHARSET);
	}
	str_table[i] = (XmString)0;
	return((XmStringTable)str_table);
}

void del_str_table()
{
int i;

	for(i=0;str_table[i];i++)
		XmStringFree(str_table[i]);

}

Pixel rgb_colors[MAX_COLORS];
static Pixmap pixmap_colors[MAX_COLORS];
static Pixmap watch_colors[MAX_COLORS];
static Pixmap locks[MAX_COLORS];
static Pixmap unlock;
static Pixmap faces[MAX_COLORS];

Pixel get_named_color(color)
int color;
{
	return(rgb_colors[color]);
}

void set_color(w, resource, color) 
Widget w;
char *resource;
int color;
{

DISABLE_AST
/*
	if(resource == XmNbackgroundPixmap)
*/
	if(!strcmp(resource,XmNbackgroundPixmap))
		set_something(w,resource,pixmap_colors[color]);
	else
		set_something(w,resource,rgb_colors[color]);
ENABLE_AST
}
	
void set_watch(w, color) 
Widget w;
int color;
{
	set_something(w,XmNbackgroundPixmap,watch_colors[color]);
}
	
void set_lock(w, color) 
Widget w;
int color;
{
	set_something(w,XmNbackgroundPixmap,locks[color]);
}
	
void set_unlock(w) 
Widget w;
{
	set_something(w,XmNbackgroundPixmap,unlock);
}
	
void set_face(w, color) 
Widget w;
int color;
{
	set_something(w,XmNbackgroundPixmap,faces[color]);
}
	
void get_all_colors(display, w)
	Display *display;
	Widget w;
{
    XColor a,b;
    Colormap cm;
    cm = DefaultColormap ( display, DefaultScreen(display));

    XAllocNamedColor ( display, cm, "Medium Aquamarine", &a,&b );
    rgb_colors[GREEN] = b.pixel;

    XAllocNamedColor ( display, cm, "Turquoise", &a,&b );
    rgb_colors[BLUE] = b.pixel;

    XAllocNamedColor ( display, cm, "Yellow", &a,&b );
    rgb_colors[YELLOW] = b.pixel;

    XAllocNamedColor ( display, cm, "Orange", &a,&b );
    rgb_colors[ORANGE] = b.pixel;

    XAllocNamedColor ( display, cm, "Red", &a,&b );
    rgb_colors[RED] = b.pixel;

    XAllocNamedColor ( display, cm, "Black", &a,&b );
    rgb_colors[BLACK] = b.pixel;

    XAllocNamedColor ( display, cm, "White", &a,&b );
    rgb_colors[WHITE] = b.pixel;

    XAllocNamedColor ( display, cm, "Light Gray", &a,&b );
    rgb_colors[GRAY] = b.pixel;

    XAllocNamedColor ( display, cm, "Gainsboro", &a,&b );
    rgb_colors[LIGHTGRAY] = b.pixel;
/*
    MrmFetchColorLiteral(hid, "green", display, 0, &rgb_colors[GREEN]);
    MrmFetchColorLiteral(hid, "blue", display, 0, &rgb_colors[BLUE]);
    MrmFetchColorLiteral(hid, "yellow", display, 0, &rgb_colors[YELLOW]);
    MrmFetchColorLiteral(hid, "orange", display, 0, &rgb_colors[ORANGE]);
    MrmFetchColorLiteral(hid, "red", display, 0, &rgb_colors[RED]);
    MrmFetchColorLiteral(hid, "black", display, 0, &rgb_colors[BLACK]);
    MrmFetchColorLiteral(hid, "white", display, 0, &rgb_colors[WHITE]);
*/
     get_something(w,XmNbackground,&rgb_colors[NONE]);
}


static int was_sensitive = 0;

void set_sensitive(widget_id)
Widget widget_id;
{

	if(was_sensitive)
		XtSetSensitive(widget_id,True);
}

void set_insensitive(widget_id)
Widget widget_id;
{

	if( (was_sensitive = XtIsSensitive(widget_id)) )
		XtSetSensitive(widget_id,False);
}
	
void set_title(w, title)
    Widget w;
    char *title;
{
    Arg al[1];

    XtSetArg(al[0], XmNtitle, title);
    XtSetValues(w, al, 1);
    /*
	XFlush(XtDisplay(w));
	*/
}

void set_icon_title(w, title)
    Widget w;
    char *title;
{
    Arg al[1];

    XtSetArg(al[0], XmNiconName, title);
    XtSetValues(w, al, 1);
    /*
	XFlush(XtDisplay(w));
	*/
}

