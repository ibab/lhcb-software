
/** @file DumpRichTracksToTextFileAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::DumpRichTracksToTextFileAlg
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
#include "MCInterfaces/IRichRecMCTruthTool.h"

// Event
#include <Event/MCRichDigitSummary.h>

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      /** @class DumpRichTracksToTextFileAlg DumpRichTracksToTextFileAlg.h
       *
       *  Simple algorithm to dump Rich tracks + minimal MC information to text files.
       *
       *  @author Chris Jones
       *  @date   2009-02-16
       */
      class DumpRichTracksToTextFileAlg : public Rich::Rec::AlgBase
      {

      public:

        /// Standard constructor
        DumpRichTracksToTextFileAlg( const std::string& name, 
                                     ISvcLocator* pSvcLocator );

        virtual ~DumpRichTracksToTextFileAlg( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution
        virtual StatusCode finalize  ();    ///< Algorithm finalisation

      private:

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool*  m_truth;

        /// Event number
        unsigned long long m_nEvt;

        /// Detectors to write files for
        std::vector<bool> m_detectors;

        /// Output file stream
        std::ofstream * m_textFile;

      };

    }
  }
}

#endif // RICHRECMCALGORITHMS_DUMPRICHHITSTOTEXTFILEALG_H
