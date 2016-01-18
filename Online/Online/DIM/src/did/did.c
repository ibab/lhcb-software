#include <stdio.h>                   
#include <ctype.h>
#include <time.h>
#include <dim.h>
#include <dic.h>
#include <dis.h>
#include "did.h"

int First_time = 1;
int Curr_view_opt = -1;	
char Curr_view_opt_par[80];	
char Curr_service_name[132];
char Curr_service_format[256];
int Curr_service_print_type = 0;	
int N_servers = 0;	
int N_services = 0;	
int no_link_int = -1;
FILE	*fptr;

char *Service_content_str;
char *Service_buffer;
int Service_size;
char *Curr_service_list = 0;
char *Curr_client_list = 0;
int Curr_service_id = 0;
Widget Curr_client_id;
Widget Curr_service_list_id;
SERVER *Got_Service_List = 0;
SERVER *Got_Client_List = 0;

Widget SubscribeButton;
Widget Subscribe10Button;

int Timer_q;

int Force_update = 0;
/*
 * Global data
 */
static XmFontList did_default_font, did_small_font, 
  did_label_font, did_server_font;

/*static MrmType class_id;*/		/* Place to keep class ID*/
/*static MrmType *dummy_class;*/            /* and class variable. */

/*static char *db_filename_vec[1];*/        /* Mrm.hierachy file list. */
/*static int db_filename_num;*/

/*
 * Forward declarations
 */
void did_exit();
void create_main();
void create_label();
void create_matrix();
void view_opts();
void dns_control();
void ok_pop_up();                                                            
void cancel_pop_up();

extern void set_something();
extern void get_something();
extern void set_color();

/*
 * Names and addresses of callback routines to register with Mrm
 */
/*
static MrmRegisterArg reglist [] = {
{"did_exit", (caddr_t)did_exit},
{"create_main", (caddr_t)create_main},
{"create_label", (caddr_t)create_label},
{"create_matrix", (caddr_t)create_matrix},
{"view_opts", (caddr_t)view_opts},
{"dns_control", (caddr_t)dns_control},
{"ok_pop_up", (caddr_t)ok_pop_up},
{"cancel_pop_up", (caddr_t)cancel_pop_up},
};

static int reglist_num = (sizeof reglist / sizeof reglist[0]);
*/
/*
 * OS transfer point.  The main routine does all the one-time setup and
 * then calls XtAppMainLoop.
 */

SERVER *Curr_servp;

XmFontList util_get_font( char *fontname, Widget top )
{
XFontStruct * mf;
XmFontList font;
/*
char * fontname;

  if ( size == 'm' ) {
    fontname = MENU_FONT;
  }
  else if ( size == 'b' ) {
    fontname = LABEL_FONT;
  }
  else {
    fontname = DEFAULT_FONT;
  }
*/
  if ( (mf = XLoadQueryFont(XtDisplay(top),fontname))==NULL) {
        printf("Couldn't open the following fonts:\n\t%s\n",
	    fontname);
        XtVaGetValues ( top, XmNdefaultFontList, &font, NULL );
  }
  else  {
     font = XmFontListCreate (mf, XmSTRING_DEFAULT_CHARSET);
  }
  return font;
}

void create_matrix_widget()
{
Widget row_col_id, top_id;
Arg arglist[10];
int n = 0;
char w_name[MAX_NAME];

	top_id = Window_id;
	XtSetArg(arglist[n], XmNborderWidth, 0); n++;
	XtSetArg(arglist[n], XmNorientation, XmVERTICAL);  n++;
        XtSetArg(arglist[n], XmNnumColumns, 4);  n++;
	XtSetArg(arglist[n], XmNpacking, XmPACK_COLUMN);  n++;
        XtSetArg(arglist[n], XmNadjustLast, False); n++;
	sprintf(w_name,"matrix_row");
	row_col_id = XmCreateRowColumn(top_id,w_name,arglist,(Cardinal)n);
	XtManageChild(row_col_id);
	Matrix_id[Curr_matrix] = row_col_id;
	/*
	XmScrolledWindowSetAreas(Window_id,NULL, NULL, Matrix_id); 
	*/
}

