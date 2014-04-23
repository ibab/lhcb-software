// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/DecayTree.h"
#include "Mint/DecayTreeComparisons.h"
#include <algorithm>

using namespace MINT;

bool lessBySVPAT::operator()(const_counted_ptr<DecayTree> a, const_counted_ptr<DecayTree> b) const{
  // first handle bad pointers
  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  
  return a->getVal().SVPAT() < b->getVal().SVPAT();
}

bool lessBySVPAT_L::operator()(const_counted_ptr<DecayTree> a, const_counted_ptr<DecayTree> b) const{
  // first handle bad pointers
  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  
  if(a->getVal().SVPAT() == b->getVal().SVPAT()){
    return a->getVal().L() < b->getVal().L();
  }else{
    return a->getVal().SVPAT() < b->getVal().SVPAT();
  }
}

bool sortBySVPAT(DecayTree& A){
  A.sortAllBy<moreByNDgtr>();
  A.sortAllBy<lessBySVPAT>();
  return true;
}

bool sortBySVPAT_L(DecayTree& A){
  A.sortAllBy<moreByNDgtr>();
  A.sortAllBy<lessBySVPAT>();
  return true;
}


bool moreByNDgtr::operator()(const_counted_ptr<DecayTree> a, const_counted_ptr<DecayTree> b) const{
  // first handle bad pointers
  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  
  return a->nDgtr() > b->nDgtr();
}

bool sortByMoreDgtr(DecayTree& A){
  A.sortAllBy<moreByNDgtr>();
  return true;
}

bool moreByMass::operator()(const_counted_ptr<DecayTree> a, const_counted_ptr<DecayTree> b) const{
  // first handle bad pointers
  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  
  return a->getVal().mass() > b->getVal().mass();
}
bool moreByL::operator()(const_counted_ptr<DecayTree> a, const_counted_ptr<DecayTree> b) const{
  // first handle bad pointers
  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  
  return a->getVal().L() > b->getVal().L();
}

bool sortByMoreMass(DecayTree& A){
  A.sortAllBy<moreByMass>();
  return true;
}
bool sortByMoreL(DecayTree& A){
  A.sortAllBy<moreByL>();
  return true;
}

bool moreByID::operator()(const_counted_ptr<DecayTree> a, const_counted_ptr<DecayTree> b) const{
  // first handle bad pointers
  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  
  return a->getVal().pdg() > b->getVal().pdg();
}

bool sortByMoreID(DecayTree& A){
  A.sortAllBy<moreByID>();  
  return true;
}

bool moreByAbsID_stillUnique::operator()(const_counted_ptr<DecayTree> a
					 , const_counted_ptr<DecayTree> b) const{

  // sorts by abs ID but is still unique. When two
  // particles have same abs ID, the one with
  // the positive id comes first

  // first handle bad pointers
  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  
  int abs_a = abs(a->getVal().pdg());
  int abs_b = abs(b->getVal().pdg());

  if(abs_a == abs_b) {
    return a->getVal().pdg() > b->getVal().pdg();
  }else{
    return abs_a > abs_b;
  }
}

bool sortByMoreAbsID_stillUnique(DecayTree& A){
  A.sortAllBy<moreByAbsID_stillUnique>();  
  return true;
}

bool standardSort(DecayTree& A){

  bool isAnti=(A.getVal().pdg() < 0);
  if(isAnti) anti(A);

  sortByMoreL(A);

  sortByMoreAbsID_stillUnique(A); 
  //this gives unique starting point.

  sortByMoreMass(A);
  sortByMoreDgtr(A);

  if(isAnti) anti(A);
  return true;
}

bool equalBySVPATRec(const DecayTree& A, const DecayTree& B){
  // checks if the two decays have the same 
  // Scalar/Vector/Pseudoscalar/Axialvector
  // structure. Superseded by equalBySVPATLRec to identify the
  // right spin factor for each
  // decay amplitude.

  // A and B already sorted - important!

  if(A.nDgtr() != B.nDgtr()){
    return false;
  }
  for(int i=0; i<A.nDgtr(); i++){
    if(! equalBySVPATRec(*(A.getDgtrTreePtr(i)), *(B.getDgtrTreePtr(i)))){
      return false;
    }
  }

  return A.getVal().SVPAT() == B.getVal().SVPAT();
}

bool equalBySVPAT_LRec(const DecayTree& A, const DecayTree& B){
  // checks if the two decays have the same 
  // Scalar/Vector/Pseudoscalar/Axialvector
  // structure. And the same orbital angular momenta
  // in the decays. Used to identify the
  // right spin factor for each
  // decay amplitude.

  // A and B already sorted - important!

  if(A.nDgtr() != B.nDgtr()){
    return false;
  }
  for(int i=0; i<A.nDgtr(); i++){
    if(! equalBySVPAT_LRec(*(A.getDgtrTreePtr(i)), *(B.getDgtrTreePtr(i)))){
      return false;
    }
  }

  bool equality  = ( A.getVal().SVPAT() == B.getVal().SVPAT() )
    &&             ( A.getVal().L()     == B.getVal().L()     );


  return equality;
}


bool equalBySVPAT(DecayTree A, DecayTree B){
  sortBySVPAT(A);
  sortBySVPAT(B);

  return equalBySVPATRec(A,B);
}
bool equalBySVPAT_L(DecayTree A, DecayTree B){
  sortBySVPAT_L(A);
  sortBySVPAT_L(B);

  return equalBySVPAT_LRec(A,B);
}


//
