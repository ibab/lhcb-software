
//-----------------------------------------------------------------------------
/** @file IRichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool interface : IRichRawBufferToSmartIDsTool
 *
 *  CVS Log :-
 *  $Id: IRichRawBufferToSmartIDsTool.h,v 1.6 2006-03-12 16:12:37 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHRAWBUFFERTOSMARTIDSTOOL_H
#define RICHKERNEL_IRICHRAWBUFFERTOSMARTIDSTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichSmartID.h"

// numberings
#include "RichDet/RichDAQDefinitions.h"

/// Static Interface Identification
static const InterfaceID IID_IRichRawBufferToSmartIDsTool( "IRichRawBufferToSmartIDsTool", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichRawBufferToSmartIDsTool IRichRawBufferToSmartIDsTool.h RichKernel/IRichRawBufferToSmartIDsTool.h
 *
 *  Interface for tool for decoding the Rich Raw Buffer
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class IRichRawBufferToSmartIDsTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRawBufferToSmartIDsTool; }

  /** Access all RichSmartIDs for the current Event
   *  @return Reference to the container of RichSmartIDs
   */
  virtual const RichDAQ::PDMap & allRichSmartIDs() const = 0;

};

#endif // RICHKERNEL_IRICHRAWBUFFERTOSMARTIDSTOOL_H