void gui_create_main_window(Widget parent)
{

Widget mw;
Widget mb;
Widget mf;
Widget tl;
Widget sl;
Widget f;
XmString xms;
int par;
int reason;
Arg ar[20];
int n;

    mw = XmCreateMainWindow ( parent, "DidMainWindow", NULL, 0 );
    XtVaSetValues( mw,
        XmNresizePolicy, XmRESIZE_ANY,
		  
        XmNfontList,            did_default_font,
        NULL);
    XtManageChild( mw );
    /* create menu bar */
    mb = XmCreateMenuBar ( mw, "DidMenuBar", NULL, 0 );
    XtVaSetValues( mb,
        XmNmarginHeight,2,
        XmNborderWidth, 0,
        XmNfontList,            did_default_font,
        NULL);
        
    gui_create_main_menu( mb );
    XtManageChild ( mb );

    /* create main form */
    mf = XmCreateForm ( mw, "DidMainForm", NULL, 0 );
    XtVaSetValues ( mf, 
		    XmNresizePolicy, XmRESIZE_NONE, 
		    		   
		    NULL );
    XtManageChild ( mf );

    /* create top label */
    xms = create_str(" \n  ");
    
    tl = XmCreateLabel ( mf, "DidTitle", NULL, 0 );
    XtVaSetValues( tl,
        XmNtopAttachment,           XmATTACH_FORM,
        XmNbottomAttachment,        XmATTACH_NONE,
        XmNleftAttachment,          XmATTACH_FORM,
        XmNrightAttachment,         XmATTACH_FORM,
        XmNleftOffset,              0,
        XmNtopOffset,               0,
        XmNbottomOffset,            0,
        XmNrightOffset,             0,
        XmNborderWidth,             0,
        XmNlabelString,             xms,          
        XmNshadowThickness,         0,
        XmNhighlightThickness,      0,
		XmNheight,					32,
        XmNalignment,               XmALIGNMENT_CENTER,
        XmNfontList,            did_label_font,
        NULL);
    XtManageChild( tl );
/*
    tl = XtVaCreateManagedWidget( "SmiTitle",
        xmPushButtonWidgetClass,    mw,
        XmNborderWidth,             0,
        XmNlabelString,             xms,          
        XmNshadowThickness,         0,
        XmNhighlightThickness,      0,
        XmNalignment,               XmALIGNMENT_CENTER,
        XmNfontList,            smid_label_font,
        NULL);
*/
    XmStringFree ( xms );
    /*
	XtAddCallback(tl, MrmNcreateCallback, 
		(XtCallbackProc)create_label, 0);
    */
    par = 0;
    reason = 0;
    create_label(tl, &par, &reason);

    /* create main form */
    /*
    mf = (Widget)XmCreateForm ( mw, "DidMainForm", NULL, 0 );
    XtVaSetValues ( mf,
        XmNborderWidth,1,
        XmNshadowThickness, 2,
		    XmNwidth,806,
		    XmNheight,300, 
		    
       XmNresizePolicy, XmRESIZE_NONE, NULL );
    XtManageChild ( mf );
    */
    /*
	XtAddCallback(mf, MrmNcreateCallback, 
			(XtCallbackProc)create_main, 0);
    */
    
    create_main(mf, &par, &reason);
    
    f = XmCreateForm( mf, "ScrollForm", NULL, 0 );
    XtVaSetValues ( f, 
		    XmNwidth, 806,
		    XmNheight,300,		    		  
        XmNtopAttachment,           XmATTACH_WIDGET,
        XmNbottomAttachment,        XmATTACH_FORM,
        XmNleftAttachment,          XmATTACH_FORM,
        XmNrightAttachment,         XmATTACH_FORM,
        XmNrightOffset,             0,
        XmNleftOffset,              0,
        XmNbottomOffset,            0,
        XmNtopWidget, tl,
        XmNtopOffset, 0,
        XmNshadowThickness, 2,
        XmNbottomOffset, 0,
		    /*
        XmNshadowType, XmSHADOW_OUT,
		    */
        XmNborderWidth,0,
        NULL);

    /*
    f = XtVaCreateManagedWidget ( "XDScrolledForm",
        xmFormWidgetClass,          mf,
        XmNtopAttachment,           XmATTACH_WIDGET,
        XmNbottomAttachment,        XmATTACH_FORM,
        XmNleftAttachment,          XmATTACH_FORM,
        XmNrightAttachment,         XmATTACH_FORM,
        XmNrightOffset,             0,
        XmNleftOffset,              0,
        XmNbottomOffset,            0,
        XmNtopWidget, tl,
        XmNtopOffset, 0,
        XmNshadowThickness, 2,
        XmNbottomOffset, 0,
        XmNshadowType, XmSHADOW_OUT,
        XmNborderWidth,1,
        NULL);
*/
    /*
	XtAddCallback(f, MrmNcreateCallback, 
		(XtCallbackProc)create_window, 0);
    */

    XtManageChild ( f );
    /* create scrolled list */
    
    n = 0;
    XtSetArg ( ar[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg ( ar[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg ( ar[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg ( ar[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg ( ar[n], XmNrightOffset, 0); n++;
    XtSetArg ( ar[n], XmNleftOffset, 0); n++;
    XtSetArg ( ar[n], XmNbottomOffset, 0); n++;
    XtSetArg ( ar[n], XmNtopOffset, 0); n++;
    /*
    XtSetArg ( ar[n], XmNvisualPolicy, XmCONSTANT); n++;
    */	    
    XtSetArg ( ar[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
		   
    XtSetArg ( ar[n], XmNscrollingPolicy, XmAUTOMATIC); n++;

    sl = XmCreateScrolledWindow ( f, "ScrollWin", ar, (Cardinal)n );
    /*
    XtVaSetValues ( sl, 
        XmNtopAttachment,           XmATTACH_FORM,
        XmNbottomAttachment,        XmATTACH_FORM,
        XmNleftAttachment,          XmATTACH_FORM,
        XmNrightAttachment,         XmATTACH_FORM,
        XmNrightOffset,             0,
        XmNleftOffset,              0,
        XmNbottomOffset,            0,
	XmNtopOffset,               0,
		    		   
        XmNvisualPolicy,        XmCONSTANT,
		    
	XmNscrollBarDisplayPolicy, XmSTATIC,
		   
        XmNscrollingPolicy, XmAUTOMATIC,

        NULL);
    */
    XtManageChild ( sl );
    /*
    create_window(sl, &par, &reason);
    */
    Window_id = sl;
    
    create_matrix_widget();
    
/* 
    sl = XtVaCreateWidget ( "DidServersScrl",
        xmScrolledWindowWidgetClass, f,
        XmNscrollingPolicy,     XmAUTOMATIC,
        XmNscrollBarDisplayPolicy, XmSTATIC,
        XmNtopAttachment,       XmATTACH_FORM,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNvisualPolicy,        XmCONSTANT,
        XmNtopOffset,           4,
        XmNleftOffset,          4,
        XmNbottomOffset,        4,
        XmNrightOffset,         4,
        NULL );
    XtManageChild ( sl );
*/
    /*
    XtVaSetValues( mw,
        XmNworkWindow,mf,
        XmNcommandWindow, tl,
        NULL);
    */
    
}

Widget create_separator(Widget parent_id)
{
	Widget w;
	Arg arglist[10];
	int n = 0;

	w = XmCreateSeparator(parent_id, "separator",
		arglist,(Cardinal)n);
	XtManageChild(w);
	return(w);
}

void gui_create_main_menu( Widget mb )
{
Widget cb;
Widget mn;
Widget bt;
XmString xms;
    /* File */
    mn = XmCreatePulldownMenu ( mb, "FileMB", NULL, 0 );
    cb = XmCreateCascadeButton(mb, "File", NULL, 0);
    XtVaSetValues ( cb, 
        XmNsubMenuId,       mn,
        NULL);
    XtManageChild ( cb );
    /*
    cb = XtVaCreateManagedWidget ( "File",
        xmCascadeButtonWidgetClass, mb,
        XtVaTypedArg,       XmNmnemonic, XmRString, "F", 2,
        XmNsubMenuId,       mn,
        NULL);
    */
        XtVaSetValues ( mn,
            XmNradioAlwaysOne, True,
            XmNradioBehavior, True,
            NULL);
        XtVaSetValues ( cb,
	    XmNfontList,            did_default_font,
            NULL);
        /* buttons */
        xms = create_str ("Exit DID");
	/*
        bt = XtVaCreateManagedWidget ( "MenuExitButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>C",
            XmNacceleratorText, xma,
            NULL);
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)did_exit, 0 );
	/*
    util_recolor ( XtParent(mn) );
	*/
    /* View */
    mn = XmCreatePulldownMenu ( mb, "ViewMB", NULL, 0 );
    cb = XmCreateCascadeButton(mb, "View", NULL, 0);
    XtVaSetValues ( cb, 
        XmNsubMenuId,       mn,
        NULL);
    XtVaSetValues ( cb,
        XmNfontList,            did_default_font,
        NULL);
    XtManageChild ( cb );
    /*
    cb = XtVaCreateManagedWidget ( "View",
        xmCascadeButtonWidgetClass, mb,
        XtVaTypedArg,       XmNmnemonic, XmRString, "V", 2,
        XmNsubMenuId,       mn,
        NULL);
    */
        XtVaSetValues ( mn,
            XmNradioAlwaysOne, True,
            XmNradioBehavior, True,
            NULL);
        /* buttons */

        xms = create_str ("All Servers");
	/*
        bt = XtVaCreateManagedWidget ( "V_MenuAllButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>A",
            XmNacceleratorText, xma,
            NULL);
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)view_opts, 
			(XtPointer)1 );

        xms = create_str ("Servers by Node");
	/*
        bt = XtVaCreateManagedWidget ( "V_MenuNodeButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>N",
            XmNacceleratorText, xma,
            NULL);
        XmStringFree ( xms );
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)view_opts, 
			(XtPointer)0 );

        xms = create_str ("Servers by Service");
	/*
        bt = XtVaCreateManagedWidget ( "V_MenuServiceButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>S",
            XmNacceleratorText, xma,
            NULL);
        XmStringFree ( xms );
        XmStringFree ( xma );
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)view_opts, 
			(XtPointer)2 );

	create_separator(mn);
	/*
        bt = XtVaCreateManagedWidget ( "W_MenuSep",
            xmSeparatorGadgetClass, mn,
            NULL);
	*/
        xms = create_str ("Servers in Error");
	/*
        bt = XtVaCreateManagedWidget ( "V_MenuErrorButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>E",
            XmNacceleratorText, xma,
            NULL);
        XmStringFree ( xms );
        XmStringFree ( xma );
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)view_opts, 
			(XtPointer)3 );
	/*
    util_recolor ( XtParent(mn) );
	*/
    /* Commands */
    mn = XmCreatePulldownMenu ( mb, "CommandMB", NULL, 0 );
    cb = XmCreateCascadeButton(mb, "Commands", NULL, 0);
    XtVaSetValues ( cb, 
        XmNsubMenuId,       mn,
        NULL);
    XtVaSetValues ( cb,
	XmNfontList,            did_default_font,
        NULL);
    XtManageChild ( cb );
    /*
    cb = XtVaCreateManagedWidget ( "Commands",
        xmCascadeButtonWidgetClass, mb,
        XtVaTypedArg,       XmNmnemonic, XmRString, "C", 2,
        XmNsubMenuId,       mn,
        XmNsensitive, commands_enable,
        NULL);
    */
        /* buttons */
        /* Utils */
    /*
        xms = util_create_str ("Show Command Buttons");
        xma = util_create_str ("Ctrl+B");
        bt = XtVaCreateManagedWidget ( "C_ShowCmndButt",
            xmToggleButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>B",
            XmNacceleratorText, xma,
            XmNset, False,
            XmNindicatorSize,12,
            XmNvisibleWhenOff, True,
            NULL);
        XmStringFree ( xms );
        XmStringFree ( xma );
        XtAddCallback ( bt, XmNvalueChangedCallback, (XtCallbackProc)show_command_buttons_callback, NULL );

        bt = XtVaCreateManagedWidget ( "W_MenuSep",
            xmSeparatorGadgetClass, mn,
            NULL);
    */
        
        xms = create_str ("LOG Connections");
	/*
        bt = XtVaCreateManagedWidget ( "C_MenuButtonLOG",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>G",
            XmNacceleratorText, xma,
            NULL);
        XmStringFree ( xms );
        XmStringFree ( xma );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control_callback, "log" );
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control, 
			(XtPointer)0 );

        create_separator(mn);
	/*
        bt = XtVaCreateManagedWidget ( "W_MenuSep",
            xmSeparatorGadgetClass, mn,
            NULL);
	*/
        xms = create_str ("Set Debug ON");
	/*
        bt = XtVaCreateManagedWidget ( "C_MenuButtonDON",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "<Key>F2:",
            XmNacceleratorText, xma,
            NULL);
        XmStringFree ( xms );
        XmStringFree ( xma );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control_callback, "on" );
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control, 
			(XtPointer)1 );

        xms = create_str ("Set Debug OFF");
	/*
        bt = XtVaCreateManagedWidget ( "C_MenuButtonDOFF",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "<Key>F3:",
            XmNacceleratorText, xma,
            NULL);
        XmStringFree ( xms );
        XmStringFree ( xma );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control_callback, "off" );
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control, 
			(XtPointer)2 );

	create_separator(mn);
	/*
        bt = XtVaCreateManagedWidget ( "W_MenuSep",
            xmSeparatorGadgetClass, mn,
            NULL);
	*/
        xms = create_str ("Print Hash Table");
	/*
        bt = XtVaCreateManagedWidget ( "C_MenuButtonPrint",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>T",
            XmNacceleratorText, xma,
            NULL);
        XmStringFree ( xms );
        XmStringFree ( xma );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control_callback, "hash" );
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control, 
			(XtPointer)4 );
	/* kill
	create_separator(mn);
	*/
	/*
        bt = XtVaCreateManagedWidget ( "W_MenuSep",
            xmSeparatorGadgetClass, mn,
            NULL);
	*/
	/* kill
        xms = create_str ("Kill DIM Servers");
	*/
	/*
        bt = XtVaCreateManagedWidget ( "C_MenuButtonKill",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>K",
            XmNacceleratorText, xma,
            NULL);
        XmStringFree ( xms );
        XmStringFree ( xma );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control_callback, "kill" );
	*/
	/* kill
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)dns_control, 
			(XtPointer)3 );
	*/
	/*
    util_recolor ( XtParent(mn) );
	*/
    /* Help */
    mn = XmCreatePulldownMenu ( mb, "HelpMB", NULL, 0 );
    /*
    cb = XtVaCreateManagedWidget ( "Help",
        xmCascadeButtonWidgetClass, mb,
        XtVaTypedArg,       XmNmnemonic, XmRString, "H", 2,
        XmNsubMenuId,       mn,
        NULL);
    */
    cb = XmCreateCascadeButton ( mb, "Help", NULL, 0 );
    XtVaSetValues( cb,
	XmNsubMenuId,       mn,
	XmNfontList,            did_default_font,
        NULL);
    XtManageChild( cb );

        xms = create_str ("Help");
	/*
        bt = XtVaCreateManagedWidget ( "C_XDAbout",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>H",
            XmNacceleratorText, xma,
            NULL);
	*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            did_default_font,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
	/*
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)about_xd_callback, NULL );
	*/
    /* set help menu */
    XtVaSetValues ( mb, XmNmenuHelpWidget, cb, NULL );
    /*
    util_recolor ( XtParent(mn) );
    */
}


Widget gui_toplevel(char **argv)
{
int n;
Arg arglist[6];

    n = 0;
    XtSetArg ( arglist[n], XmNallowShellResize, True); n++;
    XtSetArg ( arglist[n], XmNiconName, "DID"); n++;
    XtSetArg ( arglist[n], XmNtitle, "xDid");  n++;
    XtSetArg ( arglist[n], XmNtraversalOn,True); n++;
    return XtAppCreateShell(argv[0], NULL, applicationShellWidgetClass,
                            display, arglist, (Cardinal)n);
     
}

Widget gui_initialize (int argc, char **argv)
{
Widget toplevel;
void gui_create_main_window();

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    display = XtOpenDisplay(app_context, NULL, argv[0], "DID",
                            NULL, 0, &argc, argv);
    if (display == NULL) 
	{
        printf("%s:  Can't open display\n", argv[0]);
        exit(1);
	}
    toplevel = gui_toplevel(argv);
   
    did_default_font = (XmFontList)util_get_font(DEFAULT_FONT, toplevel);
    did_small_font = (XmFontList)util_get_font(MENU_FONT, toplevel);
    did_label_font = (XmFontList)util_get_font(LABEL_FONT, toplevel);
    did_server_font = (XmFontList)util_get_font(SERVER_FONT, toplevel);

    gui_create_main_window(toplevel);

    XtRealizeWidget ( toplevel );
    return toplevel;
}

int main(int argc, char *argv[])
{
    int i;
	char opt_str[20], *ptr;
	XtInputMask mask;
	void do_got_service_list();
	void do_show_clients();
	void app_initialize();
       
	dim_no_threads();
	dic_disable_padding();
	dis_disable_padding();
	
	if(argc > 1)
	{
		if(argv[1][0] == '-')
		{
			sprintf(opt_str,"%s",&argv[1][1]);
			if((!strncmp(opt_str,"node",4)) || 
			   (!strncmp(opt_str,"NODE",4)))
				Curr_view_opt = 0;
			else if((!strncmp(opt_str,"all",3)) || 
				(!strncmp(opt_str,"ALL",3)))
				Curr_view_opt = 1;
			else if((!strncmp(opt_str,"dns",3)) || 
				(!strncmp(opt_str,"DNS",3))) {
                                char text[132];
			        sprintf(text,"DIM_DNS_NODE=%s",opt_str+4);
   		  	        putenv(text);
 			        dim_set_dns_node(opt_str+4);
			}
			else if((!strncmp(opt_str,"service",7)) || 
					(!strncmp(opt_str,"SERVICE",7)))
				Curr_view_opt = 2;
			else if((!strncmp(opt_str,"error",5)) ||
					(!strncmp(opt_str,"ERROR",5)))
				Curr_view_opt = 3;
			else if((!strncmp(opt_str,"help",4)) || 
				(!strncmp(opt_str,"HELP",4)))
			  {
    printf("Did - DIM Information Display\n");
    printf("\t-all             Show ALL Servers\n");
    printf("\t-dns=<str>       Show Servers with DIM_DNS_NODE provided by <str>\n");
    printf("\t-service=<str>   Show Servers providing Service <str>\n");
    printf("\t-node=<nodename> Show Servers on Node <nodename>\n");
    printf("\t-error           Show Servers in Error\n");
    printf("\t-help            Show this message\n\n");
    exit(0);
			  }
			else
				Curr_view_opt = -1;
			if((Curr_view_opt == 0) || (Curr_view_opt == 2))
			{
  				if(!(ptr = strchr(argv[1],'=')))
				{
					if( (ptr = strchr(argv[2],'=')) )
					{
						ptr++;
						if(!(*ptr))
							ptr = argv[3];
					}
					else
						ptr++;
				}
				else
				{			
					ptr++;
					if(!(*ptr))
						ptr = argv[2];
				}
				for(i = 0;*ptr; ptr++, i++)
					Curr_view_opt_par[i] = (char)toupper((int)*ptr);
				Curr_view_opt_par[i] = '\0';
			}
		}
	}

    toplevel_widget = (Widget)gui_initialize(argc, argv);
    app_initialize();
    /* 
     * Sit around forever waiting to process X-events.  We never leave
     * XtAppMainLoop. From here on, we only execute our callback routines. 
     */

    while(1)
    {
		{
			DISABLE_AST
			mask = XtAppPending(app_context);	
			ENABLE_AST
		}
		if(mask)
		{
			DISABLE_AST
			XtAppProcessEvent(app_context, mask);
			if(Got_Service_List)
			{
				do_got_service_list(Got_Service_List);
				Got_Service_List = 0;
			}
			if(Got_Client_List)
			{
				do_show_clients(Got_Client_List);
				Got_Client_List = 0;
			}
			ENABLE_AST
		}		
		else
		{
			dim_usleep(100000);
			/*
			usleep(100000);	
			*/
		}
    }

}

static char no_link = -1;

void app_initialize(int tag)
{
void check_put_label();

void update_servers();
void update_servers_new();
void update_show_servers();
extern void get_all_colors();
extern void set_title();
extern void set_icon_title();
char dns_node[64];
int dns_port;
char title[128],icon_title[128];

	if(tag){}
	dic_get_dns_node(dns_node);
    dns_port = dic_get_dns_port();
	if(dns_port != DNS_PORT)
	{
		sprintf(title,"DID - DIM Information Display DNS=%s:%d",dns_node,dns_port);
	}
	else
	{
		sprintf(title,"DID - DIM Information Display DNS=%s",dns_node);
	}
	sprintf(icon_title,"DID %s",dns_node);
	get_all_colors(display,Matrix_id[Curr_matrix]);
	set_title(toplevel_widget,title);
	set_icon_title(toplevel_widget,icon_title);
	Timer_q = dtq_create();
	dic_info_service("DIS_DNS/SERVER_INFO",MONITORED,0,0,0,update_servers,0,
						&no_link,1);
	/*
      	dic_info_service("DIS_DNS/SERVER_LIST",MONITORED,0,0,0,
			 update_servers_new,0, &no_link,1);
	*/
	/*	
	dtq_add_entry(Timer_q, 2, check_put_label, 0);
	*/
	XtAppAddTimeOut(app_context, 1000, update_show_servers, 0); 
}

/*
 * All errors are fatal.
 */
void s_error(char *problem_string)
{
    printf("%s\n", problem_string);
    exit(0);
}

void did_exit(Widget w, int *tag, unsigned long *reason)
{
	if(w){}
	if(tag){}
	if(reason){}
	exit(0);
}

extern Pixel rgb_colors[MAX_COLORS];

void create_main (Widget w, int *tag, unsigned long *reason)
{
	if(tag){}
	if(reason){}
	Window_id = w;
/*
	dtq_start_timer(5, app_initialize, 0);
*/
}

void view_opts(Widget w, int tag, unsigned long *reason)
{
	void get_server_node(), get_server_service(), show_servers();

	if(w){}
	if(reason){}
	Curr_view_opt = tag;
	switch(tag)
	{
		case 0 :
			get_server_node();
			break;
		case 1 :
			show_servers();
			break;
		case 2 :
			get_server_service();
			break;
		case 3 :
			show_servers();
			break;
	}
}

void dns_control(Widget w, int tag, unsigned long *reason)
{

	if(w){}
	if(reason){}
	switch(tag)
	{
		case 0 :
			dic_cmnd_service("DIS_DNS/PRINT_STATS",0,0);
			break;
		case 1 :
			dic_cmnd_service("DIS_DNS/DEBUG_ON",0,0);
			break;
		case 2 :
			dic_cmnd_service("DIS_DNS/DEBUG_OFF",0,0);
			break;
		case 3 :
			put_selection(DID_KILL_ALL,"Confirmation");
			break;
		case 4 :
			dic_cmnd_service("DIS_DNS/PRINT_HASH_TABLE",0,0);
			break;
	}
}

void get_server_node()
{
Widget id,sel_id;
int i, j, n_nodes, curr_index = 0;
char nodes_str[MAX_NODE_NAME*MAX_CONNS*2], max_str[MAX_NODE_NAME];
char *ptr, *nodeptrs[MAX_CONNS*2], *curr_str, *sptr;
int get_nodes();

	sel_id = put_selection(DID_SEL_NODE,"Node Selection");
	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(id);
	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_APPLY_BUTTON);
	XtUnmanageChild(id);
	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
	XmListDeleteAllItems(id);
	n_nodes = get_nodes(nodes_str);
	ptr = nodes_str;

	for(i=0;i<n_nodes;i++)
	{
		nodeptrs[i] = ptr;
		sptr = ptr;
		ptr = strchr(ptr,'\n');
		*ptr++ = '\0';
		for(j = 0; j < (int)strlen(sptr); j++)
		  sptr[j] = (char)tolower((int)sptr[j]);
	}
	strcpy(max_str,"zzzzzzzzzzzzzzzzzzzzzzzzzzzz");
	for(i=0;i<n_nodes; i++)
	{
	  curr_str = max_str;
	  for(j=0;j<n_nodes; j++)
	  {
	    sptr = nodeptrs[j];
	    if(!sptr)
	      continue;
	    
	    if(strcmp(sptr,curr_str) < 0)
	    {
	      curr_str = sptr;
	      curr_index = j;
	    }
	  }
	  nodeptrs[curr_index] = 0;
	  XmListAddItem(id,create_str(curr_str),i+1);
	}
	/*
	for(i=0;i<n_nodes;i++)
	{
		node = ptr;
		ptr = strchr(ptr,'\n');
		*ptr++ = '\0';
		XmListAddItem(id,create_str(node),i+1);
	}
	*/
	set_something(id,XmNlistItemCount,i);
	set_something(id,XmNlistVisibleItemCount,(i < 8) ? i : 8);
}	

void get_server_service()
{
Widget id,sel_id;

	sel_id = put_selection(DID_SEL_SERVICE,"Service Selection");
	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(id);
	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_APPLY_BUTTON);
	XtUnmanageChild(id);
	
	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
	/*
	XtUnmanageChild(id);
	*/
	XtUnmapWidget(id);
	
	/*
	id = (Widget)XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST_LABEL);
	XtUnmanageChild(id);
	*/
}	

int get_nodes(char *node_ptr)
{
DNS_SERVER_INFO *ptr;
int n_nodes = 0;
SERVER *servp;

	node_ptr[0] = '\0';
	servp = Server_head;
	while( (servp = (SERVER *)sll_get_next((SLL *)servp)) )
	{
		ptr = &servp->server;
		if(strstr(node_ptr,ptr->node) <= (char *)0)
		{
			strcat(node_ptr,ptr->node);
			strcat(node_ptr,"\n");
			n_nodes++;
		}
	}
	return(n_nodes);
}

void get_service_format()
{

	char str[256], *ptr, *ptr1;
	int rpc_flag;

	strcpy(str,Curr_service_name);
	rpc_flag = 0;
	if( (ptr = strstr(str,"/RpcIn")) )
	{
		*ptr = '\0';
		rpc_flag = 1;
	}
	if( (ptr = strstr(str,"/RpcOut")) )
	{
		*ptr = '\0';
		rpc_flag = 2;
	}
	strcat(str,"|");
	if( (ptr = strstr(Curr_service_list,str)) )
	{
		if(!rpc_flag)
		{
		    ptr += strlen(str);
		    ptr1 = strchr(ptr,'|');
		}
		else if(rpc_flag == 1)
		{
		    ptr += strlen(str);
		    ptr1 = strchr(ptr,',');
		}
		else
		{
		    ptr += strlen(str);
		    ptr = strchr(ptr,',');
		    ptr++;
		    ptr1 = strchr(ptr,'|');
		}
	    strncpy(Curr_service_format,ptr,(size_t)(ptr1 - ptr));
	    Curr_service_format[(int)(ptr1-ptr)] = '\0';
	}
}

void recv_service_info(int *tag, int *buffer, int *size)
{
/*
	char str[256], *ptr, *ptr1;
	int rpc_flag;
*/
	void print_service_formatted();

	if(tag){}
	Service_content_str = malloc((size_t)(1024 + (*size)*16));
	Service_buffer = malloc((size_t)*size);
	memcpy(Service_buffer, (char *)buffer, (size_t)*size);
	Service_size = *size;
	get_service_format();
	if((*size == 4 ) && (*buffer == -1))
	{
		sprintf(Service_content_str,
			"Service %s Not Available\n", Curr_service_name);
	}
	else
	{
	  switch(Curr_service_print_type)
	  {
	  case 0:
		print_service_formatted(buffer,*size);
		break;
		/*
	  case 1:
		print_service_float(buffer, ((*size - 1) / 4) + 1);
		break;
	  case 2:
		print_service_double(buffer, ((*size - 1) / 4) + 1);
		break;
		*/
	  }
	}
	set_something(Content_label_id,XmNlabelString, Service_content_str);
	/*
	if(Matrix_id[Curr_matrix])
	  XFlush(XtDisplay(Matrix_id[Curr_matrix]));
	*/
}
	
void print_service_formatted(void *buff, int size)
{
char type;
int num, ret;
char str[128];
char *ptr;
void *buffer_ptr;
char timestr[128], aux[10];
int quality = 0, secs = 0, mili = 0; 
int did_write_string(char, int, void **, int);
time_t tsecs;

  sprintf(Service_content_str,
	  "Service %s (%s) Contents :\n  \n", Curr_service_name,
	  Curr_service_format);
  /*
  if(Curr_service_id)
  {
  */
    dic_get_timestamp(0, &secs, &mili);
    quality = dic_get_quality(0);
/*
#ifdef LYNXOS
	ctime_r((time_t *)&secs, timestr, 128);
#else
	ctime_r((time_t *)&secs, timestr);
#endif
*/
	tsecs = secs;
	my_ctime(&tsecs, timestr, 128);
    ptr = strrchr(timestr,' ');
    strcpy(aux, ptr);
    sprintf(ptr,".%03d",mili);
    strcat(timestr, aux);
    timestr[strlen(timestr)-1] = '\0';
   
    sprintf(str," Timestamp: %s               Quality: %d\n\n",
	  timestr, quality);

    strcat(Service_content_str,str);
    /*
  }
    */
   ptr = Curr_service_format;
   buffer_ptr = buff;
   while(*ptr)
   { 
     type = *ptr++;
     if(*ptr == ':')
       {
	 ptr++;
	 sscanf(ptr, "%d", &num);
	 ret = did_write_string(type, num, &buffer_ptr, size);
	 size -= ret;
	 if( (ptr = strchr(ptr,';')) )
	   ptr++;
	 else
	   break;
       }
       else
       {
	 ret = did_write_string(type, 0, &buffer_ptr, size);
	 size -= ret;
	 break;
       }
   }
}

int did_write_string(char type, int num, void **buffer_ptr, int ssize)
{
void *ptr;
int size, psize;

  void print_service_standard();
  void print_service_char();
  void print_service_short();
  void print_service_float();
  void print_service_double();

  ptr = *buffer_ptr;
  switch(type)
    {
    case 'L':
    case 'l':
      strcat(Service_content_str," L");
      if(!num)
	size = ssize/(int)sizeof(int);
      else
	size = num;
      psize = size * (int)sizeof(int);
      print_service_standard(ptr, size);
      break;
    case 'I':
    case 'i':
      strcat(Service_content_str," I");
      if(!num)
	size = ssize/(int)sizeof(int);
      else
	size = num;
      psize = size * (int)sizeof(int);
      print_service_standard(ptr, size);
      break;
    case 'S':
    case 's':
      strcat(Service_content_str," S");
      if(!num)
	size = ssize/(int)sizeof(short);
      else
	size = num;
      psize = size * (int)sizeof(short);
      print_service_short(ptr, size);
      break;
    case 'F':
    case 'f':
      strcat(Service_content_str," F");
      if(!num)
	size = ssize/(int)sizeof(float);
      else
	size = num;
      psize = size * (int)sizeof(float);
      print_service_float(ptr, size);
      break;
    case 'D':
    case 'd':
      strcat(Service_content_str," D");
      if(!num)
	size = ssize/(int)sizeof(double);
      else
	size = num;
      psize = size * (int)sizeof(double);
      print_service_double(ptr, size);
      break;
    case 'X':
    case 'x':
      strcat(Service_content_str," X");
      if(!num)
	size = ssize/(int)sizeof(longlong);
      else
	size = num;
      psize = size * (int)sizeof(longlong);
      print_service_standard(ptr, size*2);
      break;
    case 'C':
    case 'c':
    default:
      strcat(Service_content_str," C");
      if(!num)
	size = ssize;
      else
	size = num;
      psize = size;
      print_service_char(ptr, size);
    }
  ptr = (char *)ptr + psize;
  *buffer_ptr = ptr;
  return psize;
}
/*
print_service(buff, size)
int *buff, size;
{
int i,j, str_flag = 0;
char *asc, *ptr, str[80];
int last[4];

	sprintf(Service_content_str,
		"Service %s (%s) Contents :\n  \n", Curr_service_name,
		Curr_service_format);
	asc = (char *)buff;
	for( i = 0; i < size; i++)
	{
		if(i%4 == 0)
		{
			sprintf(str,"%4d: ",i);
			strcat(Service_content_str,str);
		}
		if(!(i%4))
			strcat(Service_content_str,"H");
		sprintf(str,"   %08X ",buff[i]);
		strcat(Service_content_str,str);
		last[i%4] = buff[i];
		if(i%4 == 3)
		{
			strcat(Service_content_str,"   '");
			for(j = 0; j <16; j++)
			{
				if(isprint(asc[j]))
				{
					sprintf(str,"%c",asc[j]);
					strcat(Service_content_str,str);
				}
				else
				{
					sprintf(str,".");
					strcat(Service_content_str,str);
				}
			}
			strcat(Service_content_str,"'\n");
			for(j = 0; j <4; j++)
			{
				if(j == 0)
					strcat(Service_content_str,"      D");
				sprintf(str,"%11d ",last[j]);
				strcat(Service_content_str,str);
			}
			strcat(Service_content_str,"\n");
			asc = (char *)&buff[i+1];
		}
	}
	if(i%4)
	{
			for(j = 0; j < 4 - (i%4); j++)
				strcat(Service_content_str,"            ");
			strcat(Service_content_str,"   '");
			for(j = 0; j < (i%4) * 4; j++)
			{
				if(isprint(asc[j]))
				{
					sprintf(str,"%c",asc[j]);
					strcat(Service_content_str,str);
				}
				else
					strcat(Service_content_str,".");
			}
			strcat(Service_content_str,"'\n");
			for(j = 0; j < (i%4); j++)
			{
				if(j == 0)
					strcat(Service_content_str,"      D");
				sprintf(str,"%11d ",last[j]);
				strcat(Service_content_str,str);
			}
			strcat(Service_content_str,"\n");
	}
}
*/

void print_service_standard(int *buff, int size)
{
int i,j;
char *ptr, str[80], tmp[256];
int last[4];
/*
char *asc;
	asc = (char *)buff;
*/
	ptr = Service_content_str;
	ptr += strlen(Service_content_str);
	for( i = 0; i < size; i++)
	{
	  strcpy(tmp,"");
		if(i%4 == 0)
		{
		  if(i != 0)
		    {
			strcat(tmp,"  ");
		    }
			sprintf(str,"%5d ",i);
			strcat(tmp,str);
		}
		if(!(i%4))
			strcat(tmp,"H: ");
		sprintf(str,"    %08X",buff[i]);
		strcat(tmp,str);
		last[i%4] = buff[i];
		if((i%4 == 3) || (i == (size-1)))
		{
		  /*
			if(i%4 != 3)
			{
			    for(j = 1; j < 4 - (i%4); j++)
				strcat(tmp,"            ");
			}
			strcat(tmp,"  '");
			for(j = 0; j < ((i%4)*4)+4 ; j++)
			{
				if(isprint(asc[j]))
				{
					sprintf(str,"%c",asc[j]);
					strcat(tmp,str);
				}
				else
				{
					sprintf(str,".");
					strcat(tmp,str);
				}
			}
		  */
			strcat(tmp,"\n");
			for(j = 0; j <= (i%4); j++)
			{
				if(j == 0)
					strcat(tmp,"        D: ");
				sprintf(str,"%12d",last[j]);
				strcat(tmp,str);
			}
			strcat(tmp,"\n");
/*
			asc = (char *)&buff[i+1];
*/
		}
		strcpy(ptr, tmp);
		ptr += strlen(tmp);
	}
	strcpy(tmp,"\n");
	strcpy(ptr, tmp);
}

void print_service_longlong(longlong *buff, int size)
{
int i,j;
char *ptr, str[80], tmp[256];
longlong last[4];
/*
char *asc;
	asc = (char *)buff;
*/
	ptr = Service_content_str;
	ptr += strlen(Service_content_str);
	for( i = 0; i < size; i++)
	{
	  strcpy(tmp,"");
		if(i%4 == 0)
		{
		  if(i != 0)
		    {
			strcat(tmp,"  ");
		    }
			sprintf(str,"%5d ",i);
			strcat(tmp,str);
		}
		if(!(i%4))
			strcat(tmp,"H: ");
		sprintf(str,"    %08X",(unsigned)buff[i]);
		strcat(tmp,str);
		last[i%4] = buff[i];
		if((i%4 == 3) || (i == (size-1)))
		{
			strcat(tmp,"\n");
			for(j = 0; j <= (i%4); j++)
			{
				if(j == 0)
					strcat(tmp,"        D: ");
				sprintf(str,"%12d",(int)last[j]);
				strcat(tmp,str);
			}
			strcat(tmp,"\n");
/*
			asc = (char *)&buff[i+1];
*/
		}
		strcpy(ptr, tmp);
		ptr += strlen(tmp);
	}
	strcpy(tmp,"\n");
	strcpy(ptr, tmp);
}

void print_service_short(short *buff, int size)
{
int i,j;
char *ptr, str[80], tmp[256];
short last[8];
/*
char *asc; 
	asc = (char *)buff;
*/
	ptr = Service_content_str;
	ptr += strlen(Service_content_str);
	for( i = 0; i < size; i++)
	{
	  strcpy(tmp,"");
		if(i%8 == 0)
		{
		  if(i != 0)
		    {
			strcat(tmp,"  ");
		    }
			sprintf(str,"%5d ",i);
			strcat(tmp,str);
		}
		if(!(i%8))
			strcat(tmp,"H: ");
		sprintf(str,"  %04X",buff[i]);
		strcat(tmp,str);
		last[i%8] = buff[i];
		if((i%8 == 7) || (i == (size-1)))
		{
		  /*
			if(i%7 != 7)
			{
			    for(j = 1; j < 8 - (i%8); j++)
				strcat(tmp,"      ");
			}
			strcat(tmp,"  '");
			for(j = 0; j < ((i%8)*2)+2 ; j++)
			{
				if(isprint(asc[j]))
				{
					sprintf(str,"%c",asc[j]);
					strcat(tmp,str);
				}
				else
				{
					sprintf(str,".");
					strcat(tmp,str);
				}
			}
		  */
			strcat(tmp,"\n");
			for(j = 0; j <= (i%8); j++)
			{
				if(j == 0)
					strcat(tmp,"        D: ");
				sprintf(str," %5d",last[j]);
				strcat(tmp,str);
			}
			strcat(tmp,"\n");
/*
			asc = (char *)&buff[i+1];
*/
		}
		strcpy(ptr, tmp);
		ptr += strlen(tmp);
	}
	strcpy(tmp,"\n");
	strcpy(ptr, tmp);
}

void print_service_char(char *buff, int size)
{
int i,j;
char *asc, *ptr, str[80], tmp[256];
/*
char last[16];
*/
	asc = (char *)buff;
	ptr = Service_content_str;
	ptr += strlen(Service_content_str);
	for( i = 0; i < size; i++)
	{
	  strcpy(tmp,"");
		if(i%16 == 0)
		{
		  if(i != 0)
		    {
			strcat(tmp,"  ");
		    }
			sprintf(str,"%5d ",i);
			strcat(tmp,str);
		}
		if(!(i%16))
			strcat(tmp,"H: ");
		sprintf(str,"%02X",buff[i]);
/*		strcat(tmp,str);
*/
		strcat(tmp," ");
		strcat(tmp,&str[strlen(str)-2]);
		/*
		last[i%16] = buff[i];
		if(i%4 == 3)
		  strcat(tmp," ");
		*/
		if((i%16 == 15) || (i == (size-1)))
		{
			if(i%16 != 15)
			{
			    for(j = 1; j < 16 - (i%16); j++)
				strcat(tmp,"   ");
			}
			strcat(tmp,"    '");
			for(j = 0; j <= (i%16) ; j++)
			{
				if(isprint(asc[j]))
				{
					sprintf(str,"%c",asc[j]);
					strcat(tmp,str);
				}
				else
				{
					sprintf(str,".");
					strcat(tmp,str);
				}
			}
			strcat(tmp,"'\n");
			asc = (char *)&buff[i+1];
		}
		strcpy(ptr, tmp);
		ptr += strlen(tmp);
	}
	strcpy(tmp,"\n");
	strcpy(ptr, tmp);
}

void print_service_float(float *buff, int size)
{
int i;
char *ptr, str[80], tmp[256];

	ptr = Service_content_str;
	ptr += strlen(Service_content_str);
	for( i = 0; i < size; i++)
	{
	  strcpy(tmp,"");
		if(i%4 == 0)
		{
		  if(i != 0)
		    {
			strcat(tmp,"  ");
		    }
			sprintf(str,"  %5d: ",i);
			strcat(tmp,str);
		}
		sprintf(str,"%12.3G",*(buff++));
		strcat(tmp,str);
		if((i%4 == 3) || (i == size-1))
		{
			strcat(tmp,"\n");
		}
		strcpy(ptr, tmp);
		ptr += strlen(tmp);
	}
	strcpy(tmp,"\n");
	strcpy(ptr, tmp);
	ptr += strlen(tmp);
}

void print_service_double(double *buff, int size)
{
int i;
char *ptr, str[80], tmp[256];

       	ptr = Service_content_str;
	ptr += strlen(Service_content_str);
	for( i = 0; i < size; i++)
	{
	  strcpy(tmp,"");
		if(i%4 == 0)
		{
		  if(i != 0)
		    {
			strcat(tmp,"  ");
		    }
			sprintf(str,"  %5d: ",i);
			strcat(tmp,str);
		}
		sprintf(str,"%12.3G",*(buff++));
		strcat(tmp,str);
		if((i%4 == 3) || (i == size-1))
		{
			strcat(tmp,"\n");
		}
		strcpy(ptr, tmp);
		ptr += strlen(tmp);
	}
	strcpy(tmp,"\n");
	strcpy(ptr, tmp);
	ptr += strlen(tmp);
}

void ok_pop_up (Widget w, long tag, unsigned long *reason)
{
Widget id, sel_id;
char *str, *pstr;
void recv_service_info();
void did_prepare_command();
void show_servers();

/*
	if(tag == 5)
	{
		id = (Widget)XmSelectionBoxGetChild(w,XmDIALOG_TEXT);
		str = (char *)XmTextGetString(id);
		if(!str[0])
		{
			XtFree(str);
			return;
		}
		if( ( fptr = fopen( str, "w" ) ) == (FILE *)0 )
		{
    		printf("Cannot open: %s for writing\n",str);
			return;
		}                   
		ptr = &Curr_servp->server;
		if (ptr->pid > 0x1000000)
			fprintf(fptr,"Server %s (pid = %X) on node %s\n    provides %d services :\n",
			Curr_servp->name, ptr->pid, ptr->node, ptr->n_services);
		else
			fprintf(fptr,"Server %s (pid = %d) on node %s\n    provides %d services :\n",
				Curr_servp->name, ptr->pid, ptr->node, ptr->n_services);
		service_ptr = Curr_servp->service_ptr;
		for(i=0;i<ptr->n_services; i++)
		{
			sprintf(str,service_ptr->name);
			fprintf(fptr,"        %s\n",service_ptr->name);
			service_ptr++;
		}		
		fclose(fptr);
		XtFree(str);
		return;
	}
	if(tag == 4)
	{
		sel_id = put_selection(4, "Printing...");
		id = (Widget)XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
		XtUnmanageChild(id);

		id = (Widget)XmSelectionBoxGetChild(sel_id,XmDIALOG_APPLY_BUTTON);
		XtUnmanageChild(id);
		id = (Widget)XmSelectionBoxGetChild(sel_id,XmDIALOG_TEXT);
		str = (char *)XmTextGetString(id);
		if(!str[0])
		{
			XtFree(str);
			return;
		}
		ptr = &Curr_servp->server;
		if(pstr = strrchr(str,']'))
			*(++pstr) = '\0';
		if(pstr = strrchr(str,'/'))
			*(++pstr) = '\0';
		sprintf(txt_str,"%s%s.TXT",str,Curr_servp->name);
		XtFree(str);
		XmTextSetString(id, txt_str);
		return;
	}
*/
	if(reason){}
	if(tag == DID_KILL_ALL)
	{
		dic_cmnd_service("DIS_DNS/KILL_SERVERS",0,0);
		return;
	}
	id = XmSelectionBoxGetChild(w,XmDIALOG_TEXT);
	str = XmTextGetString(id);
	if(!str[0])
	{
		XtFree(str);
		return;
	}
    if ((tag == DID_SEL_NODE) || (tag == DID_SEL_SERVICE)) 
	{
		strcpy(Curr_view_opt_par, str);
		show_servers();
		XtFree(str);
	}
    if(tag == DID_SERVICES)
	{
	  pstr = strchr(str,' ');
	  if(!pstr)
	    {
	      strcpy(Curr_service_name, str);
	      strcpy(str,"SVC");
	    }
	  else
	    {
	      pstr++;
	      strcpy(Curr_service_name, pstr);
	    }
	  if(Curr_service_id)
	    {
	      dic_release_service(Curr_service_id);
	      Curr_service_id = 0;
	    }
	  if(str[0] == 'S')
	    {
	      /*
	      if((!strstr(pstr,"/SERVICE_LIST")) && 
		 (!strstr(pstr,"/CLIENT_LIST")) &&
		 (!strstr(pstr,"/SERVER_LIST")))
		{
		  Curr_service_id = dic_info_service(Curr_service_name,
		                             MONITORED,5,0,0,
					     recv_service_info,0,
						     &no_link_int,4);
		}
	      else
		{
	      */
	      dic_info_service_stamped(Curr_service_name,
							 ONCE_ONLY,10,0,0,
					     recv_service_info,0,
						     &no_link_int,4);
		  /*
		}
		  */
	      put_selection(DID_SERVICE,"Service Contents");
	    }
	  else
	    {
	      get_service_format();
	      sel_id = put_selection(DID_COMMAND,"Send Command");
	      id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
	      XtUnmanageChild(id);
	      id = XmSelectionBoxGetChild(sel_id,XmDIALOG_APPLY_BUTTON);
	      XtUnmanageChild(id);
	      id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
	      /*
	      XtUnmanageChild(id);
	      */
	      XtUnmapWidget(id);
	    }
	  XtFree(str);
	}
    if(tag == DID_COMMAND)
	{
	  did_prepare_command(str);
	  XtFree(str);
	}
}

int get_type_size(char type)
{
  int size;

  switch(type)
    {
    case 'L':
    case 'l':
      size = sizeof(long);
      break;
    case 'I':
    case 'i':
      size = sizeof(int);
      break;
    case 'S':
    case 's':
      size = sizeof(short);
      break;
    case 'F':
    case 'f':
      size = sizeof(float);
      break;
    case 'D':
    case 'd':
      size = sizeof(double);
      break;
    case 'C':
    case 'c':
    default:
      size = 1;
    }
  return(size);
}

void did_prepare_command(char *str)
{
char type;
int num;
int size, full_size = 0;
char *ptr;
static int last_size = 0;
static void *last_buffer = 0;
void *buffer_ptr;
char *str_ptr;
void did_read_string(char, int, void **, char **);

   str_ptr = str; 
   ptr = Curr_service_format; 
   while(*ptr)
   { 
     type = *ptr++;
     if(*ptr == ':')
       {
	 ptr++;
	 size = get_type_size(type);
	 sscanf(ptr, "%d", &num);
	 full_size += size * num;
	 if( (ptr = strchr(ptr,';')) )
	   ptr++;
	 else
	   break;
       }
   }

   full_size += 256;
   if(full_size > last_size)
   {
      if(last_size)
	free(last_buffer);
      last_buffer = malloc((size_t)full_size);
      last_size = full_size;
   }
   buffer_ptr = last_buffer;
   ptr = Curr_service_format; 
   while(*ptr)
   { 
     type = *ptr++;
     if(*ptr == ':')
       {
	 ptr++;
	 sscanf(ptr, "%d", &num);
	 did_read_string(type, num, &buffer_ptr, &str_ptr);  
	 if(!str_ptr)
	     break;
	 if( (ptr = strchr(ptr,';')) )
	   ptr++;
	 else
	   break;
       }
       else
       {
	 did_read_string(type, 0, &buffer_ptr, &str_ptr);
	 break;
       }
   }
   full_size = (int) ((char *)buffer_ptr - (char *)last_buffer);
   dic_cmnd_service(Curr_service_name,last_buffer,full_size);
}

int read_str_int(char *str)
{
  int i;
  if((str[0] == '0') && (str[1] == 'x'))
    sscanf(str+2,"%x",&i);
  else
    sscanf(str,"%d",&i);
  return(i);
}

int read_str_char(char *str, char *cc)
{

  if(str[0] == '\'')
    *cc = str[1];
  else if(str[0] == '\"')
    return(0);
  else if((str[0] == '0') && (str[1] == 'x'))
    sscanf(str+2,"%x",(int *)cc);
  else if(isalpha(str[0]))
    return(-1);
  else
    sscanf(str,"%d",(int *)cc);
  return(1);
}

void did_read_string(char type, int num, void **buffer_ptr, char **str_ptr)
{
int i, ret = 0;
float ff;
double dd;
void *ptr;
char *strp, *ptr1;
char cc;
 short s;

  strp = *str_ptr; 
  ptr = *buffer_ptr;
  if(!num)
    num = 1000000;
  switch(type)
    {
    case 'L':
    case 'l':
    case 'I':
    case 'i':
      for(i = 0; i<num; i++)
      {
	*(int *)ptr = read_str_int(strp);
	ptr = (int *)ptr +1;
	if( (strp = strchr(strp,' ')) )
	  strp++;
	else
	  break;
      }
      break;
    case 'S':
    case 's':
      for(i = 0; i<num; i++)
      {
	s = (short)read_str_int(strp);
	*((short *)ptr) = s;
	ptr = (short *)ptr +1;
	if( (strp = strchr(strp,' ')) )
	  strp++;
	else
	  break;
      }
      break;
    case 'F':
    case 'f':
      for(i = 0; i<num; i++)
      {
	sscanf(strp,"%f",&ff);
	*(float *)ptr = ff;
	ptr = (float *)ptr +1;
	if( (strp = strchr(strp,' ')) )
	  strp++;
	else
	  break;
      }
      break;
    case 'D':
    case 'd':
      for(i = 0; i<num; i++)
      {
	sscanf(strp,"%f",&ff);
	dd = (double)ff;
	*(double *)ptr = dd;
	ptr = (double *)ptr +1;
	if( (strp = strchr(strp,' ')) )
	  strp++;
	else
	  break;
      }
      break;
    case 'C':
    case 'c':
    default:
      for(i = 0; i<num; i++)
      {
	if((ret = read_str_char(strp, &cc)) <= 0)
	  break;
	*(char *)ptr = cc;
	ptr = (char *)ptr +1;
	if( (strp = strchr(strp,' ')) )
	  strp++;
	else
	  break;
      }
      if(ret <= 0)
      {
	if(!ret)
	{
	  strp++;
	}
	num = (int)strlen(strp)+1;
	strncpy((char *)ptr,strp,(size_t)num);
	if( (ptr1 = (char *)strchr((char *)ptr,'\"')) )
	{
	  num--;
	  *ptr1 = '\0';
	}
	ptr = (char *)ptr + num;
	if( (strp = strchr(strp,' ')) )
	  strp++;
	else
	  break;
      }
    }
  *buffer_ptr = ptr;
  *str_ptr = strp;
}

void cancel_pop_up (Widget w, int tag, unsigned long *reason)
{
	void print_service_formatted();

	if(reason){}
	if(tag == MAX_POP_UPS+1)
	{
	  print_service_formatted(Service_buffer,Service_size);
		set_something(Content_label_id,XmNlabelString, Service_content_str);
		Curr_service_print_type = 0;
	}
	/*
	else if(tag == MAX_POP_UPS+2)
	{
		print_service_float(Service_buffer, ((Service_size - 1) / 4) + 1);
		set_something(Content_label_id,XmNlabelString, Service_content_str);
		Curr_service_print_type = 1;
	}
	else if(tag == MAX_POP_UPS+3)
	{
		print_service_double(Service_buffer, ((Service_size - 1) / 4) + 1);
		set_something(Content_label_id,XmNlabelString, Service_content_str);
		Curr_service_print_type = 2;
	}
	*/
	else if(tag == MAX_POP_UPS+4)
	{

	      if((!strstr(Curr_service_name,"/SERVICE_LIST")) && 
		 (!strstr(Curr_service_name,"/CLIENT_LIST")) &&
		 (!strstr(Curr_service_name,"/SERVER_LIST")))
		{
		  if(Curr_service_id)
		  {
		      dic_release_service(Curr_service_id);
		      Curr_service_id = 0;
		  }
		  Curr_service_id = (int)dic_info_service_stamped(Curr_service_name,
						     MONITORED,10,0,0,
						     recv_service_info,0,
						     &no_link_int,4);
		}
		XtSetSensitive(w, False);
		XtSetSensitive(SubscribeButton, True);
	}
	else if(tag == MAX_POP_UPS+5)
	{

	      if((!strstr(Curr_service_name,"/SERVICE_LIST")) && 
		 (!strstr(Curr_service_name,"/CLIENT_LIST")) &&
		 (!strstr(Curr_service_name,"/SERVER_LIST")))
		{
		  if(Curr_service_id)
		  {
		      dic_release_service(Curr_service_id);
		      Curr_service_id = 0;
		  }
		  Curr_service_id = (int)dic_info_service_stamped(Curr_service_name,
						     MONITORED,0,0,0,
						     recv_service_info,0,
						     &no_link_int,4);
		}
		XtSetSensitive(w, False);
		XtSetSensitive(Subscribe10Button, True);
	}
/*
	else if(tag == 5)
	{
	  *
		XtUnmapWidget(XtParent(pop_widget_id[4]));
	  *
	}
*/
	else if(tag == DID_SERVICE)
	{
	  if(Curr_service_id)
	    {
	      dic_release_service(Curr_service_id);
	      Curr_service_id = 0;
	    }
            XtUnmanageChild(pop_widget_id[DID_SERVICE]);
	    free(Service_content_str);
	    free(Service_buffer);
	}
}

void create_matrix(Widget w, int *tag, unsigned long *reason)
{

	if(reason){}
	Matrix_id[*tag] = w;
	if(*tag)
		XtUnmanageChild(w);
	else
		Curr_matrix = 0;
}

void create_label(Widget w, int *tag, unsigned long *reason)
{
	if(reason){}
	if(!*tag)
		Label_id = w;
	else
		Content_label_id = w;
}

void switch_matrix()
{
	/*
	XtUnmanageChild(Matrix_id[Curr_matrix]);
	Curr_matrix = (Curr_matrix) ? 0 : 1;
	XtManageChild(Matrix_id[Curr_matrix]);
	*/
	XmScrolledWindowSetAreas(Window_id,NULL, NULL, Matrix_id[Curr_matrix]); 
}

/*
static int curr_allocated_size = 0;
static DNS_SERVER_INFO *dns_info_buffer;
*/

void update_servers_new(int *tag, char *buffer, int *size)
{
	if(tag){}
	if(size){}
	printf("Server_list:\n%s\n",buffer);
}

SERVER *find_server(char *node, int pid)
{
  SERVER *servp;
  DNS_SERVER_INFO *ptr;

  servp = Server_head;
  while( (servp = (SERVER *)sll_get_next((SLL *)servp)) )
    {
      ptr = &servp->server;
      if((ptr->pid == pid) && (!strcmp(ptr->node,node)))
	{
	  return(servp);
	}
    }
  return ((SERVER *)0);
}
	  /*
	if(!(servp = (SERVER *)sll_search((SLL *)Server_head, 
		(char *)&buffer->server, MAX_NODE_NAME+MAX_TASK_NAME-4)))
	  */


void update_servers(int *tag, DNS_DID *buffer, int *size)
{
int n_services, service_size;
SERVER *servp;
int j;
char str[MAX_NAME], sname[MAX_NAME], *ptr;

	if(tag){}
	if(!Server_head)
	{
		Server_head = (SERVER *)malloc(sizeof(SERVER));
		sll_init((SLL *)Server_head);
	}
	if(First_time)
	{
		switch_matrix();
		First_time = 0;
	}

	if(!*size)
		return;
	if(*(char *)buffer == -1)
	{
		N_servers = 0;
		N_services = 0;
		return;
	}
	buffer->server.n_services = vtohl(buffer->server.n_services);
	buffer->server.pid = vtohl(buffer->server.pid);
	n_services = buffer->server.n_services;
	/*
printf("received pid %d, nservices %d\n",buffer->server.pid, n_services);
	*/
	if(n_services == 1)
	  return;
	strcpy(sname, buffer->server.task);
	/*
printf("name = %s\n", sname);
	*/
	if(n_services > 1)
	{
		for(j = 0; j < n_services; j++)
		{
			buffer->services[j].type = vtohl(
				buffer->services[j].type);
			buffer->services[j].status = vtohl(
				buffer->services[j].status);
			buffer->services[j].n_clients = vtohl(
				buffer->services[j].n_clients);
			if(strlen(sname) == MAX_TASK_NAME-4-1)
			{
				strcpy(str,buffer->services[j].name);
				if( (ptr = strstr(str,"/CLIENT_LIST")) )
				{
					*ptr = '\0';
					strcpy(sname,str);
				}
			}
		}
	}
	if (!(servp = find_server(buffer->server.node,buffer->server.pid)))
	  /*
	if(!(servp = (SERVER *)sll_search((SLL *)Server_head, 
		(char *)&buffer->server, MAX_NODE_NAME+MAX_TASK_NAME-4)))
	  */
	{
		if(n_services)
		{
			servp = (SERVER *)malloc(sizeof(SERVER));
			strcpy(servp->name,sname);
			servp->next = 0;
			servp->button_id = 0;
			servp->pop_widget_id[0] = 0;
			servp->pop_widget_id[1] = 0;
			servp->busy = 0;
			servp->server.n_services = 0;
			servp->service_ptr = 0;
			sll_insert_queue((SLL *)Server_head,(SLL *)servp);
		}
	}
	if(n_services != 0)
	{
		if(n_services == servp->server.n_services)
		{
			return;
		}
		if(servp->server.n_services == 0)
			N_servers++;
		if(servp->server.n_services != -1)
			N_services -= servp->server.n_services;
		memcpy(&servp->server,&buffer->server,sizeof(DNS_SERVER_INFO));
		if(servp->service_ptr)
		{
			free(servp->service_ptr);
			servp->service_ptr = 0;
		}
		if(n_services != -1)
		{
			service_size = n_services*(int)sizeof(DNS_SERVICE_INFO);
			servp->service_ptr = (DNS_SERVICE_INFO *)malloc((size_t)service_size);
			memcpy(servp->service_ptr, buffer->services, (size_t)service_size);
			N_services += n_services;
		}
		servp->busy = 1;
		if(strcmp(servp->name, sname))
		{
		  strcpy(servp->name,sname);
		  Force_update = 1;
		  servp->busy = 3;
		}
	}
	else
	{
	  if(servp)
	    {
		N_servers--;
		if(servp->server.n_services != -1)
		  {
			N_services -= servp->server.n_services;
		  }
		else
		  Force_update = 1;
		servp->server.n_services = 0;
		servp->busy = -1;
	    }
	}
}

void show_servers()
{
SERVER *servp;
void update_show_servers();
void remove_all_buttons();
void put_label();

	if(!Matrix_id[Curr_matrix])
		return;
	remove_all_buttons();
	
#ifndef linux	
	switch_matrix();
#endif
	put_label();
	servp = Server_head;
	while( (servp = (SERVER *)sll_get_next((SLL *)servp)) )
	{
	  servp->busy = 1;
	}
	Force_update = 1;                
}

void update_show_servers(void *tag, unsigned long *reason)
{
DNS_SERVER_INFO *server_ptr;
DNS_SERVICE_INFO *service_ptr;
int i, j, found, n_done = 0;
Widget w, create_button();
SERVER *servp, *prevp;
static int old_n_services = 0;
char node[MAX_NODE_NAME], par[MAX_NODE_NAME], *ptr;
void remove_button();
void remove_all_buttons();
void put_label();

    DISABLE_AST
	if(tag){}
	if(reason){}
    if((N_services != old_n_services) || (Force_update))
    {
        if(!Matrix_id[Curr_matrix])
	{
	    XtAppAddTimeOut(app_context, 1000, update_show_servers, 0);
	    ENABLE_AST
	    return;
	}
	if(!N_servers)
	{
		remove_all_buttons();
		if(! No_link_button_id)
		{
			No_link_button_id = create_button("DNS is down", 0);
			set_color(No_link_button_id, XmNbackground, RED);
		        get_something(No_link_button_id,XmNuserData,&w);
			set_color(w, XmNbackground, RED);
			XtSetSensitive(No_link_button_id, False);
		}
		while(!sll_empty((SLL *)Server_head))
		{
			servp = (SERVER *)sll_remove_head((SLL *)Server_head);
			if(servp->service_ptr)
				free(servp->service_ptr);
			free(servp);
		}
		put_label();
		old_n_services = N_services;
		Force_update = 0;
		XtAppAddTimeOut(app_context, 1000, update_show_servers, 0); 
		ENABLE_AST
		return;
	}
	if(No_link_button_id)
	{
		XtDestroyWidget(No_link_button_id);
		/*
		XFlush(XtDisplay(No_link_button_id));
		*/
		No_link_button_id = 0;
        }
	servp = Server_head;
	prevp = 0;
	while( (servp = (SERVER *)sll_get_next((SLL *)servp)) )
	{
		if(prevp)
		{
			free(prevp);
			prevp = 0;
		}
		if(n_done == 10)
		{
		    if(!Force_update)
		        put_label();
		    XtAppAddTimeOut(app_context, 100, update_show_servers, 0);
		    ENABLE_AST
		    return;
		}
		server_ptr = &servp->server;
		if(servp->busy == 3)
		{
		  remove_button(servp);
		  servp->busy = 1;
		}
		if(servp->busy == 1)
		{
			if(!servp->button_id)
			{
			switch(Curr_view_opt)
			{
				case 1 :
				  servp->button_id = create_button(/*server_ptr->task*/servp->name, servp);
					n_done++;
					break;
				case 0 :
				  strcpy(node, server_ptr->node);
				  strcpy(par, Curr_view_opt_par);
				  if(!isdigit(node[0]))
				  {
					ptr = strchr(node, '.');
					if(ptr)
						*ptr = '\0';
					ptr = strchr(par,'.');
					if(ptr)
						*ptr = '\0';
				  }
				  ptr = node;
				  for(i = 0; i < (int)strlen(ptr); i++)
				    ptr[i] = (char)tolower((int)ptr[i]);
				  ptr = par;
				  for(i = 0; i < (int)strlen(ptr); i++)
				    ptr[i] = (char)tolower((int)ptr[i]);
					 if(!strcmp(/*server_ptr->*/node, /*Curr_view_opt_*/par))
					{
						servp->button_id = create_button(/*server_ptr->task*/servp->name, servp);
						n_done++;
					}
					break;
				case 2 :
					found = 0;
					if(!(service_ptr = servp->service_ptr))
						break;
					for(j = 0; j < server_ptr->n_services; j++)
					{
						if(strstr(service_ptr->name, Curr_view_opt_par) > (char *)0)
						{
							found = 1;
							break;
						}
						service_ptr++;
					}
					if (found)
					  {
						servp->button_id = create_button(/*server_ptr->task*/servp->name, servp);
						n_done++;
					  }
					break;
				case 3 :
					if(server_ptr->n_services == -1)
					{
						servp->button_id = create_button(/*server_ptr->task*/servp->name, servp);
						n_done++;
					}
					else
					{
						if(servp->button_id)
							remove_button(servp);
					}
					n_done++;
					break;
			}
			}
			servp->busy = 2;
			if(servp->button_id)
			{
				if(Curr_view_opt != -1)
				{
					if (server_ptr->n_services == -1)
					{
						set_color(servp->button_id, XmNbackground, RED);
						get_something(servp->button_id,XmNuserData,&w);
						set_color(w, XmNbackground, RED);
					}
					else
					{
						set_color(servp->button_id, XmNbackground, GREEN);
						get_something(servp->button_id,XmNuserData,&w);
						set_color(w, XmNbackground, GREEN);					 
					}
				}
			}
		}
		else if (servp->busy == -1)
		{
			remove_button(servp);
			sll_remove((SLL *)Server_head, (SLL *)servp);
			if(servp->service_ptr)
			{
				free(servp->service_ptr);
				servp->service_ptr = 0;
			}
			prevp = servp;
			n_done++;
		}
	}
	if(prevp)
	{
		free(prevp);
		prevp = 0;
	}
	put_label();
	old_n_services = N_services;
	Force_update = 0;
    }
    XtAppAddTimeOut(app_context, 1000, update_show_servers, 0);
    ENABLE_AST
}

Widget create_button(char *name, SERVER *servp)
{
Arg arglist[10];
int n;
/*
int n_services = -1;
*/
Widget w, ww, w_id;
void activate_services(), activate_clients();
char w_name[MAX_NAME];
	
	w_name[0] = 0;
/*
	if(servp)
		n_services = servp->server.n_services;
*/
    strcpy(w_name,name);
	if(strlen(w_name) >= MAX_TASK_NAME - 4)
	  w_name[16] = '\0';
	n = 0;
    XtSetArg(arglist[n], XmNorientation, XmVERTICAL);  n++;
    XtSetArg(arglist[n], XmNentryAlignment, XmALIGNMENT_CENTER);  n++;
	w_id = w = XmCreateMenuBar(Matrix_id[Curr_matrix],
				(String)XmStringCreateLtoR ( w_name,XmSTRING_DEFAULT_CHARSET),
				arglist,(Cardinal)n);
/*				
	if(n_services == -1)
		set_color(w, XmNbackground, RED);
	else
		set_color(w, XmNbackground, GREEN);
*/
	XtManageChild(w);
	strcat(w_name,"1"); 
	n = 0;
    XtSetArg(arglist[n], XmNalignment, XmALIGNMENT_CENTER);  n++;
	XtSetArg(arglist[n], XmNfontList, did_server_font);  n++;
	w = XmCreateCascadeButton(w,
				(String)XmStringCreateLtoR ( w_name,XmSTRING_DEFAULT_CHARSET),
				arglist,(Cardinal)n);
	set_something(w,XmNlabelString,name);
	set_something(w,XmNalignment,XmALIGNMENT_CENTER);
/*
	if(n_services == -1)
		set_color(w, XmNbackground, RED);
	else
		set_color(w, XmNbackground, GREEN);
*/
	set_something(w_id,XmNuserData,w);
	strcat(w_name,"1"); 
		n = 0;
		ww = XmCreatePulldownMenu(w_id,
				(String)XmStringCreateLtoR ( w_name,XmSTRING_DEFAULT_CHARSET),
				arglist,(Cardinal)n);
		set_something(w,XmNsubMenuId,ww);
		XtManageChild(w);
		strcat(w_name,"1"); 
		n = 0;
		XtSetArg(arglist[n], XmNfontList, did_default_font);  n++;
		w = XmCreatePushButton(ww,
				(String)XmStringCreateLtoR ( w_name,XmSTRING_DEFAULT_CHARSET),
				arglist,(Cardinal)n);

		set_something(w,XmNlabelString,"Services");
	if(servp)
	{
		XtAddCallback(w,XmNactivateCallback, activate_services, servp);
		XtManageChild(w);
		strcat(w_name,"1"); 
		n = 0;
		XtSetArg(arglist[n], XmNfontList, did_default_font);  n++;
		w = XmCreatePushButton(ww,
				(String)XmStringCreateLtoR ( w_name,XmSTRING_DEFAULT_CHARSET),
				arglist,(Cardinal)n);

		set_something(w,XmNlabelString,"Clients");
		XtAddCallback(w,XmNactivateCallback, activate_clients, servp);
		XtManageChild(w);
		/*
		servp->popping = 0;
		create_client_popup(servp);
		*/
	}
	return(w_id);
}

void remove_all_buttons()
{
SERVER *servp;

	servp = Server_head;
	while( (servp = (SERVER *)sll_get_next((SLL *)servp)) )
	{
		if(servp->button_id)
		{
			XtDestroyWidget(servp->button_id);
			servp->button_id = 0;
			servp->busy = 0;
		}
	}
}

void remove_button(SERVER *servp)
{

	if(servp->button_id)
	{
		XtDestroyWidget(servp->button_id);
		servp->button_id = 0;
		servp->busy = 0;
	}
}

void activate_services(Widget w, SERVER *servp, unsigned long *reason)
{
DNS_SERVER_INFO *ptr;
char str[MAX_NAME];
Widget id,sel_id;
void got_service_list();
void kick_it();

	if(w){}
	if(reason){}
	if(servp->pop_widget_id[0])
	{
		XtDestroyWidget(servp->pop_widget_id[0]);
		servp->pop_widget_id[0] = 0;
		/*
		return;
		*/
	}
	Curr_servp = servp;
	ptr = &servp->server;

	sel_id = put_popup(servp, 0,"Service Info");

	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_OK_BUTTON);
	XtUnmanageChild(id);
	
	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST_LABEL);
	if (ptr->pid > 0x1000000)
		sprintf(str,"Server %s (pid = %X) on node %s\n\nprovides %d services :\n",
			servp->name, ptr->pid, ptr->node, ptr->n_services);
	else
		sprintf(str,"Server %s (pid = %d) on node %s\n\nprovides %d services :\n",
			servp->name, ptr->pid, ptr->node, ptr->n_services);
	set_something(sel_id,XmNlistLabelString,str);

	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
	Curr_service_list_id = id;

	XmListAddItem(id,create_str(
          "Ordering services alphabeticaly, please be patient..."),1);

	set_something(id,XmNlistItemCount,1);
	set_something(id,XmNlistVisibleItemCount,1);
	
	sprintf(str,"%s/SERVICE_LIST",/*ptr->task*/servp->name);
	dic_info_service(str,ONCE_ONLY,20,0,0,
			 got_service_list,(long)servp,"None",5);
	/*
#ifdef solaris
	*/
	/*
	XtAppAddTimeOut(app_context, 1000, kick_it, 0);
	*/
	/*
#endif
	*/
}

void kick_it()
{
  printf("kick_it\n");
}

typedef char DID_SLOT[MAX_NAME];

void got_service_list(SERVER **servp_ptr, char *buffer, int *size)
{
SERVER *servp;
void do_got_service_list();

	if(size){}
	servp = *servp_ptr;
	if(Curr_service_list)
	  free(Curr_service_list);
	Curr_service_list = malloc(strlen(buffer)+1);
	strcpy(Curr_service_list, buffer);
	/*
#ifdef solaris
	*/
	Got_Service_List = servp;
	/*
#else
	do_got_service_list(servp);
#endif
	*/
}

void do_got_service_list(SERVER *servp)
{
char cmd_str[256], svc_str[256];
DNS_SERVER_INFO *ptr;
DNS_SERVICE_INFO *service_ptr;
Widget id;
char *curr_str, max_str[MAX_NAME], *sptr;
DID_SLOT *service_list;
int i, j, curr_index = 0, n_services;
XmString xstr;
void delete_str();

	ptr = &servp->server;
	id = Curr_service_list_id;
	
	XmListDeleteAllItems(id);
	
	strcpy(cmd_str,"CMD: ");
	strcpy(svc_str,"SVC: ");

	service_ptr = servp->service_ptr;
	service_list = (DID_SLOT *)malloc((size_t)(ptr->n_services * MAX_NAME));
	n_services = ptr->n_services;

	for(i=0;i<n_services; i++)
	{
	  strcpy(service_list[i],service_ptr->name);
	  service_ptr++;
	}
	strcpy(max_str,"zzzzzzzzzzzzzzzzzzzzzzzzzzzz");
	for(i=0;i<n_services; i++)
	{
	  curr_str = max_str;
	  for(j=0;j<n_services; j++)
	  {
	    sptr = service_list[j];
	    if(!*sptr)
	      continue;
	    
	    if(strcmp(sptr,curr_str) < 0)
	    {
	      curr_str = sptr;
	      curr_index = j;
	    }
	  }
	  service_list[curr_index][0] = '\0';
	  service_ptr = &(servp->service_ptr[curr_index]);
	  if(service_ptr->type)
	  {
	    strcpy(&cmd_str[5],service_ptr->name);
	    xstr = create_str(cmd_str);
	    XmListAddItem(id,xstr,i+1);
	    delete_str(xstr);
	  }
	  else
	  {
	    strcpy(&svc_str[5],service_ptr->name);
	    xstr = create_str(svc_str);
	    XmListAddItem(id,xstr,i+1);
	    delete_str(xstr);
	  }
	}
	free(service_list);
	
	set_something(id,XmNlistItemCount,i);
	set_something(id,XmNlistVisibleItemCount,(i < 20) ? i : 20);
}

void show_clients(SERVER **servp_ptr, char *buffer, int *size)
{
SERVER *servp;
void do_show_clients();

	if(size){}
	servp = *servp_ptr;
	if(Curr_client_list)
	  free(Curr_client_list);
	Curr_client_list = malloc(strlen(buffer)+1);
	strcpy(Curr_client_list, buffer);
	/*
#ifdef solaris
	*/
	Got_Client_List = servp;
	/*
#else
	do_show_clients(servp);
#endif
	*/
}

void do_show_clients(SERVER *servp)
{
int i = 0;
char str[2048], *strp, *strp1;
XmString xstr;
void delete_str();

/*
DNS_SERVER_INFO *ptr;
	ptr = &servp->server;
	sel_id = servp->pop_widget_id[1];
	id = (Widget)XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST_LABEL);
*/
	if(servp){}
	if(Curr_client_list[0] == -1) 
	{
		sprintf(str,"Information not available\n");
		XmListAddItem(Curr_client_id,create_str(str),i+1);
		/*
		set_something(sel_id,XmNlistLabelString,str);
		*/
		return;
	}
	/*
	sprintf(str,"Clients of %s are :                                  \n",
	ptr->task);
	set_something(sel_id,XmNlistLabelString,str);
	
	id = (Widget)XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
	XmListDeleteAllItems(id);
	*/
	strp1 = Curr_client_list;
	while(strp1) 
	{
		if(!*strp1)
			break;
		sprintf(str,"Process ");
		strp = strp1;
		strp1 = strchr(strp,'@');
		*strp1 = '\0';
		strp1++;
		strcat(str,strp);
		strcat(str," on node ");
		strp = strp1;
		if( (strp1 = strchr(strp,'|')) )
		{
			*strp1 = '\0';
			strp1++;
		}
		strcat(str,strp);	
		xstr = create_str(str);
		XmListAddItem(Curr_client_id,xstr,i+1);
		delete_str(xstr);
       		i++;
	}
	if(!i)
	{
		sprintf(str,"NONE");
		xstr = create_str(str);
		XmListAddItem(Curr_client_id,xstr,i+1);
		delete_str(xstr);
	}
	/*
	set_something(id,XmNlistItemCount,i);
	*/
	/*
	if(Matrix_id[Curr_matrix])
	  XFlush(XtDisplay(Matrix_id[Curr_matrix]));
	*/
}

void activate_clients(Widget w, SERVER *servp, unsigned long *reason)
{
/*
DNS_SERVER_INFO *ptr;
*/
char str[100];
void show_clients();
void kick_it_again();
Widget id,sel_id;

	if(w) {}
	if(reason){}
	Curr_servp = servp;
/*
	ptr = &servp->server;
*/
	if(servp->pop_widget_id[1])
	  {
		XtDestroyWidget(servp->pop_widget_id[1]);
		servp->pop_widget_id[1] = 0;
	  }
	sel_id = put_popup(servp,1,"Client Info");

	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(id);

	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_APPLY_BUTTON);
	XtUnmanageChild(id);

	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(id);

	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_TEXT);
	XtUnmanageChild(id);

	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_SELECTION_LABEL);
	XtUnmanageChild(id);
	/*
	id = (Widget)XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST_LABEL);
	*/
	id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
	XmListDeleteAllItems(id);

	Curr_client_id = id;
	sprintf(str,"Clients of %s are :                                   \n",
		servp->name);
	set_something(sel_id,XmNlistLabelString,str);

	sprintf(str,"%s/CLIENT_LIST",/*ptr->task*/servp->name);
	dic_info_service(str,ONCE_ONLY,10,0,0,
			 show_clients,(long)servp,&no_link,1);
	/*
#ifdef solaris
	*/
	/*
	XtAppAddTimeOut(app_context, 1000, kick_it_again, 0); 
	*/
	/*
#endif
	*/
}

void kick_it_again()
{
  printf("kick_it_again\n");
}

Widget put_popup(SERVER *servp, int type, char *title)
{
    Widget id;
    void activate_services(), activate_clients();
	extern void set_title();

		if(type)
		{
		    id = create_client_dialog();
		    /*
		    XtAddCallback(id,XmNokCallback, activate_clients, servp);
		    XtAddCallback(id,XmNcancelCallback, activate_clients, servp);
		    */
		}
		else
		{
		    id = create_server_dialog();
		    /*
		    XtAddCallback(id,XmNcancelCallback, activate_services, servp);
		    */
		}
		servp->pop_widget_id[type] = id;
		/*
	}
		*/
	XtManageChild(id);
	set_title(XtParent(id),title);
	return(id);
}	

Widget put_selection(int tag, char *title)
{
    Widget id = 0;
	extern void set_title();

    if(pop_widget_id[tag])
    {
		XtDestroyWidget(pop_widget_id[tag]);
    }
	  switch(tag)
	  {
	    case DID_SEL_NODE:
	      id = create_node_selection();
	      break;
	    case DID_SEL_SERVICE:
	      id = create_service_selection();
	      break;
	    case DID_KILL_ALL:
	      id = create_kill_confirmation();
	      break;
	    case DID_SERVICE:
	      id = create_service_dialog();
	      break;
	    case DID_COMMAND:
	      id = create_send_command();
	      break;
	  }
	  
	   pop_widget_id[tag] = id;
	XtManageChild(id);
	set_title(XtParent(id),title);
	return(id);
}	

void check_put_label(int tag)
{
	static int old_n_services = 0;
/*
	static int changing = 0;
*/
	void put_label();

	if(tag){}
	if(N_services != old_n_services)
	{
		put_label();
/*
		if(N_services > old_n_services)
		  changing = 1;
*/
		old_n_services = N_services;
	
#ifdef linux
		show_servers();
#endif
	}
/*
	else
	{
	  if(changing)
	  {
	    show_servers();
	    changing = 0;
	  }
	}
*/
}

void put_label()
{
	char str[MAX_NAME], str1[MAX_NAME];
			
	DISABLE_AST
	sprintf(str,"%d Servers known - %d Services Available\n",
		N_servers,N_services);
	switch(Curr_view_opt)
	{
		case 1 :
			strcat(str,"Displaying ALL Servers");
			break;
		case 0 :
			sprintf(str1,"Displaying Servers on node %s",Curr_view_opt_par);
			strcat(str,str1);
			break;
		case 2 :
			sprintf(str1,"Displaying Servers providing Service *%s*",
				Curr_view_opt_par);
			strcat(str,str1);
			break;
		case 3 :
			strcat(str,"Displaying Servers in ERROR");
			break;
		case -1 :
			strcat(str,"Please Select Viewing Option");
			break;
	}
	set_something(Label_id,XmNlabelString,str);
	XFlush(XtDisplay(Label_id));
	ENABLE_AST
}

Widget create_client_dialog()
{
  Widget id;
  id = create_selection_dialog("Dismiss","","","","",DID_CLIENTS, 3);
  return(id);
  
}

Widget create_server_dialog()
{
  Widget id;
  id = create_selection_dialog("","View / Send","Dismiss","",
			       "Service / Command :",
			       DID_SERVICES, 20);
  return(id);
}

Widget create_node_selection()
{
  Widget id;
  id = create_selection_dialog("","","","Nodes :","Selected Node :",DID_SEL_NODE, 8);
  return(id);
  
}

Widget create_service_selection()
{
  Widget id;
  /*
  id = create_prompt_dialog("Enter Service Name :",DID_SEL_SERVICE);
  */
  id = create_selection_dialog("","","","","Enter Service Name (or search string):",DID_SEL_SERVICE, 0);
  return(id);
  
}

Widget create_send_command()
{
  Widget id;
  char str[256], str1[256];

  sprintf(str,"Command to %s (%s)\n\n",
	  Curr_service_name, Curr_service_format);

  id = create_selection_dialog("","","",str,"Command:",DID_COMMAND, 1);

  strcpy(str1,"Please enter items separated by spaces:\n(for example: 2 0x123 'A' 23.4 \"a text\")");

  set_something(id,XmNselectionLabelString,str1);

  return(id);
  
}

Widget create_kill_confirmation()
{
  Widget id;
  id = create_question_dialog("Do you really want to kill ALL DIM servers ?",
			      DID_KILL_ALL);
  return(id);
  
}

Widget create_selection_dialog(char *ok, char *apply, char *cancel, char *list, char *sel, 
							   long tag, int items)
{
Widget sd;
XmString xmOk, xmApply, xmCancel, xmList, xmSelection;
Arg ar[20];
int n;
        
    xmList = create_str(list);
    xmOk = create_str(ok);
    xmApply = create_str(apply);
    xmCancel = create_str (cancel);
    xmSelection = create_str (sel);

    n = 0;
    /*
    XtSetArg(ar[n],XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(ar[n],XmNmwmFunctions,MWM_FUNC_MOVE); n++;
    */
    XtSetArg(ar[n],XmNtitle,"Selection"); n++;
    XtSetArg(ar[n],XmNlabelFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNbuttonFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNtextFontList, did_small_font); n++;
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNlistLabelString, xmList); n++;
    XtSetArg(ar[n],XmNlistVisibleItemCount,items); n++;
    if(ok[0])
      {
      XtSetArg(ar[n],XmNokLabelString, xmOk); n++;
      }
    if(apply[0])
      {
      XtSetArg(ar[n],XmNapplyLabelString, xmApply); n++; 
      }
    if(cancel[0])
      {
      XtSetArg(ar[n],XmNcancelLabelString, xmCancel); n++;
      }
    if(sel[0])
      {
      XtSetArg(ar[n],XmNselectionLabelString, xmSelection); n++;
      }
    sd = XmCreateSelectionDialog ( toplevel_widget, "Selection", ar, (Cardinal)n );
    XmStringFree(xmList);
    XmStringFree(xmOk);
    XmStringFree(xmApply);
    XmStringFree(xmCancel);
    XmStringFree(xmSelection);
    if(tag >= 0)
      {
	  XtAddCallback ( sd, XmNcancelCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)tag );
	  XtAddCallback ( sd, XmNapplyCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)tag );
	  XtAddCallback ( sd, XmNokCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)tag );
      }
    return(sd);
}

