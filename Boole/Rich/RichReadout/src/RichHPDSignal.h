#ifndef RICHREADOUT_RICHHPDSIGNAL_H
#define RICHREADOUT_RICHHPDSIGNAL_H 1

#include <algorithm>
#include <string>
#include <cmath>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

//From RichDetTools
#include "RichDetTools/IRichDetInterface.h"

#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"

class RichHPDSignal : public Algorithm {

public:

  RichHPDSignal(const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichHPDSignal();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private: // methods

  StatusCode ProcessEvent( std::string hitLoc, double tofOffset );

private: // data

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

  IRichDetInterface* m_DetInterface;

};

#endif // RICHREADOUT_RICHHPDSIGNAL_H
