

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



#ifndef RichRecBase_RichRecTrack_H
#define RichRecBase_RichRecTrack_H 1

// Include files
#include <algorithm>
#include <vector>
#include "Kernel/CLHEPStreams.h"
#include "GaudiKernel/ContainedObject.h"
#include "Event/KeyedObject.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "RichKernel/RichParticleIDType.h"
#include "RichRecBase/RichRecTrackParentType.h"
#include "RichRecBase/RichTrackType.h"
#include "RichKernel/SerializeStl.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/StreamBuffer.h"


// Forward declarations
class RichRecPhoton;
class RichRecPixel;
class RichRecSegment;


// Class ID definition
  static const CLID& CLID_RichRecTrack = 12421;

// Namespace for locations in TDS
namespace RichRecTrackLocation {
  static const std::string& Default = "Rec/Rich/RecTracks";
}


/** @class RichRecTrack RichRecTrack.h 
 *
 *  Rich reconstruction working track object.
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  created Sat Mar 29 21:58:21 2003
 *
 */

class RichRecTrack: public KeyedObject<int>
{

public: 

  friend class RichRecTrack_dict;

  /// Default constructor
  RichRecTrack()
    : m_vertexMomentum(0), m_parentType(Rich::RecTrack::Unknown), m_nEmittedPhotons(Rich::NParticleTypes,-1), m_nDetectablePhotons(Rich::NParticleTypes,-1), m_nSignalPhotons(Rich::NParticleTypes,-1), m_nObservableSignalPhotons(Rich::NParticleTypes,-1), m_nScatteredPhotons(Rich::NParticleTypes,-1),  m_nObservableScatteredPhotons(Rich::NParticleTypes,-1), m_currentHypothesis(Rich::Pion), m_inUse(false), m_trackType(Rich::Track::Unknown)
  {
 
  }

  /// Destructor 
  virtual ~RichRecTrack() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Set the number of emitted photons for a given particle type
  void setNEmittedPhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the number of detectable photons for a given particle type
  void setNDetectablePhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the number of signal photons for a given particle type
  void setNSignalPhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the number of observable signal photons for a given particle type
  void setNObservableSignalPhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the number of scattered photons for a given particle type
  void setNScatteredPhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the number of observable scattered photons for a given particle type
  void setNObservableScatteredPhotons(const Rich::ParticleIDType& type, float photons);

  /// Returns the number of emitted photons for a given particle type
  float nEmittedPhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of detectable photons for a given particle type
  float nDetectablePhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of signal photons for a given particle type
  float nSignalPhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of observable signal photons for a given particle type
  float nObservableSignalPhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of scattered photons for a given particle type
  float nScatteredPhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of observable scattered photons for a given particle type
  float nObservableScatteredPhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of photons for a given particle type
  float nTotalObservablePhotons(const Rich::ParticleIDType& type) const;

  /// Retrieve Reconstructed momentum at vertex
  float vertexMomentum() const; 

  /// Update Reconstructed momentum at vertex
  void setVertexMomentum(float value);

  /// Retrieve Enumeration to Parent type
  const Rich::RecTrack::ParentType& parentType() const; 

  /// Retrieve Enumeration to Parent type (non-const)
  Rich::RecTrack::ParentType& parentType();

  /// Update Enumeration to Parent type
  void setParentType(const Rich::RecTrack::ParentType& value);

  /// Retrieve The number of emitted photons expected for each particle hypothesis
  const std::vector<float>& nEmittedPhotons() const; 

  /// Retrieve The number of emitted photons expected for each particle hypothesis (non-const)
  std::vector<float>& nEmittedPhotons();

  /// Update The number of emitted photons expected for each particle hypothesis
  void setNEmittedPhotons(const std::vector<float>& value);

  /// Retrieve The number of detectable emitted photons expected for each particle hypothesis
  const std::vector<float>& nDetectablePhotons() const; 

