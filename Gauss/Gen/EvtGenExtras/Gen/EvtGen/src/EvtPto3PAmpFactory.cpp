//-----------------------------------------------------------------------
// File and Version Information: 
//      $Id: EvtPto3PAmpFactory.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
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

// AmpFactory for building a P -> 3P decay
// (pseudoscalar to three pseudoscalars)

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtConst.hh"
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtCyclic3.hh"
#include "EvtGenBase/EvtSpinType.hh"
#include "EvtGenBase/EvtPto3PAmp.hh"
#include "EvtGenBase/EvtFlatAmp.hh"
#include "EvtGenBase/EvtPto3PAmpFactory.hh"
#include "EvtGenBase/EvtPropBreitWigner.hh"
#include "EvtGenBase/EvtPropBreitWignerRel.hh"
#include "EvtGenBase/EvtDalitzResPdf.hh"
#include "EvtGenBase/EvtDalitzFlatPdf.hh"

using namespace EvtCyclic3;

void EvtPto3PAmpFactory::processAmp(EvtComplex c, std::vector<std::string> vv, bool conj)
{
  if(_verbose) {
    
    printf("Make amplitude\n");
    unsigned i;
    for(i=0;i<vv.size();i++) printf("%s\n",vv[i].c_str());
    printf("\n");
  }
  
  EvtAmplitude<EvtDalitzPoint>* amp = 0;
  EvtPdf<EvtDalitzPoint>* pdf = 0;
  std::string name;
  
  unsigned int i;
  if(vv[0] == "PHASESPACE") {
    
    pdf = new EvtDalitzFlatPdf(_dp);
    amp = new EvtFlatAmp<EvtDalitzPoint>();
    name = "NR";
  }
  else if(vv[0] == "RESONANCE") {

    EvtPto3PAmp* partAmp = 0;
      
    // RESONANCE stanza
    
    Pair pairRes = strToPair(vv[1].c_str());
    EvtSpinType::spintype spinR;
    double mR, gR;      
    name = vv[2];
    EvtId resId = EvtPDL::getId(vv[2]);
    if(_verbose) printf("Particles %s form resonance %s\n",vv[1].c_str(),vv[2].c_str());

    // If no valid particle name is given, assume that 
    // it is the spin, the mass and the width of the particle.
      
    if(resId.getId() == -1) {
	
      switch(atoi(vv[2].c_str())) {
	
      case 0: { spinR = EvtSpinType::SCALAR; break; }
      case 1: { spinR = EvtSpinType::VECTOR; break; }
      case 2: { spinR = EvtSpinType::TENSOR; break; }
      case 3: { spinR = EvtSpinType::SPIN3; break; }
      case 4: { spinR = EvtSpinType::SPIN4; break; }
      default: { assert(0); break; }
      }
	
      mR = strtod(vv[3].c_str(),0);
      gR = strtod(vv[4].c_str(),0);
      i = 4;
    }
    else {
      
      // For a valid particle get spin, mass and width
      
      spinR = EvtPDL::getSpinType(resId);
      mR = EvtPDL::getMeanMass(resId);
      gR = EvtPDL::getWidth(resId);
      i = 2;
      
      // It's possible to specify mass and width of a particle 
      // explicitly
      
      if(vv[3] != "ANGULAR") {
	
	if(_verbose) 
	  printf("Setting m(%s)=%s g(%s)=%s\n",
		 vv[2].c_str(),vv[3].c_str(),vv[2].c_str(),vv[4].c_str());

	mR = strtod(vv[3].c_str(),0);
	gR = strtod(vv[4].c_str(),0);
	i = 4;
      }
    }
    
    // ANGULAR stanza
    
    if(vv[++i] != "ANGULAR") {

      printf("%s instead of ANGULAR\n",vv[i].c_str());
      exit(0);
    }
    Pair pairAng = strToPair(vv[++i].c_str());
    if(_verbose) printf("Angle is measured between particles %s\n",vv[i].c_str());
      
    // TYPE stanza
    
    assert(vv[++i] == "TYPE");
    std::string type = vv[++i];
    if(_verbose) printf("Propagator type %s\n",vv[i].c_str());
    
    if(type == "NBW") {      

      EvtPropBreitWigner prop(mR,gR);
      partAmp = new EvtPto3PAmp(_dp,pairAng,pairRes,spinR,prop,EvtPto3PAmp::NBW);
    }
    else if(type == "RBW_ZEMACH") {
      
      EvtPropBreitWignerRel prop(mR,gR);
      partAmp = new EvtPto3PAmp(_dp,pairAng,pairRes,spinR,prop,EvtPto3PAmp::RBW_ZEMACH);
    }
    else if(type == "RBW_KUEHN") {
      
      EvtPropBreitWignerRel prop(mR,gR);
      partAmp = new EvtPto3PAmp(_dp,pairAng,pairRes,spinR,prop,EvtPto3PAmp::RBW_KUEHN);
    }
    else if(type == "RBW_CLEO") {
      
      EvtPropBreitWignerRel prop(mR,gR);
      partAmp = new EvtPto3PAmp(_dp,pairAng,pairRes,spinR,prop,EvtPto3PAmp::RBW_CLEO);
    }     
    else assert(0);
      
    // Optional DVFF, BVFF stanzas
    
    if(i < vv.size()- 1) {
      if(vv[i+1] == "DVFF") {	
	i++;
	if(vv[++i] == "BLATTWEISSKOPF") {
	  
	  double R = strtod(vv[++i].c_str(),0);
	  partAmp->set_fd(R);
	}
	else assert(0);
      }
    }
      
    if(i < vv.size() - 1) {
      if(vv[i+1] == "BVFF") {	
	i++;
	if(vv[++i] == "BLATTWEISSKOPF") {

	  if(_verbose) printf("BVFF=%s\n",vv[i].c_str());
	  double R = strtod(vv[++i].c_str(),0);
	  partAmp->set_fb(R);
	}
	else assert(0);
      }
    }
      
    i++;
    
    pdf = new EvtDalitzResPdf(_dp,mR,gR,pairRes);
    amp = partAmp;
  }

  assert(amp);
  assert(pdf);

  if(!conj) {

    _amp->addOwnedTerm(c,amp);
    _pc->addOwnedTerm(abs2(c),pdf);
  }
  else {
    
    _ampConj->addOwnedTerm(c,amp);
    delete pdf;
  }
  _names.push_back(name);
}
  






