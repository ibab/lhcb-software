// $Id: RichRecDataTool.h,v 1.1.1.1 2002-07-28 10:46:22 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECDATATOOL_H
#define RICHRECTOOLS_RICHRECDATATOOL_H 1

// from STL
#include <string>
#include <iostream>
#include <stdio.h>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// local
#include "RichRecTools/IRichRecDataTool.h"

// Forward declarations
class IDataProviderSvc;

/** @class RichRecDataTool RichRecDataTool.h
 *
 *  Tool which locates the RichRec data in the TDS
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecDataTool : public AlgTool,
                        virtual public IRichRecDataTool {

public:

  /// Standard constructor
  RichRecDataTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecDataTool(){}

  /// Get RichRecTracks from TDS
  const SmartDataPtr<RichRecTracks> 
  getRichTracks (const std::string location = RichRecTrackLocation::Default);

  /// Get RichRecSegments from TDS
  const SmartDataPtr<RichRecSegments> 
  getRichSegments (const std::string location = RichRecSegmentLocation::Default);

  /// Get RichRecPixels from TDS
  const SmartDataPtr<RichRecPixels> 
  getRichPixels (const std::string location = RichRecPixelLocation::Default);

private:

  /// Pointer to event data service
  IDataProviderSvc* m_eventDataService;

};

#endif // RICHRECTOOLS_RICHRECDATATOOL_H
