
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.h
 *
 *  Header file for tool : RichPixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.h,v 1.10 2006-12-01 17:05:09 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPixelCreatorFromRawBuffer_H
#define RICHRECTOOLS_RichPixelCreatorFromRawBuffer_H 1

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

//---------------------------------------------------------------------------------
/** @class RichPixelCreatorFromRawBuffer RichPixelCreatorFromRawBuffer.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Works directly from RichSmartIDs decoded from the RawEvent
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

class RichPixelCreatorFromRawBuffer : public RichPixelCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPixelCreatorFromRawBuffer( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromRawBuffer() {}

};

#endif // RICHRECTOOLS_RichPixelCreatorFromRawBuffer_H
