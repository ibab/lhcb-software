

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



#ifndef RichRecBase_RichRecPhoton_H
#define RichRecBase_RichRecPhoton_H 1

// Include files
#include <vector>
#include "Kernel/CLHEPStreams.h"
#include "Event/KeyedObject.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "RichUtils/RichGeomPhoton.h"
#include "RichKernel/RichParticleIDType.h"
#include "RichRecBase/RichRecPhotonKey.h"
#include "RichKernel/SerializeStl.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"


// Forward declarations
class RichRecPixel;
class RichRecSegment;
class RichRecTrack;


// Class ID definition
  static const CLID& CLID_RichRecPhoton = 12424;

// Namespace for locations in TDS
namespace RichRecPhotonLocation {
  static const std::string& Default = "Rec/Rich/RecPhotons";
}


/** @class RichRecPhoton RichRecPhoton.h 
 *
 *  Rich reconstruction photon candidate. The result of an association between a RichRecTrack and a RichRecPixel.
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  created Sat Mar 29 21:58:21 2003
 *
 */

class RichRecPhoton: public KeyedObject<RichRecPhotonKey>
{

public: 

  friend class RichRecPhoton_dict;

  /// Default Constructor
  RichRecPhoton()
    : m_expPixelSignalPhots(Rich::NParticleTypes,-1), m_expPixelScattPhots(Rich::NParticleTypes,-1)
  {
 
  }

  /// Destructor 
  virtual ~RichRecPhoton() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Set the expected number of signal photons for a given particle hypothesis
  void setExpPixelSignalPhots(const Rich::ParticleIDType& type, float photons);

  /// Set the expected number of scattered photons for a given particle hypothesis
  void setExpPixelScattPhots(const Rich::ParticleIDType& type, float photons);

  /// Returns the expected number of signal photons for a given particle type
  float expPixelSignalPhots(const Rich::ParticleIDType& type) const;

  /// Returns the expected number of scattered photons for a given particle type
  float expPixelScattPhots(const Rich::ParticleIDType& type) const;

  /// Retrieve The result of the Cherenkov angles reconstruction
  const RichGeomPhoton& geomPhoton() const; 

  /// Retrieve The result of the Cherenkov angles reconstruction (non-const)
  RichGeomPhoton& geomPhoton();

  /// Update The result of the Cherenkov angles reconstruction
  void setGeomPhoton(const RichGeomPhoton& value);

  /// Retrieve Expected number of signal photons for each particle hypothesis
  const std::vector<float>& expPixelSignalPhots() const; 

  /// Retrieve Expected number of signal photons for each particle hypothesis (non-const)
  std::vector<float>& expPixelSignalPhots();

  /// Update Expected number of signal photons for each particle hypothesis
  void setExpPixelSignalPhots(const std::vector<float>& value);

  /// Retrieve Expected number of scattered photons for each particle hypothesis
  const std::vector<float>& expPixelScattPhots() const; 

  /// Retrieve Expected number of scattered photons for each particle hypothesis (non-const)
  std::vector<float>& expPixelScattPhots();

  /// Update Expected number of scattered photons for each particle hypothesis
  void setExpPixelScattPhots(const std::vector<float>& value);

  /// Retrieve The fraction of the segment path length associated with this photon
  float sharedRingFract() const; 

  /// Update The fraction of the segment path length associated with this photon
  void setSharedRingFract(float value);

  /// Retrieve Smart Reference to associated RichRecSegment (const)
  const RichRecSegment* richRecSegment() const;

  /// Retrieve Smart Reference to associated RichRecSegment (non-const)
  RichRecSegment* richRecSegment();

  /// Update Smart Reference to associated RichRecSegment
  void setRichRecSegment(const SmartRef<RichRecSegment>& value);

  /// Retrieve Smart Reference to associated RichRecTrack (const)
  const RichRecTrack* richRecTrack() const;

  /// Retrieve Smart Reference to associated RichRecTrack (non-const)
  RichRecTrack* richRecTrack();

  /// Update Smart Reference to associated RichRecTrack
  void setRichRecTrack(const SmartRef<RichRecTrack>& value);

  /// Retrieve Smart Reference to associated RichRecPixel (const)
  const RichRecPixel* richRecPixel() const;

  /// Retrieve Smart Reference to associated RichRecPixel (non-const)
  RichRecPixel* richRecPixel();

