
//---------------------------------------------------------------------------
/** @file RichRecPIDTupleAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PIDTupleAlg
 *
 *  CVS Log :-
 *  $Id: RichRecPIDTupleAlg.h,v 1.1 2007-08-09 16:17:58 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECPIDTUPLEALG_H
#define RICHRECPIDTUPLEALG_H 1

// STL
#include <sstream>

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichTrackSelector.h"

// Event model
#include "Event/RichPID.h"
#include "Event/MCParticle.h"

// boost
#include "boost/lexical_cast.hpp"


namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      /** @class PIDTupleAlg RichRecPIDTupleAlg.h
       *
       *  Simple tuple to store Rich Reconstruction and PID information
       * 
       *  @author Chris Jones
       *  @date   2007-08-06
       */
      class PIDTupleAlg : public Rich::Rec::TupleAlgBase
      {

      public:

        /// Standard constructor
        PIDTupleAlg( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~PIDTupleAlg( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution

      private:

        /// Associate RichPIDs via MCParticles
        StatusCode fillTrackTable();

      private:

        /// Track selector
        const ITrackSelector * m_trSelector;

        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface

        /// The PID locations to store results for
        std::vector<std::string> m_pidLocations;

        typedef std::vector<const LHCb::RichPID*> PIDVector;
        typedef std::map<const LHCb::MCParticle*, PIDVector > PIDMap;

        /// Map associating MCParticles to RichPID objects
        PIDMap m_pidMap;

      };

    }
  }
}

#endif // RICHRECPIDTUPLEALG_H
