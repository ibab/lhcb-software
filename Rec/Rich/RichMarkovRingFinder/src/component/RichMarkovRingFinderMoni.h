
//---------------------------------------------------------------------------
/** @file RichMarkovRingFinderMoni.h
 *
 *  Header file for algorithm class : RichMarkovRingFinderMoni
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderMoni.h,v 1.21 2008-06-09 15:02:36 shaines Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichMarkovRingFinderMoni_H
#define RICHRECMONITOR_RichMarkovRingFinderMoni_H 1

//STD
#include <sstream>

#include <cmath>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecTrack.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
//#include "MCInterfaces/IRichMCTruthTool.h"//causes compile error
#include "MCInterfaces/IRichRecMCTruthTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class RichMarkovRingFinderMoni RichMarkovRingFinderMoni.h
       *
       *  Runs various calibration tasks for the markov ring finder
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class RichMarkovRingFinderMoni : public RichRecHistoAlgBase
      {

      public:

        /// Standard constructor
        RichMarkovRingFinderMoni( const std::string& name,
                                  ISvcLocator* pSvcLocator );

        virtual ~RichMarkovRingFinderMoni( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // data

        /// Pointer RichDetector ray tracing tool
        const IRayTracing * m_rayTrace;

        /// Pointer to RichSmartID tool
        const ISmartIDTool * m_idTool;

        /// Track selector
        const ITrackSelector * m_trSelector;

        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;  ///< Pointer to RichRecMCTruthTool interface
        //mutable const Rich::MC::IMCTruthTool* m_richMCTruth;  ///< Pointer to RichRecMCTruthTool interface

 //          /// access RichMCTruthTool tool on demand
//         const Rich::MC::IMCTruthTool * richMCTool() const
//         {
//           if ( !m_richMCTruth ) { acquireTool( "RichMCTruthTool", m_richMCTruth ); }
//           return m_richMCTruth;
//         }

        /// access RichRecMCTruthTool tool on demand
        const Rich::Rec::MC::IMCTruthTool * richRecMCTool() const
        {
          if ( !m_richRecMCTruth ) { acquireTool( "RichRecMCTruthTool", m_richRecMCTruth ); }
          return m_richRecMCTruth;
        }


        /// Location of Rings in TES
        std::string m_ringLoc;
        
        /// Minimum separation between ring centres
        double m_sizesepcut;
        double m_regionscale;


        };

    }
  }
}

#endif // RICHRECMONITOR_RichMarkovRingFinderMoni_H
