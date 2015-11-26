#ifndef __UPIDEF_H
#define __UPIDEF_H

#include <cstdlib>
#include "SCR/lists.h"

/* Constants for building pages */
#define UPI_PREV_PAGE	"<- "
#define UPI_NEXT_PAGE	" ->"
#define UPI_ONLY_PAGE	"   "
#define UPI_PAGE_MARKER_SIZE 3

namespace SCR {
#if !defined(ON_OFF_DEFINED)
#define ON_OFF_DEFINED 1
enum FLAG {OFF, ON};
typedef FLAG flag;
#endif

struct Pasteboard;
struct Display;
struct Window;
}
using SCR::Display;
using SCR::Window;
using SCR::Pasteboard;
using SCR::FLAG;
using SCR::ON;
using SCR::OFF;


typedef unsigned Unsigned;
typedef void (*Routine)(int,int,...);
typedef int  (*WtRoutine)(unsigned int,void*);

#define UPI_RETURN_ON_ACCEPT   0
#define UPI_WAKE_UP_ON_CHANGE  1

#define UPI_K_MESSAGE  0
#define UPI_K_OPERATOR 1
#define UPI_K_USER     2

/*  Callback conditions.  */
#define UPI_CALL_ON_PF1           0x08
#define UPI_CALL_ON_ANY_BACKSPACE 0x10
#define UPI_CALL_ON_MOVE_LEFT     0x20
#define UPI_CALL_ON_MOVE_RIGHT    0x40

/* menu_item.enabled field */
#define UPI_DISABLED   0
#define UPI_ENABLED    1
 
/* menu_item.type field */
typedef enum {
  UPI_COMMENT,
  UPI_COMMAND,
  UPI_PARAM
} UPI_Item_types;

/* menu.type field */
typedef enum {
  UPI_NORMAL_MENU,
  UPI_PULLDOWN_MENU,
  UPI_DETACHED_MENU,
  UPI_PARAMETER_PAGE
} UPI_Menu_types;

/* Special predefined menus */
#define UPI_WARNING (-1)
#define UPI_NOTICE  (-2)
#define UPI_DLPACK  (-3)

/* parameter page options */
typedef enum {
  UPI_ACCEPT_OPTION,
  UPI_CANCEL_OPTION,
  UPI_RESET_OPTION
} UPI_Parameter_page_options;
 
/* item.type field */
typedef enum {
  UPI_ASC_FMT,
  UPI_OCT_FMT,
  UPI_DEC_FMT,
  UPI_HEX_FMT,
  UPI_REAL_FMT,
  UPI_BIN_FMT,
  UPI_LOG_FMT
} UPI_Parameter_types;

typedef union {
  int i;
  Unsigned u;
  float f;
  double d;
  char* c;
} Convert;

typedef union {
  struct {
    char* def;
    char* min;
    char* max;
    char** list;
    int list_size;
    int flag;
  } c;
  struct {
    int def, min, max;
    int* list;
    int list_size;
    int flag;
  } i;
  struct {
    double def, min, max;
    double* list;
    int list_size;
    int flag;
  } d;
} ParamArgs;

typedef struct PARAMETER   Param;
typedef struct ITEM        Item;
typedef struct PAGE        Page;
typedef struct MENU        Menu;
typedef struct ASYNC       Async;
typedef struct ASYNC_LINE  Async_line;
typedef struct CONNECTION  Connection;
typedef struct SYSTEM      System;
typedef struct HISTO       Histo;

struct HISTO {
  Histo Link_items;
  
  char *text;
  int bins;
  double min;
  double max;
  double width;
  double underflow;
  double overflow;
  double highest;
  double *first;
  Display* disp;
  int rows, cols;
  int row, col;
  int scaled;
  int pasted;
};

 
struct PARAMETER
{
  Param Link_items;
  int   id;
  
  UPI_Parameter_types type;
  size_t pos;                 /*  The position within the line      */
  size_t offset;              /*  The field offset to beg. of buffer  */
  size_t size;                /*  The size of the buffer            */
  size_t chars;               /*  The size of the visual field      */
  char  format[12];
  char  convert[12];
  char *buf;                  /*  The editable buffer               */
  int  *var;                  /*  The pointer to the user variable  */
  Convert def,
          val,
          min,
          max;
  void *list;
  int   list_size;
  int   flag;                  /*  The <list-only> flag             */
  int   fortran;               /*  Fortran variable (float/strings) */
  
  FLAG   input;                /*  Is it in edition ?               */
  size_t buf_pos;              /*  Current edit position in buffer  */
  int    list_pos;             /*  Current list scan position       */
};
 
 
struct ITEM
{
  Item  Link_items;
  int   id;
  
  struct {
    Param Linked_list_items;
    Param *cur;
  }     param;

  Menu *to;
  char *string;
  char *help;
  char  type;
  char  enabled;
  Routine action;
};

struct PAGE_HEADER
{
  Page    Link_items;
  int     id;
};
 
struct PAGE
{
  Page    Link_items;
  Display* id;
  
  struct {
    Item   Linked_list_items;
    Item  *cur;
  }       item;
    
  int     lines;
  int     cur_line;
  int     cols;
  int     row, col;
};

#define Page_address(aaa) (Page *) ((char*)aaa-sizeof(struct PAGE_HEADER))

struct MENU {
  Menu     Link_items;
  int      id;  
  struct {
    Page   Linked_list_items;
    Page  *cur;
  }        page;
  struct {
    int menu;
    int item;
    int last;
  }        from;
  
  Window*  window;
  int      type;
  int      width;
  int      items;
  int      on_screen;

  char    *mn_title;
  char    *up_title;
  char    *bt_title;
  
  int      condition;
  Routine  callback;
  void*    arg;
};


struct ASYNC    {
  Async_line Linked_list_items;
  
  Display*   id;
  int        max_entries;
  int        entries;
  int        row, col;
  int        rows, cols;
};

struct ASYNC_LINE
{
  Async_line  Link_items;
  int         render;
  char       *text;
};

struct CONNECTION
{
  int menu;
  int item;
};

struct SYSTEM   {
  System    Link_items;
  int       id;
    
  struct {
    Menu   Linked_list_items;
    Menu   *cur;
  } menu;

  struct {
    Param  Linked_list_items;
    Param  *cur;
  } param;    

  struct {
    Item   Linked_list_items;
    Item   *cur;
  } item;    

  Async     async;
    
  Page     *pop_up;
  Page     *help;
  
  int       mode;

  int       row;
  int       col;

  Pasteboard* pb;
  int       pb_rows;
  int       pb_cols;
  Window*   window;
  Window*   detached_window;
  char      result[8];
  int       items_per_page;
  Connection keypad[10];
  Connection pf1;
  Routine    message_callback;
  void*      PF1Arg;
  Routine    PF1CallBack;
  Connection global_backspace;
  Routine    GlobBSCallBack;
  void*      GlobBSArg;
  Connection ML_Callback ;
  Routine    MLCallback;
  void*      MLArg ;
  Connection MR_Callback ;
  Routine    MRCallback;
  void*      MRArg ;

};
 
#ifdef REMOTE

typedef struct CONNECT Connect;

struct CONNECT {
  Connect Link_items;
  
  char* source;
};

#endif

#include "UPI/upi_proto.h"
#endif /* __UPIDEF_H */
