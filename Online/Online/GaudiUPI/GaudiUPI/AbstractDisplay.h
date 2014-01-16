// DECLARATIONS FOR OMT MODULE abstractdisp
#ifndef _abstractdisp_H
#define _abstractdisp_H 1

#include <string>
#include <vector>
#include "RTL/rtl.h"

enum DisplayProperties    {
  DISPLAY_NOBORDER      = 1<<5,
  DISPLAY_BORDER_LINE   = 1<<6,
  DISPLAY_TOP_BORDER    = 1<<7,
  DISPLAY_BOTTOM_BORDER = 1<<8
};
enum DisplayPresentation    {
  DISPLAY_NORMAL    = 1<<1,
  DISPLAY_BLINK     = 1<<2,
  DISPLAY_REVERSE   = 1<<3,
  DISPLAY_BOLD      = 1<<4,
  DISPLAY_UNDERLINE = 1<<5
};
enum DisplayCommands    {
  DISPLAY_START_UPDATE,
  DISPLAY_STOP_UPDATE,
  DISPLAY_CLOSE,
  DISPLAY_PRINT,
  DISPLAY_UPDATE,
  DISPLAY_REPAINT,
  DISPLAY_MAP,
  DISPLAY_EXIT
};
enum DisplayStatus
{
  DISPLAY_ERROR = false,
  DISPLAY_SUCCESS = true
};


class Interactor;
struct DisplayAstData;
class AbstractDisplay;
class AbstractSubDisplay;
class AbstractMainDisplay;
class AbstractDisplayFactory;

class DisplayText {
protected:
  std::string m_board;
public:
  DisplayText ( int width = 1 ) : m_board(width,' ') {  }
  DisplayText ( const DisplayText& copy )  : m_board(copy.m_board) {  }
  virtual ~DisplayText()        {     }
  DisplayText& operator=(const DisplayText& c)  {
    if ( this != &c ) m_board = c.m_board;
    return *this;
  }
  int width() const             {    return m_board.length();  }
  operator const std::string& () const  {    return m_board;  }
  DisplayText& pad(const char c = ' ');
  DisplayText& set(const std::string& txt);
};

class DisplayBoard : public DisplayText {
protected:
  int m_video;
  bool m_change;
public:
  DisplayBoard (int width, int video = DISPLAY_NORMAL );
  DisplayBoard (const DisplayBoard& copy );
  virtual ~DisplayBoard ( void );
  DisplayBoard& operator=(const DisplayBoard& c)  {
    if ( this != &c ) { m_video = c.m_video; m_change = c.m_change; }
    return *this;
  }
  DisplayBoard& set(const std::string& txt, int video = DISPLAY_NORMAL);
  bool changed() const {
    return m_change;
  }
  DisplayBoard& setChange() {
    m_change = true;
    return *this;
  }
  DisplayBoard& resetChange() {
    m_change = false;
    return *this;
  }
  int video(void) const {
    return m_video;
  }
};

class DisplayLabel : public DisplayBoard {
protected:
  int m_border;
public:
  DisplayLabel(const std::string& txt = " ", int video = DISPLAY_NORMAL, int border = DISPLAY_NOBORDER) 
    : DisplayBoard(txt.length()+1,video), m_border(border)
  {
    set(txt);
  }
  virtual ~DisplayLabel() {
  }
  int Border() const {
    return m_border;
  }
  DisplayLabel& Set(const std::string& txt, int video = DISPLAY_NORMAL, int bord = DISPLAY_NOBORDER) {
    DisplayBoard::set( txt, video );
    m_border = bord;
    return *this;
  }
};


struct DisplayAstData   {
public:
  int pasteboardID;
  Interactor* astparameter;
  char byte[4];
};

class AbstractDisplay  {
public:
  AbstractDisplay (size_t w = 1,size_t h = 1, int id = 0, int status = 1) 
  : m_height(h),m_width(w), m_status(status), m_id(id)
  {
  }
  AbstractDisplay (const AbstractDisplay& c) 
  : m_height(c.m_height), m_width(c.m_width), m_status(c.m_status), m_id(c.m_id) 
  {
  }
  virtual ~AbstractDisplay() {}
  //  Standard destructor
  //      Arguments: None
  AbstractDisplay& operator=(const AbstractDisplay& c)  {
    if ( this != &c ) { 
      m_height = c.m_height;
      m_width = c.m_width;
      m_status = c.m_status;
      m_id = c.m_id;
    }
    return *this;
  }
  virtual int id() const     {    return m_id;                            }
  size_t height() const      {    return m_height;                        }
  size_t width() const       {    return m_width;                         }
  bool ok() const            {    return ::lib_rtl_is_success(m_status);  }
  //  Ok:
  //      Indication of success (used mainly for test of the constructor
  //      Checking the return values of the constructor and the 
  //      initialisation should be made always, since any creation 
  //      error cannot be detected otherwise.
  //      Return Value:  1     :  everything OK
  //                     0    :  failure
  int Status() const         {    return m_status;                        }
protected:
  size_t m_height;
  size_t m_width;
  int m_status;
  int m_id;
};

class AbstractSubDisplay : public AbstractDisplay
{
protected:
  size_t m_x;
  size_t m_y;
  size_t m_currLine;
  AbstractMainDisplay* m_parent;
  DisplayLabel m_label;
public:
  static AbstractSubDisplay* create (const AbstractDisplay& data);
  DisplayLabel& label()         {     return m_label;    }
  AbstractMainDisplay* parent() {     return m_parent;    }
  size_t x() const              {     return m_x;         }
  size_t y() const              {     return m_y;         }
  size_t currentLine() const    {     return m_currLine;  }
  virtual int map();
  virtual int close();
  virtual int flush();
  virtual int repaint();
  virtual int endUpdate();
  virtual int beginUpdate();
  bool operator==(const AbstractSubDisplay& cmp) const;
  virtual int putString (const std::string& text,int video = DISPLAY_NORMAL) = 0;
  virtual int init (size_t num_col,size_t num_row, AbstractMainDisplay& parent);
  virtual int label (const std::string& text,int ind = DISPLAY_TOP_BORDER,int flag = DISPLAY_BOLD|DISPLAY_REVERSE);
};

class AbstractMainDisplay : public AbstractDisplay
{
public:
  typedef int (* HandlerType)(DisplayAstData*);
  typedef std::vector<AbstractSubDisplay*> Children;

  static AbstractMainDisplay* create (const AbstractDisplay& data);
  virtual int print (const std::string& printer = 0) = 0;
  Children& children() {     return m_children; }
  Interactor* actor()  {     return m_actor;    }
  virtual int map();
  virtual int close();
  virtual int flush();
  virtual int repaint();
  virtual int endUpdate();
  virtual int beginUpdate();
  virtual int SetOutOfBandAsts (int ast_map,HandlerType handle,Interactor* actor);
protected:
  Interactor  *m_actor;
  HandlerType  m_handler;
  Children     m_children;
};

class AbstractDisplayFactory    {
public:
  virtual AbstractMainDisplay* createMainDisplay (const AbstractDisplay& data) = 0;
  virtual AbstractSubDisplay* createSubDisplay (const AbstractDisplay& data) = 0;
  static  AbstractDisplayFactory* instance();
  static  void setInstance(AbstractDisplayFactory* s);
};
#endif
