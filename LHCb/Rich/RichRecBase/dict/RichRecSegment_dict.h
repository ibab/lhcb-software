

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



#ifndef RichRecBase_RichRecSegment_H
#define RichRecBase_RichRecSegment_H 1

// Include files
#include <algorithm>
#include <vector>
#include "Kernel/CLHEPStreams.h"
#include "CLHEP/Geometry/Point3D.h"
#include "Event/KeyedObject.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "RichRecBase/RichMap.h"
#include "RichKernel/RichParticleIDType.h"
#include "RichUtils/RichTrackSegment.h"
#include "RichKernel/SerializeStl.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/StreamBuffer.h"


// Forward declarations
class RichRecPhoton;
class RichRecPixel;
class RichRecTrack;


// Class ID definition
  static const CLID& CLID_RichRecSegment = 12423;

// Namespace for locations in TDS
namespace RichRecSegmentLocation {
  static const std::string& Default = "Rec/Rich/RecSegments";
}


/** @class RichRecSegment RichRecSegment.h 
 *
 *  Rich reconstruction object describing a continuous track segment in a single radiator
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  created Sat Mar 29 21:58:21 2003
 *
 */

class RichRecSegment: public KeyedObject<int>
{

public: 

  friend class RichRecSegment_dict;

  /// Default Constructor
  RichRecSegment()
    : m_nEmittedPhotons(Rich::NParticleTypes,-1), m_nDetectablePhotons(Rich::NParticleTypes,-1), m_nSignalPhotons(Rich::NParticleTypes,-1), m_nScatteredPhotons(Rich::NParticleTypes,-1), m_nEmitPhotsPerEnBin(Rich::NParticleTypes,std::vector<float>() ), m_geomEfficiencyPerHPD(Rich::NParticleTypes, RichMap<int,float>() ), m_geomEfficiency(Rich::NParticleTypes,-1), m_averageCKTheta(Rich::NParticleTypes,-1), m_hitDetectorRegions(0), m_geomEfficiencyScat(Rich::NParticleTypes,-1)
  {
 
  }

  /// Destructor 
  virtual ~RichRecSegment() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Set the geometrical efficiency for a given particle type
  void setGeomEfficiency(const Rich::ParticleIDType& type, float eff);

  /// Set the scattered geometrical efficiency for a given particle type
  void setGeomEfficiencyScat(const Rich::ParticleIDType& type, float eff);

  /// Set the number of emitted photons for a given particle type
  void setNEmittedPhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the number of detectable photons for a given particle type
  void setNDetectablePhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the number of signal photons for a given particle type
  void setNSignalPhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the number of scattered photons for a given particle type
  void setNScatteredPhotons(const Rich::ParticleIDType& type, float photons);

  /// Set the average Cherenkov angle for a given particle type
  void setAverageCKTheta(const Rich::ParticleIDType& type, float angle);

  /// Method to create the geometrical efficiencies per HPD
  void addToGeomEfficiencyPerHPD(const Rich::ParticleIDType& id, int hpd, float signal);

  /// Returns the number of emitted photons for a given particle type
  float nEmittedPhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of detectable photons for a given particle type
  float nDetectablePhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of signal photons for a given particle type
  float nSignalPhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of scattered photons for a given particle type
  float nScatteredPhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of photons for a given particle type
  float nTotalObservablePhotons(const Rich::ParticleIDType& type) const;

  /// Returns the number of emitted photons for a given particle type
  std::vector<float>& nEmitPhotsPerEnBin(const Rich::ParticleIDType& type);

  /// Get the geometrical efficiency for a given particle type
  float geomEfficiency(const Rich::ParticleIDType& type) const;

  /// Get the scattered geometrical efficiency for a given particle type
  float geomEfficiencyScat(const Rich::ParticleIDType& type) const;

  /// Get the average Cherenkov angle for a given particle type
  float averageCKTheta(const Rich::ParticleIDType& type) const;

