
//===============================================================================
/** @file RichSimpleChargeSharing.h
 *
 *  Header file for RICH digitisation algorithm : RichSimpleChargeSharing
 *
 *  CVS Log :-
 *  $Id: RichSimpleChargeSharing.h,v 1.2 2006-11-06 09:41:56 cattanem Exp $
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
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"

// from CLHEP
#include "Kernel/Point3DTypes.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"

// kernel
#include "Kernel/ParticleID.h"

// LHCb namespace
using namespace LHCb;

//===============================================================================
/** @class RichSimpleChargeSharing RichSimpleChargeSharing.h
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

class RichSimpleChargeSharing : public RichAlgBase
{

public:

  /// Constructor
  RichSimpleChargeSharing ( const std::string& name, ISvcLocator* pSvcLocator );

  /// Desctructor
  virtual ~RichSimpleChargeSharing();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private: // data

  std::string m_RichDepositLocation;

  /// Fraction of pixels to charge share
  double m_shareFrac;

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  /// random number generator
  mutable Rndm::Numbers m_rndm;

};

#endif // RICHREADOUT_RICHSIGNAL_H
