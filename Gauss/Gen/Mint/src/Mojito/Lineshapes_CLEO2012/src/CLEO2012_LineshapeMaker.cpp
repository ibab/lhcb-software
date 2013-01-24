// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/CLEO2012_LineshapeMaker.h"
#include "Mint/ILineshape.h"
#include "Mint/Utils.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/IDalitzEventAccess.h"
#include "Mint/CLEO2012_BW_BW.h"
#include "Mint/CLEO2012_GounarisSakurai.h"
#include "Mint/CLEO2012_Lass.h"
#include "Mint/CLEO2012_Flatte.h"
#include "Mint/CLEO2012_FocusFlatte.h"
#include "Mint/CLEO2012_CrystalBarrelFOCUS.h"

#include <iostream>

using namespace std;
using namespace MINT;

/* 
   possible options
   AWAYS_BW
   RHO_OMEGA
   Flatte
   GS
*/
ILineshape* CLEO2012_LineshapeMaker(const AssociatedDecayTree* tree
			   , IDalitzEventAccess* events
			   , const std::string& lopt
			   ){
  bool dbThis=false;
  if(0 == tree) return 0;
  
  if(dbThis){
    cout << "CLEO2012_LineshapeMaker called with " 
	 << tree->getVal().pdg() 
	 << " lopt = " << lopt << endl;

    cout << tree->getVal().pdg() << ", "
	 << (abs(tree->getVal().pdg())%1000)
	 << endl;
  }
      

  if(A_is_in_B("ALWAYS_BW", lopt)){
    if(dbThis) cout << "CLEO2012_LineshapeMaker returns CLEO2012_BW_BW" << endl;
    return new CLEO2012_BW_BW(*tree, events);
  }

  if((abs(tree->getVal().pdg())%1000)==113){
    if(abs(tree->getVal().pdg()) == 113 && A_is_in_B("RHO_OMEGA", lopt)){
      cout << "CLEO2012_LineshapeMaker returning rho-omega lineshape"
	   << endl;
      
      return new CLEO2012_CrystalBarrelFOCUS(*tree, events);
      //return new CLEO2012_BW_BW(*tree, events);
    }else if((abs(tree->getVal().pdg())%1000)==113 && A_is_in_B("GS", lopt)){
      if(dbThis) cout << "CLEO2012_LineshapeMaker: return GS lineshape" << endl;
      return new CLEO2012_GounarisSakurai(*tree, events);
    }else{
      cout << "WARNING: CLEO2012_LineshapeMaker:"
	   << " returning plain Breit-Wigner (CLEO2012_BW_BW) for rho"
	   << endl;
      return new CLEO2012_BW_BW(*tree, events);
    }
  }else if(abs(tree->getVal().pdg()) == 10321 ){ // K0*(1430)
    if(A_is_in_B("CLEO2012_Lass", lopt)){
      cout << "CLEO2012_LineshapeMaker: "
	   << "\n\t> returning CLEO2012_Lass lineshape"
	   << endl;
      return new CLEO2012_Lass(*tree, events);
    }else{
      cout << "WARNING: CLEO2012_LineshapeMaker:"
	   << " returning plain Breit-Wigner (CLEO2012_BW_BW) for K0*(1430)"
	   << endl;
      return new CLEO2012_BW_BW(*tree, events);
    }
  }else if(abs(tree->getVal().pdg()) == 9010221 ){ // f0(980)
    if(A_is_in_B("CLEO2012_FocusFlatte", lopt)){
      cout << "CLEO2012_LineshapeMaker: "
	   << "\n\t> returning Flatte lineshape"
	   << endl;
      return new CLEO2012_FocusFlatte(*tree, events);
    }else if(A_is_in_B("CLEO2012_Flatte", lopt)){
      cout << "CLEO2012_LineshapeMaker: "
	   << "\n\t> returning CLEO2012_Flatte lineshape"
	   << endl;
      return new CLEO2012_Flatte(*tree, events);
    }
    else if(A_is_in_B("CLEO2012_FocusFlatte", lopt)){
    cout << "CLEO2012_LineshapeMaker: "
	   << "\n\t> returning Focus Flatte lineshape"
	   << endl;
	   return new CLEO2012_FocusFlatte(*tree, events); //laurens f0 focusflatte
    }
    else{
      cout << "WARNING: CLEO2012_LineshapeMaker:"
	   << " returning plain Breit-Wigner (CLEO2012_BW_BW) for f0(980)"
	   << endl;
      return new CLEO2012_BW_BW(*tree, events);
    }
  }else{
    if(dbThis) cout << "CLEO2012_LineshapeMaker returns CLEO2012_BW_BW" << endl;
    return new CLEO2012_BW_BW(*tree, events);
  }

}

//
