
//---------------------------------------------------------------------------------
/** @file RichRecSummaryQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::SummaryQC
 *
 *  CVS Log :-
 *  $Id: RichRecSummaryQC.h,v 1.4 2007-02-02 10:08:36 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RICHRECOQC_H
#define RICHRECQC_RICHRECOQC_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichParticleProperties.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

// Event
#include "Event/RichSummaryTrack.h"
#include "Event/MCParticle.h"

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

      //---------------------------------------------------------------------------------
      /** @class SummaryQC RichRecSummaryQC.h
       *
       *  Quality control monitor for Rich Reconstruction Summary data objects
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2002-07-02
       */
      //---------------------------------------------------------------------------------

      class SummaryQC : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        SummaryQC( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~SummaryQC( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // data

        const IParticleProperties * m_richPartProp; ///< Rich Particle properties

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;

        /// Track selector
        const ITrackSelector * m_trSelector;

        /// Location of the summary tracks
        std::string m_summaryLoc;

        // job options
        double m_minBeta;        ///< minimum beta value for 'saturated' tracks

        unsigned long m_nEvts;                   ///< Number of processed events
        unsigned long m_nTracks;                 ///< Number of summary tracks produced
        std::vector<unsigned long> m_nSegments;  ///< Number of summary segments produced per radiator
        std::vector<unsigned long> m_nSegmentsMC;  ///< Number of summary segments produced per radiator, with at least one true CK hit
        std::vector<unsigned long> m_nPhotons;   ///< Number of summary photons produced per radiator
        std::vector<unsigned long> m_nTruePhotons; ///< Number of true summary photons produced per radiator

      };

    }
  }
}

#endif // RICHRECQC_RICHRECOQC_H
