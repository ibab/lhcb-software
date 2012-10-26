
/** @file DumpRichHitsToTextFileAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::DumpRichHitsToTextFileAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2009-02-16
 */

#ifndef RICHRECMCALGORITHMS_DUMPRICHHITSTOTEXTFILEALG_H
#define RICHRECMCALGORITHMS_DUMPRICHHITSTOTEXTFILEALG_H 1

// STL
#include <sstream>
#include <fstream>

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Interfaces
#include "MCInterfaces/IRichMCTruthTool.h"

// Event
#include <Event/MCRichDigitSummary.h>

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      /** @class DumpRichHitsToTextFileAlg DumpRichHitsToTextFileAlg.h
       *
       *  Simple algorithm to dump Rich hits + minimal MC information to text files.
       *
       *  @author Chris Jones
       *  @date   2009-02-16
       */
      class DumpRichHitsToTextFileAlg : public Rich::Rec::AlgBase
      {

      public:

        /// Standard constructor
        DumpRichHitsToTextFileAlg( const std::string& name, 
                                   ISvcLocator* pSvcLocator );

        virtual ~DumpRichHitsToTextFileAlg( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution
 
      private:

        // dump the given RICH panel to text file
        StatusCode dumpToTextfile( const Rich::DetectorType rich,
                                   const Rich::Side         panel,
                                   const Rich::RadiatorType rad ) const;

      private:

        const Rich::MC::IMCTruthTool * m_truth;       ///< MC truth tool

        /// Flag to use 'radiator corrected' local positions
        bool m_useCorrPos;

        /// Event number
        unsigned long long m_nEvt;

        /// Detectors to write files for
        std::vector<bool> m_detectors;

        /// Allow only one MC association per hit
        bool m_onlyOneMCassoc;

      };

    }
  }
}

#endif // RICHRECMCALGORITHMS_DUMPRICHHITSTOTEXTFILEALG_H
