
//-----------------------------------------------------------------------------
/** @file RichMCTrueTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::MCTrueTrackSelector
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichMCTrueTrackSelector_H
#define RICHRECTOOLS_RichMCTrueTrackSelector_H 1

// base class
#include "RichRecBase/RichTrackSelectorBase.h"

// interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

// Event model
#include "Event/MCParticle.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------------
      /** @class MCTrueTrackSelector RichMCTrueTrackSelector.h
       *
       *  Extends the core track selection in RichTrackSelectorBase to add a check
       *  to see if the track has an MC association
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2006-08-12
       */
      //-----------------------------------------------------------------------------

      class MCTrueTrackSelector : public TrackSelectorBase
      {

      public: // Gaudi methods

        /// Standard constructor
        MCTrueTrackSelector( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

        /// Destructor
        virtual ~MCTrueTrackSelector( );

        /// Tool initialization
        virtual StatusCode initialize();

      public: // interface methods

        /// Test if the given Track is selected
        virtual bool trackSelected( const LHCb::Track * track ) const;

        /// Test it the given RichRecTrack is selected
        virtual bool trackSelected( const LHCb::RichRecTrack * track ) const;

      private: // data

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool * m_richRecMCTruth = nullptr;

        /// MCParticle association weight
        double m_weight;

      };

    }
  }
}

#endif // RICHRECTOOLS_RichMCTrueTrackSelector_H