Widget create_file_selection_dialog(long type)
{
Widget sd;
XmString xm1;
Arg ar[20];
int n;
        
    xm1 = create_str ("");
    n = 0;
    /*
    XtSetArg(ar[n],XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(ar[n],XmNmwmFunctions,MWM_FUNC_MOVE); n++;
    */
    XtSetArg(ar[n],XmNtitle,"FileSelection"); n++;
    XtSetArg(ar[n],XmNlabelFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNbuttonFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNtextFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNwidth, 500); n++;
    XtSetArg(ar[n],XmNdirMask, xm1); n++;
    sd = XmCreateFileSelectionDialog ( toplevel_widget, "FileSelection", ar, (Cardinal)n );
    
    XmStringFree(xm1);

    XtAddCallback ( sd, XmNcancelCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)type );
    XtAddCallback ( sd, XmNokCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)type );

	return(sd);
}

Widget create_prompt_dialog(char *label, long tag)
{
Widget sd;
XmString xm1;
Arg ar[20];
int n;
        
    xm1 = create_str (label);
    n = 0;
    XtSetArg(ar[n],XmNlabelFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNbuttonFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    /*
    XtSetArg(ar[n],XmNwidth, 450); n++;
    XtSetArg(ar[n],XmNresizePolicy, XmRESIZE_NONE); n++;
    */
    XtSetArg(ar[n],XmNselectionLabelString, xm1); n++;
    sd = XmCreatePromptDialog ( toplevel_widget, "Prompt", ar, (Cardinal)n );
    
    XmStringFree(xm1);

    XtAddCallback ( sd, XmNcancelCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)tag );
    XtAddCallback ( sd, XmNokCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)tag );

	return(sd);
}