  /// Returns the geometrical efficiency per HPD for given hypothesis
  RichMap<int,float>& geomEfficiencyPerHPD(const Rich::ParticleIDType& type);

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

  /// Retrieve The number of scattered photons expected for each particle hypothesis
  const std::vector<float>& nScatteredPhotons() const; 

  /// Retrieve The number of scattered photons expected for each particle hypothesis (non-const)
  std::vector<float>& nScatteredPhotons();

  /// Update The number of scattered photons expected for each particle hypothesis
  void setNScatteredPhotons(const std::vector<float>& value);

  /// Retrieve Number of emitted photons per enrgy bin. Internal working object to RichSegmentTool
  const std::vector< std::vector<float> >& nEmitPhotsPerEnBin() const; 

  /// Retrieve Number of emitted photons per enrgy bin. Internal working object to RichSegmentTool (non-const)
  std::vector< std::vector<float> >& nEmitPhotsPerEnBin();

  /// Update Number of emitted photons per enrgy bin. Internal working object to RichSegmentTool
  void setNEmitPhotsPerEnBin(const std::vector< std::vector<float> >& value);

  /// Retrieve The geometrical efficiency for a given hypothesis, binned per HPD
  const std::vector<RichMap<int,float> >& geomEfficiencyPerHPD() const; 

  /// Retrieve The geometrical efficiency for a given hypothesis, binned per HPD (non-const)
  std::vector<RichMap<int,float> >& geomEfficiencyPerHPD();

  /// Update The geometrical efficiency for a given hypothesis, binned per HPD
  void setGeomEfficiencyPerHPD(const std::vector<RichMap<int,float> >& value);

  /// Retrieve Geometrical efficiencies for the each particle hypothesis
  const std::vector<float>& geomEfficiency() const; 

  /// Retrieve Geometrical efficiencies for the each particle hypothesis (non-const)
  std::vector<float>& geomEfficiency();

  /// Update Geometrical efficiencies for the each particle hypothesis
  void setGeomEfficiency(const std::vector<float>& value);

  /// Retrieve The average Cherenkov theta angle for given particle ID type
  const std::vector<float>& averageCKTheta() const; 

  /// Retrieve The average Cherenkov theta angle for given particle ID type (non-const)
  std::vector<float>& averageCKTheta();

  /// Update The average Cherenkov theta angle for given particle ID type
  void setAverageCKTheta(const std::vector<float>& value);

  /// Retrieve Track segment, the passage of a track through a Rich detector
  const RichTrackSegment& trackSegment() const; 

  /// Retrieve Track segment, the passage of a track through a Rich detector (non-const)
  RichTrackSegment& trackSegment();

  /// Update Track segment, the passage of a track through a Rich detector
  void setTrackSegment(const RichTrackSegment& value);

  /// Retrieve Ray traced HPD panel impact point
  const HepPoint3D& hpdPanelHitPoint() const; 

  /// Retrieve Ray traced HPD panel impact point (non-const)
  HepPoint3D& hpdPanelHitPoint();

  /// Update Ray traced HPD panel impact point
  void setHpdPanelHitPoint(const HepPoint3D& value);

  /// Retrieve Detector regions accessible to this segment
  unsigned long hitDetectorRegions() const; 

  /// Update Detector regions accessible to this segment
  void setHitDetectorRegions(unsigned long value);

  /// Update Photons observable in x>0 region of detector
  void setPhotonsInXPlus(unsigned long value);

  /// Retrieve Photons observable in x>0 region of detector
  unsigned long photonsInXPlus() const;

  /// Update Photons observable in x<0 region of detector
  void setPhotonsInXMinus(unsigned long value);

  /// Retrieve Photons observable in x<0 region of detector
  unsigned long photonsInXMinus() const;

  /// Update Photons observable in y>0 region of detector
  void setPhotonsInYPlus(unsigned long value);

