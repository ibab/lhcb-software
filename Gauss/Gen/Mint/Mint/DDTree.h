#ifndef BASIC_DDTREE_HH
#define BASIC_DDTREE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "Mint/counted_ptr.h"

template <typename T> 
class lessByPointedTo{
 public:
  bool operator()(const T a, const T b){
    if(0 == a && 0==b) return false;
    if(0 == a && 0!=b) return true;
    if(0 !=a  && 0==b) return false;
    return a->getVal() < b->getVal();
  }
};

template <typename T> 
class equalByPointedTo{
 public:
  bool operator()(const T a, const T b){
    if(0 == a && 0==b) return true;
    if(0 == a && 0!=b) return false;
    if(0 !=a  && 0==b) return false;
    return a->getVal() == b->getVal();
  }
};


template<typename ValueType>
class DDTree{
 protected:
  // Three data members: value, and list of ptrs to daughters, ptr to parent.
  typename std::vector< MINT::counted_ptr< DDTree<ValueType> > > daughters;
  ValueType _thisVal;
  // private routines to deal with parentny
  const DDTree<ValueType>* _parentPtr; 
  // which this tree node is the daughter of. 
  //...don't use, I think it's dangerous stuff. Left in for debugging.

  //  MINT::counted_ptr<DDTree<ValueType> > Clone(DDTree<ValueType>* parent=0) const{
  //  MINT::counted_ptr<DDTree<ValueType> > top(new DDTree<ValueType>(_thisVal, parent));
  //
  //  top->daughters.resize(daughters.size());
  //  for(unsigned int i=0; i < daughters.size(); i++){
  //    top->daughters[i] = daughters[i]->Clone(top.get());
  //  }
  //  return top;
  //}

  void setParent(const DDTree<ValueType>* parent=0){_parentPtr=parent;}
  // above private -> only a tree can mess with parentny

  void disconnectAllDaughters(){
    if(daughters.empty()) return;
    for(unsigned int i=0; i<daughters.size(); i++){
      if(0!=daughters[i]) {
	daughters[i]->setParent(0);
	// Now using counted pointers - no need
	// to delete daughters, just "disconnect"
	//daughters[i]->deleteAllDaughters();
	//delete daughters[i]; // MINT::counted_ptr, no need to delete
      }
    }
    daughters.clear();
  }
 public:

  template<typename SORTERTYPE> void sortDaughtersBy(){
    SORTERTYPE sorter;
    stable_sort(daughters.begin(), daughters.end(), sorter);
    //    lessByPointedTo<ValueType> sorter;
    //    daughters.sort(sorter);
  }
  
  template<typename SORTERTYPE> void sortAllBy(){
    sortDaughtersBy<SORTERTYPE>();
    for(unsigned int i=0; i<daughters.size(); i++){
      daughters[i]->template sortDaughtersBy<SORTERTYPE>();
    }
  }

  void sortDaughters(){
    return sortDaughtersBy<lessByPointedTo< MINT::counted_ptr<DDTree<ValueType> > > >();
  }

  bool isFinalState() const{
    return nDgtr() == 0;
  }
  int nDgtr() const{
    return daughters.size();
  }
  void setVal(const ValueType& m){
    _thisVal = m;
  }
  const ValueType& getVal() const{
    return _thisVal;
  }
  ValueType& getVal(){
    return _thisVal;
  }
  const ValueType& getDgtrVal(int i) const{
    return daughters[i]->getVal();
  }
  ValueType& getDgtrVal(int i){
    return daughters[i]->getVal();
  }
  MINT::const_counted_ptr<DDTree<ValueType> > getDgtrTreePtr(int i) const{
    if(i < 0 || i >= nDgtr()){
      return MINT::const_counted_ptr< DDTree<ValueType> >(0);
    }
    return daughters[i];
  }
  MINT::counted_ptr<DDTree<ValueType> > getDgtrTreePtr(int i){
    if(i < 0 || i >= nDgtr()){
      return MINT::counted_ptr<DDTree<ValueType> >(0);
    }
    return daughters[i];
  }

  int setDgtrSize(int i){
    daughters.resize(i);
    return daughters.size();
  }
  void setDgtr(unsigned int i, const MINT::counted_ptr<DDTree<ValueType> >& tr){
    if(i >= daughters.size()) daughters.resize(i+0);
    daughters[i] = tr;
  }

  DDTree() : _parentPtr(0){};

