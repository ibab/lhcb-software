// $ID: $

// Include Files
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiUPI/PropertyEditor.h"
#include "GaudiUPI/DialogMenu.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include <sstream>
#include <cstring>

using namespace std;
static int s_lineNo = Gaudi::SubMenu::CMD_CLOSE+1000;

static const char* s_normalize[][2] =  {
    {"  ",                     " " }
  ,{", ",                     "," }
  ,{" signed ",               " " }
  ,{",signed ",               "," }
  ,{"<signed ",               "<" }
  ,{"(signed ",               "(" }
  ,{"ulonglong",              "unsigned long long" }
  ,{"longlong",               "long long" }
  ,{"long unsigned",          "unsigned long" }
  ,{"short unsigned",         "unsigned short" }
  ,{"short int",              "short" }
  ,{"long int",               "long" }
  ,{"basic_string<char> ",    "string"}
  ,{"basic_string<char>",     "string"}
  ,{"basic_string<char,allocator<char> > ","string"}
  ,{"basic_string<char,allocator<char> >","string"}
  ,{"basic_string<char,char_traits<char>,allocator<char> > ","string"}
  ,{"basic_string<char,char_traits<char>,allocator<char> >","string"}
  ,{",allocator<string> ",""}
  ,{",allocator<bool> ",""}
  ,{",allocator<int> ",""}
  ,{",allocator<float> ",""}
  ,{",allocator<unsigned int> ",""}
  ,{",allocator<double> ",""}
};

static string type_name(const string& full_nam)  {
  size_t occ;
  string nam = full_nam; //(full) ? full_nam : cl->Name();
  string s = (nam.substr(0,2) == "::") ? nam.substr(2) : nam;

  /// Completely ignore namespace std
  while ( (occ=s.find("")) != string::npos )    {
    s.replace(occ, 5, "");
  }
  /// Completely ignore namespace std
  while ( (occ=s.find(", ")) != string::npos )    {
    s.replace(occ, 2, ",");
  }

  //
  // Perform naming normalization for primitives
  // since GCC-XML, this pile of shit just generates
  // anything.
  //
  // Let's hope this will not kick us back sometimes.
  //
  for(size_t i=0; i<sizeof(s_normalize)/sizeof(s_normalize[0]); ++i) {
    /// Normalize names
    while ( (occ=s.find(s_normalize[i][0])) != string::npos )    {
      s.replace(occ, strlen(s_normalize[i][0]), s_normalize[i][1]);
    }
  }
  if ( s.find('[') != string::npos ) {
    s = s.substr(0, s.find('['));
  }
  return s;
}


static string cleanType(const type_info* t)  {
  string s = type_name(System::typeinfoName(*t));
  return s;
}

template<class T> inline const char* yesno(T o)  {  return o ? "YES" : "NO"; }

/// Initializing constructor
Gaudi::PropertyEditor::PropertyEditor(const string& nam, Property* prop, CPP::Interactor* parent)
: SubMenu("Property "+prop->name()+" of:"+nam, parent), m_property(prop), m_isVector(false),
  m_isString(false)
{
  string typ = cleanType(m_property->type_info());
  m_isVector = typ.find("vector<") != string::npos;
  m_isString = typ.find("string")  != string::npos;

  if ( m_isVector )
    m_command = new DialogItem("%24s","Add new Value: ^^^^^^^^^^^^^^^^^^^^^^^^","","","",false);
  else
    m_command = new DialogItem("%24s","Set new Value: ^^^^^^^^^^^^^^^^^^^^^^^^","","","",false);
  m_return = new DialogItem("%6s","   ^^^^^^        ^^^^^^        ^^^^^^ ","Cancel");

  m_window->addCOM(CMD_SHOW+1,"");
  m_window->addCOM(CMD_SHOW+2,m_isVector ? "To remove hit return on value ..." : "");
  m_window->addCOM(CMD_LAST_PARAM,"");
  m_window->addCOM(CMD_LAST_PARAM+1,"");
  m_window->addPAR(CMD_SET_PARAM, m_command);
  m_return->addList("Accept");
  m_return->addList("Reset");
  m_return->addList("Cancel");
  m_window->addButtonLine(CMD_CLOSE,m_return);
  mapWindow(false);
  retrieveParams();
  IOCSENSOR.send(this,CMD_PARAMS);
}

