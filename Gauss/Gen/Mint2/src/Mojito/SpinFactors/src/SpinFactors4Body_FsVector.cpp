// author: Philippe d'Argent (p.dargent@cern.ch)
// status:  Tu 6 May 2014 

#include "Mint/SpinFactors.h"
#include "Mint/DecayTree.h"
//#include "Mint/DecayTreeComparisons.h"

#include "Mint/Utils.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include "Mint/ZTspin1.h"
#include "Mint/ZTspin2.h"
#include "Mint/LeviCivita.h"
#include "Mint/SpinSumV.h"
#include "Mint/SpinSumT.h"
#include "Mint/ZTgperp.h"
#include "Mint/SymmLorentzMatrix.h"

#include "Mint/polVector.h"


using namespace std;
using namespace MINT;

DecayTree* SF_BtoAV0_AtoVP1_VtoP2P3::_exampleDecay=0;
DecayTree* SF_BtoAV0_AtoVP1Dwave_VtoP2P3::_exampleDecayDwave=0;
DecayTree* SF_BtoAV0_AtoVP1_VtoP2P3_P::_exampleDecayP=0;
DecayTree* SF_BtoAV0_AtoVP1_VtoP2P3_D::_exampleDecayD=0;
DecayTree* SF_BtoAV0_AtoSP1_StoP2P3::_exampleDecay=0;

DecayTree* SF_BtoV1V0_V1toV2P1_V2toP2P3::_exampleDecay=0;

DecayTree* SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P::_exampleDecayP=0;
DecayTree* SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D::_exampleDecayD=0;
DecayTree* SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P::_exampleDecayP=0;
DecayTree* SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P::_exampleDecayP=0;

DecayTree* SF_BtoTV0_TtoVP1_VtoP2P3_P::_exampleDecayP=0;

DecayTree* SF_BtoVP1_VtoV0S_StoP2P3::_exampleDecay=0;
DecayTree* SF_BtoV1P1_V1toV2V0_V2toP2P3::_exampleDecay=0;

DecayTree* SF_BtoAP1_AtoVV0_VtoP2P3::_exampleDecay=0;
DecayTree* SF_BtoV1V2_V1toP1V0_V2toP2P3::_exampleDecay=0;

DecayTree* SF_BtoAV_AtoP1V0_VtoP2P3::_exampleDecay=0;
DecayTree* SF_BtoAS_AtoP1V0_StoP2P3::_exampleDecay=0;
DecayTree* SF_BtoPS_PtoP1V0_StoP2P3::_exampleDecay=0;

DecayTree* SF_BtoPV_PtoP1V0_VtoP2P3::_exampleDecay=0;
DecayTree* SF_BtoPV0_PtoS_StoP2P3::_exampleDecay=0;
DecayTree* SF_BtoPV0_PtoP1V1_V1toP2P3::_exampleDecay=0;


DecayTree* SF_BtoVP1_VtoAP2_AtoV0P3::_exampleDecay=0;
DecayTree* SF_BtoVP1_VtoSV0_StoP2P3::_exampleDecay=0;
 
//==================================================================
//==================================================================
// SF 1-4

bool SF_BtoAV0_AtoVP1_VtoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
  bool debugThis=false;

  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay(pat).nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == "A") A = dgtr;
    else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
  }
  if(0==A || 0==fsPS[0]){
    cout << "ERROR in SF_BtoAV0_AtoVP1_VtoP2P3::parseTree"
	 << " Didn't find A or V0 " << A.get() << ", " << fsPS[0].get() << endl;
    return false;
  }

  for(int i=0; i< A->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == "V") V = dgtr;
    else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
  }
  if(0==V || 0==fsPS[1]){
    cout << "ERROR in SF_BtoAV0_AtoVP1_VtoP2P3::parseTree"
	 << " Didn't find V or P1 " << V.get() << ", " << fsPS[1].get() << endl;
    return false;
  }

  if(V->nDgtr() != 2){
    cout << "ERROR in SF_BtoAV0_AtoVP1_VtoP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V->getDgtrTreePtr(0);
  fsPS[3] = V->getDgtrTreePtr(1);

  ////normalOrder(fsPS[2], fsPS[3]);

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

const DecayTree& SF_BtoAV0_AtoVP1_VtoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    // B->J/psi K1(1270); K1->K*(892) pi; K*->K pi  
    _exampleDecay = new DecayTree(521);
    _exampleDecay->addDgtr(443, 10323)->addDgtr(211, 313)->addDgtr(321,-211);
  }
  return *_exampleDecay;
}

const DecayTree& SF_BtoAV0_AtoVP1_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_BtoAV0_AtoVP1Dwave_VtoP2P3::getExampleDecay(){
    if(0==_exampleDecayDwave){
        // B->J/psi K1(1270); K1->K*(892) pi; K*->K pi  
        _exampleDecayDwave = new DecayTree(521);
        DecayTree k1(10323);
        k1.getVal().setL(2);
        k1.addDgtr(211, 313)->addDgtr(321,-211);
        _exampleDecayDwave->addDgtr(443);
        _exampleDecayDwave->addDgtr(&k1);
    }
    return *_exampleDecayDwave;
}

const DecayTree& SF_BtoAV0_AtoVP1Dwave_VtoP2P3::exampleDecay(){
    return getExampleDecay();
}

const DecayTree& SF_BtoAV0_AtoVP1_VtoP2P3_P::getExampleDecay(){
    if(0==_exampleDecayP){
        // B->J/psi K1(1270); K1->K*(892) pi; K*->K pi  
        _exampleDecayP = new DecayTree(521);
        _exampleDecayP->addDgtr(443, 10323)->addDgtr(211, 313)->addDgtr(321,-211);
        _exampleDecayP->getVal().setL(1);
    }
    return *_exampleDecayP;
}

const DecayTree& SF_BtoAV0_AtoVP1_VtoP2P3_P::exampleDecay(){
    return getExampleDecay();
}

const DecayTree& SF_BtoAV0_AtoVP1_VtoP2P3_D::getExampleDecay(){
    if(0==_exampleDecayD){
        // B->J/psi K1(1270); K1->K*(892) pi; K*->K pi  
        _exampleDecayD = new DecayTree(521);
        _exampleDecayD->addDgtr(443, 10323)->addDgtr(211, 313)->addDgtr(321,-211);
        _exampleDecayD->getVal().setL(2);
    }
    return *_exampleDecayD;
}

const DecayTree& SF_BtoAV0_AtoVP1_VtoP2P3_D::exampleDecay(){
    return getExampleDecay();
}

//SF 1
complex<double> SF_BtoAV0_AtoVP1_VtoP2P3::getNewVal(IDalitzEvent& evt){
  //bool debugThis = false;
    
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
      
  double MA    = mRes(A, evt);
  double MV    = mRes(V, evt);
  double MV0   = mRes(fsPS[0], evt); // ??  
  //double MV0   = p(0, evt).M(); // ??  
  //cout << "pol in spinFactor class" << getPolarisation() << endl;

  TLorentzVector pV= p(2, evt)+p(3, evt);
  TLorentzVector qV= p(2, evt)-p(3, evt);
  TLorentzVector pA= pV+p(1, evt); 
    
    
  ZTspin1 Z(qV,pV,MV);
  SpinSumV P(pA,MA);
   
  polVector e = polVector(p(0, evt),MV0,getPolarisation());  
  e.conj();   
  
  //  e.Print();    
  complex<double> sfm(P.Sandwich(e.Re(),Z),P.Sandwich(e.Im(),Z));  
  //complex<double> sfm(P.Dot(Z).Dot(e.Re()),P.Dot(Z).Dot(e.Re()));  

  //cout << sfm/GeV << endl;  
  //  return complex<double>(1,0);  
  return sfm/GeV;
}
void SF_BtoAV0_AtoVP1_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_BtoAV0_AtoVP1_VtoP2P3"
     << "\n\t P.Sandwich(e_c,Z)"
     << "\n\t /GeV"
     << "\n\t    parsed tree " << theBareDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//SF 2
