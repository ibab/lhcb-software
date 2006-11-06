
//===============================================================================
/** @file RichSummedDeposits.h
 *
 *  Header file for RICH digitisation algorithm : RichSummedDeposits
 *
 *  CVS Log :-
 *  $Id: RichSummedDeposits.h,v 1.4 2006-11-06 09:41:56 cattanem Exp $
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

// LHCb namespace
using namespace LHCb;

/** @class RichSummedDeposits RichSummedDeposits.h
 *
 *  Performs a simulation of the photon energy desposition
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */

class RichSummedDeposits : public RichAlgBase 
{

public:

  /// Constructor
  RichSummedDeposits ( const std::string& name, ISvcLocator* pSvcLocator );

  /// Desctructor
  virtual ~RichSummedDeposits();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private: // data

  std::string m_RichSummedDepositLocation;
  std::string m_RichDepositLocation;

  /// random number generator
  mutable Rndm::Numbers m_rndm;

};

#endif // RICHREADOUT_RICHSIGNAL_H
