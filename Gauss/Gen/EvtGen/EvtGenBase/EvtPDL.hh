//--------------------------------------------------------------------------
// 
// Environment: 
// This software is part of the EvtGen package developed jointly 
// for the BaBar and CLEO collaborations.  If you use all or part 
// of it, please give an appropriate acknowledgement.
// 
// Copyright Information: See EvtGen/COPYRIGHT 
// Copyright (C) 1998 Caltech, UCSB 
// 
// Module: EvtGen/EvtPDL.hh 
// 
// Description:Class to keep track of particle properties.  
// 
// Modification history: 
//
// DJL/RYD September 25, 1996 Module created 
//
//------------------------------------------------------------------------

#ifndef EVTPDL_HH
#define EVTPDL_HH

#include "EvtGenBase/EvtPartProp.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtSpinType.hh"
#include "EvtGenBase/EvtStringHash.hh"
#include <vector>
#include <map>

const int SPIN_NAME_LENGTH = 100;

class EvtPDL {

public:

  EvtPDL();  

  ~EvtPDL();

  void read(const char* fname);
  void readPDT(const std::string fname);

  
  static double getMeanMass(EvtId i ){ return _partlist[i.getId()].getMass(); }
  static double getMass(EvtId i ){return _partlist[i.getId()].rollMass();}
  static double getRandMass(EvtId i, EvtId *parId, int nDaug, EvtId *dauId, 
                            EvtId *othDaugId,double maxMass, 
                            double *dauMasses )
  {return _partlist[i.getId()].getRandMass(parId,nDaug,dauId,
                                           othDaugId,maxMass,dauMasses);}
  static double getMassProb(EvtId i, double mass, double massPar, int nDaug, 
                            double *massDau) 
  { return _partlist[i.getId()].getMassProb(mass,massPar,nDaug,massDau);}

  static double getMaxMass(EvtId i ){return _partlist[i.getId()].getMassMax();}
  static double getMinMass(EvtId i ){return _partlist[i.getId()].getMassMin();}
  //the number we got from PDT
  static double getMaxRange(EvtId i )
  {return _partlist[i.getId()].getMaxRange();}
  static double getWidth(EvtId i ){return _partlist[i.getId()].getWidth();}
  static double getctau(EvtId i ){return _partlist[i.getId()].getctau();}
  static int getStdHep(EvtId id ){return _partlist[id.getId()].getStdHep();}
  static int getLundKC(EvtId id ){return _partlist[id.getId()].getLundKC();}
  static EvtId evtIdFromStdHep(int stdhep );
  static EvtId chargeConj(EvtId id );
  static int chg3(EvtId i ){return _partlist[i.getId()].getChg3();}
  static EvtSpinType::spintype getSpinType(EvtId i )
              {return _partlist[i.getId()].getSpinType();}
  static EvtId getId(const std::string& name );
  static std::string name(EvtId i){return _partlist[i.getAlias()].getName();}
  static void alias(EvtId num,const std::string& newname);
  static void aliasChgConj(EvtId a,EvtId abar);
  static int entries() { return _partlist.size();}
  static void reSetMass(EvtId i, double mass) 
  { _partlist[i.getId()].reSetMass(mass);}
  static void reSetWidth(EvtId i, double width) 
  { _partlist[i.getId()].reSetWidth(width);}
  static void reSetMassMin(EvtId i, double mass) 
  { _partlist[i.getId()].reSetMassMin(mass);}
  static void reSetMassMax(EvtId i,double mass) 
  { _partlist[i.getId()].reSetMassMax(mass);}
  static void reSetBlatt(EvtId i,double blatt) 
  {_partlist[i.getId()].reSetBlatt(blatt);}
  static void includeBirthFactor(EvtId i,bool yesno) 
  {_partlist[i.getId()].includeBirthFactor(yesno);}
  static void includeDecayFactor(EvtId i,bool yesno) 
  {_partlist[i.getId()].includeDecayFactor(yesno);}
  static void changeLS(EvtId i, std::string &newLS ) 
  { _partlist[i.getId()].newLineShape(newLS);}
  static void setPWForDecay(EvtId i, int spin, EvtId d1, EvtId d2) 
  {  _partlist[i.getId()].setPWForDecay(spin,d1,d2);}
private:

  void setUpConstsPdt();

  static unsigned int    _firstAlias;
  static int    _nentries;

  static std::vector<EvtPartProp> _partlist;
  static std::map<std::string, int> _particleNameLookup;
  
}; // EvtPDL.h

#endif


