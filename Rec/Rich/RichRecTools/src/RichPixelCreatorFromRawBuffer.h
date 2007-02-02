
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.h
 *
 *  Header file for tool : Rich::Rec::PixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.h,v 1.11 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPixelCreatorFromRawBuffer_H
#define RICHRECTOOLS_RichPixelCreatorFromRawBuffer_H 1

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //---------------------------------------------------------------------------------
    /** @class PixelCreatorFromRawBuffer RichPixelCreatorFromRawBuffer.h
     *
     *  Tool for the creation and book-keeping of RichRecPixel objects.
     *  Works directly from RichSmartIDs decoded from the RawEvent
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   30/10/2004
     */
    //---------------------------------------------------------------------------------

    class PixelCreatorFromRawBuffer : public PixelCreatorBase
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PixelCreatorFromRawBuffer( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

      /// Destructor
      virtual ~PixelCreatorFromRawBuffer() {}

    };

  }
}

#endif // RICHRECTOOLS_RichPixelCreatorFromRawBuffer_H