complex<double> SF_BtoAV0_AtoVP1Dwave_VtoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MA    = mRes(A, evt);
    double MV    = mRes(V, evt);
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    
    TLorentzVector pV= p(2, evt)+p(3, evt);
    TLorentzVector qV= p(2, evt)-p(3, evt);
    TLorentzVector pA= pV+p(1, evt); 
    TLorentzVector qA= pV-p(1, evt); 
    
    ZTspin1 ZV(qV,pV,MV);
    //ZTspin1 ZA(qA,pA,MA);
    //SpinSumV PA(pA,MA);
    SpinSumT DA(pA,MA);
    
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj();   
    
    complex<double> sfm(DA.Sandwich(e.Re(),ZV,qA,qA),DA.Sandwich(e.Im(),ZV,qA,qA));  
    return sfm/(GeV*GeV*GeV);
    
    //double tmp= ZV.Dot(qA);
    //complex<double> sfm(ZA.Dot(e.Re()),ZA.Dot(e.Im()));  
    
    /*
    double tmp2= qA.M2();
    //double tmp2= MA;
    TLorentzVector tmpL = PA.Dot(ZV);
    complex<double> sfm2(e.Re().Dot(tmpL),e.Im().Dot(tmpL));  
    return (2.*sfm*tmp+sfm2*tmp2)/(GeV*GeV*GeV);
    return sfm2*tmp2/(GeV*GeV*GeV);
    */
    //return sfm*tmp/(GeV*GeV*GeV);
}
void SF_BtoAV0_AtoVP1Dwave_VtoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoAV0Dwave_AtoVP1_VtoP2P3"
    << "\n\t e_c.Dot(ZA) * ZV.Dot(qA)"
    << "\n\t /GeV^3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//SF 3
complex<double> SF_BtoAV0_AtoVP1_VtoP2P3_P::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MA    = mRes(A,evt);
    double MV    = mRes(V,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    
    TLorentzVector pV= p(2,evt)+p(3,evt);
    TLorentzVector qV= p(2,evt)-p(3,evt);
    TLorentzVector pA= pV+p(1,evt); 
    //TLorentzVector qA= pV-p(1,evt); 
    TLorentzVector pB= pA+p(0,evt); 
    TLorentzVector qB= pA-p(0,evt); 
    
    ZTspin1 ZB(qB,pB,pB.M());
    ZTspin1 ZV(qV,pV,MV);
    SpinSumV PA(pA,MA);

    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj();   
    
    TLorentzVector tmp=PA.Dot(ZV);
    if(tmp==TLorentzVector(0,0,0,0))return 0.;
    
    complex<double> sfm(LeviCivita(e.Re(),tmp,ZB,pB),LeviCivita(e.Im(),tmp,ZB,pB));
    //if(getPolarisation()==0)sfm=complex<double>(LeviCivita(e.Re(),tmp,pB,ZB),0.);  
    //else sfm=complex<double>(LeviCivita(e.Re(),tmp,ZB,pB),LeviCivita(e.Im(),tmp,ZB,pB));  
    
    
    return sfm/(GeV*GeV*GeV);
}
void SF_BtoAV0_AtoVP1_VtoP2P3_P::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoAV0_AtoVP1_VtoP2P3_P"
    << "\n\t LeviCivita(e_c,PA.Dot(ZV),pB,qB)"
    << "\n\t /GeV^3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}

//SF 4
complex<double> SF_BtoAV0_AtoVP1_VtoP2P3_D::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MA    = mRes(A,evt);
    double MV    = mRes(V,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    //cout << "pol in spinFactor class" << getPolarisation() << endl;
    
    TLorentzVector pV= p(2,evt)+p(3,evt);
    TLorentzVector qV= p(2,evt)-p(3,evt);
    TLorentzVector pA= pV+p(1,evt); 
    //TLorentzVector qA= pV-p(1,evt); 
    TLorentzVector pB= pA+p(0,evt); 
    TLorentzVector qB= pA-p(0,evt); 
    
    ZTspin1 ZV(qV,pV,MV);
    //ZTspin1 ZA(qA,pA,MA);
    SpinSumV PA(pA,MA);
    SpinSumT DB(pB,pB.M());
    
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj(); 
    
    TLorentzVector tmp= PA.Dot(ZV);
    complex<double> sfm(DB.Sandwich(e.Re(),tmp,qB,qB),DB.Sandwich(e.Im(),tmp,qB,qB));
    return sfm/(GeV*GeV*GeV);
    
    //double tmp= ZA.Dot(ZV);
    //complex<double> sfm(e.Re().Dot(qB),e.Im().Dot(qB));  
    //return sfm*tmp/(GeV*GeV*GeV);
}
void SF_BtoAV0_AtoVP1_VtoP2P3_D::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoAV0_AtoVP1_VtoP2P3_D"
    << "\n\t e_c.Dot(pA) * ZA.Dot(ZV)"
    << "\n\t /GeV^3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}



//=========================================================
//SF 5

