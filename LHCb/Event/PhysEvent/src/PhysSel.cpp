// $Id : $

// Include files
#include "PhysEvent/PhysSel.h"

//-------------------------------------------------------------------
// Implementation file for class: PhysSel
//
// 10/06/2001 : Gloria Corti
//-------------------------------------------------------------------

//===================================================================
// Destructor
//===================================================================
PhysSel::~PhysSel() { 
    removePhysSelDecays();
  }


//=======================================================================
// Remove all PhysSelDecays entries in map 
//=======================================================================
void PhysSel::removePhysSelDecays() { 

  // Delete the PhysSelDecays and clear the map
  SelTable::iterator it;
  for( it=m_table.begin(); it!=m_table.end(); it++ ) {
    delete (*it).second;
  }
  
  m_table.clear(); 
}

//=======================================================================
// Add single PhysSelDecay entry to map. 
// Either the Algorithm or the Converter need to create the
// PhysSelDecay object but afterward it belong to the PhysSel class
//=======================================================================
void PhysSel::addPhysSelDecay( std::string decayName, PhysSelDecay* value ) {
  m_table.insert( SelTable::value_type( decayName, value ) );
}

//=======================================================================
// Remove single PhysSelDecay entry from vector
//   This function also delete the object "value" points to
//=======================================================================
void PhysSel::removePhysSelDecay( std::string decayName) {
 
  SelTable::iterator isel = m_table.find( decayName );
  if( isel != m_table.end() ) { 
    m_table.erase(isel);
    delete (*isel).second;
  }

}

//=======================================================================
// Retrieve all results of selection routine for specified decay channel
//=======================================================================
void PhysSel::results( std::string name, bool& mc, bool& tkr, bool& pqual, 
                           bool& sel, bool& agr, bool& tag ) {

  PhysSelDecay* pSelDecay = 0;
  retrievePhysSelDecay( name, pSelDecay );
  if( 0 != pSelDecay ) {
    pSelDecay->results( mc, tkr, pqual, sel, agr, tag );
  }
  else {
    mc = false;
    tkr = false;
    pqual = false;
    sel = false;
    agr = false;
    tag = false;
  }

  return;

}
  
//=======================================================================
// Return if the specified decay channel is present in MC tree
//=======================================================================
bool PhysSel::decayIsInMCTree( std::string name ) {

  bool mc = false;
  PhysSelDecay* pSelDecay = 0;
  retrievePhysSelDecay( name, pSelDecay );
  if( 0 != pSelDecay ) {
    mc = pSelDecay->decayIsInMCTree();
  }
  return mc;

}

//=======================================================================
// Return if the specified decay channel has all MC end particles 
// reconstructed
//=======================================================================
bool PhysSel::decayHasTrkRecon( std::string name ) {

  bool tkr = false;
  PhysSelDecay* pSelDecay = 0;
  retrievePhysSelDecay( name, pSelDecay );
  if( 0 != pSelDecay ) {
    tkr = pSelDecay->decayHasTrkRecon();
  }
  return tkr;

}

//=======================================================================
// Return if the specified decay channel has all MC end particles 
// reconstructed and of physics quality
//=======================================================================
bool PhysSel::decayHasTrkPQual( std::string name ) {
  
  bool pqual = false;
  PhysSelDecay* pSelDecay = 0;
  retrievePhysSelDecay( name, pSelDecay );
  if( 0 != pSelDecay ) {
    pqual = pSelDecay->decayHasTrkPQual();
  }
  return pqual;

}

//=======================================================================
// Return if the event contain a combination satisfying the selection
// criteria for the specified decay channel
//=======================================================================
bool PhysSel::decayIsSelected( std::string name ) {
  
  bool sel = false;
  PhysSelDecay* pSelDecay = 0;
  retrievePhysSelDecay( name, pSelDecay );
  if( 0 != pSelDecay ) {
    sel = pSelDecay->decayIsSelected();
  }
  return sel;

}

//=======================================================================
// Return if a combination satisfying the selection
// criteria for the specified decay channel is the "MC true" 
//=======================================================================
bool PhysSel::decaySelectedIsMC( std::string name ) {
  
  bool agr = false;
  PhysSelDecay* pSelDecay = 0;
  retrievePhysSelDecay( name, pSelDecay );
  if( 0 != pSelDecay ) {
    agr = pSelDecay->decaySelectedIsMC();
  }
  return agr;

}

//=======================================================================
// Return if a combination satifying the selection criteria for
// the specified decay channel would also have a flavour tag
//=======================================================================
bool PhysSel::decayIsFlavourTagged( std::string name ) {
  
  bool ftag = false;
  PhysSelDecay* pSelDecay = 0;
  retrievePhysSelDecay( name, pSelDecay );
  if( 0 != pSelDecay ) {
    ftag = pSelDecay->decayIsFlavourTagged();
  }
  return ftag;

}


//=======================================================================
// Retrieve PhysSelDecay, specifying the stringID
//=======================================================================
void PhysSel::retrievePhysSelDecay( std::string name, PhysSelDecay*& value) {

  // value = m_table[decayName];
  SelTable::iterator isel = m_table.find( name );
  if( isel != m_table.end() ) { 
    value = (*isel).second;
  }
  else {
    value = 0;
  }

}

//======================================================================
// Retrieve list of decay names stored
//======================================================================
void PhysSel::whichPhysSelDecays( std::vector<std::string>& nameList) {

  nameList.clear();  
  for( SelTable::iterator isel=m_table.begin(); isel!=m_table.end(); isel++ ) {
    std::string tmpName = (*isel).first;
    nameList.push_back(tmpName);
  }
  
}
