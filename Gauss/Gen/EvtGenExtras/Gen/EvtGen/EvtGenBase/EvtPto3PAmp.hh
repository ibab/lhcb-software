/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtPto3PAmp.hh,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

#ifndef EVT_PTO3P_AMP_HH
#define EVT_PTO3P_AMP_HH

#include "EvtGenBase/EvtCyclic3.hh"
#include "EvtGenBase/EvtSpinType.hh"
#include <vector>
using std::vector;
#include "EvtGenBase/EvtAmplitude.hh"
#include "EvtGenBase/EvtTwoBodyVertex.hh"
#include "EvtGenBase/EvtPropagator.hh"
#include "EvtGenBase/EvtDalitzPoint.hh"

class EvtComplex;
class EvtBlattWeisskopf;

class EvtPto3PAmp : public EvtAmplitude<EvtDalitzPoint> {
  

public:

  // Numerator type
  enum NumType {NBW=0,RBW_ZEMACH=1,RBW_KUEHN=2,RBW_CLEO=3};


  EvtPto3PAmp(EvtDalitzPlot dp, EvtCyclic3::Pair pairAng, EvtCyclic3::Pair pairRes,  
	      EvtSpinType::spintype spin, 
	      const EvtPropagator& prop, NumType typeN);
  

  EvtPto3PAmp(const EvtPto3PAmp& other);

  ~EvtPto3PAmp();

  virtual EvtAmplitude<EvtDalitzPoint>* clone() const
  { return new EvtPto3PAmp(*this); }


  virtual EvtComplex amplitude(const EvtDalitzPoint& p) const;
  EvtComplex numerator(const EvtDalitzPoint& p) const;
  double angDep(const EvtDalitzPoint& p) const;

  void set_fd(double R);
  void set_fb(double R);

private:

  // Pairing indices:

  EvtCyclic3::Pair _pairAng;    // angular  
  EvtCyclic3::Pair _pairRes;    // resonance

  // Spin

  EvtSpinType::spintype _spin;                                  

  // Numerator type

  NumType _typeN;


  // _Owned_ pointer to propagator factor

  EvtPropagator* _prop;                  
  double _g0; // nominal width

  // Vertices

  EvtTwoBodyVertex _vb;
  EvtTwoBodyVertex _vd;
};

#endif