  //DDTree(const std::string& s) 
  //    : _parentPtr(0){
    //std::cout << "called dummy constructor for DDTree from string"
    // << s
    //<< std::endl;
  //}
  DDTree(const ValueType& pdg_id) 
    : _thisVal(pdg_id)
    , _parentPtr(0){
    daughters.clear();
  }
  DDTree(const DDTree<ValueType>* parent) : _parentPtr(parent){}; 
  DDTree(const ValueType& val, const DDTree<ValueType>* parent) 
    : _thisVal(val)
    , _parentPtr(parent){};

  bool hasParent() const{
    return ! (0 == _parentPtr);
  }
  template<typename COMPATIBLE_TYPE>
    MINT::counted_ptr<DDTree<COMPATIBLE_TYPE> > 
    CloneVariableType(DDTree<COMPATIBLE_TYPE>* parent=0) const{
    
    MINT::counted_ptr<DDTree<COMPATIBLE_TYPE> > 
      top(new DDTree<COMPATIBLE_TYPE>(static_cast<COMPATIBLE_TYPE>(_thisVal)
				      , parent)
	  );
    
    top->setDgtrSize(daughters.size());
    for(unsigned int i=0; i < daughters.size(); i++){
      top->setDgtr(i, daughters[i]->template CloneVariableType<COMPATIBLE_TYPE>(top.get()));
    }
    return top;
  }
  MINT::counted_ptr<DDTree<ValueType> > 
    Clone(DDTree<ValueType>* parent=0) const{
    return CloneVariableType<ValueType>(parent);
  }

  template<typename COMPATIBLE_ITEM>
    void Copy(const DDTree<COMPATIBLE_ITEM>& other
	      ){
    _thisVal = (ValueType) (other.getVal());
    daughters.resize(other.nDgtr());
    for(int i=0; i < other.nDgtr(); i++){
      daughters[i] = other.getDgtrTreePtr(i)->template CloneVariableType<ValueType>(this);
    }
    return;
  }

  DDTree(const DDTree<ValueType>& other)
    :_parentPtr(0){
    this->Copy(other);
  }

  template<typename COMPATIBLE_ITEM>
    DDTree(const DDTree<COMPATIBLE_ITEM>& other)
    : _parentPtr(0){
    this->Copy(other);
  }
  DDTree<ValueType> operator=(const DDTree<ValueType>& other){
    this->Copy(other);
    return *this;
  }
  
  template<typename COMPATIBLE_ITEM>
  DDTree<ValueType> operator=(const DDTree<COMPATIBLE_ITEM>& other){
    this->Copy(other);
    return *this;
  }

  MINT::counted_ptr<DDTree<ValueType> > 
    addDgtr(const DDTree<ValueType>* treePtr){
    MINT::counted_ptr< DDTree<ValueType> > newDgtr(treePtr->Clone(this));
    daughters.push_back(newDgtr);
    //sortDaughters();
    return newDgtr;
  }
  MINT::counted_ptr<DDTree<ValueType> > addDgtr(const ValueType& pdg_id){
    MINT::counted_ptr< DDTree<ValueType> > dgtrTree(new DDTree(pdg_id, this));
    daughters.push_back(dgtrTree);
    //sortDaughters();
    return dgtrTree;
  }
  MINT::counted_ptr<DDTree<ValueType> > 
    addDgtr(const ValueType& pdg_id1, const ValueType& pdg_id2){
    addDgtr(pdg_id1);
    //sortDaughters();
    return addDgtr(pdg_id2);
  }
  MINT::counted_ptr<DDTree<ValueType> > addDgtr(const ValueType& pdg_id1
					  , const ValueType& pdg_id2
					  , const ValueType& pdg_id3
					  ){
    addDgtr(pdg_id1);
    addDgtr(pdg_id2);
    return addDgtr(pdg_id3);
  }
  MINT::counted_ptr<DDTree<ValueType> > addDgtr(const ValueType& pdg_id1
					  , const ValueType& pdg_id2
					  , const ValueType& pdg_id3
					  , const ValueType& pdg_id4
					  ){
    addDgtr(pdg_id1);
    addDgtr(pdg_id2);
    addDgtr(pdg_id3);
    return addDgtr(pdg_id4);
  }
  void addDgtrArray(ValueType pdg_id[], int n){
    for(int i=0; i<n; i++){
      addDgtr(pdg_id[i]);
    }
  }
  void addDgtrArray(DDTree<ValueType>* treePtr[], int n){
    for(int i=0; i<n; i++){
      addDgtr(treePtr[i]);
    }
  }
  virtual ~DDTree(){
    disconnectAllDaughters();
  }

  const DDTree<ValueType>* getParent() const{
    return _parentPtr;
  }

  int nSisters() const{
    if(0 == _parentPtr) return 0;    
    return _parentPtr->nDgtr() -1;
  }

