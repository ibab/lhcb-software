#include <cstring>
#include "RTL/rtl.h"
#include "CPP/Event.h"
#include "GaudiUPI/DialogMenu.h"
#include "GaudiUPI/DialogDisplay.h"
#include "GaudiUPI/DialogDisplayFactory.h"
using namespace std;

template<typename T> static inline void release(T*& x) { if ( x != 0 ) { delete x; x = 0; } }


//extern "C" int utl_print_file(dsc$descriptor&, dsc$descriptor&);

DialogSubDisplay::DialogSubDisplay (AbstractMainDisplay& source)
: m_window(0)
{
  m_parent = 0;
  m_x = m_y = 0;
  m_currLine = 0;
  m_width  = source.width();
  m_height = source.height();
  m_board  = new DisplayBoard*[height()];
  for (size_t i = 0; i < height(); i++ ) 
    m_board[i] = new DisplayBoard(width());
}

DialogSubDisplay::~DialogSubDisplay()   {
  for (size_t i = 0; i < height(); i++ ) delete m_board[i];
  delete m_board;
  delete m_window;
}

int DialogSubDisplay::putString (const string& text,int video)   {
  if ( currentLine() >= height() )    {
    m_currLine++;
    return DISPLAY_ERROR;
  }
  else {
    m_board[m_currLine++]->set(" " + text, video);
    return DISPLAY_SUCCESS;
  }
}

int DialogSubDisplay::init (size_t num_col,size_t num_row,AbstractMainDisplay& refpar)    {
  string pad(width(),' ');
  AbstractSubDisplay::init(num_col, num_row, refpar);
  DialogMainDisplay *par = (DialogMainDisplay*)parent();
  DialogSubMenuCreator win(refpar.actor(),par->window(),this);

  win->window();
  if ( label().Border() & DISPLAY_TOP_BORDER )
    m_window = win->createSubMenu ( label(), pad);
  else if ( label().Border() & DISPLAY_BOTTOM_BORDER )
    m_window = win->createSubMenu ( pad, label() );
  else
    m_window = win->createSubMenu ( "", "" );

  for (size_t line=0; line < height(); line++ ) {
    const string& s = buffer(line).resetChange().pad('-');
    win->addCOM ( line+1, s.c_str() );
    //win->disableCMD ( line+1 );
  }
  win->map ( );
  win->place ( num_col, num_row+4 );
  return DISPLAY_SUCCESS;
}

int DialogSubDisplay::beginUpdate()    {
  return AbstractSubDisplay::beginUpdate();
}

int DialogSubDisplay::endUpdate()  {
  bool change_label = false;
  string text;
  // if ( height() > 0 ) window()->enableCMD( 1 );
  for ( size_t line=0; line < height(); line++ ) {
    text = buffer(line);
    if ( !buffer(line).changed() ) continue;
    int lin = line+1;
    const string& s = buffer(line).resetChange().pad();
    window()->replCOM ( lin, s.c_str() );
  }
  text = Overflow();
  if ( currentLine() > height() && text == "" ) {
    Overflow().set("  Display Overflow   ");
    change_label = true;
  }
  else if ( currentLine() <= height() && !(text == "") ) {
    Overflow().set(" ");
    change_label = true;
  }
  if ( change_label ) {
    if ( label().Border() & DISPLAY_TOP_BORDER )
      m_window->replaceTitle ( label(),0,Overflow());
    else if ( label().Border() & DISPLAY_BOTTOM_BORDER )
      m_window->replaceTitle ( Overflow(),0,label() );
    else
      m_window->replaceTitle ( " ",0,Overflow() );
  }
  //m_window->setCursor(1);
  return AbstractSubDisplay::endUpdate();
}

int DialogSubDisplay::repaint()
{
  for(size_t i=0; i < height(); i++ ) buffer(i).setChange();
  return endUpdate();
}

int DialogSubDisplay::close()  {
  release(m_window);
  return AbstractSubDisplay::close();
}

int DialogSubDisplay::map()    {
  window()->map();
  return AbstractSubDisplay::map();
}

DialogMainDisplay::DialogMainDisplay (const AbstractMainDisplay& source) 
{
  const char *place_holder = " ^^^^^^^^^^^^ ^^^^^ ^^^^^^^ ";
  m_actor = 0;
  m_handler = 0;
  m_width = source.width();
  m_height = source.height();
  m_par = new DialogItem("A17",place_holder,"","","",true);
  m_par->deleteList();
  m_par->addList("Exit Display");
  m_par->addList("Print");
  m_par->addList("repaint");
  DialogMenuCreator win ( this );
  char txt[64];
  lib_rtl_get_node_name(txt,sizeof(txt));
  string title = txt;
  title += "::";
  lib_rtl_get_process_name(txt,sizeof(txt));
  title += txt;
  m_window = win->create(" ", title, " ");
  m_id = win->id();
  win->addButtonLine(1, m_par);
  win->map();
}
// OMTENDFUNCTION //

