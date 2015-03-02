// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT
#include "Mint/Permutation.h"

#include "Mint/Permutator.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzEventList.h"


#include "Mint/DecayTree.h"

#include "Mint/Amplitude.h"


#include <iostream>

using namespace std;
using namespace MINT;

int testPermutations(){

  Permutation p(4);

  cout << "just born: " << endl;
  cout << p << endl;

  cout << " swap 1,3 " << endl;
  p.swap(1,3);
  cout << p << endl;
  cout << " swap 2,3 " << endl;
  p.swap(2,3);
  cout << p << endl;

  cout << " getting inverse\n " << endl;
  Permutation pinv = p.getInverse();
  cout <<  pinv << endl;
  
  cout << "multiplying permutation with its inverse:\n " << endl;
  cout << p << "\n * \n" << pinv << "\n = \n"
       <<  p*pinv << "\n = \n" << pinv * p << endl;

  cout << " squaring a permutation:\n"
       << p << "\n squared = \n"
       << p*p
       << endl;

  cout << "\n now testing permutator \n" << endl;
  DalitzEventPattern pat(211, 211, 211, 211);
  Permutator permtor(pat);
  cout << permtor << endl;

  cout << "\n\n now testing symmetrised amplitudes:\n\n" << endl;


  DalitzEventPattern pdg(421, 321, -211, 211, -211);
  int Nevents=5;

  DecayTree dt1(421);
  dt1.addDgtr(113)->addDgtr(211, -211);
  dt1.addDgtr(313)->addDgtr(321, -211);

  cout << "made a tree " << dt1 << endl;

  cout << "Now making an event list:" << endl;
  DalitzEventList eventList;
  eventList.generatePhaseSpaceEvents(Nevents, pdg);
  cout << "Got " << eventList.size() << " events" << endl;

  cout << "Now I'm going to make an Amplitude for the tree." << endl;
  Amplitude amp(dt1);

  for(unsigned int i=0; i < eventList.size(); i++){
    cout << " ----- Event number " << i << "------" << endl;
    
    cout << "ask the amplitude amp to print the event" <<endl;
    eventList[i].print();
    cout << " evaluating amp: "
	 << " amp.getVal() = " << amp.getVal(eventList[i]) << endl;
    cout << "\n\n\n" << endl;
  }

  return 0;
}

int main(){

  return testPermutations();

}

//
