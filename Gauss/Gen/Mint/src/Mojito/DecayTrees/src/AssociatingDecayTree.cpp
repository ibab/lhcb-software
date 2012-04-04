// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT
#include "Mint/AssociatingDecayTree.h"
#include <algorithm>

using namespace std;
using namespace MINT;

bool AssociatingDecayTree::associate() const{
  bool success=true;
  success &= associateFinalStates();
  if(! success) return false;
  success &= associateResonances();
  _prevPattern=getPattern();
  return success;
}

bool AssociatingDecayTree::associateFinalStates() const{
  //  cout << "associateFinalStates was called " << endl;
  if(getPattern().empty())return false;
  vector<int> fs_pat= getPattern().finalStates();

  //cout << "got pattern of size " << pat.size() << endl;
  if(fs_pat.empty()) return false;
  //  cout << "pattern not empty" << endl;
  std::vector<AssociatedDecayTreeItem*> sorted 
    = _theDecay.finalStateInThisOrder(fs_pat);
  //  cout << "sorted" << endl;
  if(sorted.size() != fs_pat.size()){
	  cout << "Sizes: " << sorted.size() << " " << fs_pat.size() << endl;

    std::cout << "ERROR in AssociatingDecayTree::associate!"
	      << " fs_pattern : ";
    for(unsigned int i=0; i<fs_pat.size(); i++){
      std::cout << "(" << i << ") " << fs_pat[i] << ", ";
    }
    std::cout << std::endl;

    for(unsigned int i=0; i<sorted.size(); i++){
          std::cout << "(" << i << ") " << *(sorted[i]) << ", ";
        }
    std::cout << std::endl;

//       std::cout << "\n does not match decay:\n" << _theDecay << std::endl;
    
    return false;
  }
  //  cout << "now putting it all in" << endl;
  for(unsigned int i=0; i<sorted.size(); i++){
    sorted[i]->_asi.clear();
    sorted[i]->_asi.push_back(i+1);
  }
  //  cout << "associateFinalStates returning true" << endl;
  return true;
}

bool AssociatingDecayTree::associateResonances() const{
  return ! (addAssociations(&_theDecay).empty());
}
std::vector<int> AssociatingDecayTree::addAssociations(counted_ptr<AssociatedDecayTree> ctree) const{
  return addAssociations(ctree.get());
}

std::vector<int> AssociatingDecayTree::addAssociations(AssociatedDecayTree* tree) const{
  if(tree->isFinalState()){
    return tree->getVal()._asi;
  }
  std::vector<int> allAsis;
 
  for(int i=0; i< tree->nDgtr(); i++){
    std::vector<int> thisAsi = addAssociations(tree->getDgtrTreePtr(i));
    for(unsigned int j=0; j<thisAsi.size(); j++){ allAsis.push_back(thisAsi[j]);}
  }
  stable_sort(allAsis.begin(), allAsis.end());
  tree->getVal()._asi=allAsis;
  return allAsis;
}


const DalitzEventPattern& AssociatingDecayTree::getPattern() const{
  if(0 == getEvent()){
    return DalitzEventPattern::NoPattern;
  }
  return getEvent()->eventPattern();
}


bool AssociatingDecayTree::patternHasChanged() const{
  if(getPattern().empty()) return false;
  if(_prevPattern.size() != getPattern().size()){
    std::cout << " returning pattern has changed = true " << std::endl;
    return true;
  }
  // only compare final state:
  // (this means for D or Dbar to same final state will be treated
  //  as having the same pattern).
  for(unsigned int i=1; i< _prevPattern.size(); i++){
    if (_prevPattern[i] != getPattern()[i]){
      std::cout << " returning pattern has changed = true " << std::endl;
      return true;
    }
  }
  return false;
}

const AssociatedDecayTree* AssociatingDecayTree::getTreePtr() const{
  if(patternHasChanged()) associate();
  return & _theDecay;
}

const AssociatedDecayTree& AssociatingDecayTree::getTree() const{
  if(patternHasChanged()) associate();
  return _theDecay;
}

const DalitzEventPattern& AssociatingDecayTree::getTreePattern() const{
  return _theTreesPattern;
}

//