  std::vector<MINT::const_counted_ptr<DDTree<ValueType> > > 
    getSistersTrees() const{ 
    // i.e. parent's daughters,except yourself
    std::vector<MINT::const_counted_ptr< DDTree<ValueType> > > sistersTree;
    if(0 == _parentPtr) return sistersTree;
    
    for(int i=0; i < _parentPtr->nDgtr(); i++){
      if(_parentPtr->daughters[i].get() != this){
	sistersTree.push_back(_parentPtr->daughters[i]);
      }
    }
    return sistersTree;
  }
  int nGenerations() const{
    int nMax=0;
    if(! nDgtr()) return 1;
    for(int i=0; i<nDgtr(); i++){
      int n = daughters[i]->nGenerations();
      if(n > nMax) nMax = n;
    }
    return nMax + 1;
  }

  std::vector<const ValueType*> finalState() const{
    std::vector<const ValueType*> listOfStableDaughters;
    finalState(listOfStableDaughters);
    return listOfStableDaughters;
  }
  std::vector<ValueType*> finalState(){
    std::vector<ValueType*> listOfStableDaughters;
    finalState(listOfStableDaughters);
    return listOfStableDaughters;
  }

  MINT::counted_ptr<std::vector<const ValueType*> > finalStatePtr() const{
    MINT::counted_ptr<std::vector<const ValueType*> >
      listOfStableDaughtersPtr( new std::vector<const ValueType*> );
    finalState(*listOfStableDaughtersPtr);
    return listOfStableDaughtersPtr;
  }
  MINT::counted_ptr<std::vector<ValueType*> > finalStatePtr(){
    MINT::counted_ptr<std::vector<ValueType*> >
      listOfStableDaughtersPtr( new std::vector<ValueType*> );
    finalState(*listOfStableDaughtersPtr);
    return listOfStableDaughtersPtr;
  }
  void finalState(std::vector<const ValueType*>& theList) const{
    if(daughters.empty()){
      theList.push_back(&_thisVal);
    }else{
      for(int i=0; i<nDgtr(); i++){
	daughters[i]->finalState(theList);
      }
    }
  }
  void finalState(std::vector<ValueType*>& theList){
    if(daughters.empty()){
      theList.push_back(&_thisVal);
    }else{
      for(int i=0; i<nDgtr(); i++){
	daughters[i]->finalState(theList);
      }
    }
  }

  template<typename COMPARABLE_CLASS>
  std::vector<ValueType*> 
    finalStateInThisOrder(const std::vector<COMPARABLE_CLASS>& pattern){
    std::vector<ValueType*> stableDaughters = finalState();

    std::vector<ValueType*> sorted;

    if(stableDaughters.size() != pattern.size()){
      return sorted;
    }

    for(unsigned int i=0; i<pattern.size(); i++){
      for(typename std::vector<ValueType*>::iterator jt = stableDaughters.begin();
	  jt != stableDaughters.end();
	  jt++){
	if( *(*jt) == pattern[i] ){
	  sorted.push_back(*jt);
	  stableDaughters.erase(jt);
	  break;
	}
      }
    }
    return sorted;
  }

  void print(std::ostream& os = std::cout
	     , std::string indentString=""
	     ) const{
    os << " ";
    os << _thisVal << std::endl;
    for(int i=0; i<nDgtr(); i++){
      if(i==nDgtr()-1){
	os << indentString + " '--";
	daughters[i]->print(os, indentString + "    ");
      } else{ 
	os << indentString + " |--";
	daughters[i]->print(os, indentString + " |  ");
      }
    }
  }
  void oneLiner(std::stringstream& seam
		, int generation=0) const{
    seam << _thisVal;
    if(nDgtr() <= 0) return;
    if(generation > 0) seam << "(";
    seam << "->";
    for(int i=0; i<nDgtr(); i++){
      daughters[i]->oneLiner(seam, generation + 1);
      if(i < nDgtr()-1) seam << ",";
    }
    if(generation > 0) seam << ")";
  }
  void oneLiner(std::string& ring) const{
    std::stringstream seam;
    oneLiner(seam);
    seam >> ring;
  }
  std::string oneLiner() const{
    std::string ring;
    oneLiner(ring);
    return ring;
  }

};

template <typename ValueType>
std::ostream& operator<<(std::ostream& os
			 , const DDTree<ValueType>& theTree){
  theTree.print(os);
  return os;
}

template <typename ValueType>
std::stringstream& operator<<(std::stringstream& seam
			      , const DDTree<ValueType>& theTree){
  theTree.oneLiner(seam);
  return seam;
}

#endif
//