  /// Retrieve Photons observable in y>0 region of detector
  unsigned long photonsInYPlus() const;

  /// Update Photons observable in y<0 region of detector
  void setPhotonsInYMinus(unsigned long value);

  /// Retrieve Photons observable in y<0 region of detector
  unsigned long photonsInYMinus() const;

  /// Retrieve Geometrical efficiency for scattered component
  const std::vector<float>& geomEfficiencyScat() const; 

  /// Retrieve Geometrical efficiency for scattered component (non-const)
  std::vector<float>& geomEfficiencyScat();

  /// Update Geometrical efficiency for scattered component
  void setGeomEfficiencyScat(const std::vector<float>& value);

  /// Retrieve Smart Reference to associated RichRecTrack (const)
  const RichRecTrack* richRecTrack() const;

  /// Retrieve Smart Reference to associated RichRecTrack (non-const)
  RichRecTrack* richRecTrack();

  /// Update Smart Reference to associated RichRecTrack
  void setRichRecTrack(const SmartRef<RichRecTrack>& value);

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

  /// Retrieve SmartRef to associated RichRecPhotons (const)
  const SmartRefVector<RichRecPhoton>& richRecPhotons() const;

  /// Retrieve SmartRef to associated RichRecPhotons (non-const)
  SmartRefVector<RichRecPhoton>& richRecPhotons();

  /// Update SmartRef to associated RichRecPhotons
  void setRichRecPhotons(const SmartRefVector<RichRecPhoton>& value);

  /// Add SmartRef to associated RichRecPhotons
  void addToRichRecPhotons(const SmartRef<RichRecPhoton>& value); 

  /// Remove SmartRef to associated RichRecPhotons
  void removeFromRichRecPhotons(const SmartRef<RichRecPhoton>& value); 

  /// Clear SmartRef to associated RichRecPhotons
  void clearRichRecPhotons();

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  enum hitDetectorRegionsBits {photonsInXPlusBits = 0, photonsInXMinusBits = 1, photonsInYPlusBits = 2, photonsInYMinusBits = 3}; ///< Offsets of bitfield hitDetectorRegions
  enum hitDetectorRegionsMasks {photonsInXPlusMask = 0x00000001, photonsInXMinusMask = 0x00000002, photonsInYPlusMask = 0x00000004, photonsInYMinusMask = 0x00000008}; ///< Bitmasks for bitfield hitDetectorRegions

  std::vector<float> m_nEmittedPhotons; ///< The number of emitted photons expected for each particle hypothesis
  std::vector<float> m_nDetectablePhotons; ///< The number of detectable emitted photons expected for each particle hypothesis
  std::vector<float> m_nSignalPhotons; ///< The number of signal photons expected for each particle hypothesis
  std::vector<float> m_nScatteredPhotons; ///< The number of scattered photons expected for each particle hypothesis
  std::vector< std::vector<float> > m_nEmitPhotsPerEnBin; ///< Number of emitted photons per enrgy bin. Internal working object to RichSegmentTool
  std::vector<RichMap<int,float> > m_geomEfficiencyPerHPD; ///< The geometrical efficiency for a given hypothesis, binned per HPD
  std::vector<float> m_geomEfficiency; ///< Geometrical efficiencies for the each particle hypothesis
  std::vector<float> m_averageCKTheta; ///< The average Cherenkov theta angle for given particle ID type
  RichTrackSegment m_trackSegment; ///< Track segment, the passage of a track through a Rich detector
  HepPoint3D m_hpdPanelHitPoint; ///< Ray traced HPD panel impact point
  unsigned long m_hitDetectorRegions; ///< Detector regions accessible to this segment
  std::vector<float> m_geomEfficiencyScat; ///< Geometrical efficiency for scattered component
  SmartRef<RichRecTrack> m_richRecTrack; ///< Smart Reference to associated RichRecTrack
  SmartRefVector<RichRecPixel> m_richRecPixels; ///< SmartRefVector of RichRecPixels for which this track has valid photon associations
  SmartRefVector<RichRecPhoton> m_richRecPhotons; ///< SmartRef to associated RichRecPhotons

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


// Including forward declarations
#include "Event/RichRecTrack.h"
#include "Event/RichRecPixel.h"
#include "RichRecPhoton.h"


inline const CLID& RichRecSegment::clID() const 
{
  return RichRecSegment::classID();
}

inline const CLID& RichRecSegment::classID()
{
  return CLID_RichRecSegment;
}

inline void RichRecSegment::setGeomEfficiency(const Rich::ParticleIDType& type, float eff)
{
   m_geomEfficiency[(int)type] = eff;
}

inline void RichRecSegment::setGeomEfficiencyScat(const Rich::ParticleIDType& type, float eff)
{
   m_geomEfficiencyScat[(int)type] = eff;
}

inline void RichRecSegment::setNEmittedPhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nEmittedPhotons[(int)type] = photons;
}

