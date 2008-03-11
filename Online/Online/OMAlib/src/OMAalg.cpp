// $Id: OMAalg.cpp,v 1.2 2008-03-11 18:23:26 ggiacomo Exp $
#include "OMAlib/OMAalg.h"

OMAalg::OMAalg(std::string Name) 
  : OMAMsgInterface(),
    m_name(Name),
    m_type(CHECK),
    m_npars(0)
{
  m_parnames.clear(); 
}

OMAalg::~OMAalg() {
}

std::string OMAalg::type() {
  if(m_type == HCREATOR)
    return "HCREATOR";
  else
    return "CHECK";
}

void OMAalg::setType(std::string type) {
  if(type == "HCREATOR")
    m_type = HCREATOR;
  else
    m_type = CHECK;
}

void OMAalg::setParNames(std::vector<std::string> &ParNames) {
  std::vector<std::string>::iterator i;
  for (i=ParNames.begin(); i!= ParNames.end(); ++i) 
    m_parnames.push_back(*i);  
}

void OMAalg::raiseMessage(OMAMsgLevel level,
                          std::string message,
                          std::string histogramName ) {
  std::string msgstring;
  switch (level) {
  case WARNING:
    msgstring = "WARNING";
    break;
  case ALARM:
    msgstring = "ALARM";
    break;
  default:
    msgstring = "message";
    break;
  }
  msgstring += (" from "+ m_name + " : "+message);
  OMAMsgInterface::raiseMessage(level, msgstring, histogramName);
}
