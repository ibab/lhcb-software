// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "Mint/SpinFactorMaker.h"
#include "Mint/Utils.h"
#include "Mint/DecayTree.h"
#include "Mint/DecayTreeComparisons.h"
#include "Mint/ISpinFactor.h"
#include "Mint/SpinFactor.h"
#include "Mint/SpinFactors.h"
#include <iostream>

using namespace std;
using namespace MINT;

void PrintAllSpinFactors(std::ostream& out){
  
  out << " All known 4-body spin factors with example decays:\n"
      << " (Not all examples are necessarily physical)\n"
      << " -------------------- " << std::endl;
    
    out << " SF_BtoAV0_AtoVP1_VtoP2P3:\n" 
    << SF_BtoAV0_AtoVP1_VtoP2P3::getExampleDecay()
    << " -------------------- " << std::endl;  
    
    out << " SF_BtoAV0_AtoVP1Dwave_VtoP2P3:\n" 
    << SF_BtoAV0_AtoVP1Dwave_VtoP2P3::getExampleDecay()
    << " -------------------- " << std::endl;  
    
    out << " SF_BtoAV0_AtoVP1_VtoP2P3_P:\n" 
    << SF_BtoAV0_AtoVP1_VtoP2P3_P::getExampleDecay()
    << " -------------------- " << std::endl;  
    
    out << " SF_BtoAV0_AtoVP1_VtoP2P3_D:\n" 
    << SF_BtoAV0_AtoVP1_VtoP2P3_D::getExampleDecay()
    << " -------------------- " << std::endl;  
    
    out << " SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P:\n" 
    << SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P::getExampleDecay()
    << " -------------------- " << std::endl;  
    
    out << " SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D:\n" 
    << SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D::getExampleDecay()
    << " -------------------- " << std::endl;  
    
    out << " SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P:\n" 
    << SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P::getExampleDecay()
    << " -------------------- " << std::endl; 
    
    out << " SF_BtoTV0_TtoVP1_VtoP2P3_P:\n" 
    << SF_BtoTV0_TtoVP1_VtoP2P3_P::getExampleDecay()
    << " -------------------- " << std::endl;
    
    out << " SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P:\n" 
    << SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P::getExampleDecay()
    << " -------------------- " << std::endl;
    
    out << " SF_BtoVP1_VtoV0S_StoP2P3:\n" 
    << SF_BtoVP1_VtoV0S_StoP2P3::getExampleDecay()
    << " -------------------- " << std::endl;
    
    out << " SF_BtoV1V0_V1toV2P1_V2toP2P3:\n" 
    << SF_BtoV1V0_V1toV2P1_V2toP2P3::getExampleDecay()
    << " -------------------- " << std::endl;  
    
    out << " SF_BtoPV0_PtoP1V1_V1toP2P3:\n" 
    << SF_BtoPV0_PtoP1V1_V1toP2P3::getExampleDecay()
    << " -------------------- " << std::endl;  
    
  out << " SF_DtoPP0_PtoVP1_VtoP2P3:\n" 
      << SF_DtoPP0_PtoVP1_VtoP2P3::getExampleDecay()
      << " -------------------- " << std::endl;
  
  out << " SF_DtoAP0_AtoVP1_VtoP2P3:\n"
      << SF_DtoAP0_AtoVP1_VtoP2P3::getExampleDecay()
      << " -------------------- " << std::endl;

  out << " SF_DtoAP0_AtoVP1Dwave_VtoP2P3:\n"
      << SF_DtoAP0_AtoVP1Dwave_VtoP2P3::getExampleDecay()
      << " -------------------- " << std::endl;

  out << " SF_DtoAP0_AtoSP1_StoP2P3:\n"
      << SF_DtoAP0_AtoSP1_StoP2P3::getExampleDecay()
      << " -------------------- " << std::endl;
  
  out << " SF_DtoV1V2_V1toP0P1_V1toP2P3_S:\n"
      << SF_DtoV1V2_V1toP0P1_V1toP2P3_S::getExampleDecay()
      << " (S-Wave)\n"
      << " -------------------- " << std::endl;

  out << " SF_DtoV1V2_V1toP0P1_V1toP2P3_D:\n"
      << SF_DtoV1V2_V1toP0P1_V1toP2P3_D::getExampleDecay()
      << " (D-Wave)\n"
      << " -------------------- " << std::endl;

  out << " SF_DtoVT_VtoP0P1_TtoP2P3_P:\n"
      << SF_DtoVT_VtoP0P1_TtoP2P3_P::getExampleDecay()
      << " (P-Wave)\n"
      << " -------------------- " << std::endl;

  out << " SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1:\n"
      << SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::getExampleDecay()
      << " (S-Wave)\n"
      << " -------------------- " << std::endl;

  out << " SF_DtoVS_VtoP0P1_StoP2P3:\n"
      << SF_DtoVS_VtoP0P1_StoP2P3::getExampleDecay()
      << " -------------------- " << std::endl;

  out << " SF_DtoVS_VtoP0P1_StoP2P3_nonResV:\n"
      << SF_DtoVS_VtoP0P1_StoP2P3_nonResV::getExampleDecay()
      << " -------------------- " << std::endl;

  out << " SF_DtoPP1P2_PtoP3P4 (\"artificial\" decay):\n"
      << SF_DtoPP1P2_PtoP3P4::getExampleDecay()
      << " -------------------- " << std::endl;

  out << " SF_DtoS1S2_S1toP1P2_S2toP3P4:\n"
      << SF_DtoS1S2_S1toP1P2_S2toP3P4::getExampleDecay()
      << " -------------------- " << std::endl;

}


