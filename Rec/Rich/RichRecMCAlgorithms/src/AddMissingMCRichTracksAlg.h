
/** @file AddMissingMCRichTracksAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::AddMissingMCRichTracksAlg
 *
 *  CVS Log :-
 *  $Id: AddMissingMCRichTracksAlg.h,v 1.2 2007-10-26 10:38:34 jonrob Exp $
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

      /** @class AddMissingMCRichTracksAlg AddMissingMCRichTracksAlg.h
       *
       *  Checks the currently existing RichRecTracks to see if there are
       *  any MCRichTracks not accounted for. If there are, adds these to 
       *  the RichRecTracks.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   26/10/2007
       */

      class AddMissingMCRichTracksAlg : public RichRecAlgBase
      {

      public:

        /// Standard constructor
        AddMissingMCRichTracksAlg( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~AddMissingMCRichTracksAlg();   ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // Private data members

        ///< Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth; 

        /// Input location of MCRichTracks in TES
        std::string m_mcrTracksLocation;

        /// Private instance of the track creator for MC tracks
        const Rich::Rec::ITrackCreator * m_mcTkCreator;
        
      };

    }
  }
}

#endif // RICHRECMCALGORITHMS_AddMissingMCRichTracksAlg_H
