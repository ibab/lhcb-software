// $Id: RichRecSegmentTool.h,v 1.4 2002-12-02 09:42:21 jonrob Exp $
#ifndef RICHRECTOOLS_RICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_RICHRECSEGMENTTOOL_H 1

#include <string>
#include <cmath>
#include <map>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"

// RichUtils
#include "RichUtils/IRich1DProperty.h"
#include "RichUtils/Rich1DTabProperty.h"

// interfaces
#include "RichRecInterfaces/IRichRecSegmentTool.h"

// Detector
#include "RichDetTools/IRichDetInterface.h"

// Temporary SICB geometry package
#include "RiSicbGeom/PixelFinder.h"

// Forward declarations
class IDataProviderSvc;
class IParticlePropertySvc;

/** @class RichRecSegmentTool RichRecSegmentTool.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecSegments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecSegmentTool : public AlgTool,
                           virtual public IRichRecSegmentTool,
                           virtual public IIncidentListener {

public:

  /// Standard constructor
  RichRecSegmentTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichRecSegmentTool() {};

  /// Initialize method
  StatusCode initialize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Save a new RichRecSegment in the container
  void saveSegment( RichRecSegment * segment );

  /// Return Pointer to RichRecSegments
  RichRecSegments * richSegments();

  /// Obtain geometrical efficiency for this track and hypothesis
  double geomEfficiency ( RichRecSegment * segment,
                          const Rich::ParticleIDType & id );

  /// Expected number of observable signal photons for given segment and hypothesis
  double signalPhotons ( RichRecSegment * segment,
                         const Rich::ParticleIDType & id );

  /// Expected number of observable scattered photons for given segment and hypothesis
  double scatteredPhotons ( RichRecSegment * segment,
                            const Rich::ParticleIDType & id );

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  double allPhotons ( RichRecSegment * segment, 
                      const Rich::ParticleIDType & id );

  /// Expected number of emitted photons for given segment and hypothesis
  double emittedPhotons ( RichRecSegment * segment,
                          const Rich::ParticleIDType& id );

  /// Returns average Cherenkov angle for given particle hypothesis
  double avgCherenkovTheta( RichRecSegment * segment,
                            const Rich::ParticleIDType& id );

  /// Returns 'beta' for given particle hypothesis
  double beta( RichRecSegment * segment, const Rich::ParticleIDType& id );

  /// Is it geometrically possible for this segment to give Rich information
  bool hasRichInfo( RichRecSegment * segment );

  /// Is this segment above threshold for a given particle hypothesis
  bool aboveThreshold( RichRecSegment * segment, Rich::ParticleIDType& type );

private:

  /// Pointer to RichRecTracks
  RichRecSegments * m_segments;

  /// Pointer to event data service
  IDataProviderSvc* m_evtDataSvc;

  /// Pointer to detector data service
  IDataProviderSvc* m_detDataSvc;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

  /// Pointer to Particle property service
  IParticlePropertySvc * m_ppSvc;

  /// Pointer to PixelFinder tool (temporary for SICB pixels)
  IPixelFinder* m_pixelFinder;

  /// Flag for code profiling using ChronoStatSvc
  bool m_timing;

  /// Pointer to ChronoStat Service
  IChronoStatSvc * m_chrono;

  /// Array containing particle masses
  double m_particleMass[Rich::NParticleTypes];

  /// Location of RichRecSegments in TES
  std::string m_richRecSegmentLocation;

  /// Number of photons to use in geometrical efficiency calculation
  int m_nGeomEff;

  /// Increment parameter for HPD efficiencies
  double m_hpdInc;

  /// Cherenkov phi incrementation
  double m_incPhi;

  /// Number of photons to quit after in geometrical efficiency calculation
  /// if all so far have failed
  int m_ngeomEffBailout;

  std::vector<double> m_AvRefIndex; ///< Average refractive index for each radiator

  // Temporary variables
  std::map< Rich::Detector, double > m_detReflectorEff;

  double m_momThres[Rich::NRadiatorTypes][Rich::NParticleTypes];

};

#endif // RICHRECTOOLS_RICHRECSEGMENTTOOL_H
