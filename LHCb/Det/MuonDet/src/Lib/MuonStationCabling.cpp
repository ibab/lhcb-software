// $Id: MuonStationCabling.cpp,v 1.3 2007-02-27 11:13:57 asatta Exp $
// Include files 



// local
#include "MuonDet/MuonStationCabling.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Implementation file for class : MuonStationCabling
//
// 2004-01-07 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonStationCabling::MuonStationCabling(  ) {
  m_numberOfL1Board=0;
  
}
//=============================================================================
// Destructor
//=============================================================================
MuonStationCabling::~MuonStationCabling() {}

//=============================================================================
 StatusCode MuonStationCabling::update(long l1numb){
  m_numberOfL1Board=l1numb;
  return StatusCode::SUCCESS;
}

StatusCode MuonStationCabling::addL1Name(std::string name)
{

  if((int)m_listOfL1.size()<= m_numberOfL1Board){
    m_listOfL1.push_back(name);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;

  }
}

/// update constructor, do a deep copy of all
/// except for the properties of a generic DataObject
void MuonStationCabling::update( Condition& obj ){
  Condition::update((Condition&)obj);
  MuonStationCabling* obj1=dynamic_cast<MuonStationCabling*> (&obj);
  m_numberOfL1Board=obj1->getNumberOfL1Board();
  m_listOfL1.clear();
  m_listOfL1=obj1->getAllL1Names();
  
}

/// update constructor, do a deep copy of all
/// except for the properties of a generic DataObject
void MuonStationCabling::update( ValidDataObject& obj ){
Condition& cc=dynamic_cast<Condition&>(obj);
 Condition::update((Condition&)cc);
MuonStationCabling* obj1=dynamic_cast<MuonStationCabling*> (&obj);
  m_numberOfL1Board=obj1->getNumberOfL1Board();
  m_listOfL1.clear();
  m_listOfL1=obj1->getAllL1Names();
  
}


//long MuonStationCabling::getNumberOfL1Board(){
//std::cout<<"ciao "<<m_numberOfL1Board<<std::endl;
//return m_numberOfL1Board;
//};

