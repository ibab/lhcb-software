

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



#ifndef RichRecBase_RichRecPixel_H
#define RichRecBase_RichRecPixel_H 1

// Include files
#include <algorithm>
#include "Kernel/CLHEPStreams.h"
#include "GaudiKernel/ContainedObject.h"
#include "CLHEP/Geometry/Point3D.h"
#include "Event/KeyedObject.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "RichKernel/RichDetectorType.h"
#include "RichRecBase/RichRecPixelParentType.h"
#include "RichKernel/RichSmartID.h"
#include "RichKernel/SerializeStl.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/StreamBuffer.h"


// Forward declarations
class RichRecPhoton;
class RichRecTrack;


// Class ID definition
  static const CLID& CLID_RichRecPixel = 12422;

// Namespace for locations in TDS
namespace RichRecPixelLocation {
  static const std::string& Default = "Rec/Rich/RecPixels";
}


/** @class RichRecPixel RichRecPixel.h 
 *
 *  Rich reconstruction working pixel object
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  created Sat Mar 29 21:58:21 2003
 *
 */

class RichRecPixel: public KeyedObject<int>
{

public: 

  friend class RichRecPixel_dict;

  /// Default constructor
  RichRecPixel()
    : m_parentType(Rich::RecPixel::Unknown)
  {
 
  }

  /// Destructor 
  virtual ~RichRecPixel() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Returns Rich Detector
  Rich::DetectorType detector() const;

  /// Returns unique integer hpd identifier
  int hpd() const;

  /// Returns unique integer Rich panel identifier
  int panel() const;

  /// Retrieve RichSmartID
  const RichSmartID& smartID() const; 

  /// Retrieve RichSmartID (non-const)
  RichSmartID& smartID();

  /// Update RichSmartID
  void setSmartID(const RichSmartID& value);

  /// Retrieve Position in global coordinates
  const HepPoint3D& globalPosition() const; 

  /// Retrieve Position in global coordinates (non-const)
  HepPoint3D& globalPosition();

  /// Update Position in global coordinates
  void setGlobalPosition(const HepPoint3D& value);

  /// Retrieve Position in local coordinates
  const HepPoint3D& localPosition() const; 

  /// Retrieve Position in local coordinates (non-const)
  HepPoint3D& localPosition();

  /// Update Position in local coordinates
  void setLocalPosition(const HepPoint3D& value);

  /// Retrieve Current value for a working estimate of the background in this pixel
  float currentBackground() const; 

  /// Update Current value for a working estimate of the background in this pixel
  void setCurrentBackground(float value);

  /// Retrieve Enumeration to Parent type
  const Rich::RecPixel::ParentType& parentType() const; 

  /// Retrieve Enumeration to Parent type (non-const)
  Rich::RecPixel::ParentType& parentType();

  /// Update Enumeration to Parent type
  void setParentType(const Rich::RecPixel::ParentType& value);

  /// Retrieve SmartRefVector of RichRecTracks for which this pixel has valid photon associations (const)
  const SmartRefVector<RichRecTrack>& richRecTracks() const;

  /// Retrieve SmartRefVector of RichRecTracks for which this pixel has valid photon associations (non-const)
  SmartRefVector<RichRecTrack>& richRecTracks();

  /// Update SmartRefVector of RichRecTracks for which this pixel has valid photon associations
  void setRichRecTracks(const SmartRefVector<RichRecTrack>& value);

  /// Add SmartRefVector of RichRecTracks for which this pixel has valid photon associations
  void addToRichRecTracks(const SmartRef<RichRecTrack>& value); 

  /// Remove SmartRefVector of RichRecTracks for which this pixel has valid photon associations
  void removeFromRichRecTracks(const SmartRef<RichRecTrack>& value); 

  /// Clear SmartRefVector of RichRecTracks for which this pixel has valid photon associations
  void clearRichRecTracks();

  /// Retrieve Parent Object this pixel was produced from (const)
  const ContainedObject* parentPixel() const;

