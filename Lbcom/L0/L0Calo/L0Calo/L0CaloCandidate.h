#ifndef L0CALOCANDIDATE_H
#define L0CALOCANDIDATE_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "CLHEP/Geometry/Point3D.h"

#include "CaloGen/CaloCellID.h"

namespace L0 {
  enum L0Type {
    Electron = 1,
    Photon,
    Hadron,
    Hadron2,
    SumEt,
    Pi0Local,
    Pi0Global  };
};


const CLID& CLID_L0CaloCandidate  = 6001 ;

//-----------------------------------------------------------------------------
//
// ClassName:   L0CaloCandidate
//
// Description: This class represents the input from the calorimeter to the L0 
//              trigger
//
// Author:      Bruce Hay
// Modified:    Olivier Callot  10 October 2000 : Remove obsolete stuff.
//
//-----------------------------------------------------------------------------
class L0CaloCandidate : public ContainedObject {

public:

  //*** Standard Constructors and destructors

  L0CaloCandidate( int        type, 
		   CaloCellID ID, 
		   int        etCode,
		   double     et, 
		   HepPoint3D cellCenter, 
		   double     cellSize ) :  
    m_type     ( type       ),
    m_ID       ( ID         ),
    m_etCode   ( etCode     ),
    m_et       ( et         ),
    m_position ( cellCenter ),
    m_posTol   ( cellSize   )    { }

  virtual ~L0CaloCandidate()                      {  }

  //*** Retrieve pointer to class definition structure

  virtual const CLID& clID()    const  { return L0CaloCandidate::classID();  }
  static  const CLID& classID()        { return CLID_L0CaloCandidate;  }

  int    type()                 const  { return m_type; }    // Type of trigger
  std::string typeName()        const  { 
    if ( L0::Electron  == m_type ) { return "electron"; }
    if ( L0::Photon    == m_type ) { return "photon  "; }
    if ( L0::Hadron    == m_type ) { return "hadron  "; }
    if ( L0::Hadron2   == m_type ) { return "hadron_2"; }
    if ( L0::Pi0Local  == m_type ) { return "Pi0 loc "; }
    if ( L0::Pi0Global == m_type ) { return "Pi0 glob"; }
    if ( L0::SumEt     == m_type ) { return "Sum Et  "; }
    return "unknown";
  }
  CaloCellID        ID()       const { return m_ID;       } // Cell ID
  int               etCode()   const { return m_etCode;   } // E.Trans.  int
  double            et()       const { return m_et;       } // E.Trans. (GeV)
  const HepPoint3D& position() const { return m_position; } // Position
  double            posTol()   const { return m_posTol;   } // Error on position

//*** Serialize the pbject, for read/write

  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  virtual StreamBuffer& serialize( StreamBuffer& s );

private:
  int         m_type;          // Type of trigger, from L0:L0Type
  CaloCellID  m_ID;            // Calo Cell ID. To access MC information...
  int         m_etCode;        // integer version of Et.
  double      m_et;            // Transverse energy
  HepPoint3D  m_position;      // Position
  double      m_posTol;        // Tolerance on x/y position = 1/2 cell size
};


// Serialize the object for reading
inline StreamBuffer& L0CaloCandidate::serialize(StreamBuffer& s) {
  ContainedObject::serialize(s) >> m_type >> m_ID >> m_etCode >> m_et 
				>> m_position >> m_posTol;
  return s;
}

// Serialize the object for writing
inline StreamBuffer& L0CaloCandidate::serialize(StreamBuffer& s) const {
  ContainedObject::serialize(s) << m_type << m_ID << m_etCode << m_et 
				<< m_position << m_posTol;
  return s;
}

#endif // L0CALOCANDIDATE_H
