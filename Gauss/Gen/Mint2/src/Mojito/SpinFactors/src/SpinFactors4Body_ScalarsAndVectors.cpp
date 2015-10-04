// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT
#include "Mint/SpinFactors.h"
#include "Mint/SpinFactors4Body_ScalarsAndVectors.h"
#include "Mint/DecayTree.h"
//#include "Mint/DecayTreeComparisons.h"

#include "Mint/Utils.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include "Mint/ZTspin1.h"
#include "Mint/ZTspin2.h"
#include "Mint/LeviCivita.h"
#include "Mint/SpinSumT.h"

using namespace std;
using namespace MINT;

// Most of these come from:
// D. Coffman et al: "Resonant substructure in Kbar pi pi pi decays of D mesons"
// Physical Review D, Vol 45, Number 7, page 2196
// The spin factors are in Table II, page 2201.

DecayTree* SF_DtoPP0_PtoVP1_VtoP2P3::_exampleDecay=0;
DecayTree* SF_DtoAP0_AtoSP1_StoP2P3::_exampleDecay=0;
DecayTree* SF_DtoV1V2_V1toP0P1_V1toP2P3_S::_exampleDecayS=0;
DecayTree* SF_DtoV1V2_V1toP0P1_V1toP2P3_P::_exampleDecayP=0;// ?
DecayTree* SF_DtoV1V2_V1toP0P1_V1toP2P3_D::_exampleDecayD=0;
DecayTree* SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::_exampleDecay=0;
DecayTree* SF_DtoVS_VtoP0P1_StoP2P3::_exampleDecay=0;
DecayTree* SF_DtoVS_VtoP0P1_StoP2P3_nonResV::_exampleDecay=0;
DecayTree* SF_DtoPP1P2_PtoP3P4::_exampleDecay=0; // trivial, = 1
DecayTree* SF_DtoS1S2_S1toP1P2_S2toP3P4::_exampleDecay=0; // trival, = 1

DecayTree* SF_DtoPP0_PtoSP1_StoP3P4::_exampleDecay=0; // trival, = 1

DecayTree* SF_DtoV1P0_V1toV2P1_V2toP2P3::_exampleDecay=0;

DecayTree* SF_DtoAP0_AtoVP1_VtoP2P3::_exampleDecay=0;
DecayTree* SF_DtoAP0_AtoVP1Dwave_VtoP2P3::_exampleDecayD=0;


// ============================================================

const DecayTree& SF_DtoAP0_AtoVP1Dwave_VtoP2P3::getExampleDecay(){
  // D->a(1) pi, a(1)->rho pi, rho->pipi
  // with a(1) decaying in D-wave
  if(0 == _exampleDecayD){
    _exampleDecayD = new DecayTree(421);
    DecayTree a1(20213);
    a1.getVal().setL(2);
    a1.addDgtr(211, 113)->addDgtr(211, -211);
    _exampleDecayD->addDgtr(-211);
    _exampleDecayD->addDgtr(&a1);
  }
  return *_exampleDecayD;
}
const DecayTree& SF_DtoAP0_AtoVP1Dwave_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_DtoPP1P2_PtoP3P4::getExampleDecay(){//Lauren's wide KsSpin
  if(0 == _exampleDecay){
    _exampleDecay = new DecayTree(421);
    _exampleDecay->addDgtr(888310)->addDgtr(211, -211);
    _exampleDecay->addDgtr(321, -321);
  }
  return *_exampleDecay;
}
const DecayTree& SF_DtoPP1P2_PtoP3P4::exampleDecay(){//Laurens KsSpin
  return getExampleDecay();
}

const DecayTree& SF_DtoS1S2_S1toP1P2_S2toP3P4::getExampleDecay(){
  if(0 == _exampleDecay){
    _exampleDecay = new DecayTree(421);
    _exampleDecay->addDgtr(9010221)->addDgtr(211, -211);
    _exampleDecay->addDgtr(9010221)->addDgtr(211, -211);
  }
  return *_exampleDecay;
}
const DecayTree& SF_DtoS1S2_S1toP1P2_S2toP3P4::exampleDecay(){//Laurens KsSpin
  return getExampleDecay();
}

const DecayTree& SF_DtoPP0_PtoSP1_StoP3P4::getExampleDecay(){
  if(0 == _exampleDecay){
    _exampleDecay = new DecayTree(421);
    _exampleDecay->addDgtr(-211, 100211)->addDgtr(211, 9000221)->addDgtr(-211,211);
  }
  return *_exampleDecay;
}
const DecayTree& SF_DtoPP0_PtoSP1_StoP3P4::exampleDecay(){//Laurens KsSpin
  return getExampleDecay();
}

