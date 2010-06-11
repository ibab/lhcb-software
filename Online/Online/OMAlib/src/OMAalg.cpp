// $Id: OMAalg.cpp,v 1.11 2010-06-11 13:00:11 ggiacomo Exp $
#include "OMAlib/OMAalg.h"
#include "OMAlib/OMAlib.h"
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <sstream>

OMAalg::OMAalg(std::string Name, OMAlib* OMAenv) 
  : m_name(Name),
    m_type(CHECK),
    m_npars(0),
    m_omaEnv(OMAenv),
    m_oh(NULL),                                                 
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

bool OMAalg::getBinLabels(TH1 &Histo) {
  bool hasLabels=false;
  if(m_oh && m_omaEnv->dbSession() && Histo.GetDimension() == 1) {
    if (Histo.GetXaxis()) {
      if (m_oh->nXbinlabels() > 0 && 
          Histo.GetXaxis()->GetNbins() >= (int)m_oh->nXbinlabels() ) {
        hasLabels=true;
        std::string sopt;
        for (unsigned int il = 0; il < m_oh->nXbinlabels(); il++) {
          sopt = m_oh->binlabel(il,0);
          Histo.GetXaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
  }
  return hasLabels;
}

// get the inverse of average weight of histogram (to convert content to supposed counts)
double OMAalg::content2counts(TH1 &Histo) {
  double k2c=1;
  double sum = -1;
  TH1F* hf= dynamic_cast<TH1F*>(&Histo);
  if (hf) sum = hf->GetSum();
  TH2F* hf2= dynamic_cast<TH2F*>(&Histo);
  if (hf2) sum = hf2->GetSum();
  if (sum>0.) k2c = Histo.GetEntries() / sum;
  return k2c;    
}



// ----- OMACheckAlg ------ //

bool OMACheckAlg::notEnoughStats(TH1* h,
                                 float minstatperbin,
                                 float minstatfrac) {
  bool out=false;
  int nb=h->GetNbinsX() * h->GetNbinsY();
  double thr = minstatperbin;
  if(thr<0) thr=m_minEntriesPerBin; // use default value
  if (h->GetEntries() < (nb * thr)) out=true;

  if (!out && minstatfrac>0.) { // check we have enough bins above stat. threshold
    int nok=0;
    double cont2entr = content2counts(*h);
    for (int ix=1; ix<= h->GetNbinsX(); ix++) {
      for (int iy=1 ; iy<= h->GetNbinsY(); iy++) {
        double binEntries =   h->GetBinContent(ix, iy) * cont2entr; //assuming weights are constant..
        if (binEntries > thr) nok++;
      }
    }
    if ( ((double)nok / (double)nb) < minstatfrac) out=true;
  }
  return out;
}



bool OMACheckAlg::checkStats(TH1* h,
                             unsigned int,
                             TH1* ref,
                             std::vector<float> & input_pars,
                             float minstatperbin,
                             float minstatfrac) {
  bool ok=true;
  std::string message="";
  std::string hname=h->GetName();
  if (notEnoughStats(h, minstatperbin, minstatfrac)) {
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

