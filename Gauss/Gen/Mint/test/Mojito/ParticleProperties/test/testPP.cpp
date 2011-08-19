// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include <iostream>

using namespace std;
//using namespace MINT;

int testPP(){

  cout << "\nHi! Checking ParticlePropertiesList.h\n" << endl;
  cout << "Getting the list... " << endl;
  const ParticlePropertiesList *pList = ParticlePropertiesList::getMe();
  if(0 == pList){
    cout << " BAD: Didn't get list " << endl;
    return 0;
  }
  cout << " Got it!" << endl;

  cout << "\nA few examples: "
       << "\n  B0 mass="     
       << pList->get("B0")->mass()/GeV << " GeV"
       << "\n  D+ lifetime=" 
       << pList->get("D+")->lifetime()/picosecond << " ps"
       << "\n  rho is Scalar/Vector/Pseudo Scalar/Axial Vector/Tensor? "
       << "\n   pList->get(\"rho(770)0\")->SVPAT() = " 
       << pList->get("rho(770)0")->SVPAT()
       << endl;
  cout << "All I know about D+ "
       << endl;
  pList->get("D+")->print();
  cout << "All I know about rho"
       << endl;
  pList->get("rho(770)0")->print();
  cout << "All I know about KS"
       << endl;
  pList->get(310)->print();

  return 0;
}

int main(){
  return testPP();
}
//