// the following is not implemented, beccause Axial vector -> PP violates
// parity conservation of the strong interaction.
// The example decay was, however, until recently associated
// to SF_DtoPP0_PtoVP1_VtoP2P3
//const DecayTree& SF_DtoPP0_PtoAP1_AtoP2P3::getExampleDecay(){
//  if(0==_exampleDecay){
//   _exampleDecay = new DecayTree(421);
//    // remark: addDgtr always returns a pointer to the 
//    // last daughter that was added, thus allowing these
//    // chains:
//    _exampleDecay->addDgtr(-211, 200321)->addDgtr(321, -10323)->addDgtr(-321,211);
//  }
//  return *_exampleDecay;
//}
//

const DecayTree& SF_DtoPP0_PtoVP1_VtoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(-211, 100211)->addDgtr(211, 113)->addDgtr(-211,211);
  }
  return *_exampleDecay;
}

const DecayTree& SF_DtoPP0_PtoVP1_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoPP0_PtoVP1_VtoP2P3::parseTree(const DalitzEventPattern& pat){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay(pat).nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "P" && ! dgtr->isFinalState()) P = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
  }
  if(0==P || 0==fsPS[0]){
    cout << "ERROR in SF_DtoPP0_PtoVP1_VtoP2P3::parseTree"
	 << " Didn't find P or P1 " << P.get() << ", " << fsPS[0].get() << endl;
    return false;
  }

  for(int i=0; i< P->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= P->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "V" && ! dgtr->isFinalState()) V = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==V || 0==fsPS[1]){
    cout << "ERROR in SF_DtoPP0_PtoVP1_VtoP2P3::parseTree"
	 << " Didn't find V or P2 " << V.get() << ", " << fsPS[1].get() << endl;
    return false;
  }

  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoAP0_AtoVP1_VtoP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V->getDgtrTreePtr(0);
  fsPS[3] = V->getDgtrTreePtr(1);
  // normalOrder(fsPS[2], fsPS[3]);

  // this->printYourself();

  return true;
}

double SF_DtoPP0_PtoVP1_VtoP2P3::getVal(IDalitzEvent& evt){
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
  TLorentzVector pV = p(2, evt) + p(3, evt);
  TLorentzVector qV = p(2, evt) - p(3, evt);

  double MassV = mRes(V, evt);
  
    if (_useZemachTensors) {
        double MP = mRes(P, evt);
        TLorentzVector pP= pV + p(1, evt);
        TLorentzVector qP= pV - p(1, evt);

        ZTspin1 LP(qP,pP,MP);
        ZTspin1 LV(qV,pV,MassV);
        return (LP.Dot(LV))/(GeV*GeV);
    }   
    
  return (p(1, evt).Dot(qV)
	  -    p(1, evt).Dot(pV) * pV.Dot(qV) / (MassV*MassV))
    /(GeV*GeV)
    ;
}
void SF_DtoPP0_PtoVP1_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "---------------------------------------------------------"
     << "\n spin factor SF_DtoPP0_PtoVP1_VtoP2P3 "
     << "\n      (p(1).Dot(qV) -p(1).Dot(pV) * pV.Dot(qV) / (MassV*MassV))  /  (GeV*GeV)"
     << "\n      with pV = p(2) + p(3); qV = p(2) - p(3)"
     << "\n      parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//==================================================================

const DecayTree& SF_DtoAP0_AtoVP1_VtoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(-321, 10323)->addDgtr(321, 113)->addDgtr(211,-211);
  }
  return *_exampleDecay;
}

const DecayTree& SF_DtoAP0_AtoVP1_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}

//==================================================================
//==================================================================

bool SF_DtoAP0_AtoVP1_VtoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
  bool debugThis=false;

  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay(pat).nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr = 
      theDecay(pat).getDgtrTreePtr(i);

    if(dgtr->getVal().SVPAT() == "A") A = dgtr;
    else if(dgtr->getVal().SVPAT() == "P") fsPS[0] = dgtr;
  }
  if(0==A || 0==fsPS[0]){
    cout << "ERROR in SF_DtoAP0_AtoVP1_VtoP2P3::parseTree"
	 << " Didn't find A or P1 " << A.get() << ", " << fsPS[0].get() << endl;
    return false;
  }

  for(int i=0; i< A->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == "V") V = dgtr;
    else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
  }
  if(0==V || 0==fsPS[1]){
    cout << "ERROR in SF_DtoAP0_AtoVP1_VtoP2P3::parseTree"
	 << " Didn't find V or P2 " << V.get() << ", " << fsPS[1].get() << endl;
    return false;
  }

  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoAP0_AtoVP1_VtoP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V->getDgtrTreePtr(0);
  fsPS[3] = V->getDgtrTreePtr(1);

  // normalOrder(fsPS[2], fsPS[3]);

  if(debugThis){
    cout << "parsed Tree: V:\n"
	 << *V
	 << "\n A:"
	 << *A 
	 << endl;
    for(int i=0; i<4; i++){
      cout << "fsPS[" << i << "]\n"
	   << *(fsPS[i])
	   << endl;
    }
  }
  // this->printYourself();
  return true;
}

