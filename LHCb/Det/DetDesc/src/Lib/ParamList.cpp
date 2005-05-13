// $Id: ParamList.cpp,v 1.1 2005-05-13 16:01:10 marcocle Exp $
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
ParamList::ParamList(){}

//=============================================================================
// Copy constructor
//=============================================================================
ParamList::ParamList(const  ParamList&pl):map_type(){*this = pl;}

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
	iterator old = find(i->first);
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
  map_type::clear();
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
