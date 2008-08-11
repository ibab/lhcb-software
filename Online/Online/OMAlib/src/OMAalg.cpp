// $Id: OMAalg.cpp,v 1.3 2008-08-11 08:05:16 ggiacomo Exp $
#include "OMAlib/OMAalg.h"

OMAalg::OMAalg(std::string Name, OMAcommon* OMAenv) 
  : m_name(Name),
    m_type(CHECK),
    m_npars(0),
    m_omaEnv(OMAenv),
    m_needRef(false),
    m_ref(NULL)
{
  m_parnames.clear(); 
}

OMAalg::~OMAalg() {
}

std::string OMAalg::typestr() {
  if(m_type == HCREATOR)
    return "HCREATOR";
  else
    return "CHECK";
}


void OMAalg::setParNames(std::vector<std::string> &ParNames) {
  std::vector<std::string>::iterator i;
  for (i=ParNames.begin(); i!= ParNames.end(); ++i) 
    m_parnames.push_back(*i);  
}

void OMAalg::raiseMessage(OMAMsgInterface::OMAMsgLevel level,
                          std::string message,
                          std::string histogramName ) {
  if(m_omaEnv) {
    std::string msgstring;
    switch (level) {
    case OMAMsgInterface::WARNING:
      msgstring = "WARNING";
      break;
    case OMAMsgInterface::ALARM:
      msgstring = "ALARM";
      break;
    default:
      msgstring = "message";
      break;
    }
    msgstring += (" from "+ m_name + " : "+message);
    m_omaEnv->raiseMessage(level, msgstring, histogramName);
  }
}



