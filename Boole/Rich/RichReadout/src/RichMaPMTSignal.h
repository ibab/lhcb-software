#ifndef RICHREADOUT_RICHMaPMTSIGNAL_H
#define RICHREADOUT_RICHMaPMTSIGNAL_H 1

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

class RichMaPMTSignal : public Algorithm {

public:

  RichMaPMTSignal(const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichMaPMTSignal();

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

#endif // RICHREADOUT_RICHMaPMTSIGNAL_H
