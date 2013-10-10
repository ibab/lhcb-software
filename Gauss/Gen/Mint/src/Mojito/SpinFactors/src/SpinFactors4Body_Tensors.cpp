// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// created: 9 Oct 2013
#include "Mint/SpinFactors.h"
#include "Mint/SpinFactors4Body_Tensors.h"
#include "Mint/DecayTree.h"
#include "Mint/Utils.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include "Mint/ZTspin1.h"
#include "Mint/ZTspin2.h"
#include "Mint/LeviCivita.h"
#include "Mint/SpinSumT.h"


using namespace std;
using namespace MINT;

DecayTree* SF_DtoVT_VtoP0P1_TtoP2P3_P::_exampleDecay=0;
DecayTree* SF_DtoVT_VtoP0P1_TtoP2P3_D::_exampleDecayD=0;

DecayTree* SF_DtoT1T2_T1toP0P1_T2toP2P3_S::_exampleDecay=0;
DecayTree* SF_DtoT1T2_T1toP0P1_T2toP2P3_P::_exampleDecay=0;
DecayTree* SF_DtoT1T2_T1toP0P1_T2toP2P3_D::_exampleDecay=0;

DecayTree* SF_DtoTP0_TtoVP1_VtoP2P3::_exampleDecay=0;
DecayTree* SF_DtoT1P0_T1toT2P1_T2toP2P3::_exampleDecay=0;

DecayTree* SF_DtoTS_TtoP0P1_StoP2P3::_exampleDecay=0;

DecayTree* SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3::_exampleDecay=0;
DecayTree* SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3::_exampleDecay=0;
DecayTree* SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3::_exampleDecay=0;


DecayTree* SF_DtoAP0_AtoTP1_TtoP2P3::_exampleDecay=0;

//============================================================

// ----------- D->VT, various L states --------
const DecayTree& SF_DtoVT_VtoP0P1_TtoP2P3_P::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr( 225)->addDgtr( 221, -211);
    _exampleDecay->addDgtr( 333)->addDgtr(-321,  321);
  }
  return *_exampleDecay;
}
const DecayTree& SF_DtoVT_VtoP0P1_TtoP2P3_P::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_DtoVT_VtoP0P1_TtoP2P3_D::getExampleDecay(){
  if(0==_exampleDecayD){
    _exampleDecayD = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecayD->addDgtr( 225)->addDgtr( 221, -211);
    _exampleDecayD->addDgtr( 333)->addDgtr(-321,  321);

    _exampleDecayD->getVal().setL(2);
  }
  return *_exampleDecayD;
}
const DecayTree& SF_DtoVT_VtoP0P1_TtoP2P3_D::exampleDecay(){
  return getExampleDecay();
}

// ------- D-> TT, varous L states ----------
const DecayTree& SF_DtoT1T2_T1toP0P1_T2toP2P3_S::getExampleDecay(){
  // D->f2 f2 (with L=0)
  _exampleDecay = new DecayTree(421);
  _exampleDecay->addDgtr(225)->addDgtr(211, -211);
  _exampleDecay->addDgtr(225)->addDgtr(211, -211);
  return *_exampleDecay;
}
const DecayTree& SF_DtoT1T2_T1toP0P1_T2toP2P3_S::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_DtoT1T2_T1toP0P1_T2toP2P3_P::getExampleDecay(){
  // D->f2 f2 (with L=1)
  _exampleDecay = new DecayTree(421);
  _exampleDecay->addDgtr(225)->addDgtr(211, -211);
  _exampleDecay->addDgtr(225)->addDgtr(211, -211);
  _exampleDecay->getVal().setL(1);
  return *_exampleDecay;
}
const DecayTree& SF_DtoT1T2_T1toP0P1_T2toP2P3_P::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_DtoT1T2_T1toP0P1_T2toP2P3_D::getExampleDecay(){
  // D->f2 f2 (with L=2)
  _exampleDecay = new DecayTree(421);
  _exampleDecay->addDgtr(225)->addDgtr(211, -211);
  _exampleDecay->addDgtr(225)->addDgtr(211, -211);
  _exampleDecay->getVal().setL(2);
  return *_exampleDecay;
}
const DecayTree& SF_DtoT1T2_T1toP0P1_T2toP2P3_D::exampleDecay(){
  return getExampleDecay();
}

// ------- D->TP0, various subesquent decays of the T ----

