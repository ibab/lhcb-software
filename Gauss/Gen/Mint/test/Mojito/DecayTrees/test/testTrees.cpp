// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include <iostream>
#include "Mint/DecayTree.h"
#include "Mint/DecayTreeComparisons.h"
#include "Mint/AssociatingDecayTree.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/NamedDecayTreeList.h"

#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEventPattern.h"

#include "Mint/counted_ptr.h"

using namespace std;
using namespace MINT;

int testTrees(){
  DecayTree dt1(421);
  dt1.addDgtr(313)->addDgtr(321, -211);
  dt1.addDgtr(-321, 211);

  cout << "Tree: \n" << dt1 << endl;

  sortBySVPAT(dt1);
  cout << "same tree \n" << dt1 << endl;

  cout << "Now the same tree, but as a one-liner" << endl;
  cout << dt1.oneLiner() << endl;

  DecayTree dt2(421);
  dt2.addDgtr(-321, 10323)->addDgtr(321, 113)->addDgtr(211,-211);

  cout << "another tree \n" << dt2 << endl;

  cout << "Generating a few events to test"
       << " tree-to-event-pattern association"
       << endl;

  int Nevents = 1;
  DalitzEventPattern pdg(421, 321, -321, 211, -211);

  cout << "I'm generating according to the following"
       << " event pattern" << endl;
  for(int i=0; i<5; i++){
    cout << " " << i << " ) = " << pdg[i] << endl;
  }
  DalitzEventList eventList;
  eventList.generatePhaseSpaceEvents(Nevents, pdg);
  cout << "eventList.size() " << eventList.size() << endl;

  AssociatingDecayTree a1(dt1, &eventList);
  
  cout << "The associated decay tree is\n" << a1.getTree() << endl;

  cout << "Tree 1, once more\n" << dt1 << endl;
  cout << "Tree1, but as a one-liner" << endl;
  cout << dt1.oneLiner() << endl;
  cout << "\n and now, once more Tree 1, sorted\n";
  standardSort(dt1);
  cout << dt1 << endl;
  cout << " as one liner: " << dt1.oneLiner() << endl;
  cout << "\n and now, anti Tree 1, un-sorted\n";
  anti(dt1);
  cout << dt1;
  cout << endl;
  cout << "\n and now, anti Tree 1, sorted, onliner\n";
  cout << dt1.oneLiner() << endl;


  cout << "check copy constructor";
  DecayTree dt3(dt1);
  cout << dt3 << endl;

  std::map<std::string, DecayTree> tmap;
  tmap["test"] = dt3;
  cout << tmap["test"] << endl;

  cout << "about to list all decay trees I know about\n"
       << endl;
 
  const NamedDecayTreeList* lptr = NamedDecayTreeList::getMe();
  if(0 == lptr) cout << "didn't get NamedDecayTreeList" <<endl;

  DecayTree fromString(lptr->find("D0->K(1)(1270)+(->K(0)*(1430)0(->K+,pi-),pi+),K-").tree()); // this is usually done automatically (no way to get
  // the string right otherwise). Still, for allowed names
  // you can check the log-file, all known decays are listed
  // (with their names).

  cout << "survided createing decay tree from string."
       << " this is the tree:\n" << fromString
       << "\n and this is its oneLiner " << fromString.oneLiner()
       << endl;

  cout << "\n Now testing sister-access\n"
       << " fromString has " << fromString.nDgtr()
       << " daughters and " << fromString.nSisters()
       << " sisters."
       << endl;
  if(fromString.nDgtr() > 0){
    const_counted_ptr<DecayTree> dgt=fromString.getDgtrTreePtr(0);
    cout << "... its first daughter is: "
	 << dgt->oneLiner()
	 << endl;
    cout << " this daughter has " << dgt->nSisters()
	 << " sisters."
	 << endl;
    std::vector<const_counted_ptr<DecayTree> > sisters=dgt->getSistersTrees();
    cout << " Those sisters are: ";
    for(unsigned int i=0; i< sisters.size(); i++){
      cout << "\n     " << sisters[i]->oneLiner();
    }
    cout << endl;
  }

  cout << "\n All tests done, returning 0 " << endl;
  return 0;
}

int main(){

  return testTrees();

}

//
