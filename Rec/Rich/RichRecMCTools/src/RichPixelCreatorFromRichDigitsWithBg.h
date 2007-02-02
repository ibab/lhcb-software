
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromRichDigitsWithBg.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromRichDigitsWithBg
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigitsWithBg.h,v 1.13 2007-02-02 10:06:27 jonrob Exp $
 *
 *  @author Andy Buckley   buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H
#define RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H 1

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"

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

      //------------------------------------------------------------------------------------
      /** @class PixelCreatorFromRichDigitsWithBg RichPixelCreatorFromRichDigitsWithBg.h
       *
       *  Tool for the creation and book-keeping of RichRecPixel objects.
       *
       *  Uses RichDigits from the digitisation as the parent objects.
       *  Additionally maintains a cache of RichDigits to be used as trackless ring
       *  background for testing reconstruction robustness to untracked tracks.
       *  Cache is filled as required from real events and for these events no pixels are
       *  created so effectively these events are skipped.
       *
       *  @author Andy Buckley   buckley@hep.phy.cam.ac.uk
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   05/10/2004
       */
      //------------------------------------------------------------------------------------

      class PixelCreatorFromRichDigitsWithBg : public Rich::Rec::PixelCreatorBase
      {

      public: // methods for Gaudi

        /// Standard constructor
        PixelCreatorFromRichDigitsWithBg( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

        /// Destructor
        virtual ~PixelCreatorFromRichDigitsWithBg(){}

        // Initialize method
        StatusCode initialize();

      public: // methods (and doxygen comments) inherited from public interface

        // Returns a RichRecPixel object pointer for given object.
        // If if it not possible NULL is return.
        LHCb::RichRecPixel * newPixel( const ContainedObject * obj ) const;

        // Form all possible RichRecPixels from input RichDigits.
        // The most efficient way to make all RichRecPixel objects in the event.
        StatusCode newPixels() const;

      private: // methods

        /// Returns a RichRecPixel object for given smart ID
        LHCb::RichRecPixel * newPixel( const LHCb::RichSmartID id ) const;

        /// Add the current event's digits to a collection of bg track digits
        StatusCode fillBgTrackStack() const;

      private: // data

        /// Pointer to RichSmartID tool
        const ISmartIDTool * m_smartIDTool;

        /// Pointer to RichMCTruth tool
        const Rich::MC::IMCTruthTool * m_mcTool;

        /// String containing input RichDigits location in TES
        std::string m_recoDigitsLocation;

        /// Typedef for mapping between MCParticles and RIchSmartIDs
        typedef Rich::Map<const LHCb::MCParticle*, LHCb::RichSmartID::Vector > BgTrackStack;

        /// Stack of RichSmartIDs for a single MCParticle for use as track background
        mutable Rich::Map<Rich::DetectorType, BgTrackStack> m_digitsForTrackBg;

        /// Number of background tracks to add to each event
        std::vector<size_t> m_numBgTracksToAdd;

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H
