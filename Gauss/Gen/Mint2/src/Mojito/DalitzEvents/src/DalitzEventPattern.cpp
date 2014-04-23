// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "Mint/DalitzEventPattern.h"
#include "Mint/DecayTree.h"
#include "Mint/counted_ptr.h"

#include "Mint/AmpInitialiser.h"

#include <algorithm>

const DalitzEventPattern DalitzEventPattern::NoPattern;

using namespace std;
using namespace MINT;

DalitzEventPattern::DalitzEventPattern(int pdg_ids[], int arrayDimension)
  : vector<DecayTreeItem>(arrayDimension)
{
  for(int i=0; i<arrayDimension; i++){
    DecayTreeItem dti(pdg_ids[i]);
    (*this)[i]=dti;
  }
}
DalitzEventPattern::DalitzEventPattern(std::vector<int> pdg_ids)  
  : vector<DecayTreeItem>(pdg_ids.size())
{
  for(unsigned int i=0; i<pdg_ids.size(); i++){
    DecayTreeItem dti(pdg_ids[i]);
    (*this)[i]=dti;
  }
}

DalitzEventPattern::DalitzEventPattern(int mum, int d1, int d2)
  : vector<DecayTreeItem>(3)
{
  (*this)[0] = DecayTreeItem(mum);
  (*this)[1] = DecayTreeItem(d1);
  (*this)[2] = DecayTreeItem(d2);
}

DalitzEventPattern::DalitzEventPattern(int mum, int d1, int d2, int d3)
  : vector<DecayTreeItem>(4)
{
  (*this)[0] = DecayTreeItem(mum);
  (*this)[1] = DecayTreeItem(d1);
  (*this)[2] = DecayTreeItem(d2);
  (*this)[3] = DecayTreeItem(d3);
}

DalitzEventPattern::DalitzEventPattern(int mum,int d1,int d2,int d3,int d4)
  : vector<DecayTreeItem>(5)
{
  (*this)[0] = DecayTreeItem(mum);
  (*this)[1] = DecayTreeItem(d1);
  (*this)[2] = DecayTreeItem(d2);
  (*this)[3] = DecayTreeItem(d3);
  (*this)[4] = DecayTreeItem(d4);
}

DalitzEventPattern::DalitzEventPattern(const DalitzEventPattern& other)
  : std::vector<DecayTreeItem>(other)
{}
DalitzEventPattern::DalitzEventPattern(const std::vector<DecayTreeItem>& other)
  : std::vector<DecayTreeItem>(other)
{}

DalitzEventPattern::DalitzEventPattern(const DecayTree& dt_in){
  bool dbThis=false;
  counted_ptr<std::vector<const DecayTreeItem*> >
    fsTree_pointers( dt_in.finalStatePtr() );

  std::vector<int> finalState( fsTree_pointers->size() );

  for(unsigned int i=0; i< fsTree_pointers->size(); i++){
    finalState[i] = (*fsTree_pointers)[i]->pdg();
  }
  sort(finalState.begin(), finalState.end());

  this->resize(finalState.size() + 1);
  (*this)[0] = dt_in.getVal();
  for(unsigned int i=0; i< finalState.size(); i++){ // offspring
    (*this)[i+1] = finalState[i];
  }
  
  if(dbThis){
    cout << "made this pattern: " << *this
	 << "out of this tree " << dt_in << endl;
  }
}


DalitzEventPattern DalitzEventPattern::makeCPConjugate() const{
  DalitzEventPattern pat(*this);
  for(unsigned int i=0; i < pat.size(); i++) pat[i].antiThis();
  return pat;
}

bool DalitzEventPattern::equal(const DalitzEventPattern& other) const{
  if(size() != other.size()) return false;
  for(unsigned int i=0; i<size(); i++){
    if((*this)[i] != other[i]) return false;
  }
  return true;
}
bool DalitzEventPattern::lt(const DalitzEventPattern& other) const{
  if(equal(other)) return false;
  if(size() < other.size()) return true;
  if(size() > other.size()) return false;
  for(unsigned int i=0; i<size(); i++){
    if((*this)[i] < other[i]) return true;
    if((*this)[i] > other[i]) return false;
  }
  return false;
}
bool DalitzEventPattern::operator==(const DalitzEventPattern& rhs) const{
  return equal(rhs);
}
bool DalitzEventPattern::operator!=(const DalitzEventPattern& rhs) const{
  return ! ((*this) == rhs);
}

bool DalitzEventPattern::operator<(const DalitzEventPattern& rhs) const{
  return lt(rhs);
}
bool DalitzEventPattern::operator<=(const DalitzEventPattern& rhs) const{
  return (*this < rhs || *this == rhs);
}
bool DalitzEventPattern::operator>=(const DalitzEventPattern& rhs) const{
  return ! ((*this) < rhs);
}
bool DalitzEventPattern::operator>(const DalitzEventPattern& rhs) const{
  return ! ((*this) <= rhs);
}

