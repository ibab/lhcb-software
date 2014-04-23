#ifndef DECAYTREE_HH
#define DECAYTREE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <iostream>

#include "Mint/DDTree.h"
#include "Mint/DecayTreeItem.h"
#include "Mint/MultiQuarkContent.h"

/* Usage:
   takes pdg_id's to make decay tree

  DecayTree dt(421);

  dt.addDgtr(313)->addDgtr(321, -211);
  dt.addDgtr(-321, 211);
  cout << dt << endl;

Prints:

 D0
 |-- K*(892)0
 |   |-- K+
 |   |-- pi-
 |-- K-
 |-- pi+


Todo: Allow adding names rather than only numbers.

*/

typedef DDTree<DecayTreeItem> DecayTree;

void anti(DecayTree& dt);
//void ChargeCon(DecayTree& dt);
//void ParityCon(DecayTree& dt);
//void CPCon(DecayTree& dt);
std::string uniqueName(const DecayTree& dt_in);

MultiQuarkContent netQuarkContent(const DecayTree& dt_in);
#endif
//
