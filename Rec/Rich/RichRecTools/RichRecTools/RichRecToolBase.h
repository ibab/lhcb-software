// $Id: RichRecToolBase.h,v 1.1 2002-11-14 13:54:23 jonrob Exp $
#ifndef RICHRECALGS_RICHRECTOOLBASE_H
#define RICHRECALGS_RICHRECTOOLBASE_H 1

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// RichRecTools
#include "RichRecTools/IRichRecTrackTool.h"
#include "RichRecTools/IRichRecSegmentTool.h"
#include "RichRecTools/IRichRecPixelTool.h"
#include "RichRecTools/IRichRecPhotonTool.h"

// Rich Detector
#include "RichDetTools/IRichDetInterface.h"

/** @class RichRecToolBase RichRecToolBase.h RichRecAlgs/RichRecToolBase.h
 *
 *  base for RichRec tools
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */

class RichRecToolBase : public AlgTool {

public:

  /// Standard constructor
  RichRecToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecToolBase(){}

  /// Initialize method
  StatusCode initialize();

protected:

  /// Pointer to event data service
  IDataProviderSvc* m_evtDataSvc;

  /// Pointer to RichRecTrack tool
  IRichRecTrackTool * m_richRecTrackTool;

  /// Pointer to RichRecSegment tool
  IRichRecSegmentTool * m_richRecSegmentTool;

  /// Pointer to RichRecPixel tool
  IRichRecPixelTool * m_richRecPixelTool;

  /// Pointer to RichRecPhoton tool
  IRichRecPhotonTool * m_richRecPhotonTool;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

};

#endif // RICHRECALGS_RICHRECTOOLBASE_H