inline void RichRecSegment::setNDetectablePhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nDetectablePhotons[(int)type] = photons;
}

inline void RichRecSegment::setNSignalPhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nSignalPhotons[(int)type] = photons;
}

inline void RichRecSegment::setNScatteredPhotons(const Rich::ParticleIDType& type, float photons)
{
   m_nScatteredPhotons[(int)type] = photons;
}

inline void RichRecSegment::setAverageCKTheta(const Rich::ParticleIDType& type, float angle)
{
   m_averageCKTheta[(int)type] = angle;
}

inline void RichRecSegment::addToGeomEfficiencyPerHPD(const Rich::ParticleIDType& id, int hpd, float signal)
{
   (m_geomEfficiencyPerHPD[(int)id])[hpd] += signal;
}

inline float RichRecSegment::nEmittedPhotons(const Rich::ParticleIDType& type) const
{
   return m_nEmittedPhotons[(int)type];
}

inline float RichRecSegment::nDetectablePhotons(const Rich::ParticleIDType& type) const
{
   return m_nDetectablePhotons[(int)type];
}

inline float RichRecSegment::nSignalPhotons(const Rich::ParticleIDType& type) const
{
   return m_nSignalPhotons[(int)type];
}

inline float RichRecSegment::nScatteredPhotons(const Rich::ParticleIDType& type) const
{
   return m_nScatteredPhotons[(int)type];
}

inline float RichRecSegment::nTotalObservablePhotons(const Rich::ParticleIDType& type) const
{
   return m_nSignalPhotons[(int)type] + m_nScatteredPhotons[(int)type];
}

inline std::vector<float>& RichRecSegment::nEmitPhotsPerEnBin(const Rich::ParticleIDType& type)
{
   return m_nEmitPhotsPerEnBin[(int)type];
}

inline float RichRecSegment::geomEfficiency(const Rich::ParticleIDType& type) const
{
   return m_geomEfficiency[(int)type];
}

inline float RichRecSegment::geomEfficiencyScat(const Rich::ParticleIDType& type) const
{
   return m_geomEfficiencyScat[(int)type];
}

inline float RichRecSegment::averageCKTheta(const Rich::ParticleIDType& type) const
{
   return m_averageCKTheta[(int)type];
}

inline RichMap<int,float>& RichRecSegment::geomEfficiencyPerHPD(const Rich::ParticleIDType& type)
{
   return m_geomEfficiencyPerHPD[(int)type];
}

inline const std::vector<float>& RichRecSegment::nEmittedPhotons() const 
{
  return m_nEmittedPhotons;
}

inline std::vector<float>& RichRecSegment::nEmittedPhotons()
{
  return m_nEmittedPhotons;
}

inline void RichRecSegment::setNEmittedPhotons(const std::vector<float>& value)
{
  m_nEmittedPhotons = value; 
}

