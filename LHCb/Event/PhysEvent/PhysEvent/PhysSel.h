// $Id: PhysSel.h,v 1.2 2001-07-16 15:05:06 gcorti Exp $
#ifndef PHYSEVENT_PHYSSEL_H
#define PHYSEVENT_PHYSSEL_H 1

// Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StreamBuffer.h"
#include "PhysEvent/PhysSelDecay.h"
#include "LHCbEvent/Definitions.h"
#include <iostream>
#include <vector>
#include <map>

// Class ID
static const CLID& CLID_PhysSel = 860;

/** @class PhysSel PhysSel.h PhysEvent/PhysSel.h
 *
 *  Stores the results of the decay selection package
 *  It can be identified by "/Event/PhysEvent/PhysSel"
 *
 *              It contains:
 *              - map of PhysSelDecay with results for each decay 
 *                selection channel examined
 *
 * @version 1
 * @author  G.Corti
 * @date    19/05/2001  
 */

class PhysSel : public DataObject {

public:
  /// Constructors
  PhysSel( const char* name = "PhysSel" ) : DataObject(name), m_table() {
    setVersion(1);
  }

  /// Destructor
  ~PhysSel();

  /// Retrieve reference to class definition structure
  virtual const CLID& clID() const {
    return PhysSel::classID(); 
  }
  static const CLID& classID() {
    return CLID_PhysSel; 
  }

  /// Remove all PhysSelDecays entries
  void removePhysSelDecays();

  /// Add single PhysSelDecay entry to map.
  void addPhysSelDecay( std::string name, PhysSelDecay* value );

  /// Remove single PhysSelDecay entry from vector
  void removePhysSelDecay( std::string name );
  void removePhysSelDecay( PhysSelDecay* );

  /// Retrieve list of names for which a PhysSelDecay has been
  /// stored
  void whichPhysSelDecays( std::vector<std::string>& nameList );

  /// Retrieve the PhysSelDecay results for a decay channel 
  /// specifying its' "name" as a string
  void retrievePhysSelDecay( std::string name, PhysSelDecay*& value );

  /// Retrieve results as a set of true/false for a specific
  /// decay channel specifying its' "name" as a string
  void results( std::string name, bool& mc, bool& tkr, bool& pqual, 
                                  bool& sel, bool& agr, bool& tag );

  /// Retrieve MC true result for a specific decay channel
  bool decayIsInMCTree( std::string name );
  /// Retrieve "acceptance" result for a specific decay channel
  bool decayHasTrkRecon( std::string name );
  /// Retrieve "physics quality" result for a specific decay channel
  bool decayHasTrkPQual( std::string name );
  /// Retrieve selection result for a specific decay channel
  bool decayIsSelected( std::string name );
  /// Retrieve agreement results  for a specific decay channel
  bool decaySelectedIsMC( std::string name );
  /// Retrieve flavour tag results  for a specific decay channel
  bool decayIsFlavourTagged( std::string name );


  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );

  /// Output operator (ASCII)
  friend std::ostream& operator<< ( std::ostream& s, const PhysSel& obj ) {
    return obj.fillStream(s);
  }
  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream( std::ostream& s ) const;

private:

  typedef std::map< std::string, PhysSelDecay* > SelTable;
   
  SelTable  m_table;        ///< Map with decay channels selection results

};


//
// Inline code must be outside the class definition
//


/// Serialize the object for writing
inline StreamBuffer& PhysSel::serialize( StreamBuffer& s ) const {
  DataObject::serialize(s);
  s << m_table.size();
  SelTable::const_iterator iter;
  for( iter = m_table.begin(); iter != m_table.end(); iter++ ) {
    s << (*iter).first
      << *((*iter).second);
  }
  return s;
}


/// Serialize the object for reading
inline StreamBuffer& PhysSel::serialize( StreamBuffer& s ) {
  DataObject::serialize(s);
  
  SelTable::size_type siz;
  s >> siz;

  for( long i = 0; i < (long)siz; i++ ) {
    std::string selCode;
    PhysSelDecay* pSelDecay = new PhysSelDecay;
    s >> selCode, pSelDecay;
    m_table[selCode] = pSelDecay;
  }

  return s;
}


/// Fill the ASCII output stream
inline std::ostream& PhysSel::fillStream( std::ostream& s ) const {
  s << "class PhysSel :\n";
  
  SelTable::size_type    siz = m_table.size();
  if( 0 != siz ) {
    s << "\nSize of the PhysSel results table :"
      << LHCbEventField( LHCbEvent::field4 )
      << siz;
    long count = 0;
    SelTable::const_iterator iter;
    for( iter = m_table.begin(), count = 0;
              iter!=m_table.end();
                    iter++, count++ ) {
      s << "\nIndex "
        << LHCbEventField( LHCbEvent::field4 )
        << count
        << " of decay code " << (*iter).first
        << " bitset result " << (*iter).second;
    }    
  }
  return s;
}

#endif    // PHYSEVENT_PHYSSEL_H
