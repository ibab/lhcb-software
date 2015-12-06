// $Id: ParamList.cpp,v 1.2 2005-11-17 16:30:17 marcocle Exp $
// Include files 



// local
#include "DetDesc/ParamList.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParamList
//
// 2005-02-22 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParamList::ParamList():base_type(){}

//=============================================================================
// Copy constructor
//=============================================================================
ParamList::ParamList(const  ParamList&pl):base_type(){*this = pl;}

//=============================================================================
// Destructor
//=============================================================================
ParamList::~ParamList(){deleteItems();}

//=============================================================================
// Assignement
//=============================================================================
ParamList& ParamList::operator= (const ParamList &pl){
  clear();
  const_iterator i;
  for ( i = pl.begin(); i != pl.end() ; ++i ){
  	insert(make_pair(i->first,i->second->new_copy()));
    //(*this)[i->first] = i->second->new_copy();
  }
  return *this;
}
ParamList& ParamList::operator+= (const ParamList &pl){
  const_iterator i;
  for ( i = pl.begin(); i != pl.end() ; ++i ){
	auto old = find(i->first);
    if ( old != end() ) { // key already used
      delete old->second;
      old->second = i->second->new_copy();
    } else {
  	  insert(make_pair(i->first,i->second->new_copy()));
      //(*this)[i->first] = i->second->new_copy();
    }
  }
  return *this;
}

//=============================================================================
// Clear the list
//=============================================================================
void ParamList::clear(){
  deleteItems();
  base_type::clear();
}

//=============================================================================
// Delete the object referenced by the stored pointers
//=============================================================================
void ParamList::deleteItems(){
  iterator i;
  for (i = begin(); i != end(); ++i){
    delete i->second;
  }
}

//=============================================================================
// return a vector containing all the stored keys
//=============================================================================
std::vector<std::string> ParamList::getKeys() const {
  std::vector<std::string> v;
  const_iterator i;
  for (i = begin(); i != end(); ++i){
    v.push_back(i->first);
  }
  return v;
}


//=============================================================================
