// $Id: RichRecPhotonTool.h,v 1.1.1.1 2002-07-28 10:46:21 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECPHOTONTOOL_H 
#define RICHRECTOOLS_RICHRECPHOTONTOOL_H 1

#include <string>
#include <map>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IToolSvc.h"

// Rich Detector
#include "RichDetTools/IRichDetInterface.h"

// local
#include "RichRecTools/IRichRecDataTool.h"
#include "RichRecTools/IRichRecPhotonTool.h"

// Forward declarations
class IDataProviderSvc;

/** @class RichRecPhotonTool RichRecPhotonTool.h
 *  
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones
 *  @date   15/03/2002
 */

class RichRecPhotonTool : public AlgTool, 
                          virtual public IRichRecPhotonTool,
                          virtual public IIncidentListener {

public:

  /// Standard constructor
  RichRecPhotonTool( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent );
  
  /// Destructor
  virtual ~RichRecPhotonTool(){}

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident);

  /// Form a Photon candidate from a Segment and a pixel.
  RichRecPhoton* reconstructPhoton( const SmartRef<RichRecSegment>& segment,
                                    const SmartRef<RichRecPixel>& pixel);

  /// Form all photon candidates for a given track and pixel
  SmartRefVector<RichRecPhoton> 
  reconstructPhotons(const SmartRef<RichRecTrack>& track,
                     const SmartRef<RichRecPixel>& pixel);

  /// Form all photon candidates for a given track, with all possible pixels.
  SmartRefVector<RichRecPhoton>
  reconstructPhotons(const SmartRef<RichRecTrack>& track);

  /// Form all photon candidates for a given pixel, with all possible tracks.
  SmartRefVector<RichRecPhoton>
  reconstructPhotons(const SmartRef<RichRecPixel>& pixel);

  /// Method to perform the asssociation of all tracks and pixels
  void reconstructPhotons();

private:

  /// Is it possible to make a photon candidate using this segment and pixel.
  bool photonPossible(  const SmartRef<RichRecSegment>& segment,
                        const SmartRef<RichRecPixel>& pixel);

  /// Pointer to event data service
  IDataProviderSvc* m_eventDataService;

  /// Pointer to Tool Service
  IToolSvc* m_toolService;

  /// Pointer to RichRecDataTool interface
  IRichRecDataTool* m_richRecDataTool;

  /// Location of RichRecPhotons in TDS
  std::string m_richRecPhotonLocation;

  /// SmartRef to RichRecPhotons
  RichRecPhotons * m_photons;

  /// Pointer to photon map
  std::map< int, SmartRef<RichRecPhoton> > m_photonExists;

  /// Flag for if photons where obtained from TDS or not
  bool m_fromTDS;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

};

#endif // RICHRECTOOLS_RICHRECPHOTONTOOL_H 