const DecayTree& SF_DtoTP0_TtoVP1_VtoP2P3::getExampleDecay(){
  _exampleDecay = new DecayTree(421);
  _exampleDecay->addDgtr(-211, 215)->addDgtr(211, 113)->addDgtr(-211, 211);
  return *_exampleDecay;
}
const DecayTree& SF_DtoT1P0_T1toT2P1_T2toP2P3::exampleDecay(){
  return getExampleDecay();
}
const DecayTree& SF_DtoT1P0_T1toT2P1_T2toP2P3::getExampleDecay(){
  _exampleDecay = new DecayTree(421);
  _exampleDecay->addDgtr(-211, 100215)->addDgtr(211, 225)->addDgtr(-211, 211);
  return *_exampleDecay;
}
const DecayTree& SF_DtoTP0_TtoVP1_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}


// ----- D->TS ----------
const DecayTree& SF_DtoTS_TtoP0P1_StoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr( 225)->addDgtr( 221, -211);
    _exampleDecay->addDgtr( 9010221)->addDgtr(-321,  321);
  }
  return *_exampleDecay;
}
const DecayTree& SF_DtoTS_TtoP0P1_StoP2P3::exampleDecay(){
  return getExampleDecay();
}

// ----- D->PseudoTensor P0, various modes
const DecayTree& SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3::getExampleDecay(){
  _exampleDecay = new DecayTree(421);
  _exampleDecay->addDgtr(-211, 10215)->addDgtr(211, 225)->addDgtr(211, -211);
  return *_exampleDecay;
}
const DecayTree& SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3::exampleDecay(){
  return getExampleDecay();
}
const DecayTree& SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3::getExampleDecay(){
  _exampleDecay = new DecayTree(421);
  _exampleDecay->addDgtr(-211, 10215)->addDgtr(211, 9000221)->addDgtr(211, -211);
  return *_exampleDecay;
}
const DecayTree& SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3::exampleDecay(){
  return getExampleDecay();
}
const DecayTree& SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3::getExampleDecay(){
  // (angular momenum of pi2->rho must be 1)
  _exampleDecay = new DecayTree(421);
  _exampleDecay->addDgtr(-211, 10215)->addDgtr(211, 113)->addDgtr(211, -211);
  return *_exampleDecay;
}
const DecayTree& SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}

// ----- D->AP, A->TP, ---------
const DecayTree& SF_DtoAP0_AtoTP1_TtoP2P3::getExampleDecay(){
  _exampleDecay = new DecayTree(421);
  // example: D->a1(1260) pi, a1 -> f2(1270) pi 
  // (kinematically challenged, but in PDG - broad particles...)
  _exampleDecay->addDgtr(-211, 20213)->addDgtr(211, 225)->addDgtr(211, -211);
  return *_exampleDecay;
}
const DecayTree& SF_DtoAP0_AtoTP1_TtoP2P3::exampleDecay(){
  return getExampleDecay();
}

//=============================================================

bool SF_DtoVT_VtoP0P1_TtoP2P3_BASE::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoVT_VtoP0P1_TtoP2P3_S::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }

  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == "V" && ! dgtr->isFinalState()) V = dgtr;
    else if(dgtr->getVal().SVPAT() == "T" && ! dgtr->isFinalState()) T = dgtr;
  }

  if(0==V || 0==T){
    cout << "ERROR in SF_DtoVT_VtoP0P1_TtoP2P3_BASE::parseTree"
	 << " Didn't find V or T " << V.get() << ", " << T.get() << endl;
    return false;
  }
  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoVT_VtoP0P1_TtoP2P3_BASE::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = V->getDgtrTreePtr(0);
  fsPS[1] = V->getDgtrTreePtr(1);
  normalOrder(fsPS[0], fsPS[1]);

  if(T->nDgtr() != 2){
    cout << "ERROR in SF_DtoVT_VtoP0P1_TtoP2P3_BASE::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << T->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = T->getDgtrTreePtr(0);
  fsPS[3] = T->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);

  // this->printYourself();
  return true;
}

