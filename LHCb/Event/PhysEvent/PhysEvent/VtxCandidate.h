// $Id: VtxCandidate.h,v 1.1.1.1 2001-07-09 09:23:58 gcorti Exp $
#ifndef PHYSEVENT_VTXCANDIDATE_H
#define PHYSEVENT_VTXCANDIDATE_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "LHCbEvent/CLHEPStreams.h"
#include "LHCbEvent/Definitions.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"

// Class ID
static const CLID& CLID_VtxCandidate = 850;

// Forward Declarations
class AxPartCandidate;

/** @class VtxCandidate VtxCandidate.h PhysEvent/VtxCandidate.h
 *  
 *  Transient Class for Vertices Candidates to be used in Physics Analysis.
 *  It represents a possible vertex between "n" particles.
 *  The class VtxCandidate uses the Class Library for HEP
 * <A HREF="http://wwwinfo.cern.ch/asd/lhc++/clhep/manual/RefGuide/index.html">CLHEP</A>
 *
 *  @author Gloria Corti
 *  @date   28/05/2001
 */

class VtxCandidate  : virtual public ContainedObject {

public:

  /// Standard Constructor
  VtxCandidate() 
    : m_position(0.0, 0.0, 0.0),
      m_chiSquare(0.0),
      m_covariance(3,0),
      m_creatorID(0)
  {
  }

  /// make more constructors: with coordinates
  ///                         with coordinates and chi2?  
  ///                         with creator ID?


  /// Standard Destructor
  virtual ~VtxCandidate() { }

  /// Retrieve pointer to class defininition structure
  virtual const CLID& clID() const { return VtxCandidate::classID(); }
  static const CLID& classID()     { return CLID_VtxCandidate; }

  /// Retrieve position
  const HepPoint3D& position () const           { return m_position; }
  HepPoint3D& position ()                       { return m_position; }

  /// Update vertex position
  void setPosition (const HepPoint3D& value)    { m_position = value; }

  /// Retrieve ChiSquare
  double chiSquare() const                      { return m_chiSquare; }
  /// Set ChiSquare
  void setChiSquare( double value )             { m_chiSquare = value; }

  /** Retrieve error matrix. 
   *  It is a 3x3 symmetric matrix, where the lower triangular elements are
   *  given: (1,1), (2,1), (2,2), (3,1), (3,2), (3,3) 
   * The elements correspond to the position component
   */ 
  const HepSymMatrix& covariance() const             { return m_covariance; }
  HepSymMatrix& covariance()                         { return m_covariance; }

  /// Set the covariance matrix
  void setCovariance( const HepSymMatrix& value )    { m_covariance = value; }

  /// Retrieve pointer to vector of daughter particles (const or non-const)
  const SmartRefVector<AxPartCandidate>& daughters() const;
  SmartRefVector<AxPartCandidate>& daughters();

  /// Update all daughter particles
  void setDaughters( const SmartRefVector<AxPartCandidate>& value );

  /// Remove all daughter particles
  void removeDaughters();

  /** Add single daughter particle to vector of daughter particles.
   *   (by a C++ pointer or a smart reference)
   */
  inline void addDaughter( AxPartCandidate* value );
  inline void addDaughter( SmartRef<AxPartCandidate> value );

  /** Retrieve CreatorID. 
   *  For now a vertexer type:
   *  1 = primary, 2 = secondary, but it could/should hold more info
   */
  long creatorID() const                             { return m_creatorID; }

  /// Set CreatorID
  void setCreatorID( long value )                    { m_creatorID = value; }

  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );
  /// Fill the ASCII output stream
  virtual std::ostream& fillStream( std::ostream& s ) const;


private:
  
  HepPoint3D                      m_position;   ///< Position  
  double                          m_chiSquare;  ///< ChiSquare of vertex fit
  
  HepSymMatrix                    m_covariance; ///< Covariance matrix 
  SmartRefVector<AxPartCandidate> m_daughters;  ///< Vector of pointers to daughter particles
  long                            m_creatorID;  ///< Creator ID/type
};

///
/// Inline code
///
#include "LHCbEvent/AxPartCandidate.h"

/// Retrieve pointer to vector of daughter particles
inline const SmartRefVector<AxPartCandidate>& VtxCandidate::daughters() const { 
  return m_daughters; 
}
inline SmartRefVector<AxPartCandidate>& VtxCandidate::daughters() { 
  return m_daughters; 
}

/// Update all daughter particles
inline void VtxCandidate::setDaughters( 
                          const SmartRefVector<AxPartCandidate>& value ) { 
  m_daughters = value; 
}

/// Add single daughter particle to vector of daughters 
///   (by a C++ pointer or a smart reference)
inline void VtxCandidate::addDaughter( AxPartCandidate* value ) { 
  m_daughters.push_back(value); 
}
inline void VtxCandidate::addDaughter( SmartRef<AxPartCandidate> value ) { 
  m_daughters.push_back(value); 
}

/// Remove all daughter particles
inline void VtxCandidate::removeDaughters() { 
  m_daughters.clear(); 
}

/// Serialize the object for writing
inline StreamBuffer& VtxCandidate::serialize( StreamBuffer& s ) const {
  ContainedObject::serialize(s);
  return s
    << m_position
    << m_chiSquare
    << m_covariance
    << m_daughters(this)
    << m_creatorID;
}

/// Serialize the object for reading
inline StreamBuffer& VtxCandidate::serialize( StreamBuffer& s ) {
  ContainedObject::serialize(s);
  return s
    << m_position
    << m_chiSquare
    << m_covariance
    << m_daughters(this)
    << m_creatorID;
}

/// Fill the output stream (ASCII)
inline std::ostream& VtxCandidate::fillStream( std::ostream& s ) const {
  s << "class VtxCandidate :"
    << "\n    Position (x, y, z)      = ( "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_position.x() << ", "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_position.y() << ", "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_position.z() << " )"
    << "\n    ChiSquare               = "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_chiSquare
    << "\n    Covariance Matrix (3,3) = ";
  int  i, j, nrow = m_covariance.num_row();
  for( i=1; i<=nrow; i++ ) {
    s << "\n      ";
    for( j=1; j<=i; j++ ) {
      s << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
        << m_covariance(i,j) << "  ";
    }
  }
  s << "\n     Creator ID = "
    << LHCbEventField( LHCbEvent::field12 )
    << m_creatorID;
  return s;
}

  
// Definition of container types of VtxCandidate
template <class TYPE> class ObjectVector;
typedef ObjectVector<VtxCandidate>     VtxCandidateVector;
template <class TYPE> class ObjectList;
typedef ObjectList<VtxCandidate>       VtxCandidateList;


#endif    // PHYSEVENT_VTXCANDIDATE_H
