// $Id: RichRecTrackTool.h,v 1.1.1.1 2002-07-28 10:46:22 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECTRACKTOOL_H
#define RICHRECTOOLS_RICHRECTRACKTOOL_H 1

#include <string>
#include <iostream>
#include <stdio.h>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// local
#include "RichRecTools/IRichRecTrackTool.h"
#include "RichRecTools/IRichRecSegmentTool.h"

// Forward declarations
class IDataProviderSvc;

/** @class RichRecTrackTool RichRecTrackTool.h
 *
 *  Tool which performs useful methods on RichRecTracks
 *
 *  @author Chris Jones
 *  @date   15/03/2002
 */

class RichRecTrackTool : public AlgTool,
                         virtual public IRichRecTrackTool {

public:

  /// Standard constructor
  RichRecTrackTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichRecTrackTool(){}

  /// Locates parent MCParticle
  void parent ( const SmartRef<RichRecTrack>& track,
                SmartRef<MCParticle>& pParent );

  /// Locates parent TrStoredTrack
  void parent ( const SmartRef<RichRecTrack>& track,
                SmartRef<TrStoredTrack>& pParent );


  /// Expected photon signal for given track and hypothesis
  double expObsPhotonSignal ( const SmartRef<RichRecTrack>& track,
                              const Rich::ParticleIDType id );

  /// Expected emitted photons for given track and hypothesis
  double expPhotons ( const SmartRef<RichRecTrack>& track,
                      const Rich::ParticleIDType id );

private:

  /// Pointer to Tool Service
  IToolSvc * m_toolService;

  /// Pointer to RichRecSegment tool
  IRichRecSegmentTool * m_richRecSegmentTool;

};

#endif // RICHRECTOOLS_RICHRECTRACKTOOL_H
