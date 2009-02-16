// $Id: OMAalg.cpp,v 1.5 2009-02-16 10:38:21 ggiacomo Exp $
#include "OMAlib/OMAalg.h"

OMAalg::OMAalg(std::string Name, OMAcommon* OMAenv) 
  : m_name(Name),
    m_type(CHECK),
    m_npars(0),
    m_omaEnv(OMAenv),
    m_needRef(false)
{
  m_parnames.clear(); 
  m_parDefValues.clear(); 
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

void OMAalg::raiseMessage(unsigned int ID,
                          OMAMessage::OMAMsgLevel level,
                          std::string message,
                          std::string& histogramName) {
  if(m_omaEnv) {
    std::string msgstring = (" algorithm "+ m_name + " : "+message);
    m_omaEnv->setAnaId(ID);
    m_omaEnv->raiseMessage(level, msgstring, 
                           histogramName);
  }
}



