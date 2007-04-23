
//-----------------------------------------------------------------------------
/** @file IRichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool interface : Rich::DAQ::IRawBufferToSmartIDsTool
 *
 *  CVS Log :-
 *  $Id: IRichRawBufferToSmartIDsTool.h,v 1.9 2007-04-23 12:44:03 jonrob Exp $
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

// RichKernel
#include "RichKernel/RichDecodedData.h"

/// Static Interface Identification
static const InterfaceID IID_IRichRawBufferToSmartIDsTool( "Rich::DAQ::IRawBufferToSmartIDsTool", 1, 0 );

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class IRawBufferToSmartIDsTool IRichRawBufferToSmartIDsTool.h RichKernel/IRichRawBufferToSmartIDsTool.h
     *
     *  Interface for tool for decoding the Rich Raw Buffer
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class IRawBufferToSmartIDsTool : public virtual IAlgTool 
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichRawBufferToSmartIDsTool; }

      /** Access all RichSmartIDs for the current Event
       *  @return Reference to the decoded RICH data structure
       */
      virtual const Rich::DAQ::L1Map & allRichSmartIDs() const = 0;

    };

  }
}

#endif // RICHKERNEL_IRICHRAWBUFFERTOSMARTIDSTOOL_H