  /// Update Smart Reference to associated RichRecPixel
  void setRichRecPixel(const SmartRef<RichRecPixel>& value);

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  RichGeomPhoton m_geomPhoton; ///< The result of the Cherenkov angles reconstruction
  std::vector<float> m_expPixelSignalPhots; ///< Expected number of signal photons for each particle hypothesis
  std::vector<float> m_expPixelScattPhots; ///< Expected number of scattered photons for each particle hypothesis
  float m_sharedRingFract; ///< The fraction of the segment path length associated with this photon
  SmartRef<RichRecSegment> m_richRecSegment; ///< Smart Reference to associated RichRecSegment
  SmartRef<RichRecTrack> m_richRecTrack; ///< Smart Reference to associated RichRecTrack
  SmartRef<RichRecPixel> m_richRecPixel; ///< Smart Reference to associated RichRecPixel

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


// Including forward declarations
#include "Event/RichRecSegment.h"
#include "Event/RichRecTrack.h"
#include "Event/RichRecPixel.h"


inline const CLID& RichRecPhoton::clID() const 
{
  return RichRecPhoton::classID();
}

inline const CLID& RichRecPhoton::classID()
{
  return CLID_RichRecPhoton;
}

inline void RichRecPhoton::setExpPixelSignalPhots(const Rich::ParticleIDType& type, float photons)
{
   m_expPixelSignalPhots[type] = photons;
}

inline void RichRecPhoton::setExpPixelScattPhots(const Rich::ParticleIDType& type, float photons)
{
   m_expPixelScattPhots[type] = photons;
}

inline float RichRecPhoton::expPixelSignalPhots(const Rich::ParticleIDType& type) const
{
   return m_expPixelSignalPhots[type];
}

inline float RichRecPhoton::expPixelScattPhots(const Rich::ParticleIDType& type) const
{
   return m_expPixelScattPhots[type];
}

inline const RichGeomPhoton& RichRecPhoton::geomPhoton() const 
{
  return m_geomPhoton;
}

inline RichGeomPhoton& RichRecPhoton::geomPhoton()
{
  return m_geomPhoton;
}

inline void RichRecPhoton::setGeomPhoton(const RichGeomPhoton& value)
{
  m_geomPhoton = value; 
}

inline const std::vector<float>& RichRecPhoton::expPixelSignalPhots() const 
{
  return m_expPixelSignalPhots;
}

inline std::vector<float>& RichRecPhoton::expPixelSignalPhots()
{
  return m_expPixelSignalPhots;
}

inline void RichRecPhoton::setExpPixelSignalPhots(const std::vector<float>& value)
{
  m_expPixelSignalPhots = value; 
}

inline const std::vector<float>& RichRecPhoton::expPixelScattPhots() const 
{
  return m_expPixelScattPhots;
}

inline std::vector<float>& RichRecPhoton::expPixelScattPhots()
{
  return m_expPixelScattPhots;
}

inline void RichRecPhoton::setExpPixelScattPhots(const std::vector<float>& value)
{
  m_expPixelScattPhots = value; 
}

inline float RichRecPhoton::sharedRingFract() const 
{
  return m_sharedRingFract;
}

inline void RichRecPhoton::setSharedRingFract(float value)
{
  m_sharedRingFract = value; 
}

inline const RichRecSegment* RichRecPhoton::richRecSegment() const
{
   return m_richRecSegment;
}

inline RichRecSegment* RichRecPhoton::richRecSegment() 
{
   return m_richRecSegment;
}

inline void RichRecPhoton::setRichRecSegment(const SmartRef<RichRecSegment>& value)
{
   m_richRecSegment = value;
}

inline const RichRecTrack* RichRecPhoton::richRecTrack() const
{
   return m_richRecTrack;
}

inline RichRecTrack* RichRecPhoton::richRecTrack() 
{
   return m_richRecTrack;
}

inline void RichRecPhoton::setRichRecTrack(const SmartRef<RichRecTrack>& value)
{
   m_richRecTrack = value;
}

inline const RichRecPixel* RichRecPhoton::richRecPixel() const
{
   return m_richRecPixel;
}

inline RichRecPixel* RichRecPhoton::richRecPixel() 
{
   return m_richRecPixel;
}

inline void RichRecPhoton::setRichRecPixel(const SmartRef<RichRecPixel>& value)
{
   m_richRecPixel = value;
}

inline StreamBuffer& RichRecPhoton::serialize(StreamBuffer& s) const 
{
  KeyedObject<RichRecPhotonKey>::serialize(s);
  s << m_geomPhoton
    << m_expPixelSignalPhots
    << m_expPixelScattPhots
    << m_sharedRingFract
    << m_richRecSegment(this)
    << m_richRecTrack(this)
    << m_richRecPixel(this);
  return s;
}

inline StreamBuffer& RichRecPhoton::serialize(StreamBuffer& s)
{
  KeyedObject<RichRecPhotonKey>::serialize(s);
  s >> m_geomPhoton
    >> m_expPixelSignalPhots
    >> m_expPixelScattPhots
    >> m_sharedRingFract
    >> m_richRecSegment(this)
    >> m_richRecTrack(this)
    >> m_richRecPixel(this);
  return s;
}

inline std::ostream& RichRecPhoton::fillStream(std::ostream& s) const
{
  s << "{ "
    << " geomPhoton:\t" << m_geomPhoton << std::endl
    << "   expPixelSignalPhots:\t" << m_expPixelSignalPhots << std::endl
    << "   expPixelScattPhots:\t" << m_expPixelScattPhots << std::endl
    << "   sharedRingFract:\t" << m_sharedRingFract << " } ";
  return s;
}

//Defintion of keyed container for RichRecPhoton
typedef KeyedContainer<RichRecPhoton, Containers::HashMap> RichRecPhotons;
// typedef for std::vector of RichRecPhoton
typedef std::vector<RichRecPhoton*> RichRecPhotonVector;

#endif   ///RichRecBase_RichRecPhoton_H
