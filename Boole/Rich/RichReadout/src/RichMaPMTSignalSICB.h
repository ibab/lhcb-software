#ifndef RICHREADOUT_RICHMaPMTSIGNALSICB_H
#define RICHREADOUT_RICHMaPMTSIGNALSICB_H 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/LorentzVector.h"

// Event model
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCParticle.h"
#include "Event/ParticleID.h"

// RichKernel
#include "RichKernel/RichDetectorType.h"

// MaPMT detector tool
#include "RichDetTools/IMaPMTDetTool.h"

class RichMaPMTSignalSICB : public Algorithm {

public:

  RichMaPMTSignalSICB(const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichMaPMTSignalSICB();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  StatusCode ProcessEvent( std::string hitLoc, double tofOffset );
  double SimpleEnergy();

  MCRichSummedDeposits* mcSummedDeposits;
  MCRichDeposits* mcDeposits;

  /// String containing MCRichDeposits location in TDS
  std::string m_RichHitLocation;
  std::string m_RichPrevLocation;
  std::string m_RichPrevPrevLocation;
  std::string m_RichNextLocation;
  std::string m_RichSummedDepositLocation;
  std::string m_RichDepositLocation;

  double m_BunchSpace;
  Rndm::Numbers m_rndm;

  IMaPMTDetTool * m_mapmtDet;

};

#endif // RICHREADOUT_RICHMaPMTSIGNALSICB_H
