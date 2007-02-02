
//===============================================================================
/** @file RichSignal.h
 *
 *  Header file for RICH digitisation algorithm : Rich::MC::Digi::Signal
 *
 *  CVS Log :-
 *  $Id: RichSignal.h,v 1.12 2007-02-02 10:13:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#ifndef RICHREADOUT_RICHSIGNAL_H
#define RICHREADOUT_RICHSIGNAL_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"

// from CLHEP
#include "Kernel/Point3DTypes.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// kernel
#include "Kernel/ParticleID.h"

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

    //-----------------------------------------------------------------------------
    /** @namespace Digi
     *
     *  General namespace for RICH Digitisation simuation related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   17/01/2007
     */
    //-----------------------------------------------------------------------------
    namespace Digi
    {

      /** @class Signal RichSignal.h
       *
       *  Performs a simulation of the photon energy desposition
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @author Alex Howard   a.s.howard@ic.ac.uk
       *  @date   2003-11-06
       */

      class Signal : public RichAlgBase
      {

      public:

        /// Constructor
        Signal ( const std::string& name, ISvcLocator* pSvcLocator );

        /// Desctructor
        virtual ~Signal();

        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

      private: // methods

        /// Process the event at the given location, with the corresponding TOF offset
        StatusCode ProcessEvent( const std::string & hitLoc,
                                 const double tofOffset,
                                 const int eventType ) const;

      private: // data

        LHCb::MCRichDeposits* m_mcDeposits;

        // locations of MCRichHits in TES
        std::string m_RichHitLocation;
        std::string m_RichPrevLocation;
        std::string m_RichPrevPrevLocation;
        std::string m_RichNextLocation;
        std::string m_RichNextNextLocation;

        std::string m_RichDepositLocation;
        std::string m_lhcBkgLocation;

        /// Flag to turn on the use of the spillover events
        bool m_doSpillover;

        /// Flag to turn on the use of the LHC backgrounde events
        bool m_doLHCBkg;

        /// Flag to turn on testing on smartIDs from Gauss
        bool m_testSmartIDs;

        /// Pointer to RichSmartID tool
        const Rich::ISmartIDTool * m_smartIDTool;

        /// Pointer to RichMCTruth tool
        const Rich::MC::IMCTruthTool * m_truth;

        /// random number generator
        mutable Rndm::Numbers m_rndm;

      };

    }
  }
}

#endif // RICHREADOUT_RICHSIGNAL_H