  /// Retrieve The number of detectable emitted photons expected for each particle hypothesis (non-const)
  std::vector<float>& nDetectablePhotons();

  /// Update The number of detectable emitted photons expected for each particle hypothesis
  void setNDetectablePhotons(const std::vector<float>& value);

  /// Retrieve The number of signal photons expected for each particle hypothesis
  const std::vector<float>& nSignalPhotons() const; 

  /// Retrieve The number of signal photons expected for each particle hypothesis (non-const)
  std::vector<float>& nSignalPhotons();

  /// Update The number of signal photons expected for each particle hypothesis
  void setNSignalPhotons(const std::vector<float>& value);

  /// Retrieve The number of observable signal photons expected for each particle hypothesis
  const std::vector<float>& nObservableSignalPhotons() const; 

  /// Retrieve The number of observable signal photons expected for each particle hypothesis (non-const)
  std::vector<float>& nObservableSignalPhotons();

  /// Update The number of observable signal photons expected for each particle hypothesis
  void setNObservableSignalPhotons(const std::vector<float>& value);

  /// Retrieve The number of scattered photons expected for each particle hypothesis
  const std::vector<float>& nScatteredPhotons() const; 

  /// Retrieve The number of scattered photons expected for each particle hypothesis (non-const)
  std::vector<float>& nScatteredPhotons();

  /// Update The number of scattered photons expected for each particle hypothesis
  void setNScatteredPhotons(const std::vector<float>& value);

  /// Retrieve The number of observable scattered photons expected for each particle hypothesis
  const std::vector<float>& nObservableScatteredPhotons() const; 

  /// Retrieve The number of observable scattered photons expected for each particle hypothesis (non-const)
  std::vector<float>& nObservableScatteredPhotons();

  /// Update The number of observable scattered photons expected for each particle hypothesis
  void setNObservableScatteredPhotons(const std::vector<float>& value);

  /// Retrieve The current working particle hypothesis for this track. Used by specific algorithms
  const Rich::ParticleIDType& currentHypothesis() const; 

  /// Retrieve The current working particle hypothesis for this track. Used by specific algorithms (non-const)
  Rich::ParticleIDType& currentHypothesis();

  /// Update The current working particle hypothesis for this track. Used by specific algorithms
  void setCurrentHypothesis(const Rich::ParticleIDType& value);

  /// Retrieve Flag for use by algorithms to tag in this track is in use or not
  bool inUse() const; 

  /// Update Flag for use by algorithms to tag in this track is in use or not
  void setInUse(bool value);

  /// Retrieve Track Type. Used to define resolutions etc.
  const Rich::Track::Type& trackType() const; 

  /// Retrieve Track Type. Used to define resolutions etc. (non-const)
  Rich::Track::Type& trackType();

  /// Update Track Type. Used to define resolutions etc.
  void setTrackType(const Rich::Track::Type& value);

  /// Retrieve Parent Object this track was produced from (const)
  const ContainedObject* parentTrack() const;

  /// Retrieve Parent Object this track was produced from (non-const)
  ContainedObject* parentTrack();

  /// Update Parent Object this track was produced from
  void setParentTrack(const SmartRef<ContainedObject>& value);

  /// Retrieve Smart Reference vector of radiator segments for this track (const)
  const SmartRefVector<RichRecSegment>& richRecSegments() const;

  /// Retrieve Smart Reference vector of radiator segments for this track (non-const)
  SmartRefVector<RichRecSegment>& richRecSegments();

  /// Update Smart Reference vector of radiator segments for this track
  void setRichRecSegments(const SmartRefVector<RichRecSegment>& value);

  /// Add Smart Reference vector of radiator segments for this track
  void addToRichRecSegments(const SmartRef<RichRecSegment>& value); 

  /// Remove Smart Reference vector of radiator segments for this track
  void removeFromRichRecSegments(const SmartRef<RichRecSegment>& value); 