double SF_DtoVT_VtoP0P1_TtoP2P3_P::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pV = p(0) + p(1);
  TLorentzVector qV = p(0) - p(1);
  TLorentzVector pT = p(2) + p(3);
  TLorentzVector qT = p(2) - p(3);
  
  double MV = mRes(V);
  double MT = mRes(T);
  
  ZTspin1 tV(qV, pV, MV);
  ZTspin2 tT(qT, pT, MT);
  
  const double units = GeV*GeV*GeV*GeV;

  double returnVal = (tT.Contract(tV)).Dot(pV) / units;

  if(dbThis){
    cout << " SF_DtoVT_VtoP0P1_TtoP2P3_P::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
  
}
void SF_DtoVT_VtoP0P1_TtoP2P3_P::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoVT_VtoP0P1_TtoP2P3_P"
     << "\n\t T1(V)_{\\mu}  T2(T)^{\\mu\\nu} pV_{\\nu}"
     << "\n\t implemented as: (tT.Contract(tV)).Dot(pV) / GeV^4"
     << "\n\t with T1(V) = ZTspin1 tV(qV, pV, MV), T2(T)= ZTspin2 tT(qT, pT, MT);"
     << "\n\t and pV = p(0) + p(1), qV = p(0) - p(1), pT = p(2) + p(3), qT = p(2) - p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// -------------------------------------

double SF_DtoVT_VtoP0P1_TtoP2P3_D::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pV = p(0) + p(1);
  TLorentzVector qV = p(0) - p(1);
  TLorentzVector pT = p(2) + p(3);
  TLorentzVector qT = p(2) - p(3);

  TLorentzVector pD = pT + pV;
  TLorentzVector qD = pT - qV;
  
  double MV = mRes(V);
  double MT = mRes(T);
  
  ZTspin1 tV(qV, pV, MV);
  ZTspin2 tT(qT, pT, MT);
  TLorentzVector vecT(tT.Contract(pV));

  const double units = GeV*GeV * GeV*GeV * GeV*GeV;

  double returnVal = LeviCivita(tV, vecT, qD, pD) /units;


  if(dbThis){
    cout << " SF_DtoVT_VtoP0P1_TtoP2P3_D::getVal "
	 << " returning " << returnVal
	 << endl;
    double checkVal = LeviCivita(p(0), p(1), p(2), p(3))*MV*MT/units;
    cout << "cross check: " << checkVal
	 << " ratio " << checkVal/returnVal << endl;
  }
  return returnVal;
  
}
void SF_DtoVT_VtoP0P1_TtoP2P3_D::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoVT_VtoP0P1_TtoP2P3_D"
     << "\n\t ZTspin1 tV(qV, pV, MV);"
     << "\n\t  ZTspin2 tT(qT, pT, MT);"
     << "\n\t   TLorentzVector vecT(tT.Contract(pV));"
     << "\n\t  const double units = GeV*GeV * GeV*GeV * GeV*GeV;"
     << "\n\t  double returnVal = LeviCivita(tV, vecT, qD, pD) /units;"
     << "\n\t with pV = p(0) + p(1), qV = p(0) - p(1), pT = p(2) + p(3), qT = p(2) - p(3)"
     << "\n\t and  pD = pT + pV, qD = pT - qV;"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//------------------------------------------
// D->TT, various L states
//-------------------------------------------
// -----------------------------------------------
bool SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }

  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == "T" && ! dgtr->isFinalState()) T[i] = dgtr;
    else{
      cout << "ERROR in SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE::parseTree"
	   << " expected exactly 2 Tensors, have "
	   << theDecay().nDgtr();
      cout << "\n so daughter " << i << " is not a tensor." << endl;
      return false;
    }
  }

  if(0==T[0] || 0==T[1]){
    cout << "ERROR in SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE::parseTree"
	 << " Didn't find T or S " << T[0].get() << ", " << T[1].get() << endl;
    return false;
  }
  if(T[0]->nDgtr() != 2){
    cout << "ERROR in SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE::parseTree"
	 << " T[0] should have 2 daughters, but it says it has "
	 << T[0]->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = T[0]->getDgtrTreePtr(0);
  fsPS[1] = T[0]->getDgtrTreePtr(1);
  normalOrder(fsPS[0], fsPS[1]);

  if(T[1]->nDgtr() != 2){
    cout << "ERROR in SF_DtoT1T2_T1toP0P1_T2toP2P3_S::parseTree"
	 << " T[1] should have 2 daughters, but it says it has "
	 << T[1]->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = T[1]->getDgtrTreePtr(0);
  fsPS[3] = T[1]->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);

  // this->printYourself();
  return true;
}
// ------- same decay, s-wave -----
double SF_DtoT1T2_T1toP0P1_T2toP2P3_S::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pT1 = p(0) + p(1);
  TLorentzVector qT1 = p(0) - p(1);
  TLorentzVector pT2 = p(2) + p(3);
  TLorentzVector qT2 = p(2) - p(3);
  
  double MT1 = mRes(T[0]);
  double MT2 = mRes(T[1]);
  
  ZTspin2 tT1(qT1, pT1, MT1);
  ZTspin2 tT2(qT2, pT2, MT2);
  
  const double units = GeV*GeV*GeV*GeV * GeV*GeV*GeV*GeV;

  // Contract_1 turns GeV^2 * GeV^2 to GeV^4
  // Contract_2 squares that!
  double returnVal = tT1.Contract_2(tT2) / units;

  if(dbThis){
    cout << " SF_DtoT1T2_T1toP0P1_T2toP2P3_S::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
  
}

