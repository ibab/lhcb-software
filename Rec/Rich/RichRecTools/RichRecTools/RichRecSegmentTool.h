// $Id: RichRecSegmentTool.h,v 1.1.1.1 2002-07-28 10:46:22 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_RICHRECSEGMENTTOOL_H 1

#include <string>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <map>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// local
#include "RichRecTools/IRichRecSegmentTool.h"

// Detector
#include "RichDetTools/IRichDetInterface.h"

// Forward declarations
class IDataProviderSvc;
class IParticlePropertySvc;

/** @class RichRecSegmentTool RichRecSegmentTool.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecSegments
 *
 *  @author Chris Jones
 *  @date   15/03/2002
 */

class RichRecSegmentTool : public AlgTool,
                           virtual public IRichRecSegmentTool {

public:

  /// Standard constructor
  RichRecSegmentTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichRecSegmentTool() {}

  /// Obtain geometrical efficiency for this track and hypothesis
  double geomEfficiency ( SmartRef<RichRecSegment>& segment,
                          const Rich::ParticleIDType id );

  /// Expected photon signal for given segment and hypothesis
  double expObsPhotonSignal ( SmartRef<RichRecSegment>& segment,
                              const Rich::ParticleIDType id );

  /// Expected emitted photons for given segment and hypothesis
  double expPhotons ( SmartRef<RichRecSegment>& segment,
                      const Rich::ParticleIDType id );

private:

  /// Pointer to Tool Service
  IToolSvc * m_toolService;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

  /// Pointer to Particle property service
  IParticlePropertySvc * m_ppSvc;

  /// Map containing particle masses
  std::map<Rich::ParticleIDType,double> m_particleMass;

};

#endif // RICHRECTOOLS_RICHRECSEGMENTTOOL_H
