// $Id: IRichRecDataTool.h,v 1.1.1.1 2002-07-28 10:46:22 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECDATATOOL_H
#define RICHRECTOOLS_IRICHRECDATATOOL_H 1

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"

// Rich
#include "RichKernel/RichDefinitions.h"
#include "Event/RichRecTrack.h"
#include "Event/RichRecPixel.h"

/** @class RichRecDataTool RichRecDataTool.h
 *
 *  Interface for tool which locates the RichRec data in the TDS
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecDataTool( "IRichRecDataTool" ,1 ,0 );

class IRichRecDataTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRecDataTool; }
  
  /// Get RichRecTracks from TDS
  virtual const SmartDataPtr<RichRecTracks> 
  getRichTracks (const std::string location = RichRecTrackLocation::Default) =0;

  /// Get RichRecSegments from TDS
  virtual const SmartDataPtr<RichRecSegments> 
  getRichSegments (const std::string location = RichRecSegmentLocation::Default) =0;

  /// Get RichRecPixels from TDS
  virtual const SmartDataPtr<RichRecPixels> 
  getRichPixels (const std::string location = RichRecPixelLocation::Default) =0;
  
protected:

private:

};
#endif // RICHRECTOOLS_IRICHRECDATATOOL_H
