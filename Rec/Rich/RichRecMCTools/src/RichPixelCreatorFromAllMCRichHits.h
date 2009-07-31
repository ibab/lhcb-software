
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromAllMCRichHits.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromAllMCRichHits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromAllMCRichHits.h,v 1.1 2009-07-31 11:57:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichPixelCreatorFromAllMCRichHits_H
#define RICHRECMCTOOLS_RichPixelCreatorFromAllMCRichHits_H 1

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "MCInterfaces/IRichMCTruthTool.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //--------------------------------------------------------------------------------------
      /** @class PixelCreatorFromAllMCRichHits RichPixelCreatorFromAllMCRichHits.h
       *
       *  Tool for the creation and book-keeping of RichRecPixel objects.
       *
       *  Creates a single RichRecPixel object for each MCRichHit in the event.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   15/09/2003
       */
      //--------------------------------------------------------------------------------------

      class PixelCreatorFromAllMCRichHits : public Rich::Rec::PixelCreatorBase
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PixelCreatorFromAllMCRichHits( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

        /// Destructor
        virtual ~PixelCreatorFromAllMCRichHits(){}

        // Initialize method
        StatusCode initialize();

      protected: // methods

        /// Build a new RichRecPixel from an Rich::HPDPixelCluster
        virtual LHCb::RichRecPixel * buildPixel ( const Rich::HPDPixelCluster& cluster ) const;

        /// Build a new RichRecPixel from a single LHCb::RichSmartID
        virtual LHCb::RichRecPixel * buildPixel( const LHCb::RichSmartID& id ) const;

      private: // data

        /// MC Truth tool
        const Rich::MC::IMCTruthTool * m_mcTool;

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RichPixelCreatorFromAllMCRichHits_H
