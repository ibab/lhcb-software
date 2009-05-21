
//---------------------------------------------------------------------------------
/** @file RichTrackSelEffMoni.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::TrackSelEff
 *
 *  CVS Log :-
 *  $Id: RichTrackSelEffMoni.h,v 1.1 2009-05-21 17:22:46 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   21/05/2009
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RichTrackSelEffMoni_H
#define RICHRECQC_RichTrackSelEffMoni_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichTrackSelector.h"
//#include "RichKernel/IRichParticleProperties.h"

// RichKernel
//#include "RichKernel/RichStatDivFunctor.h"

// boost
//#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------
      /** @class TrackSelEff RichTrackSelEffMoni.h
       *
       *  Monitors the efficiency of the RICH track selection
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   21/05/2009
       */
      //---------------------------------------------------------------------------------

      class TrackSelEff : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        TrackSelEff( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~TrackSelEff( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization
        
      private: // data

        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;  ///< Pointer to RichRecMCTruthTool interface
        const ITrackSelector * m_trSelector;  ///< Track selector

        /// Input location of Tracks in TES
        std::string m_trTracksLocation;

        /// Number of histogram bins
        unsigned int m_nBins;

      };

    }
  }
}

#endif // RICHRECQC_RichTrackSelEffMoni_H