ISpinFactor* SpinFactorMaker(const DecayTree& thisDcy
			     , char SPD_Wave
			     , const std::string& lopt
			     ){
  bool dbThis=false;

  ISpinFactor *sf;

  if(dbThis) cout << "SpinFactorMaker called with option " << lopt << endl;

  if(A_is_in_B("BgSpinZero", lopt)){
    if(dbThis) cout << "returning flat" << endl;
    return new SpinFactorTrivial;
  }

  int nFinalStates = thisDcy.finalState().size();
  if(nFinalStates < 3  || nFinalStates == thisDcy.nDgtr() ){
    // either 2 or less bodies, or non-resonant.
    sf = new SpinFactorTrivial;
  }else if( 3 == nFinalStates){
    if(A_is_in_B("GS", lopt)){
      if(dbThis) cout << "... returning GS spin factor" << endl;
      sf = new GSSpinFactor(thisDcy);
      if(dbThis) cout << " .. with name: " << sf->name() << endl;
    }else{
      sf = new SpinFactor3(thisDcy);
    }
  }else if( 4 == nFinalStates){
    sf = SpinFactorMaker4Body(thisDcy, SPD_Wave,lopt);
  }else{
    cout << " SpinFactorMaker: Don't know any spin"
	 << " factors for a final state with "
	 << nFinalStates << " particles." << endl;
    return 0;
  }

  if( 0 == sf ){
    cout << "ERROR in SpinFactorMaker! Couldn't find spin factor"
	 << "\n I will crash now!" << endl;
    throw "no spin factor";
  }
  return sf;
}

