
/** @file AddMissingMCRichTracksAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::AddMissingMCRichTracksAlg
 *
 *  CVS Log :-
 *  $Id: AddMissingMCRichTracksAlg.h,v 1.3 2007-12-05 17:41:08 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   26/10/2007
 */

#ifndef RICHRECMCALGORITHMS_AddMissingMCRichTracksAlg_H
#define RICHRECMCALGORITHMS_AddMissingMCRichTracksAlg_H 1

// STD
#include <set>

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// Event
#include "Event/MCRichTrack.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------
      /** @class AddMissingMCRichTracksAlg AddMissingMCRichTracksAlg.h
       *
       *  Checks the currently existing RichRecTracks to see if there are
       *  any MCRichTracks not accounted for. If there are, adds these to
       *  the RichRecTracks.
       *
       *  In addition, if required, a random fraction of these tracks can
       *  be added (default is to add all missing tracks).
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   26/10/2007
       */
      //-----------------------------------------------------------------------

      class AddMissingMCRichTracksAlg : public RichRecAlgBase
      {

      public:

        /// Standard constructor
        AddMissingMCRichTracksAlg( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~AddMissingMCRichTracksAlg();   ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalisation

      private: // Private data members

        ///< Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;

        /// Input location of MCRichTracks in TES
        std::string m_mcrTracksLocation;

        /// Private instance of the track creator for MC tracks
        const Rich::Rec::ITrackCreator * m_mcTkCreator;

        /// random number generator
        mutable Rndm::Numbers m_rndm;

        /// Fraction of tracks to reject
        double m_rejFrac;

      };

    }
  }
}

#endif // RICHRECMCALGORITHMS_AddMissingMCRichTracksAlg_H