const DecayTree& SF_BtoAV0_AtoSP1_StoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->J/Psi K1(1270); K1-> K0^*(1430) pi; K0^*->K pi  
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(443, 10323)->addDgtr(211, 10311)->addDgtr(321,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoAV0_AtoSP1_StoP2P3::exampleDecay(){
    return getExampleDecay();
}

bool SF_BtoAV0_AtoSP1_StoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "A") A = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
    }
    if(0==A || 0==fsPS[0]){
        cout << "ERROR in SF_BtoAV0_AtoSP1_StoP2P3::parseTree"
        << " Didn't find A or V0 " << A.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    for(int i=0; i< A->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "S") S = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==S || 0==fsPS[1]){
        cout << "ERROR in SF_BtoAV0_AtoSP1_StoP2P3::parseTree"
        << " Didn't find S or P1  " << S.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    if(S->nDgtr() != 2){
        cout << "ERROR in SF_BtoAV0_AtoSP1_StoP2P3::parseTree"
        << " S should have 2 daughters, but it says it has "
        << S->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = S->getDgtrTreePtr(0);
    fsPS[3] = S->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: A:\n"
        << *A
        << "\n S:"
        << *S
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

complex<double> SF_BtoAV0_AtoSP1_StoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MA    = mRes(A,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    
    //cout << "pol in spinFactor class" << getPolarisation() << endl;
    
    TLorentzVector pA= p(2,evt)+p(3,evt)+p(1,evt);
    TLorentzVector qA= p(2,evt)+p(3,evt)-p(1,evt);
    ZTspin1 ZA(qA,pA,MA);
    
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj();   
    
    complex<double> sfm(ZA.Dot(e.Re()),ZA.Dot(e.Im()));  
    
    return sfm/GeV;
}
void SF_BtoAV0_AtoSP1_StoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoAV0_AtoSP1_StoP2P3"
    << "\n\t ZA.Dot(e_c)"
    << "\n\t /GeV"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//==================================================================
// SF 6

bool SF_BtoV1V0_V1toV2P1_V2toP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V" &&   !(dgtr->isFinalState())) V1 = dgtr;
        else if(dgtr->getVal().SVPAT() == "V" &&   dgtr->isFinalState() ) fsPS[0] = dgtr;
    }
    if(0==V1 || 0==fsPS[0]){
        cout << "ERROR in SF_BtoV1V0_V1toV2P1_V2toP2P3::parseTree"
        << " Didn't find V1 or V0 " << V1.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    for(int i=0; i< V1->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= V1->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") V2 = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==V2 || 0==fsPS[1]){
        cout << "ERROR in SF_BtoV1V0_V1toV2P1_V2toP2P3::parseTree"
        << " Didn't find V2 or P1 " << V2.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    if(V2->nDgtr() != 2){
        cout << "ERROR in SF_BtoV1V0_V1toV2P1_V2toP2P3::parseTree"
        << " V2 should have 2 daughters, but it says it has "
        << V2->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = V2->getDgtrTreePtr(0);
    fsPS[3] = V2->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: V1:\n"
        << *V1
        << "\n V2:"
        << *V2
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

const DecayTree& SF_BtoV1V0_V1toV2P1_V2toP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->J/psi K1(1270); K1->K*(892) pi; K*->K pi  
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(443, 100323)->addDgtr(211, 313)->addDgtr(321,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoV1V0_V1toV2P1_V2toP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoV1V0_V1toV2P1_V2toP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MV1    = mRes(V1,evt);
    double MV2    = mRes(V2,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    //cout << "pol in spinFactor class" << getPolarisation() << endl;
    
    TLorentzVector pV2= p(2,evt)+p(3,evt);
    TLorentzVector qV2= p(2,evt)-p(3,evt);
    TLorentzVector pV1= pV2+p(1,evt); 
    TLorentzVector qV1= pV2-p(1,evt); 
    TLorentzVector pB= pV1+p(0,evt); 
    TLorentzVector qB= pV1-p(0,evt); 
    
    ZTspin1 ZV1(qV1,pV1,MV1);
    ZTspin1 ZV2(qV2,pV2,MV2);
    SpinSumV PV1(pV1,MV1);
    
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj();  
    
    TLorentzVector  tmp= PV1.Dot(ZV2);
    if(tmp==TLorentzVector(0,0,0,0))return 0.;
    complex<double> sfm(LeviCivita(PV1.Dot(e.Re()),ZV1,pB,tmp),LeviCivita(PV1.Dot(e.Im()),ZV1,pB,tmp));

    //if(getPolarisation()==0)sfm=complex<double>(LeviCivita(PV1.Dot(e.Re()),ZV2,pB,qB),0.);
    //else sfm=complex<double>(LeviCivita(PV1.Dot(e.Re()),ZV2,pB,qB),LeviCivita(PV1.Dot(e.Im()),ZV2,pB,qB));  
    //if(getPolarisation()==0)sfm=complex<double>(LeviCivita(PV1.Dot(e.Re()),ZV1,pV1,tmp),0.);
    //else sfm=complex<double>(LeviCivita(PV1.Dot(e.Re()),ZV1,pB,tmp),LeviCivita(PV1.Dot(e.Im()),ZV1,pB,tmp));  
    
    return sfm/(GeV*GeV*GeV);
}
void SF_BtoV1V0_V1toV2P1_V2toP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoV1V0_V1toV2P1_V2toP2P3"
    << "\n\t PV1.Contract(e_c),ZV2,qB,pB)"
    << "\n\t /GeV^3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//=========================================================
//SF 7-8

bool SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "PT") PT = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
    }
    if(0==PT || 0==fsPS[0]){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3::parseTree"
        << " Didn't find PT or V0 " << PT.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    for(int i=0; i< PT->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= PT->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") V = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==V || 0==fsPS[1]){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3::parseTree"
        << " Didn't find V or P1 " << V.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    if(V->nDgtr() != 2){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3::parseTree"
        << " V should have 2 daughters, but it says it has "
        << V->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = V->getDgtrTreePtr(0);
    fsPS[3] = V->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: V:\n"
        << *V
        << "\n PT:"
        << *PT 
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

const DecayTree& SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P::getExampleDecay(){
    if(0==_exampleDecayP){
        // B->J/psi K2(1580); K2->K*(892) pi; K*->K pi  
        _exampleDecayP = new DecayTree(521);
        _exampleDecayP->addDgtr(443, 9000325)->addDgtr(211, 313)->addDgtr(321,-211);
    }
    return *_exampleDecayP;
}

const DecayTree& SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P::exampleDecay(){
    return getExampleDecay();
}

const DecayTree& SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D::getExampleDecay(){
    if(0==_exampleDecayD){
        // B->J/psi K2(1580); K2->K*(892) pi; K*->K pi  
        _exampleDecayD = new DecayTree(521);
        _exampleDecayD->addDgtr(443, 9000325)->addDgtr(211, 313)->addDgtr(321,-211);
        _exampleDecayD->getVal().setL(2);
    }
    return *_exampleDecayD;
}

const DecayTree& SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D::exampleDecay(){
    return getExampleDecay();
}


//SF 7
complex<double> SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MT    = mRes(PT,evt);
    double MV    = mRes(V,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    
    TLorentzVector pV= p(2,evt)+p(3,evt);
    TLorentzVector qV= p(2,evt)-p(3,evt);
    TLorentzVector pT= pV+p(1,evt); 
    TLorentzVector qT= pV-p(1,evt); 
    TLorentzVector qB= pT-p(0,evt); 
    TLorentzVector pB= pT+p(0,evt); 
    
    ZTspin1 ZB(qB,pB,pB.M());
    ZTspin1 ZV(qV,pV,MV);
    ZTspin1 ZT(qT, pT, MT);
    //ZTspin2 ZT_2(qT, pT, MT);
    SpinSumT DT(pT,MT);
    SpinSumV PT(pT,MT);

    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj(); 
    
    //TLorentzVector tmp = ZT.Contract(ZV);
    //complex<double> sfm(e.Re().Dot(tmp),e.Im().Dot(tmp));  
    
    //complex<double> sfm(DT.Sandwich(e.Re(),qT,qT,ZV),DT.Sandwich(e.Im(),qT,qT,ZV));
    
    TLorentzVector tmp = PT.Dot(ZV);
    if(tmp==TLorentzVector(0,0,0,0))return 0.;
    complex<double> sfm(DT.Sandwich(e.Re(),ZB,ZT,tmp),DT.Sandwich(e.Im(),ZB,ZT,tmp));
    
    return sfm/(GeV*GeV*GeV);
}
void SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P"
    << "\n\t e_c.Dot(ZT.Contract(ZV))"
    << "\n\t /GeV^3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}

//SF 8
complex<double> SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MT    = mRes(PT,evt);
    double MV    = mRes(V,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    
    TLorentzVector pV= p(2,evt)+p(3,evt);
    TLorentzVector qV= p(2,evt)-p(3,evt);
    TLorentzVector pT= pV+p(1,evt); 
    TLorentzVector qT= pV-p(1,evt); 
    TLorentzVector pB= pT+p(0,evt);
    TLorentzVector qB= pT-p(0,evt);
    
    ZTspin1 ZV(qV,pV,MV);
    ZTspin2 ZT(qT, pT, MT);
    
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj(); 
    
    TLorentzVector tmp = ZT.Contract(ZV);
    if(tmp==TLorentzVector(0,0,0,0))return 0.;

    complex<double> sfm(LeviCivita(e.Re(),tmp,pB,qB),LeviCivita(e.Im(),tmp,pB,qB));
    //if(getPolarisation()==0)sfm=complex<double>(LeviCivita(e.Re(),tmp,pB,qB),0.);
    //else sfm=complex<double>(LeviCivita(e.Re(),tmp,pB,qB),LeviCivita(e.Im(),tmp,pB,qB));
    
    return sfm/(GeV*GeV*GeV*GeV*GeV);
}
void SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D"
    << "\n\t LeviCivita(e_c,ZT.Contract(ZV),pB,qB)"
    << "\n\t /GeV^5"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//=========================================================
//SF 9

bool SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "PT") PT = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
    }
    if(0==PT || 0==fsPS[0]){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3::parseTree"
        << " Didn't find PT or V0 " << PT.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    for(int i=0; i< PT->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= PT->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "S") S = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==S || 0==fsPS[1]){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3::parseTree"
        << " Didn't find S or P1 " << S.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    if(S->nDgtr() != 2){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3::parseTree"
        << " S should have 2 daughters, but it says it has "
        << S->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = S->getDgtrTreePtr(0);
    fsPS[3] = S->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: S:\n"
        << *S
        << "\n PT:"
        << *PT 
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

const DecayTree& SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P::getExampleDecay(){
    if(0==_exampleDecayP){
        // B->J/psi K2(1770); K2->K f0(980); f0->pi pi  
        _exampleDecayP = new DecayTree(521);
        _exampleDecayP->addDgtr(443, 10325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    }
    return *_exampleDecayP;
}

const DecayTree& SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MT    = mRes(PT,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    
    TLorentzVector pT= p(2,evt)+p(3,evt)+p(1,evt); 
    TLorentzVector qT= p(2,evt)+p(3,evt)-p(1,evt); 
    TLorentzVector qB= pT-p(0,evt);
    TLorentzVector pB= pT+p(0,evt);
    
    //ZTspin2 ZT(qT, pT, MT);
    SpinSumT DT(pT,MT);
    ZTspin1 ZB(qB, pB, pB.M());

    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj(); 
    
    //TLorentzVector tmp = ZT.Contract(qB);
    //  complex<double> sfm(e.Re().Dot(tmp),e.Im().Dot(tmp));  
    
    complex<double> sfm(DT.Sandwich(e.Re(),ZB,qT,qT),DT.Sandwich(e.Im(),ZB,qT,qT));  
    return sfm/(GeV*GeV*GeV);
}
void SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P"
    << "\n\t e_c.Dot(ZT.Contract(qB))"
    << "\n\t /GeV^3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//=========================================================
//SF 10

bool SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "PT") PT = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
    }
    if(0==PT || 0==fsPS[0]){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3::parseTree"
        << " Didn't find PT or V0 " << PT.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    for(int i=0; i< PT->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= PT->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "T") T = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==T || 0==fsPS[1]){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3::parseTree"
        << " Didn't find T or P1 " << T.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    if(T->nDgtr() != 2){
        cout << "ERROR in SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3::parseTree"
        << " T should have 2 daughters, but it says it has "
        << T->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = T->getDgtrTreePtr(0);
    fsPS[3] = T->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: T:\n"
        << *T
        << "\n PT:"
        << *PT 
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

const DecayTree& SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P::getExampleDecay(){
    if(0==_exampleDecayP){
        // B->J/psi K2(1770); K2->K f2(1270); f2->pi pi  
        _exampleDecayP = new DecayTree(521);
        _exampleDecayP->addDgtr(443, 10325)->addDgtr(321, 225)->addDgtr(211,-211);
    }
    return *_exampleDecayP;
}

const DecayTree& SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MPT    = mRes(PT,evt);
    double MT    = mRes(T,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    TLorentzVector pT= p(2,evt)+p(3,evt); 
    TLorentzVector qT= p(2,evt)-p(3,evt); 
    TLorentzVector pPT= pT+p(1,evt); 
    TLorentzVector qPT= pT-p(1,evt); 
    TLorentzVector pB= pPT+p(0,evt); 
    TLorentzVector qB= pPT-p(0,evt); 
    
    SpinSumT D_PT(pPT,MPT);
    ZTspin1 Z_1T(qT,pT,MT);
    ZTspin1 Z_B(qB,pB,pB.M());

    // SpinSumV P_PT(pPT,MPT);
    //ZTgperp P_PT_matrix(pPT,MPT);
    //ZTspin2 ZT(qT, pT, MT);
    
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj(); 
    
    //TLorentzVector tmp= P_PT.Dot(qB);
    //TLorentzVector tmp2= ZT.Contract(tmp);
    //double tmp3= (P_PT_matrix.Contract_2(ZT))/(GeV*GeV);
    //complex<double> sf(P_PT.Sandwich(e.Re(),tmp2)-1./3.*(P_PT.Sandwich(e.Re(),qB)*tmp3),P_PT.Sandwich(e.Im(),tmp2)-1./3.*(P_PT.Sandwich(e.Im(),qB)*tmp3));
    
    complex<double> sf(D_PT.Sandwich(e.Re(),Z_B,Z_1T,Z_1T)+1./3.*Z_1T.Mag2()/MT/MT * D_PT.Sandwich(e.Re(),Z_B,pT,pT), D_PT.Sandwich(e.Im(),Z_B,Z_1T,Z_1T)+1./3.*Z_1T.Mag2()/MT/MT * D_PT.Sandwich(e.Im(),Z_B,pT,pT) );
    return sf/(GeV*GeV*GeV);
}
void SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P"
    << "\n\t e_c.Dot(ZT.Contract(qB))"
    << "\n\t /GeV^3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}




//=========================================================
//SF 11

bool SF_BtoTV0_TtoVP1_VtoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "T") T = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
    }
    if(0==T || 0==fsPS[0]){
        cout << "ERROR in SF_BtoTV0_TtoVP1_VtoP2P3::parseTree"
        << " Didn't find T or V0 " << T.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    for(int i=0; i< T->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= T->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") V = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==V || 0==fsPS[1]){
        cout << "ERROR in SF_BtoTV0_TtoVP1_VtoP2P3::parseTree"
        << " Didn't find V or P1 " << V.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    if(V->nDgtr() != 2){
        cout << "ERROR in SF_SF_BtoTV0_TtoVP1_VtoP2P3::parseTree"
        << " V should have 2 daughters, but it says it has "
        << V->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = V->getDgtrTreePtr(0);
    fsPS[3] = V->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: V:\n"
        << *V
        << "\n T:"
        << *T 
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

const DecayTree& SF_BtoTV0_TtoVP1_VtoP2P3_P::getExampleDecay(){
    if(0==_exampleDecayP){
        // B->J/psi K2*(1430); K2->K*(892) pi; K*->K pi  
        _exampleDecayP = new DecayTree(521);
        _exampleDecayP->addDgtr(443, 325)->addDgtr(211, 313)->addDgtr(321,-211);
    }
    return *_exampleDecayP;
}

const DecayTree& SF_BtoTV0_TtoVP1_VtoP2P3_P::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoTV0_TtoVP1_VtoP2P3_P::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MT    = mRes(T,evt);
    double MV    = mRes(V,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    
    TLorentzVector pV= p(2,evt)+p(3,evt);
    TLorentzVector qV= p(2,evt)-p(3,evt);
    TLorentzVector pT= pV+p(1,evt); 
    TLorentzVector qT= pV-p(1,evt); 
    TLorentzVector pB= pT+p(0,evt);
    TLorentzVector qB= pT-p(0,evt);

    ZTspin1 ZB(qB,pB,pB.M());
    ZTspin1 ZV(qV,pV,MV);
    ZTspin1 Z_1T(qT,pT,MT);
    SpinSumV PT(pT,MT);
    //ZTspin2 ZT(qT, pT, MT);
    
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj(); 
    
    complex<double> sfm;
    
    TLorentzVector tmp0 = PT.Dot(ZV);
    if(tmp0==TLorentzVector(0,0,0,0))return 0.;
    
    TLorentzVector tmp1 = PT.Dot(ZB);
    TLorentzVector tmp2_Re = Z_1T * Z_1T.Dot(e.Re()) + 1./3. * Z_1T.Mag2() * PT.Dot(e.Re());
    TLorentzVector tmp2_Im = Z_1T * Z_1T.Dot(e.Im()) + 1./3. * Z_1T.Mag2() * PT.Dot(e.Im());
    
    TLorentzVector tmp3_Re = PT.Dot(e.Re());
    TLorentzVector tmp3_Im = PT.Dot(e.Im());
    TLorentzVector tmp4 = Z_1T * Z_1T.Dot(ZB) + 1./3. * Z_1T.Mag2() * PT.Dot(ZB);
    
    
    if(getPolarisation()==0)sfm=complex<double>(LeviCivita(pT,tmp0,tmp1,tmp2_Re)+LeviCivita(pT,tmp0,tmp4,tmp3_Re),0.);
    else sfm=complex<double>(LeviCivita(pT,tmp0,tmp1,tmp2_Re)+LeviCivita(pT,tmp0,tmp4,tmp3_Re),LeviCivita(pT,tmp0,tmp1,tmp2_Im)+LeviCivita(pT,tmp0,tmp4,tmp3_Im)); 
    
    //TLorentzVector tmp_Re = Z_1T * (Z_1T.Contract(e.Re())) - 1./3. * Z_1T.Mag2() * PT.Dot(e.Re()) ;
    //TLorentzVector tmp_Im = Z_1T * (Z_1T.Contract(e.Im())) - 1./3. * Z_1T.Mag2() * PT.Dot(e.Im()) ;
    //complex<double> sfm;
    //if(getPolarisation()==0)sfm=complex<double>(LeviCivita(tmp_Re,ZV,pB,qB),0.);
    //else sfm=complex<double>(LeviCivita(tmp_Re,ZV,pB,qB),LeviCivita(tmp_Im,ZV,pB,qB)); 
    
    //if(getPolarisation()==0)sfm=complex<double>(LeviCivita(ZT.Contract(e.Re()),ZV,pB,qB),0.);
    //else sfm=complex<double>(LeviCivita(ZT.Contract(e.Re()),ZV,pT,qT),LeviCivita(ZT.Contract(e.Im()),ZV,pT,qT));  

    
    return sfm/(GeV*GeV*GeV*GeV*GeV);
}
void SF_BtoTV0_TtoVP1_VtoP2P3_P::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoTV0_TtoVP1_VtoP2P3_P"
    << "\n\t LeviCivita(ZT.Contract(e_c),ZV,pB,qB)"
    << "\n\t /GeV^5"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}

//=========================================================
//SF 12/13

const DecayTree& SF_BtoVP1_VtoV0S_StoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->psi(2S) K; psi(2S)-> J/psi pi pi (non resonant)  
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(321, 100443)->addDgtr(443, 9981)->addDgtr(211,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoVP1_VtoV0S_StoP2P3::exampleDecay(){
    return getExampleDecay();
}

bool SF_BtoVP1_VtoV0S_StoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") V = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==V || 0==fsPS[1]){
        cout << "ERROR in SF_BtoVP1_VtoV0S_StoP2P3::parseTree"
        << " Didn't find V or P1 " << V.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    for(int i=0; i< V->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= V->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "S") S = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
    }
    if(0==S || 0==fsPS[0]){
        cout << "ERROR in SF_BtoVP1_VtoV0S_StoP2P3::parseTree"
        << " Didn't find S or V0  " << S.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    if(S->nDgtr() != 2){
        cout << "ERROR in SF_BtoVP1_VtoV0S_StoP2P3::parseTree"
        << " S should have 2 daughters, but it says it has "
        << S->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = S->getDgtrTreePtr(0);
    fsPS[3] = S->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: V:\n"
        << *V
        << "\n S:"
        << *S
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

complex<double> SF_BtoVP1_VtoV0S_StoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MV    = mRes(V,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    TLorentzVector pV= p(0,evt)+p(2,evt)+p(3,evt);
    TLorentzVector qV= p(2,evt)+p(3,evt)-p(0,evt);
    TLorentzVector qB= p(1,evt)-pV;
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj();   

    complex<double> sfm;
    double scale = (p(2,evt)+p(3,evt)).M2() - 4.*mRes(fsPS[3],evt)*mRes(fsPS[3],evt);
    //double scale = (p(2,evt)+p(3,evt)).M2() - 4.35*mRes(fsPS[3],evt)*mRes(fsPS[3],evt); ??

    if(is3bodyDecay()){
        complex<double> tmp(qB.Dot(e.Re())*scale,qB.Dot(e.Im())*scale);
        sfm=tmp/(GeV*GeV*GeV);
        //compley<double> tmp(scale,0.);
        //tmp=tmp/(GeV*GeV);
    }
    else{
        ZTspin1 Z(qV,pV,MV);
        complex<double> tmp(Z.Dot(e.Re()),Z.Dot(e.Im()));  
        sfm=tmp/(GeV);
    }

    return sfm;
}
void SF_BtoVP1_VtoV0S_StoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoVP1_VtoV0S_StoP2P3"
    << "\n\t Z.Dot(e_c)"
    << "\n\t /GeV"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}





//=========================================================
//SF 14

const DecayTree& SF_BtoV1P1_V1toV2V0_V2toP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->psi(2S) K; psi(2S)-> J/psi rho   
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(321, 100443)->addDgtr(443, 113)->addDgtr(211,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoV1P1_V1toV2V0_V2toP2P3::exampleDecay(){
    return getExampleDecay();
}

bool SF_BtoV1P1_V1toV2V0_V2toP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") V1 = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==V1 || 0==fsPS[1]){
        cout << "ERROR in SF_BtoV1P1_V1toV2V0_V2toP2P3::parseTree"
        << " Didn't find V1 or P1 " << V1.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    for(int i=0; i< V1->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= V1->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V" && !(dgtr->isFinalState())) V2 = dgtr;
        else if(dgtr->getVal().SVPAT() == "V" && dgtr->isFinalState()) fsPS[0] = dgtr;
    }
    if(0==V2 || 0==fsPS[0]){
        cout << "ERROR in SF_BtoV1P1_V1toV2V0_V2toP2P3::parseTree"
        << " Didn't find V2 or V0  " << V2.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    if(V2->nDgtr() != 2){
        cout << "ERROR in SF_BtoV1P1_V1toV2V0_V2toP2P3::parseTree"
        << " V2 should have 2 daughters, but it says it has "
        << V2->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = V2->getDgtrTreePtr(0);
    fsPS[3] = V2->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: V1:\n"
        << *V1
        << "\n V2:"
        << *V2
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

complex<double> SF_BtoV1P1_V1toV2V0_V2toP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MV1    = mRes(V1,evt);
    double MV2   = mRes(V2,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    
    TLorentzVector pV2= p(2,evt)+p(3,evt);
    TLorentzVector qV2= p(2,evt)-p(3,evt);
    TLorentzVector pV1= p(0,evt)+pV2;
    TLorentzVector qV1= pV2-p(0,evt);
    TLorentzVector qB= pV1-p(1,evt);
    TLorentzVector pB= pV1+p(1,evt);
    
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj();   

    ZTspin1 ZB(qB,pB,pB.M());
    ZTspin1 Z1(qV1,pV1,MV1);
    ZTspin1 Z2(qV2,pV2,MV2);

    if(getLS_S()==0){
        complex<double> sf(Z2.Dot(e.Re()),Z2.Dot(e.Im()));  
        return sf*Z1.Dot(ZB)/(GeV*GeV*GeV);
    }
    else if(getLS_S()==1){
        SpinSumV PV1(pV1,MV1);
        TLorentzVector tmp= PV1.Dot(ZB);
        complex<double> sf(tmp.Dot(Z2)*Z1.Dot(e.Re())-tmp.Dot(e.Re())*Z1.Dot(Z2),tmp.Dot(Z2)*Z1.Dot(e.Im())-tmp.Dot(e.Im())*Z1.Dot(Z2));  
        return sf*pV1.M2()/(GeV*GeV*GeV*GeV*GeV);
    }
    if(getLS_S()==2){
        SpinSumV PV1(pV1,MV1);
        SpinSumT DV1(pV1,MV1);
        TLorentzVector tmp= PV1.Dot(ZB);
        complex<double> sf(DV1.Sandwich(tmp,Z1,e.Re(),Z2),DV1.Sandwich(tmp,Z1,e.Im(),Z2));  
        return sf/(GeV*GeV*GeV);
    }
    return 1;
}
void SF_BtoV1P1_V1toV2V0_V2toP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoV1P1_V1toV2V0_V2toP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}



//=========================================================
//SF 15/16

const DecayTree& SF_BtoAP1_AtoVV0_VtoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->X K; X-> J/psi rho 
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(321, 9983)->addDgtr(443, 113)->addDgtr(211,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoAP1_AtoVV0_VtoP2P3::exampleDecay(){
    return getExampleDecay();
}

bool SF_BtoAP1_AtoVV0_VtoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "A") A = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==A || 0==fsPS[1]){
        cout << "ERROR in SF_BtoAP1_AtoVV0_VtoP2P3::parseTree"
        << " Didn't find A or P1 " << A.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    for(int i=0; i< A->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V" &&   !(dgtr->isFinalState())) V = dgtr;
        else if(dgtr->getVal().SVPAT() == "V" && dgtr->isFinalState()) fsPS[0] = dgtr;
    }
    if(0==V || 0==fsPS[0]){
        cout << "ERROR in SF_BtoAP1_AtoVV0_VtoP2P3::parseTree"
        << " Didn't find V or V0  " << V.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    if(V->nDgtr() != 2){
        cout << "ERROR in SF_BtoAP1_AtoVV0_VtoP2P3::parseTree"
        << " V should have 2 daughters, but it says it has "
        << V->nDgtr() << "."
        << endl;
        return false;
    }
    fsPS[2] = V->getDgtrTreePtr(0);
    fsPS[3] = V->getDgtrTreePtr(1);
    
    //normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: A:\n"
        << *A
        << "\n V:"
        << *V
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

complex<double> SF_BtoAP1_AtoVV0_VtoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MA    = mRes(A,evt);
    double MV    = mRes(V,evt);
    double MV0   = mRes(fsPS[0],evt); // ??  
    //double MV0   = p(0,evt).M(); // ??  
    
    TLorentzVector pV= p(2,evt)+p(3,evt);
    TLorentzVector qV= p(2,evt)-p(3,evt);
    TLorentzVector pA= p(0,evt)+pV;
    //TLorentzVector qA= pV-p(0,evt);
    TLorentzVector qB= pA-p(1,evt);
    TLorentzVector pB= pA+p(1,evt);
    polVector e = polVector(p(0,evt),MV0,getPolarisation());  
    e.conj();   
    
    complex<double> sfm;
    double scale = (p(2,evt)+p(3,evt)).M2() - 4.*mRes(fsPS[3],evt)*mRes(fsPS[3],evt);
    //double scale = (p(2,evt)+p(3,evt)).M2() - 4.35*mRes(fsPS[3],evt)*mRes(fsPS[3],evt); ??
    
    if(is3bodyDecay()){
        complex<double> tmp(qB.Dot(e.Re())*scale,qB.Dot(e.Im())*scale);
        sfm=tmp/(GeV*GeV*GeV);
        //compley<double> tmp(scale,0.);
        //sfm=tmp/(GeV*GeV);
        //sfm=tmp*scale/(GeV*GeV)
    }
    else{
        //ZTspin1 ZA(qA,pA,MA);
        ZTspin1 ZV(qV,pV,MV);
        SpinSumV PA(pA,MA);
        ZTspin1 ZB(qB,pB,pB.M());

        if(getPolarisation()==0)sfm=complex<double>(LeviCivita(e.Re(),PA.Dot(ZB),ZV,pB),0.);
        else sfm=complex<double>(LeviCivita(e.Re(),PA.Dot(ZB),ZV,pA),LeviCivita(e.Im(),PA.Dot(ZB),ZV,pA)); 
        //if(getPolarisation()==0)sfm=complex<double>(LeviCivita(e.Re(),ZA,ZV,pB),0.);
        //else sfm=complex<double>(LeviCivita(e.Re(),ZA,ZV,pA),LeviCivita(e.Im(),ZA,ZV,pA));  
        sfm=sfm/(GeV*GeV*GeV);
    }
    return sfm;
}
void SF_BtoAP1_AtoVV0_VtoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoAP1_AtoVV0_VtoP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}



//==================================================================
// SF 17
bool SF_BtoV1V2_V1toP1V0_V2toP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V" && (dgtr->getDgtrTreePtr(0)->getVal().SVPAT() == "V" ||dgtr->getDgtrTreePtr(1)->getVal().SVPAT() == "V" )) V1 = dgtr;
        else if(dgtr->getVal().SVPAT() == "V" && dgtr->getDgtrTreePtr(0)->getVal().SVPAT() == "P" && dgtr->getDgtrTreePtr(1)->getVal().SVPAT() == "P" ) V2 = dgtr;
    }
    if(0==V1 || 0==V2){
        cout << "ERROR in SF_BtoV1V2_V1toP1V0_V2toP2P3::parseTree"
        << " Didn't find V1 or V2 " << V1.get() << ", " << V2.get() << endl;
        return false;
    }
    
    if(V1->nDgtr() != 2){
        cout << "ERROR in SF_BtoV1V2_V1toP1V0_V2toP2P3::parseTree"
        << " V1 should have 2 daughters, but it says it has "
        << V1->nDgtr() << "."
        << endl;
        return false;
    }
    if(V2->nDgtr() != 2){
        cout << "ERROR in SF_BtoV1V2_V1toP1V0_V2toP2P3::parseTree"
        << " V2 should have 2 daughters, but it says it has "
        << V2->nDgtr() << "."
        << endl;
        return false;
    }
    
    for(int i=0; i< V1->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= V1->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    
    if(0==fsPS[0] || 0==fsPS[1]){
        cout << "ERROR in SF_BtoV1V2_V1toP1V0_V2toP2P3::parseTree"
        << " Didn't find V0 or P1 " << fsPS[0].get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    fsPS[2] = V2->getDgtrTreePtr(0);
    fsPS[3] = V2->getDgtrTreePtr(1);
    ////normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: V1:\n"
        << *V1
        << "\n A:"
        << *V2 
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

const DecayTree& SF_BtoV1V2_V1toP1V0_V2toP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->Z(4430) K*; Z->J/psi pi; K*->K pi  
        _exampleDecay = new DecayTree(521);
        DecayTree Ks(313);
        Ks.addDgtr(321,-211);
        DecayTree Z(9993);
        Z.addDgtr(443,211);
        _exampleDecay->addDgtr(&Z);
        _exampleDecay->addDgtr(&Ks);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoV1V2_V1toP1V0_V2toP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoV1V2_V1toP1V0_V2toP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MV1    = mRes(V1, evt);
    double MV2    = mRes(V2, evt);
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    
    TLorentzVector pV2= p(2, evt)+p(3, evt);
    TLorentzVector qV2= p(2, evt)-p(3, evt);
    TLorentzVector pV1= p(1, evt)+p(0, evt);
    TLorentzVector qV1= p(1, evt)-p(0, evt);
    //TLorentzVector pB= pV1+pV2;

    ZTspin1 ZV1(qV1,pV1,MV1);
    ZTspin1 ZV2(qV2,pV2,MV2);
    SpinSumV PV1(pV1,MV1);
    
    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
    TLorentzVector tmp= PV1.Dot(ZV2);
    //complex<double> sfm;
    //if(getPolarisation()==0)sfm=complex<double>(LeviCivita(e.Re(),tmp,qV1,pB),0.);
    //else sfm=complex<double>(LeviCivita(e.Re(),tmp,qV1,pB),LeviCivita(e.Im(),tmp,qV1,pB));  
        
    complex<double> sfm;
    if(getPolarisation()==0)sfm=complex<double>(LeviCivita(tmp,ZV1,pV1,PV1.Dot(e.Re())),0.);
    else sfm=complex<double>(LeviCivita(tmp,ZV1,pV1,PV1.Dot(e.Re())),LeviCivita(tmp,ZV1,pV1,PV1.Dot(e.Im())));  
    return sfm/(GeV*GeV*GeV);
}
void SF_BtoV1V2_V1toP1V0_V2toP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoV1V2_V1toP1V0_V2toP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}



//==================================================================
// SF 18
bool SF_BtoAV_AtoP1V0_VtoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "A") A = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") V = dgtr;
    }
    if(0==A || 0==V){
        cout << "ERROR in SF_BtoAV_AtoP1V0_VtoP2P3::parseTree"
        << " Didn't find A or V " << A.get() << ", " << V.get() << endl;
        return false;
    }
    
    if(V->nDgtr() != 2){
        cout << "ERROR in SF_BtoAV_AtoP1V0_VtoP2P3::parseTree"
        << " V should have 2 daughters, but it says it has "
        << V->nDgtr() << "."
        << endl;
        return false;
    }
    if(A->nDgtr() != 2){
        cout << "ERROR in SF_BtoAV_AtoP1V0_VtoP2P3::parseTree"
        << " A should have 2 daughters, but it says it has "
        << A->nDgtr() << "."
        << endl;
        return false;
    }
    
    for(int i=0; i< A->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    
    if(0==fsPS[0] || 0==fsPS[1]){
        cout << "ERROR in SF_BtoAV_AtoP1V0_VtoP2P3::parseTree"
        << " Didn't find V0 or P1 " << fsPS[0].get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    fsPS[2] = V->getDgtrTreePtr(0);
    fsPS[3] = V->getDgtrTreePtr(1);
    ////normalOrder(fsPS[2], fsPS[3]);
    
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


const DecayTree& SF_BtoAV_AtoP1V0_VtoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->Z(4430) K*; Z->J/psi pi; K*->K pi  
        _exampleDecay = new DecayTree(521);
        DecayTree Ks(313);
        Ks.addDgtr(321,-211);
        DecayTree Z(9983);
        Z.addDgtr(443,211);
        _exampleDecay->addDgtr(&Ks);
        _exampleDecay->addDgtr(&Z);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoAV_AtoP1V0_VtoP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoAV_AtoP1V0_VtoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MA    = mRes(A, evt);
    double MV    = mRes(V, evt);
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    
    TLorentzVector pV= p(2, evt)+p(3, evt);
    TLorentzVector qV= p(2, evt)-p(3, evt);
    TLorentzVector pA= p(0, evt)+p(1, evt); 
    
    ZTspin1 Z(qV,pV,MV);
    SpinSumV P(pA,MA);
    
    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
    complex<double> sfm(P.Sandwich(e.Re(),Z),P.Sandwich(e.Im(),Z));  

    return sfm/GeV;
}
void SF_BtoAV_AtoP1V0_VtoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoAV_AtoP1V0_VtoP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//==================================================================
// SF 19
bool SF_BtoAS_AtoP1V0_StoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "A") A = dgtr;
        else if(dgtr->getVal().SVPAT() == "S") S = dgtr;
    }
    if(0==A || 0==S){
        cout << "ERROR in SF_BtoAS_AtoP1V0_StoP2P3::parseTree"
        << " Didn't find A or S " << A.get() << ", " << S.get() << endl;
        return false;
    }
    
    if(A->nDgtr() != 2){
        cout << "ERROR in SF_BtoAS_AtoP1V0_StoP2P3::parseTree"
        << " A should have 2 daughters, but it says it has "
        << A->nDgtr() << "."
        << endl;
        return false;
    }
    if(S->nDgtr() != 2){
        cout << "ERROR in SF_BtoAS_AtoP1V0_StoP2P3::parseTree"
        << " S should have 2 daughters, but it says it has "
        << S->nDgtr() << "."
        << endl;
        return false;
    }
    
    for(int i=0; i< A->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    
    if(0==fsPS[0] || 0==fsPS[1]){
        cout << "ERROR in SF_BtoAS_AtoP1V0_StoP2P3::parseTree"
        << " Didn't find V0 or P1 " << fsPS[0].get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    fsPS[2] = S->getDgtrTreePtr(0);
    fsPS[3] = S->getDgtrTreePtr(1);
    ////normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: A:\n"
        << *A
        << "\n S:"
        << *S 
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


const DecayTree& SF_BtoAS_AtoP1V0_StoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->Z(4430) (K pi)_S
        _exampleDecay = new DecayTree(521);
        DecayTree K(9981);
        K.addDgtr(321,-211);
        DecayTree Z(9983);
        Z.addDgtr(443,211);
        _exampleDecay->addDgtr(&K);
        _exampleDecay->addDgtr(&Z);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoAS_AtoP1V0_StoP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoAS_AtoP1V0_StoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MA    = mRes(A, evt);
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    TLorentzVector pA= p(0, evt)+p(1, evt); 
    TLorentzVector qA= p(1, evt)-p(0, evt); 
    ZTspin1 Z(qA,pA,MA);
    
    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
    complex<double> sfm(Z.Dot(e.Re()),Z.Dot(e.Im()));  
    
    return sfm/GeV;
}
void SF_BtoAS_AtoP1V0_StoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoAS_AtoP1V0_StoP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//==================================================================
// SF 20
bool SF_BtoPS_PtoP1V0_StoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "P") P = dgtr;
        else if(dgtr->getVal().SVPAT() == "S") S = dgtr;
    }
    if(0==P || 0==S){
        cout << "ERROR in SF_BtoPS_PtoP1V0_StoP2P3::parseTree"
        << " Didn't find P or S " << P.get() << ", " << S.get() << endl;
        return false;
    }
    
    if(P->nDgtr() != 2){
        cout << "ERROR in SF_BtoPS_PtoP1V0_StoP2P3::parseTree"
        << " P should have 2 daughters, but it says it has "
        << P->nDgtr() << "."
        << endl;
        return false;
    }
    if(S->nDgtr() != 2){
        cout << "ERROR in SF_BtoPS_PtoP1V0_StoP2P3::parseTree"
        << " S should have 2 daughters, but it says it has "
        << S->nDgtr() << "."
        << endl;
        return false;
    }
    
    for(int i=0; i< P->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= P->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    
    if(0==fsPS[0] || 0==fsPS[1]){
        cout << "ERROR in SF_BtoPS_PtoP1V0_StoP2P3::parseTree"
        << " Didn't find V0 or P1 " << fsPS[0].get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    fsPS[2] = S->getDgtrTreePtr(0);
    fsPS[3] = S->getDgtrTreePtr(1);
    ////normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: P:\n"
        << *P
        << "\n S:"
        << *S 
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


const DecayTree& SF_BtoPS_PtoP1V0_StoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B-> (J/psi pi)_P (K pi)_S
        _exampleDecay = new DecayTree(521);
        DecayTree S(9981);
        S.addDgtr(321,-211);
        DecayTree P(9991);
        P.addDgtr(443,221);
        _exampleDecay->addDgtr(&P);
        _exampleDecay->addDgtr(&S);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoPS_PtoP1V0_StoP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoPS_PtoP1V0_StoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double M    = mRes(P, evt);
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    TLorentzVector pP= p(0, evt)+p(1, evt); 
    TLorentzVector qP= p(1, evt)-p(0, evt); 
    ZTspin1 Z(qP,pP,M);
    
    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
    complex<double> sfm(Z.Dot(e.Re()),Z.Dot(e.Im()));  
    
    return sfm/GeV;
}
void SF_BtoPS_PtoP1V0_StoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoPS_PtoP1V0_StoP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//==================================================================
// SF 21
bool SF_BtoPV_PtoP1V0_VtoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "P") P = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") V = dgtr;
    }
    if(0==P || 0==V){
        cout << "ERROR in SF_BtoPV_PtoP1V0_VtoP2P3::parseTree"
        << " Didn't find P or S " << P.get() << ", " << V.get() << endl;
        return false;
    }
    
    if(P->nDgtr() != 2){
        cout << "ERROR in SF_BtoPV_PtoP1V0_VtoP2P3::parseTree"
        << " P should have 2 daughters, but it says it has "
        << P->nDgtr() << "."
        << endl;
        return false;
    }
    if(V->nDgtr() != 2){
        cout << "ERROR in SF_BtoPV_PtoP1V0_VtoP2P3::parseTree"
        << " V should have 2 daughters, but it says it has "
        << V->nDgtr() << "."
        << endl;
        return false;
    }
    
    for(int i=0; i< P->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= P->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    
    if(0==fsPS[0] || 0==fsPS[1]){
        cout << "ERROR in SF_BtoPV_PtoP1V0_VtoP2P3::parseTree"
        << " Didn't find V0 or P1 " << fsPS[0].get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    fsPS[2] = V->getDgtrTreePtr(0);
    fsPS[3] = V->getDgtrTreePtr(1);
    ////normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: P:\n"
        << *P
        << "\n V:"
        << *V 
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


const DecayTree& SF_BtoPV_PtoP1V0_VtoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B-> Z(4239) K*
        _exampleDecay = new DecayTree(521);
        DecayTree V(313);
        V.addDgtr(321,-211);
        DecayTree P(9999996);
        P.addDgtr(443,221);
        _exampleDecay->addDgtr(&P);
        _exampleDecay->addDgtr(&V);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoPV_PtoP1V0_VtoP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoPV_PtoP1V0_VtoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MP    = mRes(P, evt);
    double MV    = mRes(V, evt);
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    TLorentzVector pP= p(1, evt)+p(0, evt); 
    TLorentzVector qP= p(1, evt)-p(0, evt); 
    TLorentzVector pV= p(2, evt)+p(3, evt); 
    TLorentzVector qV= p(2, evt)-p(3, evt); 
    TLorentzVector pB= pP+pV; 
    TLorentzVector qB= pP-pV; 

    ZTspin1 ZB(qB,pB,pB.M());
    ZTspin1 ZV(qV,pV,MV);
    ZTspin1 ZP(qP,pP,MP);

    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
    double tmp= ZB.Dot(ZV);
    complex<double> sf(ZP.Dot(e.Re()),ZP.Dot(e.Im()));
    return sf*tmp/(GeV*GeV*GeV);
    
    //complex<double> sfm(pP.Dot(e.Re()),pP.Dot(e.Im()));  
    //return sfm*ZV.Dot(qB)/(GeV*GeV*GeV);
}
void SF_BtoPV_PtoP1V0_VtoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoPV_PtoP1V0_VtoP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}



//==================================================================
// SF 22
bool SF_BtoPV0_PtoS_StoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "P") P = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;;
    }
    if(0==P || 0==fsPS[0]){
        cout << "ERROR in SF_BtoPV0_PtoS_StoP2P3::parseTree"
        << " Didn't find P or V0 " << P.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    
    if(P->nDgtr() != 2){
        cout << "ERROR in SF_BtoPV0_PtoS_StoP2P3::parseTree"
        << " P should have 2 daughters, but it says it has "
        << P->nDgtr() << "."
        << endl;
        return false;
    }
   
    for(int i=0; i< P->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= P->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "S") S = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    
    if(S->nDgtr() != 2){
        cout << "ERROR in SF_BtoPV0_PtoS_StoP2P3::parseTree"
        << " S should have 2 daughters, but it says it has "
        << S->nDgtr() << "."
        << endl;
        return false;
    }
    
    fsPS[2] = S->getDgtrTreePtr(0);
    fsPS[3] = S->getDgtrTreePtr(1);
    ////normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: P:\n"
        << *P
        << "\n S:"
        << *S
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


const DecayTree& SF_BtoPV0_PtoS_StoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->J/psi (K pi pi)_S
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(443, 9991)->addDgtr(321,9981)->addDgtr(211,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoPV0_PtoS_StoP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoPV0_PtoS_StoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    TLorentzVector pP= p(1, evt)+p(2, evt)+p(3,evt); 
    TLorentzVector pB=pP+p(0,evt);
    TLorentzVector qB=pP-p(0,evt);    
    
    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
//    complex<double> sfm(pP.Dot(e.Re()),pP.Dot(e.Im()));  
//    SpinSumV P(pP,pP.M());
    ZTspin1 ZB(qB,pB,pB.M());
    
    complex<double> sfm(ZB.Dot(e.Re()),ZB.Dot(e.Im()));
    return sfm/(GeV);
}
void SF_BtoPV0_PtoS_StoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoPV0_PtoS_StoP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}

//

//==================================================================
// SF 23

bool SF_BtoVP1_VtoAP2_AtoV0P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") V = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==V || 0==fsPS[1]){
        cout << "ERROR in SF_BtoVP1_VtoAP2_AtoV0P3::parseTree"
        << " Didn't find V or P1 " << V.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    for(int i=0; i< V->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= V->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "A") A = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[2] = dgtr;
    }
    if(0==A || 0==fsPS[2]){
        cout << "ERROR in SF_BtoVP1_VtoAP2_AtoV0P3::parseTree"
        << " Didn't find A or P2 " << A.get() << ", " << fsPS[2].get() << endl;
        return false;
    }
    
    if(A->nDgtr() != 2){
        cout << "ERROR in SF_BtoVP1_VtoAP2_AtoV0P3::parseTree"
        << " A should have 2 daughters, but it says it has "
        << A->nDgtr() << "."
        << endl;
        return false;
    }
    
    for(int i=0; i< A->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[3] = dgtr;
    }
    
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