  /// Clear Smart Reference vector of radiator segments for this track
  void clearRichRecSegments();

  /// Retrieve SmartRefVector of RichRecPixels for which this track has valid photon associations (const)
  const SmartRefVector<RichRecPixel>& richRecPixels() const;

  /// Retrieve SmartRefVector of RichRecPixels for which this track has valid photon associations (non-const)
  SmartRefVector<RichRecPixel>& richRecPixels();

  /// Update SmartRefVector of RichRecPixels for which this track has valid photon associations
  void setRichRecPixels(const SmartRefVector<RichRecPixel>& value);

  /// Add SmartRefVector of RichRecPixels for which this track has valid photon associations
  void addToRichRecPixels(const SmartRef<RichRecPixel>& value); 

  /// Remove SmartRefVector of RichRecPixels for which this track has valid photon associations
  void removeFromRichRecPixels(const SmartRef<RichRecPixel>& value); 

  /// Clear SmartRefVector of RichRecPixels for which this track has valid photon associations
  void clearRichRecPixels();

  /// Retrieve SmartRefVector of associated RichRecPhotons (const)
  const SmartRefVector<RichRecPhoton>& richRecPhotons() const;

  /// Retrieve SmartRefVector of associated RichRecPhotons (non-const)
  SmartRefVector<RichRecPhoton>& richRecPhotons();

  /// Update SmartRefVector of associated RichRecPhotons
  void setRichRecPhotons(const SmartRefVector<RichRecPhoton>& value);

  /// Add SmartRefVector of associated RichRecPhotons
  void addToRichRecPhotons(const SmartRef<RichRecPhoton>& value); 

  /// Remove SmartRefVector of associated RichRecPhotons
  void removeFromRichRecPhotons(const SmartRef<RichRecPhoton>& value); 

  /// Clear SmartRefVector of associated RichRecPhotons
  void clearRichRecPhotons();

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  float m_vertexMomentum; ///< Reconstructed momentum at vertex
  Rich::RecTrack::ParentType m_parentType; ///< Enumeration to Parent type
  std::vector<float> m_nEmittedPhotons; ///< The number of emitted photons expected for each particle hypothesis
  std::vector<float> m_nDetectablePhotons; ///< The number of detectable emitted photons expected for each particle hypothesis
  std::vector<float> m_nSignalPhotons; ///< The number of signal photons expected for each particle hypothesis
  std::vector<float> m_nObservableSignalPhotons; ///< The number of observable signal photons expected for each particle hypothesis
  std::vector<float> m_nScatteredPhotons; ///< The number of scattered photons expected for each particle hypothesis
  std::vector<float> m_nObservableScatteredPhotons; ///< The number of observable scattered photons expected for each particle hypothesis
  Rich::ParticleIDType m_currentHypothesis; ///< The current working particle hypothesis for this track. Used by specific algorithms
  bool m_inUse; ///< Flag for use by algorithms to tag in this track is in use or not
  Rich::Track::Type m_trackType; ///< Track Type. Used to define resolutions etc.
  SmartRef<ContainedObject> m_parentTrack; ///< Parent Object this track was produced from
  SmartRefVector<RichRecSegment> m_richRecSegments; ///< Smart Reference vector of radiator segments for this track
  SmartRefVector<RichRecPixel> m_richRecPixels; ///< SmartRefVector of RichRecPixels for which this track has valid photon associations
  SmartRefVector<RichRecPhoton> m_richRecPhotons; ///< SmartRefVector of associated RichRecPhotons

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


// Including forward declarations
#include "RichRecSegment.h"
#include "RichRecPixel.h"
#include "RichRecPhoton.h"


inline const CLID& RichRecTrack::clID() const 
{
  return RichRecTrack::classID();
}

inline const CLID& RichRecTrack::classID()
{
  return CLID_RichRecTrack;
}

inline void RichRecTrack::setNEmittedPhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nEmittedPhotons[(int)type] = photons;
}