void SF_DtoT1T2_T1toP0P1_T2toP2P3_S::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoT1T2_T1toP0P1_T2toP2P3_S"
     << "\n\t  ZTspin2 tT1(qT1, pT1, MT1);"
     << "\n\t  ZTspin2 tT2(qT2, pT2, MT2);"
     << "\n\t  return: tT1.Contract_2(tT2) / GeV^8"
     << "\n\t with pT1 = p(0) + p(1), qT = p(0) - p(1), pT2 = p(2) + p(3), qT2 = p(2)-p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// ------- same decay, P-wave -----
double SF_DtoT1T2_T1toP0P1_T2toP2P3_P::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pT1 = p(0) + p(1);
  TLorentzVector qT1 = p(0) - p(1);
  TLorentzVector pT2 = p(2) + p(3);
  TLorentzVector qT2 = p(2) - p(3);
  TLorentzVector pD  = pT1 + pT2;
  TLorentzVector qD  = pT1 + pT2;
  

  double MT1 = mRes(T[0]);
  double MT2 = mRes(T[1]);
  
  ZTspin2 tT1(qT1, pT1, MT1);
  ZTspin2 tT2(qT2, pT2, MT2);
  
  const double units = GeV*GeV * GeV*GeV * GeV*GeV ;

  double returnVal = LeviCivita(pD, qD, tT1.Contract_1(tT2)) / units;

  if(dbThis){
    cout << " SF_DtoT1T2_T1toP0P1_T2toP2P3_P::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
  
}

void SF_DtoT1T2_T1toP0P1_T2toP2P3_P::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoT1T2_T1toP0P1_T2toP2P3_P"
     << "\n\t  ZTspin2 tT1(qT1, pT1, MT1);"
     << "\n\t  ZTspin2 tT2(qT2, pT2, MT2);"
     << "\n\t  pD = pT1 + pT2 "
     << "\n\t  qD = pT1 - pT2 "
     << "\n\t  return:  LeviCivita(pD, qD, tT1.Contract_1(tT2))/ GeV^4"
     << "\n\t  with pT1 = p(0) + p(1), qT = p(0) - p(1), pT2 = p(2) + p(3), qT2 = p(2)-p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// ------- same decay, D-wave -----
double SF_DtoT1T2_T1toP0P1_T2toP2P3_D::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pT1 = p(0) + p(1);
  TLorentzVector qT1 = p(0) - p(1);
  TLorentzVector pT2 = p(2) + p(3);
  TLorentzVector qT2 = p(2) - p(3);
  TLorentzVector qD  = pT1 - pT2;

  double MT1 = mRes(T[0]);
  double MT2 = mRes(T[1]);
  
  ZTspin2 tT1(qT1, pT1, MT1);
  ZTspin2 tT2(qT2, pT2, MT2);
  
  TLorentzVector X1(tT1.Contract(qD));
  TLorentzVector X2(tT2.Contract(qD));

  const double units = GeV*GeV*GeV * GeV*GeV*GeV;

  double returnVal = X1.Dot(X2) / units;

  if(dbThis){
    cout << " SF_DtoT1T2_T1toP0P1_T2toP2P3_D::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
  
}

void SF_DtoT1T2_T1toP0P1_T2toP2P3_D::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoT1T2_T1toP0P1_T2toP2P3_D"
     << "\n\t  ZTspin2 tT1(qT1, pT1, MT1);"
     << "\n\t  ZTspin2 tT2(qT2, pT2, MT2);"
     << "\n\t  qD = pT1 - pT2"
     << "\n\t  return: (tT1.Contract(qD))* (tT2.Contract(qD))/ GeV^4"
     << "\n\t  with pT1 = p(0) + p(1), qT = p(0) - p(1), pT2 = p(2) + p(3), qT2 = p(2)-p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

