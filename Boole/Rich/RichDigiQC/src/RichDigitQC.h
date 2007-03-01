
//------------------------------------------------------------------------------------
/** @file RichDigitQC.h
 *
 *  Header file for RICH Digitisation Quality Control algorithm : Rich::MC::Digi::DigitQC
 *
 *  CVS Log :-
 *  $Id: RichDigitQC.h,v 1.24 2007-03-01 19:36:11 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//------------------------------------------------------------------------------------

#ifndef RICHDIGIQC_RICHDIGITQC_H
#define RICHDIGIQC_RICHDIGITQC_H 1

// STD
#include <sstream>

// Boost
#include "boost/lexical_cast.hpp"

// base class
#include "RichKernel/RichHistoAlgBase.h"

// Event model
#include "Event/MCRichDigit.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// RICH Interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      /** @class DigitQC RichDigitQC.h
       *
       *  Monitor for Rich digitisation and DAQ simulation
       *
       *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
       *  @date   2003-09-08
       */

      class DigitQC : public Rich::HistoAlgBase
      {

      public:

        /// Standard constructor
        DigitQC( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~DigitQC( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // methods

        /// Returns the location of container for the MCRichHit associated to the given digit
        std::string mchitLocation( const LHCb::MCRichDigit * digit ) const;

      private: // data

        /// Pointer to RICH system detector element
        const DeRichSystem * m_richSys;

        /// Pointer to RichSmartID tool
        const Rich::ISmartIDTool * m_smartIDs;

        /// Pointer to MC truth tool
        const Rich::MC::IMCTruthTool * m_mcTool;

        // job options
        std::string m_digitTDS;  ///< Location of MCRichDigits in TES
        bool m_extraHists;       ///< Flag to turn on the production of additional histograms

        /// Number of events processed
        unsigned int m_evtC;

        /// L1 occupancy counter
        typedef Rich::HashMap< const Rich::DAQ::Level1ID, unsigned int > L1Counter;

        /// Counter for hits in each HPD
        typedef Rich::HashMap< const LHCb::RichSmartID, unsigned int > HPDCounter;
        HPDCounter m_nHPD[Rich::NRiches]; ///< Tally for HPD occupancy, in each RICH

        typedef Rich::HashMap< std::string, unsigned int > SpillCount;
        typedef std::vector< SpillCount > SpillDetCount;

        /// Number of digitised hits per RICH detector and event location
        SpillDetCount m_spillDigits;

        /// Number of digitised hits per RICH detector and event location
        SpillDetCount m_totalSpills;

        /// Number of hits in each RICH
        std::vector< unsigned int > m_allHits;

        /// Number of rayleigh scattered hits in each RICH
        std::vector< unsigned int > m_scattHits;

        /// Number of charged track hits in each RICH
        std::vector< unsigned int > m_chrgTkHits;

        /// Number of gas quartz window CK hits in each RICH
        std::vector< unsigned int > m_gasQCK;

        /// Number of HPD quartz window CK hits in each RICH
        std::vector< unsigned int > m_hpdQCK;

        /// Number of nitrogen CK hits in each RICH
        std::vector< unsigned int > m_nitroQCK;

        /// Number of nitrogen CK hits in each RICH
        std::vector< unsigned int > m_aeroFiltQCK;

        /// Number of background hits in each RICH
        std::vector< unsigned int > m_bkgHits;

        /// Number of charge shared hits in each RICH
        std::vector< unsigned int > m_chrgShrHits;

        /// List of event locations to look for MCRichHits in
        typedef Rich::HashMap< std::string, bool > EventLocations;
        EventLocations m_evtLocs;

      };

      inline std::string DigitQC::mchitLocation( const LHCb::MCRichDigit * digit ) const
      {
        // Always just use the first hit, since this will always be signal if the digit has a signal contribution
        return ( digit->hits().empty() ?
                 "UNKNOWN" : objectLocation( digit->hits().front().mcRichHit()->parent() ) );
      }

    }
  }
}

#endif // RICHDIGIQC_RICHDIGITQC_H