inline void RichRecTrack::setNDetectablePhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nDetectablePhotons[(int)type] = photons;
}

inline void RichRecTrack::setNSignalPhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nSignalPhotons[(int)type] = photons;
}

inline void RichRecTrack::setNObservableSignalPhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nObservableSignalPhotons[(int)type] = photons;
}

inline void RichRecTrack::setNScatteredPhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nScatteredPhotons[(int)type] = photons;
}

inline void RichRecTrack::setNObservableScatteredPhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nObservableScatteredPhotons[(int)type] = photons;
}

inline float RichRecTrack::nEmittedPhotons(const Rich::ParticleIDType& type) const
{
   return m_nEmittedPhotons[(int)type];
}

inline float RichRecTrack::nDetectablePhotons(const Rich::ParticleIDType& type) const
{
   return m_nDetectablePhotons[(int)type];
}

inline float RichRecTrack::nSignalPhotons(const Rich::ParticleIDType& type) const
{
   return m_nSignalPhotons[(int)type];
}

inline float RichRecTrack::nObservableSignalPhotons(const Rich::ParticleIDType& type) const
{
   return m_nObservableSignalPhotons[(int)type];
}

inline float RichRecTrack::nScatteredPhotons(const Rich::ParticleIDType& type) const
{
   return m_nScatteredPhotons[(int)type];
}

inline float RichRecTrack::nObservableScatteredPhotons(const Rich::ParticleIDType& type) const
{
   return m_nObservableScatteredPhotons[(int)type];
}

inline float RichRecTrack::nTotalObservablePhotons(const Rich::ParticleIDType& type) const
{
   return m_nSignalPhotons[(int)type] + m_nScatteredPhotons[(int)type];
}

inline float RichRecTrack::vertexMomentum() const 
{
  return m_vertexMomentum;
}

inline void RichRecTrack::setVertexMomentum(float value)
{
  m_vertexMomentum = value; 
}

inline const Rich::RecTrack::ParentType& RichRecTrack::parentType() const 
{
  return m_parentType;
}

inline Rich::RecTrack::ParentType& RichRecTrack::parentType()
{
  return m_parentType;
}

inline void RichRecTrack::setParentType(const Rich::RecTrack::ParentType& value)
{
  m_parentType = value; 
}

inline const std::vector<float>& RichRecTrack::nEmittedPhotons() const 
{
  return m_nEmittedPhotons;
}

inline std::vector<float>& RichRecTrack::nEmittedPhotons()
{
  return m_nEmittedPhotons;
}

inline void RichRecTrack::setNEmittedPhotons(const std::vector<float>& value)
{
  m_nEmittedPhotons = value; 
}

inline const std::vector<float>& RichRecTrack::nDetectablePhotons() const 
{
  return m_nDetectablePhotons;
}

inline std::vector<float>& RichRecTrack::nDetectablePhotons()
{
  return m_nDetectablePhotons;
}

inline void RichRecTrack::setNDetectablePhotons(const std::vector<float>& value)
{
  m_nDetectablePhotons = value; 
}

inline const std::vector<float>& RichRecTrack::nSignalPhotons() const 
{
  return m_nSignalPhotons;
}

inline std::vector<float>& RichRecTrack::nSignalPhotons()
{
  return m_nSignalPhotons;
}

inline void RichRecTrack::setNSignalPhotons(const std::vector<float>& value)
{
  m_nSignalPhotons = value; 
}

inline const std::vector<float>& RichRecTrack::nObservableSignalPhotons() const 
{
  return m_nObservableSignalPhotons;
}

inline std::vector<float>& RichRecTrack::nObservableSignalPhotons()
{
  return m_nObservableSignalPhotons;
}

inline void RichRecTrack::setNObservableSignalPhotons(const std::vector<float>& value)
{
  m_nObservableSignalPhotons = value; 
}