inline const std::vector<float>& RichRecSegment::nDetectablePhotons() const 
{
  return m_nDetectablePhotons;
}

inline std::vector<float>& RichRecSegment::nDetectablePhotons()
{
  return m_nDetectablePhotons;
}

inline void RichRecSegment::setNDetectablePhotons(const std::vector<float>& value)
{
  m_nDetectablePhotons = value; 
}

inline const std::vector<float>& RichRecSegment::nSignalPhotons() const 
{
  return m_nSignalPhotons;
}

inline std::vector<float>& RichRecSegment::nSignalPhotons()
{
  return m_nSignalPhotons;
}

inline void RichRecSegment::setNSignalPhotons(const std::vector<float>& value)
{
  m_nSignalPhotons = value; 
}

inline const std::vector<float>& RichRecSegment::nScatteredPhotons() const 
{
  return m_nScatteredPhotons;
}

inline std::vector<float>& RichRecSegment::nScatteredPhotons()
{
  return m_nScatteredPhotons;
}

inline void RichRecSegment::setNScatteredPhotons(const std::vector<float>& value)
{
  m_nScatteredPhotons = value; 
}

inline const std::vector< std::vector<float> >& RichRecSegment::nEmitPhotsPerEnBin() const 
{
  return m_nEmitPhotsPerEnBin;
}

inline std::vector< std::vector<float> >& RichRecSegment::nEmitPhotsPerEnBin()
{
  return m_nEmitPhotsPerEnBin;
}

inline void RichRecSegment::setNEmitPhotsPerEnBin(const std::vector< std::vector<float> >& value)
{
  m_nEmitPhotsPerEnBin = value; 
}

inline const std::vector<RichMap<int,float> >& RichRecSegment::geomEfficiencyPerHPD() const 
{
  return m_geomEfficiencyPerHPD;
}

inline std::vector<RichMap<int,float> >& RichRecSegment::geomEfficiencyPerHPD()
{
  return m_geomEfficiencyPerHPD;
}

inline void RichRecSegment::setGeomEfficiencyPerHPD(const std::vector<RichMap<int,float> >& value)
{
  m_geomEfficiencyPerHPD = value; 
}

inline const std::vector<float>& RichRecSegment::geomEfficiency() const 
{
  return m_geomEfficiency;
}

inline std::vector<float>& RichRecSegment::geomEfficiency()
{
  return m_geomEfficiency;
}

inline void RichRecSegment::setGeomEfficiency(const std::vector<float>& value)
{
  m_geomEfficiency = value; 
}

inline const std::vector<float>& RichRecSegment::averageCKTheta() const 
{
  return m_averageCKTheta;
}

inline std::vector<float>& RichRecSegment::averageCKTheta()
{
  return m_averageCKTheta;
}

inline void RichRecSegment::setAverageCKTheta(const std::vector<float>& value)
{
  m_averageCKTheta = value; 
}

inline const RichTrackSegment& RichRecSegment::trackSegment() const 
{
  return m_trackSegment;
}

inline RichTrackSegment& RichRecSegment::trackSegment()
{
  return m_trackSegment;
}

inline void RichRecSegment::setTrackSegment(const RichTrackSegment& value)
{
  m_trackSegment = value; 
}

inline const HepPoint3D& RichRecSegment::hpdPanelHitPoint() const 
{
  return m_hpdPanelHitPoint;
}

inline HepPoint3D& RichRecSegment::hpdPanelHitPoint()
{
  return m_hpdPanelHitPoint;
}

inline void RichRecSegment::setHpdPanelHitPoint(const HepPoint3D& value)
{
  m_hpdPanelHitPoint = value; 
}

inline unsigned long RichRecSegment::hitDetectorRegions() const 
{
  return m_hitDetectorRegions;
}

inline void RichRecSegment::setHitDetectorRegions(unsigned long value)
{
  m_hitDetectorRegions = value; 
}

