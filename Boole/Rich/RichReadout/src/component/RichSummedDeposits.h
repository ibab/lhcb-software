
//===============================================================================
/** @file RichSummedDeposits.h
 *
 *  Header file for RICH digitisation algorithm : Rich::MC::Digi::SummedDeposits
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

// Event model
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"

// Math
#include "GaudiKernel/Point3DTypes.h"

// kernel
#include "Kernel/ParticleID.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      /** @class SummedDeposits RichSummedDeposits.h
       *
       *  Performs a simulation of the photon energy desposition
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @author Alex Howard   a.s.howard@ic.ac.uk
       *  @date   2003-11-06
       */

      class SummedDeposits : public Rich::AlgBase
      {

      public:

        /// Constructor
        SummedDeposits ( const std::string& name, ISvcLocator* pSvcLocator );

        /// Desctructor
        virtual ~SummedDeposits();

        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

      private: // data

        std::string m_RichSummedDepositLocation;
        std::string m_RichDepositLocation;

        /// random number generator
        mutable Rndm::Numbers m_rndm;

      };

    }
  }
}

#endif // RICHREADOUT_RICHSIGNAL_H
