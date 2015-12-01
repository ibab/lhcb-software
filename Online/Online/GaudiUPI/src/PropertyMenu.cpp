// $ID: $

// Include Files
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiUPI/PropertyMenu.h"
#include "GaudiUPI/PropertyEditor.h"
#include "GaudiUPI/DialogMenu.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include <cstring>
#include <cstdio>

using namespace std;
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
Gaudi::PropertyMenu::PropertyMenu(const string& nam, IInterface* iface, CPP::Interactor* parent)
: SubMenu("Properties of:"+nam, parent), m_name(nam), m_property(0)
{
  SmartIF<IProperty> iprp(iface);
  if ( iprp.isValid() )  {
    m_property = iprp;
    char fmt[128];
    const vector<Property*>& prps = m_property->getProperties();
    size_t nlen = 0, tlen = 0, vlen = 0;
    for(vector<Property*>::const_iterator i=prps.begin(); i != prps.end(); ++i)  {
      const Property* p = (*i);
      string val, type = cleanType(p->type_info());
      m_property->getProperty(p->name(),val);
      if ( val.length()  > vlen ) vlen = val.length();
      if ( type.length() > tlen ) tlen = type.length();
      if ( p->name().length() > nlen ) nlen = p->name().length();
    }
    int cmd = CMD_SHOW+1;
    if ( vlen > 32 ) vlen = 32;
    ::snprintf(fmt,sizeof(fmt),"%%-%lds%%-%lds%%-%lds %%-3s %%-3s",long(nlen+2),long(tlen+2),long(vlen+2));
    m_window->addCOM(cmd,fmt,"Property Name","Type","Value","CBR","CBU");
    for(vector<Property*>::const_iterator i=prps.begin(); i != prps.end(); ++i)  {
      const Property* p = (*i);
      string val, type = cleanType(p->type_info());
      m_property->getProperty(p->name(),val);
      if ( val.length()>33 ) val = val.substr(0,31)+"...";
      m_lines.insert(make_pair(++cmd,p));
      m_window->addCMD(cmd,  fmt, p->name().c_str(), type.c_str(), val.c_str(),
        yesno(p->readCallBack()),yesno(p->updateCallBack()));
    }
  }
  else  {
    m_window->addCOM(CMD_SHOW,"No valid IProperty interface found!");
  }
  mapWindow();
}

/// Standard destructor
Gaudi::PropertyMenu::~PropertyMenu()  {
}

/// Interactor overload: handle menu interupts
void Gaudi::PropertyMenu::handle (const CPP::Event& event)   {
  if ( event.eventtype == UpiEvent )  {
    int cmd = event.command_id;
    if ( cmd > CMD_SHOW )  {
      Lines::iterator i=m_lines.find(cmd);
      if ( i != m_lines.end() )  {
        Property* p = const_cast<Property*>((*i).second);
        if ( m_child ) delete m_child;
        m_child = new PropertyEditor(m_name, p, this);
      }
      return;
    }
  }
  SubMenu::handle(event);
}