  /// Retrieve Parent Object this pixel was produced from (non-const)
  ContainedObject* parentPixel();

  /// Update Parent Object this pixel was produced from
  void setParentPixel(const SmartRef<ContainedObject>& value);

  /// Retrieve SmartRefVector to associated RichRecPhotons (const)
  const SmartRefVector<RichRecPhoton>& richRecPhotons() const;

  /// Retrieve SmartRefVector to associated RichRecPhotons (non-const)
  SmartRefVector<RichRecPhoton>& richRecPhotons();

  /// Update SmartRefVector to associated RichRecPhotons
  void setRichRecPhotons(const SmartRefVector<RichRecPhoton>& value);

  /// Add SmartRefVector to associated RichRecPhotons
  void addToRichRecPhotons(const SmartRef<RichRecPhoton>& value); 

  /// Remove SmartRefVector to associated RichRecPhotons
  void removeFromRichRecPhotons(const SmartRef<RichRecPhoton>& value); 

  /// Clear SmartRefVector to associated RichRecPhotons
  void clearRichRecPhotons();

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  RichSmartID m_smartID; ///< RichSmartID
  HepPoint3D m_globalPosition; ///< Position in global coordinates
  HepPoint3D m_localPosition; ///< Position in local coordinates
  float m_currentBackground; ///< Current value for a working estimate of the background in this pixel
  Rich::RecPixel::ParentType m_parentType; ///< Enumeration to Parent type
  SmartRefVector<RichRecTrack> m_richRecTracks; ///< SmartRefVector of RichRecTracks for which this pixel has valid photon associations
  SmartRef<ContainedObject> m_parentPixel; ///< Parent Object this pixel was produced from
  SmartRefVector<RichRecPhoton> m_richRecPhotons; ///< SmartRefVector to associated RichRecPhotons

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


// Including forward declarations
#include "Event/RichRecTrack.h"
#include "RichRecPhoton.h"


inline const CLID& RichRecPixel::clID() const 
{
  return RichRecPixel::classID();
}

inline const CLID& RichRecPixel::classID()
{
  return CLID_RichRecPixel;
}

inline Rich::DetectorType RichRecPixel::detector() const
{
   return (Rich::DetectorType)m_smartID.rich();
}

inline int RichRecPixel::hpd() const
{
   return (int)m_smartID.hpdID();
}

inline int RichRecPixel::panel() const
{
   return (int)m_smartID.panelID();
}

inline const RichSmartID& RichRecPixel::smartID() const 
{
  return m_smartID;
}

inline RichSmartID& RichRecPixel::smartID()
{
  return m_smartID;
}

inline void RichRecPixel::setSmartID(const RichSmartID& value)
{
  m_smartID = value; 
}

inline const HepPoint3D& RichRecPixel::globalPosition() const 
{
  return m_globalPosition;
}

inline HepPoint3D& RichRecPixel::globalPosition()
{
  return m_globalPosition;
}

inline void RichRecPixel::setGlobalPosition(const HepPoint3D& value)
{
  m_globalPosition = value; 
}

inline const HepPoint3D& RichRecPixel::localPosition() const 
{
  return m_localPosition;
}

inline HepPoint3D& RichRecPixel::localPosition()
{
  return m_localPosition;
}

inline void RichRecPixel::setLocalPosition(const HepPoint3D& value)
{
  m_localPosition = value; 
}

inline float RichRecPixel::currentBackground() const 
{
  return m_currentBackground;
}

inline void RichRecPixel::setCurrentBackground(float value)
{
  m_currentBackground = value; 
}

inline const Rich::RecPixel::ParentType& RichRecPixel::parentType() const 
{
  return m_parentType;
}

inline Rich::RecPixel::ParentType& RichRecPixel::parentType()
{
  return m_parentType;
}

inline void RichRecPixel::setParentType(const Rich::RecPixel::ParentType& value)
{
  m_parentType = value; 
}

inline const SmartRefVector<RichRecTrack>& RichRecPixel::richRecTracks() const
{
   return m_richRecTracks;
}

inline SmartRefVector<RichRecTrack>& RichRecPixel::richRecTracks()
{
   return m_richRecTracks;
}

inline void RichRecPixel::setRichRecTracks(const SmartRefVector<RichRecTrack>& value)
{
   m_richRecTracks = value;
}

inline void RichRecPixel::addToRichRecTracks(const SmartRef<RichRecTrack>& value)
{
   m_richRecTracks.push_back(value);
}

inline void RichRecPixel::removeFromRichRecTracks(const SmartRef<RichRecTrack>& value)
{
  SmartRefVector<RichRecTrack>::iterator iter =
    std::remove(m_richRecTracks.begin(), m_richRecTracks.end(), value);
  m_richRecTracks.erase(iter, m_richRecTracks.end());
}

inline void RichRecPixel::clearRichRecTracks()
{
   m_richRecTracks.clear();
}

inline const ContainedObject* RichRecPixel::parentPixel() const
{
   return m_parentPixel;
}

inline ContainedObject* RichRecPixel::parentPixel() 
{
   return m_parentPixel;
}

inline void RichRecPixel::setParentPixel(const SmartRef<ContainedObject>& value)
{
   m_parentPixel = value;
}

inline const SmartRefVector<RichRecPhoton>& RichRecPixel::richRecPhotons() const
{
   return m_richRecPhotons;
}

inline SmartRefVector<RichRecPhoton>& RichRecPixel::richRecPhotons()
{
   return m_richRecPhotons;
}

inline void RichRecPixel::setRichRecPhotons(const SmartRefVector<RichRecPhoton>& value)
{
   m_richRecPhotons = value;
}

inline void RichRecPixel::addToRichRecPhotons(const SmartRef<RichRecPhoton>& value)
{
   m_richRecPhotons.push_back(value);
}

inline void RichRecPixel::removeFromRichRecPhotons(const SmartRef<RichRecPhoton>& value)
{
  SmartRefVector<RichRecPhoton>::iterator iter =
    std::remove(m_richRecPhotons.begin(), m_richRecPhotons.end(), value);
  m_richRecPhotons.erase(iter, m_richRecPhotons.end());
}

inline void RichRecPixel::clearRichRecPhotons()
{
   m_richRecPhotons.clear();
}

inline StreamBuffer& RichRecPixel::serialize(StreamBuffer& s) const 
{
  KeyedObject<int>::serialize(s);
  s << m_smartID
    << m_globalPosition
    << m_localPosition
    << m_currentBackground
    << m_parentType
    << m_richRecTracks(this)
    << m_parentPixel(this)
    << m_richRecPhotons(this);
  return s;
}

inline StreamBuffer& RichRecPixel::serialize(StreamBuffer& s)
{
  KeyedObject<int>::serialize(s);
  s >> m_smartID
    >> m_globalPosition
    >> m_localPosition
    >> m_currentBackground
    >> m_parentType
    >> m_richRecTracks(this)
    >> m_parentPixel(this)
    >> m_richRecPhotons(this);
  return s;
}

inline std::ostream& RichRecPixel::fillStream(std::ostream& s) const
{
  s << "{ "
    << " smartID:\t" << m_smartID << std::endl
    << "   globalPosition:\t" << m_globalPosition << std::endl
    << "   localPosition:\t" << m_localPosition << std::endl
    << "   currentBackground:\t" << m_currentBackground << std::endl
    << "   parentType:\t" << m_parentType << " } ";
  return s;
}

//Defintion of keyed container for RichRecPixel
typedef KeyedContainer<RichRecPixel, Containers::HashMap> RichRecPixels;
// typedef for std::vector of RichRecPixel
typedef std::vector<RichRecPixel*> RichRecPixelVector;

#endif   ///RichRecBase_RichRecPixel_H
