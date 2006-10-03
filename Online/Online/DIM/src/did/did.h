#include <Mrm/MrmAppl.h>                /* Motif Toolkit and MRM */
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/ScrolledW.h>
#include <Xm/Separator.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/SelectioB.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <dui_colors.h>
/* VUIT routines for the user to call */
void s_error();
/*
#define LABEL_FONT "-*-NEW CENTURY SCHOOLBOOK-BOLD-R-*--*-140-*-*-*-*-ISO8859-1"
#define DEFAULT_FONT "-*-TIMES-BOLD-R-*--*-120-*-*-*-*-ISO8859-1"

#define MENU_FONT "-*-TIMES-BOLD-R-*--*-120-*-*-*-*-ISO8859-1"
*/
#define LABEL_FONT "-*-HELVETICA-BOLD-R-*--*-120-*-*-*-*-ISO8859-1"

#define DEFAULT_FONT "-*-HELVETICA-BOLD-R-*--*-100-*-*-*-*-ISO8859-1"

#define MENU_FONT "-*-COURIER-BOLD-R-*--*-100-*-*-*-*-ISO8859-1"

#define SERVER_FONT "-*-TIMES-BOLD-R-*--*-100-*-*-*-*-ISO8859-1"

typedef enum { DID_SERVICES, DID_CLIENTS, DID_SEL_NODE, DID_SEL_SERVICE, 
    DID_KILL_ALL, DID_SERVICE, DID_COMMAND, MAX_POP_UPS } POPUPS; 

/* Motif Global variables */
Display         *display;			/* Display variable */
XtAppContext    app_context;		/* application context */
Widget			toplevel_widget;	/* Root widget ID of application */
MrmHierarchy	s_MrmHierarchy;		/* MRM database hierarchy ID */

typedef struct item{
    struct item *next;
	DNS_SERVER_INFO server;
	DNS_SERVICE_INFO *service_ptr;
        char name[132];
	Widget button_id;
	Widget pop_widget_id[2];
  /*
	int popping;
  */
	int busy;
}SERVER;

SERVER *Server_head = (SERVER *)0;

Widget Matrix_id[2] = {0, 0};
int Curr_matrix;
Widget Label_id = 0;
Widget Content_label_id = 0;
Widget Window_id = 0;
Widget pop_widget_id[MAX_POP_UPS] = {0,0,0,0,0,0,0};
Widget No_link_button_id;

XmString create_str();
XmFontList util_get_font();

Widget create_selection_dialog();
Widget create_file_selection_dialog();
Widget create_prompt_dialog();
Widget create_question_dialog();
Widget create_service_dialog();
Widget create_server_dialog();
Widget create_client_dialog();
Widget create_node_selection();
Widget create_service_selection();
Widget create_kill_confirmation();
Widget create_send_command();
Widget create_push_button();
Widget put_popup();
Widget put_selection();

Widget gui_toplevel();
Widget gui_initialize();
void gui_create_main_menu();
void gui_create_main_window();









