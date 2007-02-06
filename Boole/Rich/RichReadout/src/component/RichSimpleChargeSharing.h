
//===============================================================================
/** @file RichSimpleChargeSharing.h
 *
 *  Header file for RICH digitisation algorithm : Rich::MC::Digi::SimpleChargeSharing
 *
 *  CVS Log :-
 *  $Id: RichSimpleChargeSharing.h,v 1.4 2007-02-06 15:44:36 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/01/2006
 */
//===============================================================================

#ifndef RICHREADOUT_RICHSIGNAL_H
#define RICHREADOUT_RICHSIGNAL_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"

// Math
#include "GaudiKernel/Point3DTypes.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"

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

      //===============================================================================
      /** @class SimpleChargeSharing RichSimpleChargeSharing.h
       *
       *  Performs a simplistic probabilistic simulation of charge sharing
       *  in the RICH HPDs.
       *
       *  Simply adds to a fraction of pixels one neighbouring pixel with the same
       *  TOF and energy deposit. This algorithm is not a proper simulation
       *  but a simple method to replicate the observed affect in the 2005/2006
       *  testbeams.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   23/01/2006
       */
      //===============================================================================

      class SimpleChargeSharing : public RichAlgBase
      {

      public:

        /// Constructor
        SimpleChargeSharing ( const std::string& name, ISvcLocator* pSvcLocator );

        /// Desctructor
        virtual ~SimpleChargeSharing();

        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

      private: // data

        std::string m_RichDepositLocation;

        /// Fraction of pixels to charge share
        double m_shareFrac;

        /// Pointer to RichSmartID tool
        const Rich::ISmartIDTool * m_smartIDTool;

        /// random number generator
        mutable Rndm::Numbers m_rndm;

      };

    }
  }
}

#endif // RICHREADOUT_RICHSIGNAL_H
