
/** @file IRichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool interface : IRichRawBufferToSmartIDsTool
 *
 *  CVS Log :-
 *  $Id: IRichRawBufferToSmartIDsTool.h,v 1.2 2004-11-03 09:31:54 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2004/10/30 19:18:14  jonrob
 *  Add in tool  interface for RawBuffer decoding
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */

#ifndef RICHDAQ_IRICHRAWBUFFERTOSMARTIDSTOOL_H
#define RICHDAQ_IRICHRAWBUFFERTOSMARTIDSTOOL_H 1

// Kernel
#include "Kernel/RichSmartID.h"

/// Static Interface Identification
static const InterfaceID IID_IRichRawBufferToSmartIDsTool( "IRichRawBufferToSmartIDsTool", 1, 0 );

/** @class IRichRawBufferToSmartIDsTool IRichRawBufferToSmartIDsTool.h RichKernel/IRichRawBufferToSmartIDsTool.h
 *
 *  Interface for tool for decoding the Rich Raw Buffer
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichRawBufferToSmartIDsTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRawBufferToSmartIDsTool; }

  /** Access all RichSmartIDs for the current Event
   * @return Container of RichSmartIDs
   */
  virtual const RichSmartID::Collection & allRichSmartIDs() const = 0;

};

#endif // RICHDAQ_IRICHRAWBUFFERTOSMARTIDSTOOL_H