ISpinFactor* SpinFactorMaker4Body(const DecayTree& thisDcy, char SPD_Wave, const std::string& lopt){
    bool dbThis=false;

    if(dbThis){
        std::cout << "SpinFactorMaker4Body called for"
	      << thisDcy
	      << std::endl;
    }
   
    // final state vector
    if( equalBySVPAT_L(SF_BtoAV0_AtoVP1_VtoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3(thisDcy,-1);  
        else return new SF_BtoAV0_AtoVP1_VtoP2P3(thisDcy,-999);  
    }
  
    if( equalBySVPAT_L(SF_BtoAV0_AtoVP1Dwave_VtoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoAV0_AtoVP1Dwave_VtoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoAV0_AtoVP1Dwave_VtoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoAV0_AtoVP1Dwave_VtoP2P3(thisDcy,-1);  
        else return new SF_BtoAV0_AtoVP1Dwave_VtoP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoAV0_AtoVP1_VtoP2P3_P::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3_P(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3_P(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3_P(thisDcy,-1);  
        else return new SF_BtoAV0_AtoVP1_VtoP2P3_P(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoAV0_AtoVP1_VtoP2P3_D::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3_D(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3_D(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoAV0_AtoVP1_VtoP2P3_D(thisDcy,-1);  
        else return new SF_BtoAV0_AtoVP1_VtoP2P3_D(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P(thisDcy,-1);  
        else return new SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_P(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D(thisDcy,-1);  
        else return new SF_BtoPseudoTV0_PseudoTtoVP1_VtoP2P3_D(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P(thisDcy,-1);  
        else return new SF_BtoPseudoTV0_PseudoTtoSP1_StoP2P3_P(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoAV0_AtoSP1_StoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoAV0_AtoSP1_StoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoAV0_AtoSP1_StoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoAV0_AtoSP1_StoP2P3(thisDcy,-1);  
        else return new SF_BtoAV0_AtoSP1_StoP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoTV0_TtoVP1_VtoP2P3_P::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoTV0_TtoVP1_VtoP2P3_P(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoTV0_TtoVP1_VtoP2P3_P(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoTV0_TtoVP1_VtoP2P3_P(thisDcy,-1);  
        else return new SF_BtoTV0_TtoVP1_VtoP2P3_P(thisDcy,-999);
    }
    
    
    if( equalBySVPAT_L(SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P(thisDcy,-1);  
        else return new SF_BtoPseudoTV0_PseudoTtoTP1_TtoP2P3_P(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoVP1_VtoV0S_StoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoVP1_VtoV0S_StoP2P3(thisDcy,1,false);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoVP1_VtoV0S_StoP2P3(thisDcy,0,false);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoVP1_VtoV0S_StoP2P3(thisDcy,-1,false);  
        else if(A_is_in_B("3Body", lopt)) return new SF_BtoVP1_VtoV0S_StoP2P3(thisDcy,-999,true);
        else return new SF_BtoVP1_VtoV0S_StoP2P3(thisDcy,-999,false);
    }
    
    if( equalBySVPAT_L(SF_BtoV1V0_V1toV2P1_V2toP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" 
            << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoV1V0_V1toV2P1_V2toP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoV1V0_V1toV2P1_V2toP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoV1V0_V1toV2P1_V2toP2P3(thisDcy,-1);  
        else return new SF_BtoV1V0_V1toV2P1_V2toP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoV1P1_V1toV2V0_V2toP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoV1P1_V1toV2V0_V2toP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoV1P1_V1toV2V0_V2toP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoV1P1_V1toV2V0_V2toP2P3(thisDcy,-1);  
        else {
            int LS_s = 0;
            if(A_is_in_B("LS_11", lopt))LS_s=1;
            if(A_is_in_B("LS_12", lopt))LS_s=2;
            return new SF_BtoV1P1_V1toV2V0_V2toP2P3(thisDcy,-999,LS_s);
        }
    }
    
    if( equalBySVPAT_L(SF_BtoAP1_AtoVV0_VtoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoAP1_AtoVV0_VtoP2P3(thisDcy,1,false);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoAP1_AtoVV0_VtoP2P3(thisDcy,0,false);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoAP1_AtoVV0_VtoP2P3(thisDcy,-1,false);  
        else if(A_is_in_B("3Body", lopt)) return new SF_BtoAP1_AtoVV0_VtoP2P3(thisDcy,-999,true);
        else return new SF_BtoAP1_AtoVV0_VtoP2P3(thisDcy,-999,false);
    }
    
    if( equalBySVPAT_L(SF_BtoV1V2_V1toP1V0_V2toP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoV1V2_V1toP1V0_V2toP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoV1V2_V1toP1V0_V2toP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoV1V2_V1toP1V0_V2toP2P3(thisDcy,-1);  
        else return new SF_BtoV1V2_V1toP1V0_V2toP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoAV_AtoP1V0_VtoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoAV_AtoP1V0_VtoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoAV_AtoP1V0_VtoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoAV_AtoP1V0_VtoP2P3(thisDcy,-1);  
        else return new SF_BtoAV_AtoP1V0_VtoP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoAS_AtoP1V0_StoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoAS_AtoP1V0_StoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoAS_AtoP1V0_StoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoAS_AtoP1V0_StoP2P3(thisDcy,-1);  
        else return new SF_BtoAS_AtoP1V0_StoP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoPS_PtoP1V0_StoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoPS_PtoP1V0_StoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoPS_PtoP1V0_StoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoPS_PtoP1V0_StoP2P3(thisDcy,-1);  
        else return new SF_BtoPS_PtoP1V0_StoP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoPV_PtoP1V0_VtoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoPV_PtoP1V0_VtoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoPV_PtoP1V0_VtoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoPV_PtoP1V0_VtoP2P3(thisDcy,-1);  
        else return new SF_BtoPV_PtoP1V0_VtoP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoPV0_PtoS_StoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoPV0_PtoS_StoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoPV0_PtoS_StoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoPV0_PtoS_StoP2P3(thisDcy,-1);  
        else return new SF_BtoPV0_PtoS_StoP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoVP1_VtoSV0_StoP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoVP1_VtoSV0_StoP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoVP1_VtoSV0_StoP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoVP1_VtoSV0_StoP2P3(thisDcy,-1);  
        else return new SF_BtoVP1_VtoSV0_StoP2P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoVP1_VtoAP2_AtoV0P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoVP1_VtoAP2_AtoV0P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoVP1_VtoAP2_AtoV0P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoVP1_VtoAP2_AtoV0P3(thisDcy,-1);  
        else return new SF_BtoVP1_VtoAP2_AtoV0P3(thisDcy,-999);
    }
    
    if( equalBySVPAT_L(SF_BtoPV0_PtoP1V1_V1toP2P3::getExampleDecay(), thisDcy) ){
        if(dbThis){
            std::cout << "found a spin factor for this decay:\n" << thisDcy << std::endl;
        }
        if(A_is_in_B("PLUS", lopt))return new SF_BtoPV0_PtoP1V1_V1toP2P3(thisDcy,1);  
        else if(A_is_in_B("ZERO", lopt))return new SF_BtoPV0_PtoP1V1_V1toP2P3(thisDcy,0);  
        else if(A_is_in_B("MINUS", lopt))return new SF_BtoPV0_PtoP1V1_V1toP2P3(thisDcy,-1);  
        else return new SF_BtoPV0_PtoP1V1_V1toP2P3(thisDcy,-999);
    }
    
  // final state pseudo scalars  
  if( equalBySVPAT(SF_DtoPP0_PtoVP1_VtoP2P3::getExampleDecay()
		   , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoPP0_PtoVP1_VtoP2P3(thisDcy);

  }else if( equalBySVPAT_L(SF_DtoAP0_AtoVP1_VtoP2P3::getExampleDecay()
			   , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoAP0_AtoVP1_VtoP2P3(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoAP0_AtoVP1Dwave_VtoP2P3::getExampleDecay()
			   , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoAP0_AtoVP1Dwave_VtoP2P3(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoAP0_AtoSP1_StoP2P3::getExampleDecay()
			   , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoAP0_AtoSP1_StoP2P3(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoV1V2_V1toP0P1_V1toP2P3_S::getExampleDecay()
			   , thisDcy) 
	    ){//&& (SPD_Wave == 'S' || SPD_Wave == '?')){
    if(dbThis){
      std::cout << "found a spin factor for this decay as " 
		<< "S-wave\n"
		<< thisDcy << std::endl;
    }
    return new SF_DtoV1V2_V1toP0P1_V1toP2P3_S(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoV1V2_V1toP0P1_V1toP2P3_P::getExampleDecay()
			   , thisDcy) 
	    ){//  && (SPD_Wave == 'P')){
    if(dbThis){
      std::cout << "found a spin factor for this decay as " 
		<< "P-wave\n"
		<< thisDcy << std::endl;
    }
    return new SF_DtoV1V2_V1toP0P1_V1toP2P3_P(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoV1V2_V1toP0P1_V1toP2P3_D::getExampleDecay()
			   , thisDcy) 
	    ){// && (SPD_Wave == 'D')){
    if(dbThis){
      std::cout << "found a spin factor for this decay as " 
		<< "D-wave\n"
		<< thisDcy << std::endl;
    }
    return new SF_DtoV1V2_V1toP0P1_V1toP2P3_D(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoVT_VtoP0P1_TtoP2P3_P::getExampleDecay()
			   , thisDcy) 
	    ){//&& (SPD_Wave == 'S' || SPD_Wave == '?')){
    if(dbThis){
      std::cout << "found a spin factor for this decay as " 
		<< "P-wave\n"
		<< thisDcy << std::endl;
    }
    return new SF_DtoVT_VtoP0P1_TtoP2P3_P(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoVT_VtoP0P1_TtoP2P3_D::getExampleDecay()
			   , thisDcy) 
	    ){//&& (SPD_Wave == 'S' || SPD_Wave == '?')){
    if(dbThis){
      std::cout << "found a spin factor for this decay as " 
		<< "D-wave\n"
		<< thisDcy << std::endl;
    }
    return new SF_DtoVT_VtoP0P1_TtoP2P3_D(thisDcy);
 }else if( equalBySVPAT_L(SF_DtoTS_TtoP0P1_StoP2P3::getExampleDecay()
			   , thisDcy) 
	    ){//&& (SPD_Wave == 'S' || SPD_Wave == '?')){
    if(dbThis){
      std::cout << "found a spin factor for this decay." 
		<< thisDcy << std::endl;
    }
    return new SF_DtoTS_TtoP0P1_StoP2P3(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::getExampleDecay()
			   , thisDcy) 
	    ){// && (SPD_Wave == 'S' || SPD_Wave == '?')){
    if(dbThis){
      std::cout << "found a spin factor for this decay as " 
		<< "S-wave\n"
		<< thisDcy << std::endl;
      std::cout << "WARNING!! You didn't specify that this is supposed to be"
		<< " treated as S-Wave!!\n I just assume it is." << std::endl;
    }
    return new SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoVS_VtoP0P1_StoP2P3::getExampleDecay()
			   , thisDcy) 
	    ){// && (SPD_Wave == 'S' || SPD_Wave == '?')){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoVS_VtoP0P1_StoP2P3(thisDcy);
  }else if( equalBySVPAT_L(SF_DtoVS_VtoP0P1_StoP2P3_nonResV::getExampleDecay()
			   , thisDcy) 
	    ){//&& (SPD_Wave == 'S' || SPD_Wave == '?')){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n"
		<< thisDcy << std::endl;
    }
    return new SF_DtoVS_VtoP0P1_StoP2P3_nonResV(thisDcy);
  }else if(equalBySVPAT_L(SF_DtoPP1P2_PtoP3P4::getExampleDecay() 
			  //Lauren's wide Ks Spin
			  , thisDcy)
	   ){
    return new SF_DtoPP1P2_PtoP3P4(thisDcy);
  }else if(equalBySVPAT_L(SF_DtoS1S2_S1toP1P2_S2toP3P4::getExampleDecay() 
			  //Lauren's wide Ks Spin
			  , thisDcy)
	   ){
    return new SF_DtoS1S2_S1toP1P2_S2toP3P4(thisDcy);
  }else if( equalBySVPAT(SF_DtoV1P0_V1toV2P1_V2toP2P3::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoV1P0_V1toV2P1_V2toP2P3(thisDcy);
  }else if( equalBySVPAT(SF_DtoT1T2_T1toP0P1_T2toP2P3_S::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoT1T2_T1toP0P1_T2toP2P3_S(thisDcy);
  }else if( equalBySVPAT(SF_DtoT1T2_T1toP0P1_T2toP2P3_P::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoT1T2_T1toP0P1_T2toP2P3_P(thisDcy);
  }else if( equalBySVPAT(SF_DtoT1T2_T1toP0P1_T2toP2P3_D::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoT1T2_T1toP0P1_T2toP2P3_D(thisDcy);
  }else if( equalBySVPAT(SF_DtoTP0_TtoVP1_VtoP2P3::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoTP0_TtoVP1_VtoP2P3(thisDcy);
  }else if( equalBySVPAT(SF_DtoT1P0_T1toT2P1_T2toP2P3::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoT1P0_T1toT2P1_T2toP2P3(thisDcy);
  }else if( equalBySVPAT(SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3(thisDcy);
  }else if( equalBySVPAT(SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3(thisDcy);
  }else if( equalBySVPAT(SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3(thisDcy);
  }else if( equalBySVPAT(SF_DtoPP0_PtoSP1_StoP3P4::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoPP0_PtoSP1_StoP3P4(thisDcy);
  }else if( equalBySVPAT(SF_DtoAP0_AtoTP1_TtoP2P3::getExampleDecay()
			 , thisDcy) ){
    if(dbThis){
      std::cout << "found a spin factor for this decay:\n" 
		<< thisDcy << std::endl;
    }
    return new SF_DtoAP0_AtoTP1_TtoP2P3(thisDcy);
  }else{
    std::cout << "SpinFactorMaker WARNING!!"
	      << " don't know spin factor of this decay:\n" 
	      << thisDcy 
	      << " (" << SPD_Wave << "-wave)" << std::endl;
    return 0;
  }

}

//