double SF_DtoAP0_AtoVP1_VtoP2P3::getVal(IDalitzEvent& evt){
  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());

   TLorentzVector pV = p(2, evt) + p(3, evt);  
   TLorentzVector qV = p(2, evt) - p(3, evt);
   TLorentzVector pA = p(1, evt) + p(2, evt) + p(3, evt);
   TLorentzVector p0 = p(0, evt);
   TLorentzVector pD = p(0, evt) + pA ;
   TLorentzVector qD = pA - p(0, evt) ;    
    
   double MA    = mRes(A, evt);
   double MassV = mRes(V, evt);   
    
  if(debugThis){
    for(int i=0; i<4; i++){
      cout << " numbered by SF: p(" << i << ") " << p(i, evt) << endl;
    }
    for(int i=0; i<4; i++){
      cout << " numbered 'normally' p(" << i << ") " 
	   << p(i, evt) << endl;
    }
    cout << "got pV " << pV << endl;
  }
  
  if(_useZemachTensors){  
      ZTspin1 LB(qD,pD,pD.M());
      ZTspin1 LV(qV,pV,MassV);
      SpinSumV PA(pA,MA);  
      
      TLorentzVector tmp= PA.Dot(LV);
      return (LB.Dot(tmp))/(GeV*GeV);
  }
    

  
  return (p(0, evt).Dot(qV)
	  -    p0.Dot(pA) * pA.Dot(qV) / (MA*MA)
	  -    p0.Dot(pV) * pV.Dot(qV) / (MassV*MassV)
	  +    p0.Dot(pA) * pA.Dot(pV) * pV.Dot(qV) / (MA*MA * MassV*MassV)
	  )
    /(GeV*GeV)
	  ;
}
void SF_DtoAP0_AtoVP1_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoAP0_AtoVP1_VtoP2P3"
     << "\n\t (p(0).Dot(qV)"
     << "\n\t   -    p(0).Dot(pA) * pA.Dot(qV) / (MA*MA)"
     << "\n\t   -    p(0).Dot(pV) * pV.Dot(qV) / (MassV*MassV)"
     << "\n\t   +    p(0).Dot(pA) * pA.Dot(pV) * pV.Dot(qV) / (MA*MA * MassV*MassV)"
     << "\n\t   )"
     << "\n\t /(GeV*GeV)"
     << "\n\t with qV = p(2) - p(3) and pA = p(1) + p(2) + p(3)"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================

const DecayTree& SF_DtoAP0_AtoSP1_StoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(-321, 10323)->addDgtr(211, 10311)->addDgtr(321,-211);
  }
  return *_exampleDecay;
}

const DecayTree& SF_DtoAP0_AtoSP1_StoP2P3::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoAP0_AtoSP1_StoP2P3::parseTree(const DalitzEventPattern& pat){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay(pat).nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= 
      theDecay(pat).getDgtrTreePtr(i);

    if     (dgtr->getVal().SVPAT() == "A" && ! dgtr->isFinalState()) A = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
  }
  if(0==A || 0==fsPS[0]){
    cout << "ERROR in SF_DtoAP0_AtoSP1_StoP2P3::parseTree"
	 << " Didn't find A or P1 " << A.get() << ", " << fsPS[0].get() << endl;
    return false;
  }

  for(int i=0; i< A->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "S" && ! dgtr->isFinalState()) S = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==S || 0==fsPS[1]){
    cout << "ERROR in SF_DtoAP0_AtoSP1_StoP2P3::parseTree"
	 << " Didn't find S or P2 " << S.get() << ", " << fsPS[1].get() << endl;
    return false;
  }

  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoAP0_AtoSP1_StoP2P3::parseTree"
	 << " S should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);

  // normalOrder(fsPS[2], fsPS[3]);

  // this->printYourself();
  return true;
}

