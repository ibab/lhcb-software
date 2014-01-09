//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/Filter.cpp,v 1.5 2008-10-21 13:53:51 frankb Exp $

// Framework include files
#include "ROLogger/Filter.h"
#include "ROLogger/MessageLine.h"
#include "RTL/strdef.h"
#include <iomanip>
#include <cstring>
#ifdef _WIN32
#define strcasecmp _stricmp
#endif
using namespace ROLogger;
using namespace std;

union _Match {
  const int* p;
  unsigned char* match;
  _Match(const int* m) : p(m) {}  
};

Filter& Filter::operator=(const Filter& f) {
  if ( &f != this ) {
    type      = f.type;
    node      = f.node;
    utgid     = f.utgid;
    component = f.component;
    message   = f.message;
  }
  return *this;
}

void Filter::setNodeMatch(const string& value, unsigned char typ) {
  _Match m(&type);
  m.match[0] = typ;
  node = value;
}

void Filter::setUtgidMatch(const string& value, unsigned char typ) {
  _Match m(&type);
  m.match[1] = typ;
  utgid = value;
}

void Filter::setComponentMatch(const string& value, unsigned char typ) {
  _Match m(&type);
  m.match[2] = typ;
  component = value;
}

void Filter::setMessageMatch(const string& value, unsigned char typ) {
  _Match m(&type);
  m.match[3] = typ;
  message = value;
}

int Filter::acceptMatch(const string& cand, const string& pattern, unsigned char match) const  {
  int result = 0;
  if ( match&MATCH_WILD && match&MATCH_NOCASE )
    result = ::strcase_match_wild(cand.c_str(),pattern.c_str());
  else if ( match&MATCH_WILD )
    result = ::str_match_wild(cand.c_str(),pattern.c_str());
  else if ( match&MATCH_EXACT )
    result = ::strcmp(cand.c_str(),pattern.c_str())==0 ? 1 : 0;
  else if ( match&MATCH_NOCASE )
    result = ::strcasecmp(cand.c_str(),pattern.c_str())==0 ? 1 : 0;

  /// If NOT(the match is 1 and the Select bit is set) ignore this message
  if (  result && (match&MATCH_SELECT) )
    return 1;
  else if ( !result && (match&MATCH_INVERSE) ) 
    return 1;
  else if ( !result && (match&MATCH_REQUIRE) )
    return 2;
  return 0;
}

bool Filter::acceptMessage(const string& message) const {
  MessageLine msg(message);
  return acceptMessage(msg);
}

static void doNothing(...) {}
#define __dbgPrint doNothing

bool Filter::acceptMessage(const MessageLine& msg) const {
  int res;
  _Match m(&type);
  if ( 0 == (res=acceptMatch(msg.node(),node,m.match[0])) ) {
    __dbgPrint("NODE match [failed]: %s <> %s\n",node.c_str(),msg.node().c_str());
    return false;
  }
  if ( 2 == res ) {
    __dbgPrint("NODE match [required]: %s <> %s\n",node.c_str(),msg.node().c_str());
    return true;
  }
  res = 2;
  if ( msg.type() > 0 )  {
    if ( 0 == (res=acceptMatch(msg.utgid(),utgid,m.match[1])) ) {
      __dbgPrint("UTGID match [failed]: %s <> %s\n",utgid.c_str(),msg.utgid().c_str());
      return false;
    }
  }
  if ( 2 == res && 0 != (m.match[1]&MATCH_SELECT) && utgid != "*" )  {
    __dbgPrint("UTGID match [failed]: %s <> %s\n",utgid.c_str(),msg.utgid().c_str());
    return false;
  }
  else if ( 2 == res && 0 != (m.match[1]&MATCH_REQUIRE) )  {
    __dbgPrint("UTGID match [required]: %s <> %s\n",utgid.c_str(),msg.utgid().c_str());
    return true;
  }
  res = 2;
  if ( msg.type() > 0 )  {
    if ( 0 == (res=acceptMatch(msg.component(),component,m.match[2])) ) {
      __dbgPrint("COMPONENT match [failed]: %s <> %s [%d]\n",component.c_str(),msg.component().c_str(),msg.type());
      return false;
    }
  }
  if ( 2 == res && 0 != (m.match[2]&MATCH_SELECT) && component != "*" )  {
    __dbgPrint("COMPONENT match [failed]: %s <> %s [%d]\n",component.c_str(),msg.component().c_str(),msg.type());
    return false;
  }
  else if ( 2 == res && 0 != (m.match[2]&MATCH_REQUIRE) )  {
    __dbgPrint("COMPONENT match [required]: %s <> %s\n",component.c_str(),msg.component().c_str());
    return true;
  }
  if ( 0 == acceptMatch(msg.message(),message,m.match[3]) ) {
    __dbgPrint("MESSAGE match [failed]: %s <> %s\n",message.c_str(),msg.message().c_str());
    return false;
  }
  __dbgPrint("COMPONENT match [ACCEPTED]: %s <> %s [%d]\n",component.c_str(),msg.component().c_str(),m.match[2]&MATCH_SELECT);
  __dbgPrint("MESSAGE match   [ACCEPTED]: %s <> %s [%d]\n",message.c_str(),msg.message().c_str(),msg.type());
  return true;
}

/// Write filter object to file
ostream& Filter::write(ostream& os) const {
  os << "<FILTER>"  <<endl
    << hex    << setw(8) << setfill('0') << type << endl
    << node        << endl
    << utgid       << endl
    << component   << endl
    << message     << endl
    << "</FILTER>" << endl;
  return os;
}

#include "UPI/upidef.h"

/// Dump filter info
void Filter::dump() const {
  ::upic_write_message2("Filter match: Node: [%d]  %s",match()[NODE],node.c_str());
  ::upic_write_message2("             UTGID: [%d]  %s",match()[UTGID],utgid.c_str());
  ::upic_write_message2("         Component: [%d]  %s",match()[COMPONENT],component.c_str());
  ::upic_write_message2("           Message: [%d]  %s",match()[MESSAGE],message.c_str());
}

/// Dump filter info
void Filter::dump(ostream& os) const {
  const unsigned char* m = match();
  os << "Filter match: Node: [" << hex << (int)m[NODE]      << "]  " << node << endl
    << "             UTGID: [" << hex << (int)m[UTGID]     << "]  " << utgid << endl
    << "         Component: [" << hex << (int)m[COMPONENT] << "]  " << component << endl
    << "           Message: [" << hex << (int)m[MESSAGE]   << "]  " << message << endl;
}

/// read filter object from file
size_t Filter::read(istream& in) {
  unsigned int ui;
  int   start = 0;
  char txt[1024];
  while( in.getline(txt,sizeof(txt)).good() ) {
    if( ::strncmp(txt,"<FILTER>",8)==0 )
      start=1;
    else if ( ::strncmp(txt,"</FILTER>",9)==0 ) {
      //dump();
      return 1;
    }
    else if ( start ) {
      char* p = ::strchr(txt,'\n');
      if ( p ) *p = 0;
      //::upic_write_message2("%d: %ld %ld %s",start,siz,::strlen(txt),txt);
      switch(start) {
      case 1:
        ::sscanf(txt,"%08X",&ui);
	type = (int)ui;
        break;
      case 2:
        node = txt;
        break;
      case 3:
        utgid = txt;
        break;
      case 4:
        component = txt;
        break;
      case 5:
        message = txt;
        break;
      default:
        return 0;
      }
      start++;
    }
  }
  return 0;
}
