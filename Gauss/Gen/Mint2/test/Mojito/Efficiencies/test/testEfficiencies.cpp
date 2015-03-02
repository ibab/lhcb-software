#include "Mint/symPolyTerm.h"
#include "Mint/symMultiPolyTerm.h"
#include <iostream>
#include "Mint/Eff4piSymmetric.h"
#include "Mint/DalitzEventList.h"

#include "Mint/CLHEPSystemOfUnits.h"

using namespace std;

int testEff4piSymmetric(){
  DalitzEventPattern pat(421, 211, 211, 211, 211);

  DalitzEventList evtList;
  
  evtList.generatePhaseSpaceEvents(100, pat);


  Eff4piSymmetric eps(5, pat, 0);

  cout << eps << endl;

  cout << " a few values: " << endl;
  cout << eps.getVal(1*GeV*GeV, 1*GeV*GeV, 1*GeV*GeV, 1*GeV*GeV, 1*GeV*GeV)
       << endl;

  cout << eps.getVal(2*GeV*GeV, 2*GeV*GeV, 1*GeV*GeV, 1*GeV*GeV, 1*GeV*GeV)
       << endl;

  cout << eps.getVal(0*GeV*GeV, 2*GeV*GeV, 1*GeV*GeV, 4*GeV*GeV, 1*GeV*GeV)
       << endl;

  cout << "now for real Dalitz events:" << endl;
  for(unsigned int i = 0; i < evtList.size(); i++){
    cout << " " << i << ": " << eps.getVal(evtList[i]) << endl;
  }

  return 0;
}

int testSymMultiPolyTerm(){
  symMultiPolyTerm D40(4,0), D42(4,2), D24(2, 4);
  symMultiPolyTerm D10(1,0), D01(0,1), D15(1,5);
  symMultiPolyTerm D20(2,0), D21(2,1), D25(2,5);
  cout << "D40 " << D40 << endl;
  cout << "D42 " << D42 << endl;
  cout << "D24 " << D24 << endl;
  cout << " -- " << endl;
  cout << "D10 " << D10 << endl;
  cout << "D01 " << D01 << endl;
  cout << "D15 " << D15 << endl;
  cout << " -- " << endl;
  cout << "D20 " << D20 << endl;
  cout << "D21 " << D21 << endl;
  cout << "D25 " << D25 << endl;
  
  return 0;
}

int testSymPolyTerm(){
  symPolyTerm D2_4(4, 0), D2_31(3, 1), D2_22(2, 2), D2_13(1, 3);
  
  cout << " D2_4 " << D2_4
       << "\n D2_31 " << D2_31
       << "\n D2_22 " << D2_22
       << "\n D2_13 " << D2_13
       << endl;
  
  cout << "D2_4(1, 1) = " << D2_4.eval(1, 1) << endl;
  cout << "D2_4(2, 1) = " << D2_4.eval(2, 1) << endl;
  cout << "D2_4(2, 2) = " << D2_4.eval(2, 2) << endl;
  cout << " -- " << endl;
  cout << "D2_31(1, 1) = " << D2_31.eval(1, 1) << endl;
  cout << "D2_31(2, 1) = " << D2_31.eval(2, 1) << endl;
  cout << "D2_31(2, 2) = " << D2_31.eval(2, 2) << endl;

  symPolyTerm D3_4(4, 0, 0), D3_31(3, 1, 0), D3_22(2, 2, 0), D3_211(2, 1, 1);
  
  cout << " D3_4 " << D3_4
       << "\n D3_31 " << D3_31
       << "\n D3_22 " << D3_22
       << "\n D3_211 " << D3_211
       << endl;
  
  cout << "D3_4(1, 1, 1) = " << D3_4.eval(1, 1, 1) << endl;
  cout << "D3_4(2, 1, 1) = " << D3_4.eval(2, 1, 1) << endl;
  cout << "D3_4(2, 2, 1) = " << D3_4.eval(2, 2, 1) << endl;
  cout << " -- " << endl;
  cout << "D3_31(1, 1, 1) = " << D3_31.eval(1, 1, 1) << endl;
  cout << "D3_31(2, 1, 1) = " << D3_31.eval(2, 1, 1) << endl;
  cout << "D3_31(2, 2, 1) = " << D3_31.eval(2, 2, 1) << endl;
  cout << " -- " << endl;
  cout << "D3_22(1, 1, 1) = " << D3_22.eval(1, 1, 1) << endl;
  cout << "D3_22(2, 1, 1) = " << D3_22.eval(2, 1, 1) << endl;
  cout << "D3_22(2, 2, 1) = " << D3_22.eval(2, 2, 1) << endl;
  cout << " -- " << endl;
  cout << "D3_211(1, 1, 1) = " << D3_211.eval(1, 1, 1) << endl;
  cout << "D3_211(2, 1, 1) = " << D3_211.eval(2, 1, 1) << endl;
  cout << "D3_211(2, 2, 1) = " << D3_211.eval(2, 2, 1) << endl;
 return 0;
}

int main(){
  return testEff4piSymmetric();
  // return testSymMultiPolyTerm();
  //  return testSymPolyTerm();
}