double SF_DtoAP0_AtoSP1_StoP2P3::getVal(IDalitzEvent& evt){
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());

  TLorentzVector p0(p(0, evt)), p1(p(1, evt)), p2(p(2, evt)), p3(p(3, evt));
  TLorentzVector pV = p2+p3;

  TLorentzVector pA =  p1 + p2  + p3;
  TLorentzVector qA = (p2 + p3) - p1;
    
  TLorentzVector pD = pA + p0;  
  TLorentzVector qD = pA - p0;
    
  double MA = mRes(A, evt);
  
  if(_useZemachTensors){
      ZTspin1 LD(qD,pD,pD.M());
      ZTspin1 LA(qA,pA,MA);
      return (LD.Dot(LA))/(GeV*GeV);
  }  
    
    
  return (p0.Dot(qA)
	  -    p0.Dot(pA) * pA.Dot(qA) / (MA*MA)
	  )    
    /(GeV*GeV)
    ;
}
void SF_DtoAP0_AtoSP1_StoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor  SF_DtoAP0_AtoSP1_StoP2P3"
     << "\n\t ( p(0).Dot(qA) -  p(0).Dot(pA) * pA.Dot(qA) / (MA*MA) )  /  (GeV*GeV)"
     << "\n\t with pV = p(2) + p(3), pA =  p(1) + p(2)  + p(3),  qA = (p(2) + p(3)) - p(1)"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_S::getExampleDecay(){
  if(0==_exampleDecayS){
    _exampleDecayS = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecayS->addDgtr( 313)->addDgtr( 321, -211);
    _exampleDecayS->addDgtr(-313)->addDgtr(-321,  211);
 }
  return *_exampleDecayS;
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_P::getExampleDecay(){
  if(0==_exampleDecayP){
    _exampleDecayP = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecayP->addDgtr( 313)->addDgtr( 321, -211);
    _exampleDecayP->addDgtr(-313)->addDgtr(-321,  211);

    _exampleDecayP->getVal().setL(1);
  }
  return *_exampleDecayP;
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_D::getExampleDecay(){
  if(0==_exampleDecayD){
    _exampleDecayD = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecayD->addDgtr( 313)->addDgtr( 321, -211);
    _exampleDecayD->addDgtr(-313)->addDgtr(-321,  211);

    _exampleDecayD->getVal().setL(2);
  }
  return *_exampleDecayD;
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_S::exampleDecay(){
  return getExampleDecay();
}
const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_P::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_D::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE::parseTree(const DalitzEventPattern& pat){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(theDecay(pat).nDgtr() != 2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay(pat).nDgtr();
    return false;
  }
  V1 = theDecay(pat).getDgtrTreePtr(0);
  V2 = theDecay(pat).getDgtrTreePtr(1);
  
  if(0==V1 || 0==V2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE::parseTree"
	 << " Didn't find V1 or V2 " << V1.get() << ", " << V2.get() << endl;
    return false;
  }
  if(V1->nDgtr() != 2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE::parseTree"
	 << " V1 should have 2 daughters, but it says it has "
	 << V1->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = V1->getDgtrTreePtr(0);
  fsPS[1] = V1->getDgtrTreePtr(1);
  // normalOrder(fsPS[0], fsPS[1]);

  if(V2->nDgtr() != 2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE::parseTree"
	 << " V2 should have 2 daughters, but it says it has "
	 << V2->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V2->getDgtrTreePtr(0);
  fsPS[3] = V2->getDgtrTreePtr(1);

  // normalOrder(fsPS[2], fsPS[3]);

  return true;
}

double SF_DtoV1V2_V1toP0P1_V1toP2P3_S::getVal(IDalitzEvent& evt){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ){
    parseTree(evt.eventPattern());
  }
  
  TLorentzVector pV1 = p(0, evt) + p(1, evt);
  TLorentzVector qV1 = p(0, evt) - p(1, evt);
  TLorentzVector pV2 = p(2, evt) + p(3, evt);
  TLorentzVector qV2 = p(2, evt) - p(3, evt);
  
  double MV1 = mRes(V1, evt);
  double MV2 = mRes(V2, evt);
  
  
  double returnVal= (qV1.Dot(qV2)
		     -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)
		     -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)
		     +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) 
		     / (MV1*MV1 * MV2*MV2)
		     )
    /(GeV*GeV)
    ;
  
  if(dbThis){
    ZTspin1 tV1(qV1, pV1, MV1);
    ZTspin1 tV2(qV2, pV2, MV2);
    
    double zResult = tV1.Contract(tV2);
    cout << "SF_DtoV1V2_V1toP0P1_V1toP2P3_S compare: " << zResult << " / "
	 << returnVal << " = " << zResult/returnVal << endl;
  }
  return returnVal; 
}

void SF_DtoV1V2_V1toP0P1_V1toP2P3_S::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1V2_V1toP0P1_V1toP2P3_S"
     << "\n\t        (  qV1.Dot(qV2)"
     << "\n\t 	    -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)"
     << "\n\t 	    -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)"
     << "\n\t 	    +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) "
     << "\n\t 	         / (MV1*MV1 * MV2*MV2)"
     << "\n\t 	    )"
     << "\n\t      /(GeV*GeV)"
     << "\n\t with pV1 = p(0) + p(1), qV1 = p(0) - p(1), pV2 = p(2) + p(3), qV2 = p(2) - p(3)"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// -------------------------

double SF_DtoV1V2_V1toP0P1_V1toP2P3_P::getVal(IDalitzEvent& evt){
  //bool dbThis=true;
  // double check this!!
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
  
  TLorentzVector pV1 = p(0, evt) + p(1, evt);
  TLorentzVector qV1 = p(0, evt) - p(1, evt);
  TLorentzVector pV2 = p(2, evt) + p(3, evt);
  TLorentzVector qV2 = p(2, evt) - p(3, evt);
  
  TLorentzVector pD = pV1 + pV2;
  TLorentzVector qD = pV1 - pV2;
    
  if (_useZemachTensors) {
      double MV1 = mRes(V1, evt);
      double MV2 = mRes(V2, evt);
      
      ZTspin1 LD(qD,pD,pD.M());
      ZTspin1 LV1(qV1,pV1,MV1);
      ZTspin1 LV2(qV2,pV2,MV2);
      
      return LeviCivita(pD,LD,LV1,LV2)/(GeV*GeV*GeV*GeV);
  }  
  
  /*
  double sum=0;
  for(int a=0; a<4; a++){
    for(int b=0; b<4; b++){
      if(a==b) continue;
      for(int c=0; c<4; c++){
	if(a==c || b==c) continue;
	for(int d=0; d<4; d++){
	  if(a==d || b==d || c==d) continue;
	  sum += LeviCita(a, b, c, d)
	    * pD[  (a + 3)%4 ] // coping with the stupid, stupid
	    * qD[  (b + 3)%4 ] // stupid, stupid convention in TLorentzVector
	    * qV1[ (c + 3)%4 ] // that index 0 = p_x, 1=py, 2=pz, 3=Energy
	    * qV2[ (d + 3)%4 ] // arrrrgh. ARRRRRRRGH!!! Rene!!! Why????
	    ; // (0 + 3)%4 = 3, (1+3)%4 = 0, (2+3)%4 = 1 (3+3)%4 = 2
	}
      }
    }
  }
  
  if(dbThis){
    cout << " compare leviCivitas " << sum << ", " 
	 << LeviCivita(pD, qD, qV1, qV2)
	 << endl;
  }
  return sum;
  */
  return LeviCivita(pD, qD, qV1, qV2)/(GeV*GeV*GeV*GeV);
}
void SF_DtoV1V2_V1toP0P1_V1toP2P3_P::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1V2_V1toP0P1_V1toP2P3_P"
     << "\n\t LeviCivita(pD, qD, qV1, qV2)"
     << "\n\t with pV1 = p(0) + p(1), qV1 = p(0) - p(1), pV2 = p(2) + p(3), qV2 = p(2) - p(3)"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// -------------------------

double SF_DtoV1V2_V1toP0P1_V1toP2P3_D::getVal(IDalitzEvent& evt){
  // double check this!!
  bool dbThis=false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
  
  TLorentzVector pV1 = p(0, evt) + p(1, evt);
  TLorentzVector qV1 = p(0, evt) - p(1, evt);
  TLorentzVector pV2 = p(2, evt) + p(3, evt);
  TLorentzVector qV2 = p(2, evt) - p(3, evt);
  
  double MV1 = mRes(V1, evt);
  double MV2 = mRes(V2, evt);
    
    
  if (_useZemachTensors) {
        TLorentzVector pD = pV1 + pV2;
        TLorentzVector qD = pV1 - pV2;
        double mD = pD.M();
        
        ZTspin1 tV1(qV1, pV1, MV1);
        ZTspin1 tV2(qV2, pV2, MV2);
        ZTspin2 tD(qD, pD, mD);
        
        return tV1.Contract(tD.Contract(tV2))/(GeV*GeV*GeV*GeV);
  }  
  
  double returnVal = 
         (  qV1.Dot(pV2) - qV1.Dot(pV1) * pV1.Dot(pV2)/(MV1*MV1)
	 )*( 
	    qV2.Dot(pV1) - qV2.Dot(pV2) * pV2.Dot(pV1)/(MV2*MV2)
	 )
         / (GeV*GeV*GeV*GeV)
    ;
  
  if(dbThis){
    TLorentzVector pD = -(pV1 + pV2);
    TLorentzVector qD = pV1 - pV2;
    double mD = pD.M();

   
    ZTspin1 tV1(qV1, pV1, MV1);
    ZTspin1 tV2(qV2, pV2, MV2);
    //ZTspin2 tD(pD + pV1, pD - pV1, mD);
    ZTspin2 tD(qD, pD, mD);
    //ZTspin2 tD(qD, pD, mD);
    
    double z1Result = tV1.Contract(tD.Contract(tV2))/(GeV*GeV*GeV*GeV);
    double z2Result = tV1.Dot(pV2) * tV2.Dot(pV1)/(GeV*GeV*GeV*GeV);
    double z3Result = tV1.Dot(tV2) * tD.Contract_2(tD)/(GeV*GeV* GeV*GeV*GeV*GeV *GeV*GeV*GeV*GeV);
    cout << "SF_DtoV1V2_V1toP0P1_V1toP2P3_D compare: 1/n " << z1Result << " / "
	 << returnVal << " = " << z1Result/returnVal << endl;
    cout << " .... and compare: 2/n " << z2Result << " / "
	 << returnVal << " = " << z2Result/returnVal << endl;
    cout << " .... and compare: 2/1 " << z2Result << " / "
	 << z1Result << " = " << z2Result/z1Result << endl;
    cout << " .... and compare: 3/n " << z3Result << " / "
	 << returnVal << " = " << z3Result/returnVal << endl;
    cout << " .... and compare: 3/1 " << z3Result << " / "
	 << z1Result << " = " << z3Result/z1Result << endl;

  }

  return returnVal;
 
}
void SF_DtoV1V2_V1toP0P1_V1toP2P3_D::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1V2_V1toP0P1_V1toP2P3_D"
     << "\n\t (  qV1.Dot(pV2) - qV1.Dot(pV1) * pV1.Dot(pV2)/(MV1*MV1)"
     <<	"\n\t  )*( "
     << "\n\t	    qV2.Dot(pV1) - qV2.Dot(pV2) * pV2.Dot(pV1)/(MV2*MV2)"
     << "\n\t	 )"
     << "\n\t    / (GeV*GeV*GeV*GeV)"
     << "\n\t with pV1 = p(0) + p(1), qV1 = p(0) - p(1), pV2 = p(2) + p(3), qV2 = p(2) - p(3)"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//=========================================================

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(-321,  321);
    _exampleDecay->addDgtr( 113)->addDgtr( 211, -211);
 }
  return *_exampleDecay;
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::parseTree(const DalitzEventPattern& pat){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  int P_index=0;
  for(int i=0; i< theDecay(pat).nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == "V" && ! dgtr->isFinalState()) V2 = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" && dgtr->isFinalState()){
      fsPS[P_index++] = dgtr;
    }
  }
  if(0==V2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::parseTree"
	 << " Didn't find V " << V2.get() << endl;
    return false;
  }
  if(P_index != 2 || 0==(fsPS[0]) || 0==(fsPS[1])  ){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::parseTree"
	 << " Didn't find P1, P2 " << endl;
    return false;
  }
  if(V2->nDgtr() != 2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V2->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V2->getDgtrTreePtr(0);
  fsPS[3] = V2->getDgtrTreePtr(1);
  // normalOrder(fsPS[2], fsPS[3]);
  
  // this->printYourself();
  return true;
}

double SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::getVal(IDalitzEvent& evt){
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());

    TLorentzVector pV1 = p(0, evt) + p(1, evt);
    TLorentzVector qV1 = p(0, evt) - p(1, evt);
    TLorentzVector pV2 = p(2, evt) + p(3, evt);
    TLorentzVector qV2 = p(2, evt) - p(3, evt);

    double MV1 = pV1.M();
    double MV2 = mRes(V2, evt);

    return (qV1.Dot(qV2)
	    -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)
	    -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)
	    +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) 
	    / (MV1*MV1 * MV2*MV2)
	    )
      /(GeV*GeV)
      ;

}
void SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1"
     << " - I recommend not to use this, use nonRes particles insted "
     << " anyway, this is what I do:"
     << "\n\t (qV1.Dot(qV2)"
     << "\n\t 	    -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)"
     << "\n\t 	    -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)"
     << "\n\t 	    +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) "
     << "\n\t 	    / (MV1*MV1 * MV2*MV2)"
     << "\n\t 	    )"
     << "\n\t   /(GeV*GeV)"
     << "\n\t with: pV1 = p(0) + p(1), qV1 = p(0) - p(1), pV2 = p(2) + p(3) qV2 = p(2) - p(3);"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================

const DecayTree& SF_DtoVS_VtoP0P1_StoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(    333)->addDgtr( 321, -321); // V
    _exampleDecay->addDgtr(9010221)->addDgtr(-211,  211); // S
  }
  return *_exampleDecay; // this example is not necessarily physical.
}

