// $Id: RichRecTrackTool.h,v 1.5 2002-12-19 09:32:44 cattanem Exp $
#ifndef RICHRECTOOLS_RICHRECTRACKTOOL_H
#define RICHRECTOOLS_RICHRECTRACKTOOL_H 1

#include <string>
#include <map>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"

// interfaces
#include "RichRecInterfaces/IRichRecTrackTool.h"
#include "RichRecInterfaces/IRichRecSegmentTool.h"

// Rich Detector
#include "RichDetTools/IRichDetInterface.h"

// Forward declarations
class IDataProviderSvc;

/** @class RichRecTrackTool RichRecTrackTool.h
 *
 *  Tool which performs useful methods on RichRecTracks
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecTrackTool : public AlgTool,
                         virtual public IRichRecTrackTool,
                         virtual public IIncidentListener {

public:

  /// Standard constructor
  RichRecTrackTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichRecTrackTool(){}

  /// Initialize and finalize methods
  StatusCode initialize();
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  void handle( const Incident& incident );

  /// Forms a new RichRecTrack object from a TrStoredTrack
  RichRecTrack * newTrack ( TrStoredTrack * trTrack );

  /// Form all tracks from input TrStoredTracks
  StatusCode newTracks();

  /// Expected number of observable signal photons for given track 
  /// and hypothesis
  double signalPhotons ( RichRecTrack * track,
                         const Rich::ParticleIDType& id );

  /// Expected number of observable signal+scattered photons for 
  /// given track and hypothesis
  double allPhotons ( RichRecTrack * track,
                      const Rich::ParticleIDType& id );

  /// Expected number of emitted photons for given track and hypothesis
  double emittedPhotons ( RichRecTrack * track,
                          const Rich::ParticleIDType& id );

  /// Expected number of observable scattered photons for given track and hypothesis
  double scatteredPhotons ( RichRecTrack * track,
                            const Rich::ParticleIDType& id );

  /// Is this track active in this radiator medium for given particle id
  bool activeInRadiator( RichRecTrack * track,
                         const Rich::RadiatorType rad,
                         const Rich::ParticleIDType id = Rich::Electron );

  /// Is it possible for this track to give Rich information
  bool hasRichInfo( RichRecTrack * track );

  /// Return Pointer to RichRecTracks
  RichRecTracks * richTracks();

  /// Locates parent MCParticle
  MCParticle * parentMCP ( const RichRecTrack * track );
  
  /// Locates parent TrStoredTrack
  TrStoredTrack * parentTrTrack ( const RichRecTrack * track );

  /// Is this track above threshold for a given particle type
  bool aboveThreshold( RichRecTrack * track, Rich::ParticleIDType & type );

  /// Is this track above threshold for a given particle type in a given radiator
  bool aboveThreshold( RichRecTrack * track,
                       Rich::ParticleIDType & type,
                       Rich::RadiatorType & radiator );

private:

  /// Pointer to RichRecTracks
  RichRecTracks * m_tracks;

  /// Pointer to event data service
  IDataProviderSvc* m_evtDataSvc;

  /// Pointer to RichRecSegment tool
  IRichRecSegmentTool * m_richRecSegmentTool;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Location of RichRecTracks in TES
  std::string m_richRecTrackLocation;

  /// Flag for code profiling using ChronoStatSvc
  bool m_timing;

  /// Pointer to ChronoStat Service
  IChronoStatSvc * m_chrono;

  /// Flag to signify all tracks have been formed
  bool m_allDone;

  /// track map
  std::map< int, bool > m_trackDone;

};

#endif // RICHRECTOOLS_RICHRECTRACKTOOL_H
