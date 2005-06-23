
//===============================================================================
/** @file RichSignal.h
 *
 *  Header file for RICH digitisation algorithm : RichSignal
 *
 *  CVS Log :-
 *  $Id: RichSignal.h,v 1.2 2005-06-23 15:10:12 jonrob Exp $
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
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"

// kernel
#include "Kernel/ParticleID.h"

/** @class RichSignal RichSignal.h
 *  
 *  Performs a simulation of the photon energy desposition
 * 
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */

class RichSignal : public RichAlgBase {

public:

  /// Constructor
  RichSignal ( const std::string& name, ISvcLocator* pSvcLocator );

  /// Desctructor
  virtual ~RichSignal();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private: // methods

  /// Process the event at the given location, with the corresponding TOF offset
  StatusCode ProcessEvent( const std::string & hitLoc, 
                           const double tofOffset,
                           const int eventType ) const;

private: // data

  MCRichSummedDeposits* m_mcSummedDeposits;
  MCRichDeposits* m_mcDeposits;

  std::string m_RichHitLocation;
  std::string m_RichPrevLocation;
  std::string m_RichPrevPrevLocation;
  std::string m_RichNextLocation;
  std::string m_RichNextNextLocation;
  std::string m_RichSummedDepositLocation;
  std::string m_RichDepositLocation;
  std::string m_lhcBkgLocation;

  /// Flag to turn on the use of the spillover events
  bool m_doSpillover;

  /// Flag to turn on the use of the LHC backgrounde events
  bool m_doLHCBkg;

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  mutable Rndm::Numbers m_rndm;

};

#endif // RICHREADOUT_RICHSIGNAL_H