void Gaudi::PropertyEditor::retrieveParams()   {
  stringstream s;
  string w = "", v = "";
  s << *m_property;
  while( s.get() != ':' && !s.fail() ) { }
  for (int c=s.get(); !s.fail(); c=s.get()) {
    switch(c)  {
      case '|':
      case '[':
        v = "";
        break;
      case ',':
        m_lines.insert(make_pair(s_lineNo++,v));
        v = "";
        break;
      case '"':
        m_isString = true;
        break;
      case ']':
        goto Done;
        break;
      case ' ':
        if ( v.empty() ) break;
      default:
        v += char(c);
        break;
    }
  }
Done:
  if ( !v.empty() && m_isVector )  {
    m_lines.insert(make_pair(s_lineNo++,v));
  }
  if ( !m_isVector )  {
    m_lines.clear();
    m_lines.insert(make_pair(s_lineNo++,v));
    m_command->setCurrent((ClientData)v.c_str());
  }
  m_defaults = m_lines;
}

void Gaudi::PropertyEditor::deleteItems()   {
  for(Lines::const_iterator i=m_lines.begin(); i != m_lines.end(); ++i)
    m_window->deleteCMD((*i).first);
}

void Gaudi::PropertyEditor::insertItems()   {
  int cnt = 0;
  for(Lines::const_iterator i=m_lines.begin(); i != m_lines.end(); ++i)
    m_window->insertCMD(CMD_LAST_PARAM,(*i).first," [%-2ld]   %s", cnt++, (*i).second.c_str());
}

void Gaudi::PropertyEditor::setCurrent()   {
  if ( m_isVector )  {
    m_command->setCurrent((ClientData)"");
  }
  else {
    char* value = *m_command;
    m_command->setCurrent((ClientData)value);
  }
  m_window->replace(m_command);
  m_window->setCursor(CMD_CLOSE);
}

void Gaudi::PropertyEditor::updateParams()   {
  if ( m_isVector )  {
    deleteItems();
    insertItems();
  }
  setCurrent();
}

void Gaudi::PropertyEditor::setProperties()  {
  string value;
  if ( m_isVector )  {
    value = "[";
    for(Lines::const_iterator i=m_lines.begin(); i != m_lines.end(); ++i) {
      if ( m_isString ) value += '"';
      value += (*i).second;
      if ( m_isString ) value += '"';
      value += ", ";
    }
    value = value.substr(0,value.length()-2);
    value += ']';
  }
  else  {
    value = (char*)*m_command;
  }
  print("New value: %s = \"%s\"",m_property->name().c_str(),value.c_str());
  m_property->fromString(value);
}

/// Standard destructor
Gaudi::PropertyEditor::~PropertyEditor()  {
  delete m_return;
  delete m_command;
}

/// Interactor overload: handle menu interupts
void Gaudi::PropertyEditor::handle (const CPP::Event& e)   {
  if ( e.eventtype == UpiEvent )  {
    // print("Menu:%d Command:%d Param:%d Item:%d",e.menu_id, e.command_id, e.param_id, e.index_id);
    Lines::iterator i;
    if ( e.command_id == CMD_SET_PARAM )  {
      char* value = *m_command;
      if ( m_isVector )  {
        Lines::value_type v(s_lineNo++,value);
        m_lines.insert(v);
        m_window->insertCMD(CMD_LAST_PARAM,v.first," [%-2ld]   %s", m_lines.size(),value);
        m_command->setCurrent((ClientData)"");
        return;
      }
      (*m_lines.begin()).second = value;
      m_command->setCurrent((ClientData)value);
      return;
    }
    else if ( (i=m_lines.find(e.command_id)) != m_lines.end() )  {
      m_window->deleteCMD((*i).first);
      m_lines.erase(i);
      return;
    }
    else if ( e.command_id == CMD_CLOSE )  {
      switch(e.param_id)  
      {
      case 1:
        m_defaults = m_lines;
        setProperties();
        SubMenu::handle(e);
        return;
      case 2:
        if ( m_isVector )  {
          deleteItems();
          m_lines = m_defaults;
          insertItems();
        }
        setCurrent();
        return;
      case 3:
        SubMenu::handle(e);
        return;
      }
    }
  }
  else if ( e.eventtype == IocEvent )  {
    if ( e.type == CMD_PARAMS ) {
      updateParams();
      return;
    }
  }
  SubMenu::handle(e);
}