const DecayTree& SF_DtoVS_VtoP0P1_StoP2P3::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoVS_VtoP0P1_StoP2P3::parseTree(const DalitzEventPattern& pat){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay(pat).nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == "V" && ! dgtr->isFinalState()) V = dgtr;
    else if(dgtr->getVal().SVPAT() == "S" && ! dgtr->isFinalState()) S = dgtr;
  }
  if(0==V || 0==S){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3::parseTree"
	 << " Didn't find V or S " << V.get() << ", " << S.get() << endl;
    return false;
  }
  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = V->getDgtrTreePtr(0);
  fsPS[1] = V->getDgtrTreePtr(1);
  // normalOrder(fsPS[0], fsPS[1]);

  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3::parseTree"
	 << " S should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);
  // normalOrder(fsPS[2], fsPS[3]);

  // this->printYourself();
  return true;
}

double SF_DtoVS_VtoP0P1_StoP2P3::getVal(IDalitzEvent& evt){
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());

    TLorentzVector pS = p(2, evt) + p(3, evt);
    TLorentzVector pV = p(0, evt) + p(1, evt);
    TLorentzVector qV = p(0, evt) - p(1, evt);

    double MassV = mRes(V, evt);
    
    if(_useZemachTensors){
        TLorentzVector pD= pV + pS;
        TLorentzVector qD= pV - pS;
        ZTspin1 LD(qD,pD,pD.M());
        ZTspin1 LV(qV,pV,MassV);
        return (LD.Dot(LV))/(GeV*GeV);
    }

    return (pS.Dot(qV)
	    -    pS.Dot(pV) * pV.Dot(qV) / (MassV*MassV)
	    )
      /(GeV*GeV)
      ;
}
void SF_DtoVS_VtoP0P1_StoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoVS_VtoP0P1_StoP2P3"
     << "\n\t ( pS.Dot(qV) - pS.Dot(pV) * pV.Dot(qV) / (MassV*MassV) )  / (GeV*GeV)"
     << "\n\t with: pS = p(2) + p(3), pV = p(0) + p(1), qV = p(0) - p(1)"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================