Widget create_question_dialog(char *label, long tag)
{
Widget sd;
XmString xm1;
Arg ar[20];
int n;
        
    xm1 = create_str (label);
    n = 0;
    XtSetArg(ar[n],XmNlabelFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNbuttonFontList, did_default_font); n++;
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    /*
    XtSetArg(ar[n],XmNwidth, 450); n++;
    XtSetArg(ar[n],XmNresizePolicy, XmRESIZE_NONE); n++;
    */
    XtSetArg(ar[n],XmNmessageString, xm1); n++;
    sd = XmCreateQuestionDialog ( toplevel_widget, "Question", ar, (Cardinal)n );
    
    XmStringFree(xm1);

    XtAddCallback ( sd, XmNcancelCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)tag );
    XtAddCallback ( sd, XmNokCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)tag );

	return(sd);
}

Widget create_service_dialog()
{
Widget fd, rc, sw, lb, rc1;
XmString xm1;
Arg ar[20];
int n, par;
unsigned long reason;
        
    n = 0; 
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNresizePolicy, XmRESIZE_ANY); n++;
    fd = XmCreateFormDialog ( toplevel_widget, "Form", ar, (Cardinal)n );
    XtManageChild(fd);

    /* create rowcolumn */
    n = 0; 
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(ar[n],XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNbottomOffset, 0); n++;
    XtSetArg(ar[n],XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNrightOffset, 0); n++;
    XtSetArg(ar[n],XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNtopOffset, 0); n++;
    XtSetArg(ar[n],XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNleftOffset, 0); n++;
    rc = XmCreateRowColumn ( fd, "rowcol", ar, (Cardinal)n );
    XtManageChild(rc);

    /* create scrolled window */
    n = 0;	    
    XtSetArg ( ar[n], XmNwidth, 770); n++;
    XtSetArg ( ar[n], XmNheight, 350); n++;
    XtSetArg ( ar[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
    XtSetArg ( ar[n], XmNscrollingPolicy, XmAUTOMATIC); n++;

    sw = XmCreateScrolledWindow ( rc, "ScrollWin", ar, (Cardinal)n );
    XtManageChild ( sw );

    /* create label */
    n = 0; 
    xm1 = create_str(" ");
    XtSetArg(ar[n],XmNfontList, did_small_font); n++;
    XtSetArg(ar[n],XmNlabelString, xm1); n++;
    XtSetArg(ar[n],XmNalignment, XmALIGNMENT_BEGINNING); n++;
    lb = XmCreateLabel ( sw, "label", ar, (Cardinal)n );
    XtManageChild(lb);
    XmStringFree(xm1);
    par = 1;
    reason = 0;
    create_label(lb, &par, &reason);

    /* create button rowcolumn */
    n = 0; 
    XtSetArg(ar[n],XmNborderWidth, 0); n++;
    XtSetArg(ar[n],XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(ar[n],XmNorientation, XmVERTICAL); n++;
    XtSetArg(ar[n],XmNnumColumns, 3); n++;
    XtSetArg(ar[n],XmNpacking, XmPACK_COLUMN); n++;
    rc1 = XmCreateRowColumn ( rc, "buttons", ar, (Cardinal)n );
    XtManageChild(rc1);
    /*    
    create_push_button(rc1,"View Standard",MAX_POP_UPS+1); 
    create_push_button(rc1,"View Float",MAX_POP_UPS+2); 
    create_push_button(rc1,"View Double",MAX_POP_UPS+3); 
    */
    SubscribeButton = create_push_button(rc1,"            Subscribe (On Change)            ",
		       MAX_POP_UPS+5); 
  Subscribe10Button = create_push_button(rc1,"      Subscribe (Update Rate 10 seconds)     ",
		       MAX_POP_UPS+4); 
    create_push_button(rc1,"Dismiss",DID_SERVICE);
    Curr_service_print_type = 0;

    return(fd);
}

Widget create_push_button(Widget parent, char *str, long tag)
{
Widget b;
XmString xm1;
Arg ar[20];
int n;
        
    n = 0; 
    xm1 = create_str(str);
    XtSetArg(ar[n],XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(ar[n],XmNfontList, did_default_font); n++;
    XtSetArg(ar[n],XmNlabelString, xm1); n++;
    b = XmCreatePushButton ( parent, "button", ar, (Cardinal)n );
 
    XtManageChild(b);
    XmStringFree(xm1);

    XtAddCallback ( b, XmNactivateCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)tag ); 
    return(b);
}

