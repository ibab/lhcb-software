#ifndef __UPIDEF_H
#define __UPIDEF_H

#include <cstdlib>
#include "SCR/lists.h"


/* function return codes */
#define UPI_NORMAL 1
#define UPI_ERROR  0
 
/* error codes */
#define UPI_FACILITY      80*256
 
#define UPI_NEXMENU      (UPI_FACILITY+2)      /* non existing menu id */
#define UPI_NEXCOMM      (UPI_FACILITY+4)      /* non existing command id */
#define UPI_DISCOMM      (UPI_FACILITY+6)      /* command disabled */

#define UPI_SS_DTBFULL	 (UPI_FACILITY+0x4)
#define UPI_SS_ABORTED	 (UPI_FACILITY+0x6)
#define UPI_SS_EMPTYMENU (UPI_FACILITY+0x8)
#define UPI_SS_INVWINDOW (UPI_FACILITY+0xC)
#define UPI_SS_INVFATHER (UPI_FACILITY+0xE)
#define UPI_SS_DUPLCOMM  (UPI_FACILITY+0x10)
#define UPI_SS_NOOPENMENU (UPI_FACILITY+0x12)
#define UPI_SS_ERRCONV    (UPI_FACILITY+0x14)
#define UPI_SS_INVFORM    (UPI_FACILITY+0x16)
#define UPI_SS_INVPARAM   (UPI_FACILITY+0x18)
#define UPI_SS_INVCOMMAND (UPI_FACILITY+0x1C)
#define UPI_SS_INVMENU    (UPI_FACILITY+0x1E)
#define UPI_SS_NORMAL     1

/* Constants for building pages */
#define PREV_PAGE	"<- "
#define NEXT_PAGE	" ->"
#define ONLY_PAGE	"   "
#define PAGE_MARKER_SIZE 3

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

#define RETURN_ON_ACCEPT   0
#define WAKE_UP_ON_CHANGE  1

#define UPI_K_MESSAGE  0
#define UPI_K_OPERATOR 1
#define UPI_K_USER     2

/*  Callback conditions.  */
#define CALL_ON_BACK_SPACE    0x01
#define CALL_ON_ENTER         0x02
#define CALL_ON_DRAG          0x04
/*  Callback conditions.  */
#define CALL_ON_PF1           0x08
#define CALL_ON_ANY_BACKSPACE 0x10
#define CALL_ON_MOVE_LEFT     0x20
#define CALL_ON_MOVE_RIGHT    0x40

/* menu_item.enabled field */
#define DISABLED   0
#define ENABLED    1
 
/* menu_item.type field */
enum {
  COMMENT,
  COMMAND,
  PARAM
} Item_types;

/* menu.type field */
typedef enum {
  NORMAL_MENU,
  PULLDOWN_MENU,
  DETACHED_MENU,
  PARAMETER_PAGE
} Menu_types;

/* Special predefined menus */
#define WARNING (-1)
#define NOTICE  (-2)
#define DLPACK  (-3)

/* parameter page options */
typedef enum {
  ACCEPT_OPTION,
  CANCEL_OPTION,
  RESET_OPTION
} Parameter_page_options;
 
/* item.type field */
typedef enum {
  ASC_FMT,
  OCT_FMT,
  DEC_FMT,
  HEX_FMT,
  REAL_FMT,
  BIN_FMT,
  LOG_FMT
} Parameter_types;

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
  
  Parameter_types type;
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
