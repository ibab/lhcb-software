//-----------------------------------------------------------------------
// File and Version Information: 
//      $Id: EvtPto3PAmpFactory.hh,v 1.1 2003-10-02 17:25:55 robbep Exp $
// 
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998 Caltech, UCSB
//
// Module creator:
//      Alexei Dvoretskii, Caltech, 2001-2002.
//-----------------------------------------------------------------------

#ifndef EVT_PTO3P_AMP_FACTORY_HH
#define EVT_PTO3P_AMP_FACTORY_HH

#include <vector>
#include <string>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtAmpFactory.hh"
#include "EvtGenBase/EvtDalitzPlot.hh"
#include "EvtGenBase/EvtDalitzPoint.hh"

class EvtPto3PAmpFactory : public EvtAmpFactory<EvtDalitzPoint> {
  
public:
  
  EvtPto3PAmpFactory(const EvtDalitzPlot& dp) 
    : EvtAmpFactory<EvtDalitzPoint>(), _dp(dp)
  {}
  EvtPto3PAmpFactory(const EvtPto3PAmpFactory& other) 
    : EvtAmpFactory<EvtDalitzPoint>(other), _dp(other._dp)
  {}
  virtual ~EvtPto3PAmpFactory() 
  {}
  
  virtual EvtAmpFactory<EvtDalitzPoint>* clone() const 
  { 
    return new EvtPto3PAmpFactory(*this); 
  }
  
  virtual void processAmp(EvtComplex c, std::vector<std::string> vv, bool conj);
  
private:

  EvtDalitzPlot _dp;

};

#endif





