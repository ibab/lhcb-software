// $Id: RichRecToolBase.h,v 1.1.1.1 2003-04-01 13:21:56 jonesc Exp $
#ifndef RICHRECALGS_RICHRECTOOLBASE_H
#define RICHRECALGS_RICHRECTOOLBASE_H 1

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// RichRecTools
#include "RichRecBase/IRichRecTrackTool.h"
#include "RichRecBase/IRichRecSegmentTool.h"
#include "RichRecBase/IRichRecPixelTool.h"
#include "RichRecBase/IRichRecPhotonTool.h"

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
  virtual ~RichRecToolBase() {};

  /// Initialize method
  virtual StatusCode initialize();

  /// Finalize method
  virtual StatusCode finalize();

protected:

  /// Pointer to event data service
  IDataProviderSvc * m_evtDataSvc;

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

  /// Message service printout level
  int m_msgLevel;

};

#endif // RICHRECALGS_RICHRECTOOLBASE_H
