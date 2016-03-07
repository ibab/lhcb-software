// Include files 
#include <algorithm>


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
  for (auto  i = pl.begin(); i != pl.end() ; ++i ){
  	insert(make_pair(i->first,i->second->new_copy()));
    //(*this)[i->first] = i->second->new_copy();
  }
  return *this;
}
ParamList& ParamList::operator+= (const ParamList &pl){
  for ( auto i = pl.begin(); i != pl.end() ; ++i ){
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
  std::for_each(begin(),end(),[](const std::pair<const std::string, BasicParam*>& i) 
                { delete i.second; } );
}

//=============================================================================
// return a vector containing all the stored keys
//=============================================================================
std::vector<std::string> ParamList::getKeys() const {
  std::vector<std::string> v; v.reserve(size());
  std::transform(begin(),end(),std::back_inserter(v),
                 [](const std::pair<const std::string,BasicParam*>& i) {
        return i.first;
  });
  return v;
}

//=============================================================================
