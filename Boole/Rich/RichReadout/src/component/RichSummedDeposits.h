
//===============================================================================
/** @file RichSummedDeposits.h
 *
 *  Header file for RICH digitisation algorithm : Rich::MC::Digi::SummedDeposits
 *
 *  CVS Log :-
 *  $Id: RichSummedDeposits.h,v 1.5 2007-02-02 10:13:42 jonrob Exp $
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
#include "Event/MCRichSummedDeposit.h"

// from CLHEP
#include "Kernel/Point3DTypes.h"

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

      /** @class SummedDeposits RichSummedDeposits.h
       *
       *  Performs a simulation of the photon energy desposition
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @author Alex Howard   a.s.howard@ic.ac.uk
       *  @date   2003-11-06
       */

      class SummedDeposits : public RichAlgBase
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