const DecayTree& SF_DtoVS_VtoP0P1_StoP2P3_nonResV::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr( 321, -321);                   // V (non res)
    _exampleDecay->addDgtr(9010221)->addDgtr(-211,  211); // S
  }
  return *_exampleDecay; // this example is not necessarily physical.
}

const DecayTree& SF_DtoVS_VtoP0P1_StoP2P3_nonResV::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoVS_VtoP0P1_StoP2P3_nonResV::parseTree(const DalitzEventPattern& pat){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  int P_index=0;
  for(int i=0; i< theDecay(pat).nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == "S" && ! dgtr->isFinalState()) S = dgtr;
    else if(dgtr->getVal().SVPAT() == "P" && dgtr->isFinalState()){
      fsPS[P_index++] = dgtr;
    }
  }
  if(0==S){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3_nonResV::parseTree"
	 << " Didn't find S " << S.get() << endl;
    return false;
  }
  if(P_index != 2 || 0==(fsPS[0]) || 0==(fsPS[1])  ){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3_nonResV::parseTree"
	 << " Didn't find P1, P2 " << endl;
    return false;
  }
  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3_nonResV::parseTree"
	 << " S should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);
  // normalOrder(fsPS[2], fsPS[3]);

  // this->printYourself();
  return true;
}

double SF_DtoVS_VtoP0P1_StoP2P3_nonResV::getVal(IDalitzEvent& evt){
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());

    TLorentzVector pS = p(2, evt) + p(3, evt);
    TLorentzVector pV = p(0, evt) + p(1, evt);
    TLorentzVector qV = p(0, evt) - p(1, evt);

    double MassV = pV.M();

    return (pS.Dot(qV)
	    -    pS.Dot(pV) * pV.Dot(qV) / (MassV*MassV)
	    )
      /(GeV*GeV)
      ;
}
void SF_DtoVS_VtoP0P1_StoP2P3_nonResV::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoVS_VtoP0P1_StoP2P3_nonResV"
     << " - recommend not to use this, use nonRes particles instead"
     << "\n anyway, I return:"
     << "\n\t (pS.Dot(qV) - pS.Dot(pV) * pV.Dot(qV) / (MassV*MassV) ) / (GeV*GeV)"
     << "\n\t with: pS = p(2) + p(3), pV = p(0) + p(1), qV = p(0) - p(1)"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//=========================================================
// provided by Loic Esteve:
const DecayTree& SF_DtoV1P0_V1toV2P1_V2toP2P3::getExampleDecay(){
 if(0==_exampleDecay){
   _exampleDecay = new DecayTree(421);
   // remark: addDgtr always returns a pointer to the 
   // last daughter that was added, thus allowing these
   // chains:
   _exampleDecay->addDgtr(310, 223)->addDgtr(111, 113)->addDgtr(211, -211);
 }
 return *_exampleDecay;
}
const DecayTree& SF_DtoV1P0_V1toV2P1_V2toP2P3::exampleDecay(){
 return getExampleDecay();
}