// ------------------------------------------
// D->TP0, various subesquent decays of the T
//------------------------------------------------
bool SF_DtoTP0_TtoVP1_VtoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoTP0_TtoVP1_VtoP2P3_BASE::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
    else if(dgtr->getVal().SVPAT() == "T" && ! dgtr->isFinalState()) T = dgtr;
  }
  if(0==T || 0==fsPS[0]){
    cout << "ERROR in SF_DtoTP0_TtoVP1_VtoP2P3_BASE::parseTree"
	 << " Didn't find T or P0 " << T.get() << ", " << fsPS[0].get() << endl;
    return false;
  }
  
  if(T->nDgtr() != 2){
    cout << "ERROR in SF_DtoTP0_TtoVP1_VtoP2P3_BASE::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << T->nDgtr() << "."
	 << endl;
    return false;
  }
  for(int i=0; i< T->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= T->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "V" && ! dgtr->isFinalState()) V = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==V || 0==fsPS[1]){
    cout << "ERROR in SF_DtoTP0_TtoVP1_VtoP2P3_BASE::parseTree"
	 << " Didn't find V2 or P1 " << V.get() << ", " << fsPS[1].get() << endl;
    return false;
  }
  
  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoTP0_TtoVP1_VtoP2P3_BASE::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V->getDgtrTreePtr(0);
  fsPS[3] = V->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);
  
  // this->printYourself();
  return true;
}
// ------- note that T->VP1 has to be in D-wave for parity and ang mom ----
double SF_DtoTP0_TtoVP1_VtoP2P3::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pV = p(2) + p(3);
  TLorentzVector qV = p(2) - p(3);
  TLorentzVector pT = pV + p(1);
  TLorentzVector qT = pV - p(1);
  TLorentzVector pD = pT + p(0);
  TLorentzVector qD = pT - p(0);

  double MT = mRes(T);
  double MV = mRes(V);
  
  ZTspin2 tT(qT, pT, MT);
  ZTspin1 tV(qV, pV, MV);
  
  TLorentzVector DT(tT.Contract(qD));

  const double units = GeV*GeV * GeV*GeV * GeV*GeV;

  double returnVal = LeviCivita(pD, qD, DT, tV) / units;

  if(dbThis){
    cout << " SF_DtoTP0_TtoVP1_VtoP2P3::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
}

void SF_DtoTP0_TtoVP1_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoTP0_TtoVP1_VtoP2P3"
     << "\n\t  ZTspin2 tT1(qT, pT, MT);"
     << "\n\t  ZTspin1 tT2(qV, pV, MV);"
     << "\n\t  pV = p(2) + p(3); qV = p(2) - p(3);"
     << "\n\t  pT = pV + p(1); qT = pV - p(1);"
     << "\n\t  pD = pT + p(0); qD = pT - p(0);"
     << "\n\t  return: DT(tT.Contract(qD))/GeV^6"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// -----------------------------------------------
bool SF_DtoT1P0_T1toT2P1_T2toP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
    else if(dgtr->getVal().SVPAT() == "T" && ! dgtr->isFinalState()) T1 = dgtr;
  }
  if(0==T1 || 0==fsPS[0]){
    cout << "ERROR in SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE::parseTree"
	 << " Didn't find T or P0 " << T1.get() << ", " << fsPS[0].get() << endl;
    return false;
  }
  
  if(T1->nDgtr() != 2){
    cout << "ERROR in SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << T1->nDgtr() << "."
	 << endl;
    return false;
  }
  for(int i=0; i< T1->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= T1->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "T" && ! dgtr->isFinalState()) T2 = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==T2 || 0==fsPS[1]){
    cout << "ERROR in SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE::parseTree"
	 << " Didn't find T2 or P1 " << T2.get() << ", " << fsPS[1].get() << endl;
    return false;
  }
  
  if(T2->nDgtr() != 2){
    cout << "ERROR in SF_DtoT1P0_T1toT2P1_T2toP2P3_BASE::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << T2->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = T2->getDgtrTreePtr(0);
  fsPS[3] = T2->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);
  
  // this->printYourself();
  return true;
}
// ------- note that T->TP1 has to be in P-wave (or F, etc) for parity and ang mom ----
double SF_DtoT1P0_T1toT2P1_T2toP2P3::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pT2 = p(2) + p(3);
  TLorentzVector qT2 = p(2) - p(3);
  TLorentzVector pT1 = pT2 + p(1);
  TLorentzVector qT1 = pT2 - p(1);
  TLorentzVector pD = pT1 + p(0);
  TLorentzVector qD = pT1 - p(0);

  double MT1 = mRes(T1);
  double MT2 = mRes(T2);
  
  ZTspin2 tT1(qD, pT1, MT1);
  ZTspin2 tT2(qT2, pT2, MT2);
  
  TLorentzVector DT(tT1.Contract(qT1));

  const double units = GeV*GeV * GeV*GeV * GeV*GeV;

  double returnVal = LeviCivita(pD, DT, tT2) / units;

  if(dbThis){
    cout << " SF_DtoT1P0_T1toT2P1_T2toP2P3::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
}

