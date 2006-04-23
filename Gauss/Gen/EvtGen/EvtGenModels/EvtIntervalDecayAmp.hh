//-----------------------------------------------------------------------
// File and Version Information: 
//      $Id: EvtIntervalDecayAmp.hh,v 1.3 2006-04-23 20:36:33 robbep Exp $
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

// Decay model that uses the "amplitude on an interval"
// templatization

#ifndef EVT_INTERVAL_DECAY_AMP
#define EVT_INTERVAL_DECAY_AMP

#define VERBOSE true
#include <iostream>
#include <vector>
#include <string>
#include "EvtGenBase/EvtDecayAmp.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtMacros.hh"
#include "EvtGenBase/EvtPdf.hh"
#include "EvtGenBase/EvtAmpFactory.hh"
#include "EvtGenBase/EvtMultiChannelParser.hh"
#include "EvtGenBase/EvtAmpPdf.hh"
#include "EvtGenBase/EvtIncoherentMixing.hh"
#include "EvtGenBase/EvtPDL.hh"

template <class T>
class EvtIntervalDecayAmp : public  EvtDecayAmp {

public:
  
  EvtIntervalDecayAmp()
    : _probMax(0.), _nScan(0), _fact(0)
  {}

  EvtIntervalDecayAmp(const EvtIntervalDecayAmp<T>& other)
    : _probMax(other._probMax), _nScan(other._nScan),
      COPY_PTR(_fact)
  {}

  virtual ~EvtIntervalDecayAmp()
  {
    delete _fact;
  }


  // Initialize model

  virtual void init()
  {
    // Collect model parameters and parse them
    
    vector<std::string> args;
    int i;
    for(i=0;i<getNArg();i++) args.push_back(getArgStr(i));
    EvtMultiChannelParser parser;
    parser.parse(args);
    
    // Create factory and interval
    
    if(VERBOSE) report(INFO,"EvtGen") << "Create factory and interval" << std::endl;
    _fact = createFactory(parser);
    
    
    // Maximum PDF value over the Dalitz plot can be specified, or a scan 
    // can be performed.
    
    _probMax = parser.pdfMax();
    _nScan = parser.nScan();
    if(VERBOSE) report(INFO,"EvtGen") << "Pdf maximum " << _probMax << std::endl;
    if(VERBOSE) report(INFO,"EvtGen") << "Scan number " << _nScan << std::endl;    
  }
  
    
  virtual void initProbMax()
  {
    if(0 == _nScan) {
      
      if(_probMax > 0) setProbMax(_probMax);
      else assert(0);
    }
    else {
      
      double factor = 1.2; // increase maximum probability by 20%
      EvtAmpPdf<T> pdf(*_fact->getAmp());
      EvtPdfSum<T>* pc = _fact->getPC();
      EvtPdfDiv<T> pdfdiv(pdf,*pc);
      printf("Sampling %d points to find maximum\n",_nScan);
      EvtPdfMax<T> x = pdfdiv.findMax(*pc,_nScan);
      _probMax = factor * x.value();
      printf("Found maximum %f\n",x.value());
      printf("Increase to   %f\n",_probMax);
      setProbMax(_probMax);
    }
  }
      
  virtual void decay(EvtParticle *p)
  {
    // Set things up in most general way
    
    static EvtId B0=EvtPDL::getId("B0");
    static EvtId B0B=EvtPDL::getId("anti-B0");
    double t;
    EvtId other_b;  
    EvtComplex ampl(0.,0.);
    
    // Sample using pole-compensator pdf
    
    EvtPdfSum<T>* pc = _fact->getPC();
    T x = pc->randomPoint();
    
    if(_fact->isCPModel()) {
      
      EvtIncoherentMixing::OtherB(p,t,other_b);
      EvtComplex A = _fact->getAmp()->evaluate(x);
      EvtComplex Abar = _fact->getAmpConj()->evaluate(x);
      double dm = _fact->dm();
      
      if (other_b==B0B) ampl=A*cos(dm*t/(2*EvtConst::c))+Abar*sin(dm*t/(2*EvtConst::c));
      if (other_b==B0) ampl=A*cos(dm*t/(2*EvtConst::c))-Abar*sin(dm*t/(2*EvtConst::c));
    }
    else {
      
      ampl = _fact->getAmp()->evaluate(x);
    }
    
    // Pole-compensate
    
    double comp = sqrt(pc->evaluate(x));
    assert(comp > 0);
    vertex(ampl/comp);
    
    // Now generate random angles, rotate and setup 
    // the daughters
    
    std::vector<EvtVector4R> v = initDaughters(x);
    
    unsigned int N = p->getNDaug();  
    if(v.size() != N) {
      
      report(INFO,"EvtGen") << "Number of daughters " << N << std::endl;
      report(INFO,"EvtGen") << "Momentum vector size " << v.size() << std::endl;
      assert(0);
    }
    
    unsigned int i;
    for(i=0;i<N;i++){
      
      p->getDaug(i)->init(getDaugs()[i],v[i]);
    }    
  }
  
  virtual EvtAmpFactory<T>* createFactory(const EvtMultiChannelParser& parser) = 0;
  virtual std::vector<EvtVector4R> initDaughters(const T& p) const = 0;

protected:

  double _probMax;          // Maximum probability
  int _nScan;               // Number of points for max prob DP scan

  EvtAmpFactory<T>*  _fact; // factory
};


#endif