DialogMainDisplay::DialogMainDisplay (DialogMenu* parent) 
{
  m_actor = parent->actor();
  m_handler = 0;
  m_width = 0;
  m_height = 0;
  m_par = 0;
  m_id = parent->id();
  m_window = parent;
}

DialogMainDisplay::~DialogMainDisplay()
{
  AbstractMainDisplay::close();
  release(m_par);
  release(m_window);
}

int DialogMainDisplay::print (const string& printer)  {
  char TheFile[132];
  size_t w = width();
  char *text = new char[width()], *label = new char[w];
  FILE *destination = stdout;
  if ( !printer.empty() ) {
    ::snprintf(TheFile,sizeof(TheFile),"display_prt_%X.txt",lib_rtl_pid());
    destination = ::fopen(TheFile,"w");
    if ( destination == 0 ) destination = stdout;
  }
  for(size_t line = 0; line <= height()+1; line++) {
    ::memset(text,' ',width());
    for(Children::iterator i=children().begin(); i!=children().end(); ++i)  {
      DialogSubDisplay& child((DialogSubDisplay&)**i);
      int bord = child.label().Border();
      size_t xpos = child.x();
      size_t ypos = child.y() + 1;
      size_t high = child.height();
      size_t wid  = child.width();
      const string& l = child.label();
      ::strncpy(label,l.c_str(),w);
      label[w-1] = 0;
      if ( bord & DISPLAY_BORDER_LINE ) {
        wid -= 2;
        ypos--;
        high++;
        if ( line == ypos || line == ypos+high )  {
          if ( bord & DISPLAY_BORDER_LINE ) {
            ::memset(&text[xpos], '-', wid);
            text[xpos-1] = text[xpos+wid] = '+';
          }
          if ( bord & DISPLAY_BOTTOM_BORDER && line == ypos+high )
            ::memcpy(&text[xpos],label,min(strlen(label),wid+1));
          else if ( bord & DISPLAY_TOP_BORDER && line == ypos )
            ::memcpy(&text[xpos],label,min(strlen(label),wid+1));
        }
        else if ( line > ypos && line <= ypos + high )  {
          const string& s = child.buffer(line-ypos-1);
          ::memcpy(&text[xpos-1],s.c_str(),wid+1);
          text[xpos-1] = text[xpos+wid] = '|';
        }
      }
      else if ( line >= ypos && line < ypos + high ) {
        const string& s = child.buffer(line-ypos);
        ::memcpy(&text[xpos-1],s.c_str(),wid);
      }
    }
    text[width()] = 0;
    ::fputs(text,destination);
    ::fputs("\n",destination);
  }
  if ( destination != stdout ) {
    string ThePrinterQueue(printer.empty() ? "LWACR$TEXT_132" : printer);
    ::fclose(destination);
    if ( !ThePrinterQueue.empty() ) {
      //int status = utl_print_file ( TheFile, ThePrinterQueue );
    }
  }
  return DISPLAY_SUCCESS;
}
// OMTENDFUNCTION //

//012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678912
//+--------------------------------------------------------------------------------------------------------------------------------+  
void DialogMainDisplay::handle (const CPP::Event& ev) {
  if ( m_actor != 0 && m_handler != 0 && ev.command_id == 1 ) {
    DisplayAstData ast_data;
    ast_data.astparameter = m_actor;
    switch ( ev.param_id ) {
        case 1:   ast_data.byte[0] = 25; break; // Ctrl-P == Exit
        case 2:   ast_data.byte[0] = 16; break; // Ctrl-P == Print
        case 3:   ast_data.byte[0] = 23; break; // Ctrl-W == RePaint
        case 4:   ast_data.byte[0] = 0;  break; // Help
        default:  ast_data.byte[0] = 0;  break;
    }
    (*m_handler)(&ast_data);
  }
}

int DialogMainDisplay::endUpdate()  {
  return AbstractMainDisplay::endUpdate();
}

AbstractMainDisplay* DialogDisplayFactory::createMainDisplay (const AbstractDisplay& data)  {
  DialogMainDisplay *disp = new DialogMainDisplay((AbstractMainDisplay&)data);
  return disp;
}

AbstractSubDisplay* DialogDisplayFactory::createSubDisplay (const AbstractDisplay& data)  {
  DialogSubDisplay *disp = new DialogSubDisplay((AbstractMainDisplay&)data);
  return disp;
}
