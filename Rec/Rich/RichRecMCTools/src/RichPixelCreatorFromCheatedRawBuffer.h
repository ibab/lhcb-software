
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRawBuffer.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromCheatedRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRawBuffer.h,v 1.4 2007-03-09 22:57:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichPixelCreatorFromCheatedRawBuffer_H
#define RICHRECMCTOOLS_RichPixelCreatorFromCheatedRawBuffer_H 1

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "RichKernel/IRichMCTruthTool.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //--------------------------------------------------------------------------------------
      /** @class PixelCreatorFromCheatedRawBuffer RichPixelCreatorFromCheatedRawBuffer.h
       *
       *  Tool for the creation and book-keeping of RichRecPixel objects.
       *
       *  Uses RichSmartIDs from RawBuffer but then refers to the
       *  MCRichOpticalPhoton objects to use the true MC hit positions.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   15/09/2003
       *
       *  @todo Find a way to deal properly with more than one MCRichHit per RichSmartID
       */
      //--------------------------------------------------------------------------------------

      class PixelCreatorFromCheatedRawBuffer : public Rich::Rec::PixelCreatorBase
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PixelCreatorFromCheatedRawBuffer( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

        /// Destructor
        virtual ~PixelCreatorFromCheatedRawBuffer(){}

        // Initialize method
        StatusCode initialize();

      protected: // methods

        /// Build a new RichRecPixel
        virtual LHCb::RichRecPixel * buildPixel ( const Rich::HPDPixelCluster& cluster ) const;

      private: // data

        /// MC Truth tool
        const Rich::MC::IMCTruthTool * m_mcTool;

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RichPixelCreatorFromCheatedRawBuffer_H