inline void RichRecSegment::setPhotonsInXPlus(unsigned long value)
{
  m_hitDetectorRegions &= ~photonsInXPlusMask;
  m_hitDetectorRegions |= ((value << photonsInXPlusBits) & photonsInXPlusMask);
}

inline unsigned long RichRecSegment::photonsInXPlus() const
{
  return ((m_hitDetectorRegions & photonsInXPlusMask) >> photonsInXPlusBits);
}

inline void RichRecSegment::setPhotonsInXMinus(unsigned long value)
{
  m_hitDetectorRegions &= ~photonsInXMinusMask;
  m_hitDetectorRegions |= ((value << photonsInXMinusBits) & photonsInXMinusMask);
}

inline unsigned long RichRecSegment::photonsInXMinus() const
{
  return ((m_hitDetectorRegions & photonsInXMinusMask) >> photonsInXMinusBits);
}

inline void RichRecSegment::setPhotonsInYPlus(unsigned long value)
{
  m_hitDetectorRegions &= ~photonsInYPlusMask;
  m_hitDetectorRegions |= ((value << photonsInYPlusBits) & photonsInYPlusMask);
}

inline unsigned long RichRecSegment::photonsInYPlus() const
{
  return ((m_hitDetectorRegions & photonsInYPlusMask) >> photonsInYPlusBits);
}

inline void RichRecSegment::setPhotonsInYMinus(unsigned long value)
{
  m_hitDetectorRegions &= ~photonsInYMinusMask;
  m_hitDetectorRegions |= ((value << photonsInYMinusBits) & photonsInYMinusMask);
}

inline unsigned long RichRecSegment::photonsInYMinus() const
{
  return ((m_hitDetectorRegions & photonsInYMinusMask) >> photonsInYMinusBits);
}

inline const std::vector<float>& RichRecSegment::geomEfficiencyScat() const 
{
  return m_geomEfficiencyScat;
}

inline std::vector<float>& RichRecSegment::geomEfficiencyScat()
{
  return m_geomEfficiencyScat;
}

inline void RichRecSegment::setGeomEfficiencyScat(const std::vector<float>& value)
{
  m_geomEfficiencyScat = value; 
}

inline const RichRecTrack* RichRecSegment::richRecTrack() const
{
   return m_richRecTrack;
}

inline RichRecTrack* RichRecSegment::richRecTrack() 
{
   return m_richRecTrack;
}

inline void RichRecSegment::setRichRecTrack(const SmartRef<RichRecTrack>& value)
{
   m_richRecTrack = value;
}

inline const SmartRefVector<RichRecPixel>& RichRecSegment::richRecPixels() const
{
   return m_richRecPixels;
}

inline SmartRefVector<RichRecPixel>& RichRecSegment::richRecPixels()
{
   return m_richRecPixels;
}

inline void RichRecSegment::setRichRecPixels(const SmartRefVector<RichRecPixel>& value)
{
   m_richRecPixels = value;
}

inline void RichRecSegment::addToRichRecPixels(const SmartRef<RichRecPixel>& value)
{
   m_richRecPixels.push_back(value);
}

inline void RichRecSegment::removeFromRichRecPixels(const SmartRef<RichRecPixel>& value)
{
  SmartRefVector<RichRecPixel>::iterator iter =
    std::remove(m_richRecPixels.begin(), m_richRecPixels.end(), value);
  m_richRecPixels.erase(iter, m_richRecPixels.end());
}

inline void RichRecSegment::clearRichRecPixels()
{
   m_richRecPixels.clear();
}

inline const SmartRefVector<RichRecPhoton>& RichRecSegment::richRecPhotons() const
{
   return m_richRecPhotons;
}

inline SmartRefVector<RichRecPhoton>& RichRecSegment::richRecPhotons()
{
   return m_richRecPhotons;
}

inline void RichRecSegment::setRichRecPhotons(const SmartRefVector<RichRecPhoton>& value)
{
   m_richRecPhotons = value;
}

