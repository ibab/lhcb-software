#ifndef EvtBcVHad_HH
#define EvtBcVHad_HH

#include <iostream>
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtScalarParticle.hh"

#include "EvtGenBase/EvtPatches.hh"
#include <iomanip>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenModels/EvtTauHadnu.hh"
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor4C.hh"
#include "EvtGenBase/EvtIdSet.hh"
#include "EvtGenBase/EvtParser.hh"

#include "EvtGenModels/EvtWHad.hh"
#include "EvtGenModels/EvtBCVFF2.hh"


using std::endl;
using std::fstream;
using std::ifstream;
using std::cout;
using std::string;

class EvtBcVHad:public  EvtDecayAmp  {
public:
  EvtBcVHad() : nCall( 0 ) , whichfit( 0 ) , idVector( 0 ) , out_code( 0 ) , 
                ffmodel( 0 ) , wcurr( 0 ) {  };
    virtual ~EvtBcVHad();
    std::string getName();
    EvtDecayBase* clone();
    void initProbMax();
    void init();
    void decay(EvtParticle *p); 
protected:
  int nCall;

  
// whichfit --- code of the Bc -> VW formfactor set:
//   1 - SR
//   2 - PM
  int whichfit;
  
// idVector --- final vector particle code
  int idVector;

// out_code: code of the hadronic final state
//   1 - pi+
//   2 - pi+ pi0
//   3 -  pi+ pi+ pi-
//   4 - 4pi
//   5 - pi+ pi+ pi- pi- pi+
//   6 - K+ K- pi+
//   7 - K+ pi+ pi-
  int out_code; 
  
  EvtBCVFF2 *ffmodel;
  EvtWHad *wcurr;
  EvtVector4C _hardCurr(EvtParticle *root_particle);


};
#endif

