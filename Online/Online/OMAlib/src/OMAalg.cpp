// $Id: OMAalg.cpp,v 1.10 2010-01-26 14:25:37 ggiacomo Exp $
#include "OMAlib/OMAalg.h"
#include "OMAlib/OMAlib.h"
#include <TH1.h>
#include <sstream>

OMAalg::OMAalg(std::string Name, OMAlib* OMAenv) 
  : m_name(Name),
    m_type(CHECK),
    m_npars(0),
    m_omaEnv(OMAenv),
    m_oh(NULL)
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


void OMAalg::raiseMessage(unsigned int Id,
                          bool warnCondition,
                          bool alarmCondition,
                          std::string message,
                          std::string& histogramName) {
  OMAMessage::OMAMsgLevel level=OMAMessage::INFO;
  if (alarmCondition) {
    level = OMAMessage::ALARM;
  }
  else if (warnCondition) {
    level = OMAMessage::WARNING;
  }
  if (level != OMAMessage::INFO) {
    raiseMessage(Id, level, message, histogramName);
  }
  return;
}

bool OMACheckAlg::notEnoughStats(TH1* h) {
  int nb=h->GetXaxis()->GetNbins();
  return (h->GetEntries() < m_minEntries || 
          h->GetEntries() < (nb * m_minEntriesPerBin) );
}


bool OMACheckAlg::refMissing(TH1* ref,
                        std::vector<float> & input_pars) {
  ref = (input_pars.empty() ? ref : NULL); // cheat compiler (avoid warnings)
  return false; // never mind by default (will be overloaded for algorihtm who care)
}


bool OMACheckAlg::checkStats(TH1* h,
                        unsigned int anaID,
                        TH1* ref,
                        std::vector<float> & input_pars) {

  bool ok=true;
  std::string message="";
  std::string hname=h->GetName();
  if (notEnoughStats(h)) {
    raiseMessage(anaID, OMAMessage::NOSTAT, message, hname);
    std::stringstream warn;
    warn << "not enough stats for histogram " << hname <<
      ": entries=" << h->GetEntries();
    m_omaEnv->sendinfo(warn.str().data());
    ok=false;
  }
  if (refMissing(ref, input_pars))  {
    std::string warn = "Reference requested but not available for algorithm " +
      m_name;
    m_omaEnv->sendwarning(warn.data());
    ok=false;
  }
  return ok;
}