bool SF_DtoV1P0_V1toV2P1_V2toP2P3::parseTree(const DalitzEventPattern& pat){
 //  bool debugThis=false;
 if(fsPS.size() < 4) fsPS.reserve(4);
 for(int i=0; i< theDecay(pat).nDgtr(); i++){
   const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
   if     (dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[0] = dgtr;
   else if(dgtr->getVal().SVPAT() == "V" && ! dgtr->isFinalState()) V1 = dgtr;
 }
 if(0==V1 || 0==fsPS[0]){
   cout << "ERROR in SF_DtoV1P0_V1toV2P1_V2toP2P3::parseTree"
	 << " Didn't find V1 or P0 " << V1.get() << ", " << fsPS[0].get() << endl;
   return false;
 }

 for(int i=0; i< V1->nDgtr(); i++){
   const_counted_ptr<AssociatedDecayTree> dgtr= V1->getDgtrTreePtr(i);
   if     (dgtr->getVal().SVPAT() == "V" && ! dgtr->isFinalState()) V2 = dgtr;
   else if(dgtr->getVal().SVPAT() == "P" &&   dgtr->isFinalState()) fsPS[1] = dgtr;
 }
 if(0==V2 || 0==fsPS[1]){
   cout << "ERROR in SF_DtoV1P0_V1toV2P1_V2toP2P3::parseTree"
	 << " Didn't find V2 or P1 " << V2.get() << ", " << fsPS[1].get() << endl;
   return false;
 }

 if(V2->nDgtr() != 2){
   cout << "ERROR in SF_DtoV1P0_V1toV2P1_V2toP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V2->nDgtr() << "."
	 << endl;
   return false;
 }
 fsPS[2] = V2->getDgtrTreePtr(0);
 fsPS[3] = V2->getDgtrTreePtr(1);
 // normalOrder(fsPS[2], fsPS[3]);

 // this->printYourself();
 return true;
}

double SF_DtoV1P0_V1toV2P1_V2toP2P3::getVal(IDalitzEvent& evt){
 if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());

 TLorentzVector pV1 = p(1, evt) + p(2, evt) + p(3, evt);
 TLorentzVector pV2 = p(2, evt) + p(3, evt);
 TLorentzVector pP1 = p(0, evt);

 TLorentzVector qV1 = (p(2, evt) + p(3, evt)) - p(1, evt);
 TLorentzVector qV2 = p(2, evt) - p(3, evt);

 double units = GeV*GeV*GeV*GeV;
    
    if (_useZemachTensors) {
        double MV1 = mRes(V1, evt);
        double MV2 = mRes(V2, evt);
        
        TLorentzVector pD = pV1 + p(0, evt);
        TLorentzVector qD = pV1 - p(0, evt);
        
        ZTspin1 LD(qD,pD,pD.M());
        ZTspin1 LV1(qV1,pV1,MV1);
        ZTspin1 LV2(qV2,pV2,MV2);
        SpinSumV PV1(pV1,MV1);
        
        TLorentzVector tmp = PV1.Dot(LD);
        if(tmp == TLorentzVector(0,0,0,0)) return 0.;
        return LeviCivita(tmp,LV1,LV2,pV1)/units;
    }   
    
    
 return LeviCivita(pV1, qV1, pP1, qV2)/units;
}

void SF_DtoV1P0_V1toV2P1_V2toP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1P0_V1toV2P1_V2toP2P3"
     << "\n\t  return: LeviCivita(pV1, qV1, pP1, qV2)/ / GeV^4"
     << "\n\t with pV1 = p(1) + p(2) + p(3), pP1 = p(0)"
     << "\n\t and qV1 = (p(2) + p(3)) - p(1), qV2 = p(2) - p(3)"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//=========================================================

double SF_DtoAP0_AtoVP1Dwave_VtoP2P3::getVal(IDalitzEvent& evt){
  //  bool debugThis = false;

  // from pg 35 of Phys.Rev.D75:052003,2007
  // at http://prd.aps.org/abstract/PRD/v75/i5/e052003
  // and at http://arxiv.org/abs/hep-ex/0701001
  // except that we use MA^2 instead of p_a^2 and MV^2 instead of p_v^2

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());

  TLorentzVector pV = p(2, evt) + p(3, evt);
  TLorentzVector qV = p(2, evt) - p(3, evt);

  TLorentzVector pA = p(1, evt) + p(2, evt) + p(3, evt);
  TLorentzVector qA = p(1, evt) - (p(2, evt) + p(3, evt));

  TLorentzVector pD = p(0, evt) + pA;  
  TLorentzVector qD = pA - p(0, evt);  

  double MA = mRes(A, evt);
  double MV = mRes(V, evt);
  
  ZTspin1 tA(qA, pA, MA);
  ZTspin1 tV(qV, pV, MV);

  double units = GeV*GeV*GeV*GeV;

  if(_useZemachTensors){  
        ZTspin1 LD(qD,pD,pD.M());
        ZTspin1 LV(qV,pV,MV);
        ZTspin2 LA(qA,pA,MA); 
        TLorentzVector tmp= LA.Contract(LV);
      
        //ZTspin1 LA1(qA,pA,MA); 
        //SpinSumV PA(pA,MA); 
        //return LD.Dot(LA1)*(LA1.Dot(LV)) + 1./3. * LD.Dot(PA.Dot(LV)) * LA1.Dot(LA1);
      
        return (LD.Dot(tmp))/units;
   }  
    
  return p(0, evt).Dot(tA) * tV.Dot(pA) / units;
}
void SF_DtoAP0_AtoVP1Dwave_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoAP0_AtoVP1Dwavve_VtoP2P3"
     << " with\t pV = p(2) + p(3), qV = p(2) - p(3) "
     << "\n\t and pA = p(1) + p(2) + p(3), qA = p(1) - (p(2) + p(3));"
     << "\n\t evaluates as:"
     << "\n\t ZTspin1 tA(qA, pA, MA);"
     << "\n\t ZTspin1 tV(qV, pV, MV);"
     << "\n\t p(0).Dot(tA) * tV.Dot(pA) / (GeV*GeV*GeV*GeV);"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================
//
//