void DalitzEventPattern::print(std::ostream& os)const{
  os << "(";
  for(unsigned int i=0; i< this->size(); i++){
    os << (*this)[i];
    if(i + 1< this->size()) os << ", ";
  }
  os << ")";
}

std::vector<int> DalitzEventPattern::finalStates() const{
  std::vector<int> fs;
  for(unsigned int i=1; i<this->size(); i++){
    fs.push_back((*this)[i]);
  }
  return fs;
}

bool DalitzEventPattern::compatibleWith(const DecayTree& tree) const{
  if(this->empty()) return false;

  if(tree.getVal().pdg() != (*this)[0]) return false; // check mother

  return compatibleWithFinalState(tree); // check daughters
}

bool DalitzEventPattern::compatibleWithFinalState(const AmpInitialiser& ampInitContainsTree)const{
  return compatibleWithFinalState(ampInitContainsTree.tree());
}


bool DalitzEventPattern::compatibleWithFinalState(const DecayTree& tree) const{
  if(this->empty()) return false;

  std::vector<const DecayTreeItem*> fsTree_pointers = tree.finalState();

  std::vector<int> fsTree;
  for(unsigned int i=0; i< fsTree_pointers.size(); i++){
    fsTree.push_back(fsTree_pointers[i]->pdg());
  }

  return compatibleWithFinalState(fsTree);
}

bool DalitzEventPattern::compatibleWithFinalState(const DalitzEventPattern& 
						  pat) const{
  if(this->empty()) return false;
  std::vector<int> otherFs = pat.finalStates();
  return compatibleWithFinalState(otherFs);
}

bool DalitzEventPattern::compatibleWithFinalState(const vector<int>& 
						  otherFs_in) const{

  std::vector<int> fs = finalStates(); // my own;  
  if(otherFs_in.size() != fs.size()) return false;

  std::vector<int> otherFs(otherFs_in);
  sort(fs.begin()     , fs.end());
  sort(otherFs.begin(), otherFs.end());

  for(unsigned int i=0; i<fs.size(); i++){
    if(fs[i] != otherFs[i]) return false;
  }
  
  return true;
  
}

bool DalitzEventPattern::selfConjugateFS() const{
  return this->compatibleWithFinalState(this->makeCPConjugate());
}

double DalitzEventPattern::sijMin(int i, int j) const{
  std::vector<int> indices(2);
  indices[0] = i; indices[1] = j;
  return sijMin(indices);
}
double DalitzEventPattern::sijMax(int i, int j) const{
  std::vector<int> indices(2);
  indices[0] = i; indices[1] = j;
  return sijMax(indices);
}

double DalitzEventPattern::sijMin(int i, int j, int k) const{
  std::vector<int> indices(3);
  indices[0] = i; indices[1] = j; indices[2] = k;
  return sijMin(indices);
}

double DalitzEventPattern::sijMax(int i, int j, int k) const{
  std::vector<int> indices(3);
  indices[0] = i; indices[1] = j; indices[2]=k;
  return sijMax(indices);
}

double DalitzEventPattern::sijMin(const std::vector<int>& indices) const{
  double sum=0;
  for(unsigned int i=0; i< indices.size(); i++){
    if(indices[i] < 1 || indices[i] >= (int) this->size()){
      // indices should be dgtr indices, i.e. from 1 to num-daughters.
      return -9999.0;
    }
    sum += (*this)[indices[i]].mass();
  }
  return sum*sum;
}

double DalitzEventPattern::sijMax(const std::vector<int>& indices) const{
  if(this->empty()) return -9999;

  double sum=(*this)[0].mass();
  for(unsigned int i=1; i<this->size(); i++){
    sum -= (*this)[i].mass();
  }
  // at this stage, sum = mother mass - (all daugher masses).

  for(unsigned int i=0; i< indices.size(); i++){
    if(indices[i] < 1 || indices[i] >= (int) this->size()){
      // indices should be dgtr indices, i.e. from 1 to num-daughters.
      return -9999.0;
    }
    sum += (*this)[indices[i]].mass();
  }

  // now sum = mother mass - (missing daughter masses)
  return sum*sum;
}

std::vector<int> DalitzEventPattern::getVectorOfInts() const{
  std::vector<int> v(this->size());
  for(unsigned int i=0; i<this->size(); i++){
    v[i] = (int) (*this)[i];
  }
  return v;
}

std::string DalitzEventPattern::name() const{
  std::string s("");
  if(this->size() < 1) return "";
  s = (*this)[0].name() + "->";
  for(unsigned int i=1; i < this->size(); i++){
    s += (*this)[i].name();
  }
  return s;
}

std::ostream& operator<<(std::ostream& os, const DalitzEventPattern& p){
  p.print(os);
  return os;
}
//