void SF_DtoT1P0_T1toT2P1_T2toP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoT1P0_T1toT2P1_T2toP2P3"
     << "\n\t  ZTspin2 tT1(qT, pT, MT);"
     << "\n\t  ZTspin1 tT2(qV, pV, MV);"
     << "\n\t    TLorentzVector DT(tT1.Contract(qT1));"
     << "\n\t  pV = p(2) + p(3); qV = p(2) - p(3);"
     << "\n\t  pT = pV + p(1); qT = pV - p(1);"
     << "\n\t  pD = pT + p(0); qD = pT - p(0);"
     << "\n\t  return: LeviCivita(pD, DT, tT2)/GeV^6"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

// -------------------
//     D->TS
// ------------------
bool SF_DtoTS_TtoP0P1_StoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoTS_TtoP0P1_StoP2P3::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }

  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == "T" && ! dgtr->isFinalState()) T = dgtr;
    else if(dgtr->getVal().SVPAT() == "S" && ! dgtr->isFinalState()) S = dgtr;
  }

  if(0==S || 0==T){
    cout << "ERROR in SF_DtoTS_TtoP0P1_StoP2P3::parseTree"
	 << " Didn't find T or S " << T.get() << ", " << S.get() << endl;
    return false;
  }
  if(T->nDgtr() != 2){
    cout << "ERROR in SF_DtoTS_TtoP0P1_StoP2P3::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << T->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = T->getDgtrTreePtr(0);
  fsPS[1] = T->getDgtrTreePtr(1);
  normalOrder(fsPS[0], fsPS[1]);

  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoTS_TtoP0P1_StoP2P3::parseTree"
	 << " S should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);

  // this->printYourself();
  return true;
}


// -----------------------------------------------

double SF_DtoTS_TtoP0P1_StoP2P3::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pT = p(0) + p(1);
  TLorentzVector qT = p(0) - p(1);
  TLorentzVector pS = p(2) + p(3);
  TLorentzVector qD = pT - pS;
  
  double MT = mRes(T);
  
  ZTspin2 tT(qT, pT, MT);
  
  const double units = GeV*GeV * GeV*GeV;

  double returnVal = (tT.Contract(qD)).Dot(qD) / units;

  if(dbThis){
    cout << " SF_DtoTS_TtoP0P1_StoP2P3::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
  
}

void SF_DtoTS_TtoP0P1_StoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoTS_TtoP0P1_StoP2P3"
     << "\n\t  ZTspin2 tT(qT, pT, MT);"
     << "\n\t  return: (tT.Contract(qD)).Dot(qD) / GeV^4"
     << "\n\t with pT = p(0) + p(1), qT = p(0) - p(1), pS = p(2) + p(3), qD = pT - qV;"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

// ---------------
//    D->pseudoTensor P0, various modes
// --------------

// -----------------------------------------------
bool SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3_BASE::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
    else if(dgtr->getVal().SVPAT() == "PT" && ! dgtr->isFinalState()) PT = dgtr;
  }
  if(0==PT || 0==fsPS[0]){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3_BASE::parseTree"
	 << " Didn't find T or P0 " << PT.get() << ", " << fsPS[0].get() << endl;
    return false;
  }
  
  if(PT->nDgtr() != 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3_BASE::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << PT->nDgtr() << "."
	 << endl;
    return false;
  }
  for(int i=0; i< PT->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= PT->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "V" && ! dgtr->isFinalState()) V = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==V || 0==fsPS[1]){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3_BASE::parseTree"
	 << " Didn't find V2 or P1 " << V.get() << ", " << fsPS[1].get() << endl;
    return false;
  }
  
  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3_BASE::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V->getDgtrTreePtr(0);
  fsPS[3] = V->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);
  
  // this->printYourself();
  return true;
}
// ------- note that pseudoT->VP1 has to be in P-wave for parity and ang mom ----
double SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pV = p(2) + p(3);
  TLorentzVector qV = p(2) - p(3);
  TLorentzVector pT = pV + p(1);
  TLorentzVector qT = pV - p(1);
  TLorentzVector pD = pT + p(0);
  TLorentzVector qD = pT - p(0);

  double MT = mRes(PT);
  double MV = mRes(V);
  
  ZTspin1 tV(qV, pV, MV);
  ZTspin2 tT(qD, pT, MT);
  //SpinSumT ProT(pT, MT);
  
  const double units = GeV*GeV * GeV*GeV;

  double returnVal = (tT.Contract(qT)).Dot(tV) / units;

  if(dbThis){
    cout << " SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3::getVal "
	 << " returning " << returnVal
	 << endl;
    SpinSumT ProT(pT, MT);
    cout << " compare to: "
	 << ProT.Sandwich(qD, qD, qT, tV) / units
	 << endl;
  }
  return returnVal;
}

void SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3"
     << "\n\t  ProT(pT, MT)"
     << "\n\t  ZTspin1 tT2(qV, pV, MV);"
     << "\n\t  pV = p(2) + p(3); qV = p(2) - p(3);"
     << "\n\t  pT = pV + p(1); qT = pV - p(1);"
     << "\n\t  pD = pT + p(0); qD = pT - p(0);"
     << "\n\t  return: ProT.Sandwich(qD, qD, qT, tV) / GeV^4"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// -----------------------------------------------
bool SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3_BASE::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
    else if(dgtr->getVal().SVPAT() == "PT" && ! dgtr->isFinalState()) PT = dgtr;
  }
  if(0==PT || 0==fsPS[0]){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3_BASE::parseTree"
	 << " Didn't find T or P0 " << PT.get() << ", " << fsPS[0].get() << endl;
    return false;
  }
  
  if(PT->nDgtr() != 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3_BASE::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << PT->nDgtr() << "."
	 << endl;
    return false;
  }
  for(int i=0; i< PT->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= PT->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "S" && ! dgtr->isFinalState()) S = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==S || 0==fsPS[1]){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3_BASE::parseTree"
	 << " Didn't find S or P1 " << S.get() << ", " << fsPS[1].get() << endl;
    return false;
  }
  
  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3_BASE::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);
  
  // this->printYourself();
  return true;
}
// ------- note that pseudoT->SP1 has to be in D-wave for parity and ang mom ----
double SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pS = p(2) + p(3);
  TLorentzVector qS = p(2) - p(3);
  TLorentzVector pT = pS + p(1);
  TLorentzVector qT = pS - p(1);
  TLorentzVector pD = pT + p(0);
  TLorentzVector qD = pT - p(0);

  double MT = mRes(PT);
  //double MV = mRes(S);
  
  ZTspin2 tT(qT, pT, MT);
  
  const double units = GeV*GeV * GeV*GeV;

  double returnVal =  (tT.Contract(qD)).Dot(qD) / units;

  if(dbThis){
    cout << " SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
}

void SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3"
     << "\n\t  ProT(pT, MT)"
     << "\n\t  ZTspin1 tT2(qV, pV, MV);"
     << "\n\t  pS = p(2) + p(3); qS = p(2) - p(3);"
     << "\n\t  pT = pV + p(1); qT = pV - p(1);"
     << "\n\t  pD = pT + p(0); qD = pT - p(0);"
     << "\n\t  (tT.Contract(qD)).Dot(qD) / GeV^4"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

// -------------------------------------------
bool SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3_BASE::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
    else if(dgtr->getVal().SVPAT() == "PT" && ! dgtr->isFinalState()) PT = dgtr;
  }
  if(0==PT || 0==fsPS[0]){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3_BASE::parseTree"
	 << " Didn't find T or P0 " << PT.get() << ", " << fsPS[0].get() << endl;
    return false;
  }
  
  if(PT->nDgtr() != 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3_BASE::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << PT->nDgtr() << "."
	 << endl;
    return false;
  }
  for(int i=0; i< PT->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= PT->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "T" && ! dgtr->isFinalState()) T = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==T || 0==fsPS[1]){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3_BASE::parseTree"
	 << " Didn't find T or P1 " << T.get() << ", " << fsPS[1].get() << endl;
    return false;
  }
  
  if(T->nDgtr() != 2){
    cout << "ERROR in SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3_BASE::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << T->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = T->getDgtrTreePtr(0);
  fsPS[3] = T->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);
  
  // this->printYourself();
  return true;
}
// ------- note that pseudoT->SP1 has to be in D-wave for parity and ang mom ----
double SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

  double invGeV = 1;// 1.0/GeV;
  
  TLorentzVector pT = p(2) + p(3);
  TLorentzVector qT = p(2) - p(3);
  TLorentzVector pPT = pT + p(1);
  TLorentzVector qPT = pT - p(1);
  TLorentzVector pD = pPT + p(0);
  TLorentzVector qD = pPT - p(0);

  double MPT = mRes(PT);
  double MT = mRes(T);
  //double MV = mRes(S);
  
  ZTspin2 tPT(qD*invGeV, pPT*invGeV, MPT*invGeV);
  ZTspin2 tT(qT*invGeV, pT*invGeV, MT*invGeV);

  // Contract_1 turns GeV^2 * GeV^2 to GeV^4
  // Contract_2 squares that!
  const double units = GeV*GeV*GeV*GeV * GeV*GeV*GeV*GeV;

  double returnVal =  tPT.Contract_2(tT) / units;

  if(dbThis){
    cout << " SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3::getVal "
	 << " returning " << returnVal
	 << " = " << tPT.Contract_2(tT) << " / " << units
	 << "\n MPT = " << MPT << ", MT = " << MT
	 << endl;
  }
  return returnVal;
}

void SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3"
     << "\n\t  ZTspin2 tPT(qD, pPT, MPT);"
     << "\n\t  ZTspin2 tT(qT, pT, MT);"
     << "\n\t  pT = p(2) + p(3); qT = p(2) - p(3);"
     << "\n\t  pPT = pT + p(1); qPT = pT - p(1);"
     << "\n\t  pD = pPT + p(0); qD = pPT - p(0);"
     << "\n\t  tPT.Contract_2(tT)/ GeV^8"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

// ==========================================
// -----------------------------------------------
// D->PA, A->TP (ang mom L=1 at each vtx)
// -----------------------------------------------

bool SF_DtoAP0_AtoTP1_TtoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoAP0_AtoTP1_TtoP2P3_BASE::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
    else if(dgtr->getVal().SVPAT() == "A" && ! dgtr->isFinalState()) A = dgtr;
  }
  if(0==A || 0==fsPS[0]){
    cout << "ERROR in SF_DtoAP0_AtoTP1_TtoP2P3_BASE::parseTree"
	 << " Didn't find T or P0 " << A.get() << ", " << fsPS[0].get() << endl;
    return false;
  }
  
  if(A->nDgtr() != 2){
    cout << "ERROR in SF_DtoAP0_AtoTP1_TtoP2P3_BASE::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << A->nDgtr() << "."
	 << endl;
    return false;
  }
  for(int i=0; i< A->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "T" && ! dgtr->isFinalState()) T = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==T || 0==fsPS[1]){
    cout << "ERROR in SF_DtoAP0_AtoTP1_TtoP2P3_BASE::parseTree"
	 << " Didn't find T or P1 " << T.get() << ", " << fsPS[1].get() << endl;
    return false;
  }
  
  if(T->nDgtr() != 2){
    cout << "ERROR in SF_DtoAP0_AtoTP1_TtoP2P3_BASE::parseTree"
	 << " A should have 2 daughters, but it says it has "
	 << T->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = T->getDgtrTreePtr(0);
  fsPS[3] = T->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);
  
  // this->printYourself();
  return true;
}
// ---
double SF_DtoAP0_AtoTP1_TtoP2P3::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

  TLorentzVector pT = p(2) + p(3);
  TLorentzVector qT = p(2) - p(3);
  TLorentzVector pA = pT + p(1);
  TLorentzVector qA = pT - p(1);
  //TLorentzVector pD = pPT + p(0);
  TLorentzVector qD = pA - p(0);

  double MA = mRes(A);
  double MT = mRes(T);
  //double MV = mRes(S);
  
  ZTspin1 tA(qA, pA, MA);
  ZTspin2 tT(qT, pT, MT);

  const double units = GeV*GeV*GeV*GeV;

  double returnVal =  (tT.Contract(tA)).Dot(qD) / units;

  if(dbThis){
    cout << " SF_DtoAP0_AtoTP1_TtoP2P3::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
}

void SF_DtoAP0_AtoTP1_TtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoAP0_AtoTP1_TtoP2P3"
     << "\n\t  ZTspin1 tA(qA, pA, MA);"
     << "\n\t  ZTspin2 tT(qT, pT, MT);"
     << "\n\t  pT = p(2) + p(3); qT = p(2) - p(3);"
     << "\n\t  pA = pT + p(1); qA = pT - p(1);"
     << "\n\t  pD = pA + p(0); qD = pA - p(0);"
     << "\n\t  (tT.Contract(tA)).Dot(qD) / GeV^4"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//====================================

//====================================
