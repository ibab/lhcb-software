
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRawBuffer.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromCheatedRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRawBuffer.h,v 1.3 2007-02-02 10:06:27 jonrob Exp $
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

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @namespace MC
     *
     *  General namespace for RICH MC related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   05/12/2006
     */
    //-----------------------------------------------------------------------------
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
        virtual LHCb::RichRecPixel * buildPixel ( const LHCb::RichSmartID id ) const;

      private: // data

        /// MC Truth tool
        const Rich::MC::IMCTruthTool * m_mcTool;

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RichPixelCreatorFromCheatedRawBuffer_H
