

//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************



#ifndef PhysEvent_VtxCandidate_H
#define PhysEvent_VtxCandidate_H 1

// Include files
#include <algorithm>
#include "LHCbEvent/CLHEPStreams.h"
#include "GaudiKernel/ContainedObject.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"


// Forward declarations
class AxPartCandidate;


// Class ID definition
  static const CLID& CLID_VtxCandidate = 850;

/** @class VtxCandidate VtxCandidate.h 
 *
 *  Vertex to be used in Physics Analysis. It represents a possible vertex between n particles. 
 *
 *  @author Gloria Corti
 *  created Mon Feb  4 20:21:56 2002
 *
 */

class VtxCandidate: public ContainedObject
{

public: 

  /// Default Constructor 
  VtxCandidate() 
    : m_position(0.0,0.0,0.0),
    m_chiSquare(0.0),
    m_covariance(3,0),
    m_creatorID(0) {}

  /// Destructor 
  virtual ~VtxCandidate() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Retrieve position
  const HepPoint3D& position() const; 

  /// Update position
  void setPosition(const HepPoint3D& value);

  /// Retrieve ChiSquare of vertex fit
  double chiSquare() const; 

  /// Update ChiSquare of vertex fit
  void setChiSquare(double value);

  /// Retrieve Covariance matrix (3x3)
  const HepSymMatrix& covariance() const; 

  /// Update Covariance matrix (3x3)
  void setCovariance(const HepSymMatrix& value);

  /// Retrieve Creator ID/type 1=primary
  long creatorID() const; 

  /// Update Creator ID/type 1=primary
  void setCreatorID(long value);

  /// Retrieve Reference to daughter particles
  const SmartRefVector<AxPartCandidate>& daughters() const;

  /// Update Reference to daughter particles
  void setDaughters(const SmartRefVector<AxPartCandidate>& value);

  /// Add Reference to daughter particles
  void addToDaughters(AxPartCandidate* value);
  void addToDaughters(const SmartRef<AxPartCandidate>& value); 

  /// Remove Reference to daughter particles
  void removeFromDaughters(AxPartCandidate* value);
  void removeFromDaughters(const SmartRef<AxPartCandidate>& value); 

  /// Clear Reference to daughter particles
  void clearDaughters();

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;


protected: 


private: 

  HepPoint3D m_position; ///<         position
  double m_chiSquare; ///<         ChiSquare of vertex fit
  HepSymMatrix m_covariance; ///<         Covariance matrix (3x3)
  long m_creatorID; ///<         Creator ID/type 1=primary
  SmartRefVector<AxPartCandidate> m_daughters; ///<         Reference to daughter particles

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


// Including forward declarations
#include "LHCbEvent/AxPartCandidate.h"


inline const CLID& VtxCandidate::clID() const 
{
  return VtxCandidate::classID();
}

inline const CLID& VtxCandidate::classID()
{
  return CLID_VtxCandidate;
}

inline const HepPoint3D& VtxCandidate::position() const 
{
  return m_position;
}

inline void VtxCandidate::setPosition(const HepPoint3D& value)
{
  m_position = value; 
}

inline double VtxCandidate::chiSquare() const 
{
  return m_chiSquare;
}

inline void VtxCandidate::setChiSquare(double value)
{
  m_chiSquare = value; 
}

inline const HepSymMatrix& VtxCandidate::covariance() const 
{
  return m_covariance;
}

inline void VtxCandidate::setCovariance(const HepSymMatrix& value)
{
  m_covariance = value; 
}

inline long VtxCandidate::creatorID() const 
{
  return m_creatorID;
}

inline void VtxCandidate::setCreatorID(long value)
{
  m_creatorID = value; 
}

inline const SmartRefVector<AxPartCandidate>& VtxCandidate::daughters() const
{
   return m_daughters;
}

inline void VtxCandidate::setDaughters(const SmartRefVector<AxPartCandidate>& value)
{
   m_daughters = value;
}

inline void VtxCandidate::addToDaughters(AxPartCandidate* value)
{
   m_daughters.push_back(value);
}

inline void VtxCandidate::addToDaughters(const SmartRef<AxPartCandidate>& value)
{
   m_daughters.push_back(value);
}

inline void VtxCandidate::removeFromDaughters(AxPartCandidate* value)
{
  SmartRefVector<AxPartCandidate>::iterator iter;
  for (iter = m_daughters.begin(); iter != m_daughters.end(); ++iter)
  {
    if (iter->target() == value)
    {
      m_daughters.erase(iter);
    }
  }
}

inline void VtxCandidate::removeFromDaughters(const SmartRef<AxPartCandidate>& value)
{
  SmartRefVector<AxPartCandidate>::iterator iter =
    std::find(m_daughters.begin(), m_daughters.end(), value);
  if (iter != m_daughters.end() )
  {
    m_daughters.erase(iter);
  }
}

inline void VtxCandidate::clearDaughters()
{
   m_daughters.clear();
}

inline StreamBuffer& VtxCandidate::serialize(StreamBuffer& s) const 
{
  s << m_position
    << (float)m_chiSquare
    << m_covariance
    << m_creatorID
    << m_daughters(this);
  return s;
}

inline StreamBuffer& VtxCandidate::serialize(StreamBuffer& s)
{
  float l_chiSquare;
  s >> m_position
    >> l_chiSquare
    >> m_covariance
    >> m_creatorID
    >> m_daughters(this);
  m_chiSquare = l_chiSquare;
  return s;
}

inline std::ostream& VtxCandidate::fillStream(std::ostream& s) const
{
  s << "{ "
    << " position:\t" << m_position << std::endl
    << "   chiSquare:\t" << (float)m_chiSquare << std::endl
    << "   covariance:\t" << m_covariance << std::endl
    << "   creatorID:\t" << m_creatorID << " } ";
  return s;
}

// Defintion of vector container type for VtxCandidate
template <class TYPE> class ObjectVector;
typedef ObjectVector<VtxCandidate> VtxCandidateVector;
// Defintion of all list container types for VtxCandidate
template <class TYPE> class ObjectList;
typedef ObjectList<VtxCandidate> VtxCandidateList;

#endif   ///PhysEvent_VtxCandidate_H