inline const std::vector<float>& RichRecTrack::nScatteredPhotons() const 
{
  return m_nScatteredPhotons;
}

inline std::vector<float>& RichRecTrack::nScatteredPhotons()
{
  return m_nScatteredPhotons;
}

inline void RichRecTrack::setNScatteredPhotons(const std::vector<float>& value)
{
  m_nScatteredPhotons = value; 
}

inline const std::vector<float>& RichRecTrack::nObservableScatteredPhotons() const 
{
  return m_nObservableScatteredPhotons;
}

inline std::vector<float>& RichRecTrack::nObservableScatteredPhotons()
{
  return m_nObservableScatteredPhotons;
}

inline void RichRecTrack::setNObservableScatteredPhotons(const std::vector<float>& value)
{
  m_nObservableScatteredPhotons = value; 
}

inline const Rich::ParticleIDType& RichRecTrack::currentHypothesis() const 
{
  return m_currentHypothesis;
}

inline Rich::ParticleIDType& RichRecTrack::currentHypothesis()
{
  return m_currentHypothesis;
}

inline void RichRecTrack::setCurrentHypothesis(const Rich::ParticleIDType& value)
{
  m_currentHypothesis = value; 
}

inline bool RichRecTrack::inUse() const 
{
  return m_inUse;
}

inline void RichRecTrack::setInUse(bool value)
{
  m_inUse = value; 
}

inline const Rich::Track::Type& RichRecTrack::trackType() const 
{
  return m_trackType;
}

inline Rich::Track::Type& RichRecTrack::trackType()
{
  return m_trackType;
}

inline void RichRecTrack::setTrackType(const Rich::Track::Type& value)
{
  m_trackType = value; 
}

inline const ContainedObject* RichRecTrack::parentTrack() const
{
   return m_parentTrack;
}

inline ContainedObject* RichRecTrack::parentTrack() 
{
   return m_parentTrack;
}

inline void RichRecTrack::setParentTrack(const SmartRef<ContainedObject>& value)
{
   m_parentTrack = value;
}

inline const SmartRefVector<RichRecSegment>& RichRecTrack::richRecSegments() const
{
   return m_richRecSegments;
}

inline SmartRefVector<RichRecSegment>& RichRecTrack::richRecSegments()
{
   return m_richRecSegments;
}

inline void RichRecTrack::setRichRecSegments(const SmartRefVector<RichRecSegment>& value)
{
   m_richRecSegments = value;
}

inline void RichRecTrack::addToRichRecSegments(const SmartRef<RichRecSegment>& value)
{
   m_richRecSegments.push_back(value);
}

inline void RichRecTrack::removeFromRichRecSegments(const SmartRef<RichRecSegment>& value)
{
  SmartRefVector<RichRecSegment>::iterator iter =
    std::remove(m_richRecSegments.begin(), m_richRecSegments.end(), value);
  m_richRecSegments.erase(iter, m_richRecSegments.end());
}

inline void RichRecTrack::clearRichRecSegments()
{
   m_richRecSegments.clear();
}

inline const SmartRefVector<RichRecPixel>& RichRecTrack::richRecPixels() const
{
   return m_richRecPixels;
}

inline SmartRefVector<RichRecPixel>& RichRecTrack::richRecPixels()
{
   return m_richRecPixels;
}

inline void RichRecTrack::setRichRecPixels(const SmartRefVector<RichRecPixel>& value)
{
   m_richRecPixels = value;
}

inline void RichRecTrack::addToRichRecPixels(const SmartRef<RichRecPixel>& value)
{
   m_richRecPixels.push_back(value);
}

inline void RichRecTrack::removeFromRichRecPixels(const SmartRef<RichRecPixel>& value)
{
  SmartRefVector<RichRecPixel>::iterator iter =
    std::remove(m_richRecPixels.begin(), m_richRecPixels.end(), value);
  m_richRecPixels.erase(iter, m_richRecPixels.end());
}