inline void RichRecSegment::addToRichRecPhotons(const SmartRef<RichRecPhoton>& value)
{
   m_richRecPhotons.push_back(value);
}

inline void RichRecSegment::removeFromRichRecPhotons(const SmartRef<RichRecPhoton>& value)
{
  SmartRefVector<RichRecPhoton>::iterator iter =
    std::remove(m_richRecPhotons.begin(), m_richRecPhotons.end(), value);
  m_richRecPhotons.erase(iter, m_richRecPhotons.end());
}

inline void RichRecSegment::clearRichRecPhotons()
{
   m_richRecPhotons.clear();
}

inline StreamBuffer& RichRecSegment::serialize(StreamBuffer& s) const 
{
  KeyedObject<int>::serialize(s);
  s << m_nEmittedPhotons
    << m_nDetectablePhotons
    << m_nSignalPhotons
    << m_nScatteredPhotons
    << m_nEmitPhotsPerEnBin
    << m_geomEfficiencyPerHPD
    << m_geomEfficiency
    << m_averageCKTheta
    << m_trackSegment
    << m_hpdPanelHitPoint
    << m_hitDetectorRegions
    << m_geomEfficiencyScat
    << m_richRecTrack(this)
    << m_richRecPixels(this)
    << m_richRecPhotons(this);
  return s;
}

inline StreamBuffer& RichRecSegment::serialize(StreamBuffer& s)
{
  KeyedObject<int>::serialize(s);
  s >> m_nEmittedPhotons
    >> m_nDetectablePhotons
    >> m_nSignalPhotons
    >> m_nScatteredPhotons
    >> m_nEmitPhotsPerEnBin
    >> m_geomEfficiencyPerHPD
    >> m_geomEfficiency
    >> m_averageCKTheta
    >> m_trackSegment
    >> m_hpdPanelHitPoint
    >> m_hitDetectorRegions
    >> m_geomEfficiencyScat
    >> m_richRecTrack(this)
    >> m_richRecPixels(this)
    >> m_richRecPhotons(this);
  return s;
}

inline std::ostream& RichRecSegment::fillStream(std::ostream& s) const
{
  s << "{ "
    << " nEmittedPhotons:\t" << m_nEmittedPhotons << std::endl
    << "   nDetectablePhotons:\t" << m_nDetectablePhotons << std::endl
    << "   nSignalPhotons:\t" << m_nSignalPhotons << std::endl
    << "   nScatteredPhotons:\t" << m_nScatteredPhotons << std::endl
    << "   nEmitPhotsPerEnBin:\t" << m_nEmitPhotsPerEnBin << std::endl
    << "   geomEfficiencyPerHPD:\t" << m_geomEfficiencyPerHPD << std::endl
    << "   geomEfficiency:\t" << m_geomEfficiency << std::endl
    << "   averageCKTheta:\t" << m_averageCKTheta << std::endl
    << "   trackSegment:\t" << m_trackSegment << std::endl
    << "   hpdPanelHitPoint:\t" << m_hpdPanelHitPoint << std::endl
    << "   hitDetectorRegions:\t" << m_hitDetectorRegions << std::endl
    << "   photonsInXPlus:\t" << photonsInXPlus() << std::endl
    << "   photonsInXMinus:\t" << photonsInXMinus() << std::endl
    << "   photonsInYPlus:\t" << photonsInYPlus() << std::endl
    << "   photonsInYMinus:\t" << photonsInYMinus() << std::endl
    << "   geomEfficiencyScat:\t" << m_geomEfficiencyScat << " } ";
  return s;
}

//Defintion of keyed container for RichRecSegment
typedef KeyedContainer<RichRecSegment, Containers::HashMap> RichRecSegments;
// typedef for std::vector of RichRecSegment
typedef std::vector<RichRecSegment*> RichRecSegmentVector;

#endif   ///RichRecBase_RichRecSegment_H