const DecayTree& SF_BtoVP1_VtoAP2_AtoV0P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->K X(4260); X->Z pi; Z->J/psi pi  
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(321, 9999992)->addDgtr(211, 9999993)->addDgtr(443,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoVP1_VtoAP2_AtoV0P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoVP1_VtoAP2_AtoV0P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MA    = mRes(A, evt);
    double MV    = mRes(V, evt);
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    
    TLorentzVector pA= p(0,evt)+p(3, evt); 
    //TLorentzVector qA= p(0,evt)-p(3, evt); 
    TLorentzVector pV= pA+p(2, evt);
    //TLorentzVector qV= pA-p(2, evt);
    TLorentzVector pB= pV+p(1, evt); 
    TLorentzVector qB= pV-p(1, evt); 
    
    ZTspin1 ZB(qB,pB,pB.M());
    SpinSumV PA(pA,MA);
    SpinSumV PV(pV,MV);
    
    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
    complex<double> sfm(PV.Sandwich(PA.Dot(e.Re()),ZB),PV.Sandwich(PA.Dot(e.Im()),ZB));  

    return sfm/GeV;
}
void SF_BtoVP1_VtoAP2_AtoV0P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoVP1_VtoAP2_AtoV0P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//==================================================================
// SF 24

bool SF_BtoVP1_VtoSV0_StoP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") V = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    if(0==V || 0==fsPS[1]){
        cout << "ERROR in SF_BtoVP1_VtoSV0_StoP2P3::parseTree"
        << " Didn't find V or P1 " << V.get() << ", " << fsPS[1].get() << endl;
        return false;
    }
    
    for(int i=0; i< V->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= V->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "S") S = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;
    }
    if(0==S || 0==fsPS[0]){
        cout << "ERROR in SF_BtoVP1_VtoSV0_StoP2P3::parseTree"
        << " Didn't find S or V0 " << S.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    if(S->nDgtr() != 2){
        cout << "ERROR in SF_BtoVP1_VtoSV0_StoP2P3::parseTree"
        << " S should have 2 daughters, but it says it has "
        << S->nDgtr() << "."
        << endl;
        return false;
    }
    
    fsPS[2] = S->getDgtrTreePtr(0);
    fsPS[3] = S->getDgtrTreePtr(1);

    if(debugThis){
        cout << "parsed Tree: V:\n"
        << *V
        << "\n S:"
        << *S 
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

const DecayTree& SF_BtoVP1_VtoSV0_StoP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->K X(4260); X->f0 J/psi; f0->pi pi  
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(321, 9999992)->addDgtr(443, 9010221)->addDgtr(211,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoVP1_VtoSV0_StoP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoVP1_VtoSV0_StoP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MV    = mRes(V, evt);
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    

    TLorentzVector pV= p(0,evt)+p(2,evt)+p(3, evt);
    //TLorentzVector qV= pA-p(2, evt);
    TLorentzVector pB= pV+p(1, evt); 
    TLorentzVector qB= pV-p(1, evt); 
    
    ZTspin1 ZB(qB,pB,pB.M());
    SpinSumV PV(pV,MV);
    
    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
    complex<double> sfm(PV.Sandwich(e.Re(),ZB),PV.Sandwich(e.Im(),ZB));  
    
    return sfm/GeV;
}
void SF_BtoVP1_VtoSV0_StoP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoVP1_VtoSV0_StoP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}




//==================================================================
// SF 25
bool SF_BtoPV0_PtoP1V1_V1toP2P3_BASE::parseTree(const DalitzEventPattern& pat){
    bool debugThis=false;
    
    if(fsPS.size() < 4) fsPS.reserve(4);
    for(int i=0; i< theDecay(pat).nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= theDecay(pat).getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "P") P = dgtr;
        else if(dgtr->getVal().SVPAT() == "V") fsPS[0] = dgtr;;
    }
    if(0==P || 0==fsPS[0]){
        cout << "ERROR in SF_BtoPV0_PtoP1V1_V1toP2P3::parseTree"
        << " Didn't find P or V0 " << P.get() << ", " << fsPS[0].get() << endl;
        return false;
    }
    
    
    if(P->nDgtr() != 2){
        cout << "ERROR in SF_BtoPV0_PtoP1V1_V1toP2P3::parseTree"
        << " P should have 2 daughters, but it says it has "
        << P->nDgtr() << "."
        << endl;
        return false;
    }
    
    for(int i=0; i< P->nDgtr(); i++){
        const_counted_ptr<AssociatedDecayTree> dgtr= P->getDgtrTreePtr(i);
        if(dgtr->getVal().SVPAT() == "V") V = dgtr;
        else if(dgtr->getVal().SVPAT() == "P") fsPS[1] = dgtr;
    }
    
    if(V->nDgtr() != 2){
        cout << "ERROR in SF_BtoPV0_PtoP1V1_V1toP2P3::parseTree"
        << " V should have 2 daughters, but it says it has "
        << V->nDgtr() << "."
        << endl;
        return false;
    }
    
    fsPS[2] = V->getDgtrTreePtr(0);
    fsPS[3] = V->getDgtrTreePtr(1);
    ////normalOrder(fsPS[2], fsPS[3]);
    
    if(debugThis){
        cout << "parsed Tree: P:\n"
        << *P
        << "\n V:"
        << *V
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


const DecayTree& SF_BtoPV0_PtoP1V1_V1toP2P3::getExampleDecay(){
    if(0==_exampleDecay){
        // B->J/psi (K(1460)-> K rho)
        _exampleDecay = new DecayTree(521);
        _exampleDecay->addDgtr(443, 100321)->addDgtr(321,213)->addDgtr(211,-211);
    }
    return *_exampleDecay;
}

const DecayTree& SF_BtoPV0_PtoP1V1_V1toP2P3::exampleDecay(){
    return getExampleDecay();
}

complex<double> SF_BtoPV0_PtoP1V1_V1toP2P3::getNewVal(IDalitzEvent& evt){
    //bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree(evt.eventPattern());
    
    double MV0   = mRes(fsPS[0], evt); // ??  
    //double MV0   = p(0, evt).M(); // ??  
    TLorentzVector pP= p(1, evt)+p(2, evt)+p(3,evt); 
    TLorentzVector qP= p(1, evt)-p(2, evt)-p(3,evt); 
    ZTspin1 ZP(qP,pP,pP.M());
    
    TLorentzVector pB=pP+p(0,evt);
    TLorentzVector qB=pP-p(0,evt);    
    ZTspin1 ZB(qB,pB,pB.M());

    TLorentzVector pV= p(2, evt)+p(3,evt); 
    TLorentzVector qV= p(2, evt)-p(3,evt);    
    ZTspin1 ZV(qV,pV,pV.M());
    
    polVector e = polVector(p(0, evt),MV0,getPolarisation());  
    e.conj();   
    
    double tmp = ZP.Dot(ZV); 
    
    complex<double> sfm(ZB.Dot(e.Re()),ZB.Dot(e.Im()));
    return sfm*tmp/(GeV*GeV*GeV);
}
void SF_BtoPV0_PtoP1V1_V1toP2P3::printYourself(ostream& os) const{
    //  bool debugThis = false;
    
    if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
    os << "spin factor SF_BtoPV0_PtoP1V1_V1toP2P3"
    << "\n\t    parsed tree " << theBareDecay().oneLiner()
    << "\n      like this:" << endl;
    this->printParsing(os);
}


//=========================================================
//
//