inline void RichRecTrack::clearRichRecPixels()
{
   m_richRecPixels.clear();
}

inline const SmartRefVector<RichRecPhoton>& RichRecTrack::richRecPhotons() const
{
   return m_richRecPhotons;
}

inline SmartRefVector<RichRecPhoton>& RichRecTrack::richRecPhotons()
{
   return m_richRecPhotons;
}

inline void RichRecTrack::setRichRecPhotons(const SmartRefVector<RichRecPhoton>& value)
{
   m_richRecPhotons = value;
}

inline void RichRecTrack::addToRichRecPhotons(const SmartRef<RichRecPhoton>& value)
{
   m_richRecPhotons.push_back(value);
}

inline void RichRecTrack::removeFromRichRecPhotons(const SmartRef<RichRecPhoton>& value)
{
  SmartRefVector<RichRecPhoton>::iterator iter =
    std::remove(m_richRecPhotons.begin(), m_richRecPhotons.end(), value);
  m_richRecPhotons.erase(iter, m_richRecPhotons.end());
}

inline void RichRecTrack::clearRichRecPhotons()
{
   m_richRecPhotons.clear();
}

inline StreamBuffer& RichRecTrack::serialize(StreamBuffer& s) const 
{
  KeyedObject<int>::serialize(s);
  s << m_vertexMomentum
    << m_parentType
    << m_nEmittedPhotons
    << m_nDetectablePhotons
    << m_nSignalPhotons
    << m_nObservableSignalPhotons
    << m_nScatteredPhotons
    << m_nObservableScatteredPhotons
    << m_currentHypothesis
    << m_trackType
    << m_parentTrack(this)
    << m_richRecSegments(this)
    << m_richRecPixels(this)
    << m_richRecPhotons(this);
  return s;
}

inline StreamBuffer& RichRecTrack::serialize(StreamBuffer& s)
{
  KeyedObject<int>::serialize(s);
  s >> m_vertexMomentum
    >> m_parentType
    >> m_nEmittedPhotons
    >> m_nDetectablePhotons
    >> m_nSignalPhotons
    >> m_nObservableSignalPhotons
    >> m_nScatteredPhotons
    >> m_nObservableScatteredPhotons
    >> m_currentHypothesis
    >> m_trackType
    >> m_parentTrack(this)
    >> m_richRecSegments(this)
    >> m_richRecPixels(this)
    >> m_richRecPhotons(this);
  return s;
}

inline std::ostream& RichRecTrack::fillStream(std::ostream& s) const
{
  char l_inUse = (m_inUse) ? 'T' : 'F';
  s << "{ "
    << " vertexMomentum:\t" << m_vertexMomentum << std::endl
    << "   parentType:\t" << m_parentType << std::endl
    << "   nEmittedPhotons:\t" << m_nEmittedPhotons << std::endl
    << "   nDetectablePhotons:\t" << m_nDetectablePhotons << std::endl
    << "   nSignalPhotons:\t" << m_nSignalPhotons << std::endl
    << "   nObservableSignalPhotons:\t" << m_nObservableSignalPhotons << std::endl
    << "   nScatteredPhotons:\t" << m_nScatteredPhotons << std::endl
    << "   nObservableScatteredPhotons:\t" << m_nObservableScatteredPhotons << std::endl
    << "   currentHypothesis:\t" << m_currentHypothesis << std::endl
    << "   inUse:\t" << l_inUse << std::endl
    << "   trackType:\t" << m_trackType << " } ";
  return s;
}

//Defintion of keyed container for RichRecTrack
typedef KeyedContainer<RichRecTrack, Containers::HashMap> RichRecTracks;
// typedef for std::vector of RichRecTrack
typedef std::vector<RichRecTrack*> RichRecTrackVector;

#endif   ///RichRecBase_RichRecTrack_H
