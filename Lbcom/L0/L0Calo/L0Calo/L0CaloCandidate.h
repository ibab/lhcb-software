#ifndef L0CALO_L0CALOCANDIDATE_H
#define L0CALO_L0CALOCANDIDATE_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/L0Calo/L0CaloCandidate.h,v 1.4 2001-05-10 14:44:32 ocallot Exp $

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRefVector.h"
#include "CLHEP/Geometry/Point3D.h"

#include "CaloGen/CaloCellID.h"

/** @namespace L0
 *
 *  L0Type describes the type of candidate.
 */
namespace L0 {
  enum L0Type {
    Electron = 1,
    Photon,
    Hadron,
    Hadron2,
    SumEt,
    Pi0Local,
    Pi0Global,
  };
};


const CLID& CLID_L0CaloCandidate  = 6001 ;

/** @class L0CaloCandidate L0CaloCandidate.h L0Calo/L0CaloCandidate.h
 *
 *  This class represents the input from the calorimeter to the L0 trigger
 *
 * @author Olivier Callot
 * @date   10 October 2000
 */
class L0CaloCandidate : public ContainedObject {

public:

/** Constructors has many arguments
 *
 * @param type : type of candidate, from L0::L0Type
 * @param ID   : Cell ID to tag the candidate's position
 * @param etCode : Integer value for Et
 * @param et     : float value of Et, converted from the integer value
 * @param cellCenter : position of the 2x2 center
 * @param cellSize   : Dimension of a cell
 */
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


  virtual const CLID& clID()    const  { return L0CaloCandidate::classID();  }
  static  const CLID& classID()        { return CLID_L0CaloCandidate;  }

/// Returns the type as integer
  int    type()                 const  { return m_type; }

/// Returns the type of candidate as a string, for printing
  std::string typeName()        const  { 
    if ( L0::Electron    == m_type ) { return "electron"; }
    if ( L0::Photon      == m_type ) { return "photon  "; }
    if ( L0::Hadron      == m_type ) { return "hadron  "; }
    if ( L0::Hadron2     == m_type ) { return "hadron_2"; }
    if ( L0::Pi0Local    == m_type ) { return "Pi0 loc "; }
    if ( L0::Pi0Global   == m_type ) { return "Pi0 glob"; }
    if ( L0::SumEt       == m_type ) { return "Sum Et  "; }
    return "unknown";
  }

// ** various getters.

  CaloCellID        ID()       const { return m_ID;       } ///< Cell ID
  int               etCode()   const { return m_etCode;   } ///< E.Trans.  int
  double            et()       const { return m_et;       } ///< E.Trans. (GeV)
  const HepPoint3D& position() const { return m_position; } ///< Position
  double            posTol()   const { return m_posTol;   } ///< cell size

/// Serialize the object, for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );

/// Serialize the object, for write
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;

/// Print the object on the MsgStream
  virtual MsgStream&    fillStream( MsgStream&   s ) const ;

private:
  int         m_type;          ///< Type of trigger, from L0:L0Type
  CaloCellID  m_ID;            ///< Calo Cell ID. To access MC information...
  int         m_etCode;        ///< integer version of Et.
  double      m_et;            ///< Transverse energy
  HepPoint3D  m_position;      ///< Position
  double      m_posTol;        ///< Tolerance on x/y position = 1/2 cell size
};


// Serialize the object for reading
inline StreamBuffer& L0CaloCandidate::serialize(StreamBuffer& s) {
  double x, y, z;
  ContainedObject::serialize(s) >> m_type 
				>> m_ID 
				>> m_etCode 
				>> m_et 
				>> x 
				>> y 
				>> z
				>> m_posTol;
  m_position.setX( x );
  m_position.setY( y );
  m_position.setZ( z );
  return s;
}

// Serialize the object for writing
inline StreamBuffer& L0CaloCandidate::serialize(StreamBuffer& s) const {
  ContainedObject::serialize(s) << m_type 
				<< m_ID 
				<< m_etCode 
				<< m_et 
				<< m_position.x() 
				<< m_position.y() 
				<< m_position.z() 
				<< m_posTol;
  return s;
}

inline MsgStream& operator<<( MsgStream& ms, const L0CaloCandidate& cand) {
  return cand.fillStream( ms );
}

inline MsgStream& operator<<( MsgStream& ms, const L0CaloCandidate* cand) {
  if ( 0 != cand ) {
    ms << *cand;
  } else {
    ms << "L0CaloCandidate* points to null";
  }
  return ms;
}

// Object printing on MsgStream
inline MsgStream& L0CaloCandidate::fillStream( MsgStream& os ) const {
    os << "L0Calo " << typeName() << " Et(GeV)";
    char line[80];
    sprintf( line, "%4d %6.2f ",  m_etCode,  m_et/GeV );
    os << line;
    if ( L0::SumEt != m_type ) {
      os << " ID " << m_ID << " x,y,z(cm)";
      sprintf( line, "%7.1f %7.1f %7.1f Error(cm) %5.2f",
	       m_position.x()/centimeter,
	       m_position.y()/centimeter,
	       m_position.z()/centimeter,
	       m_posTol/centimeter         );
      os << line;
    }
    return os ;
}


template <class TYPE> class ObjectVector;
typedef  ObjectVector<L0CaloCandidate> L0CaloCandidateVector;

#endif // L0CALO_L0CALOCANDIDATE_H
