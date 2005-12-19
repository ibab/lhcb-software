// $Id: MCSTDepositCreator.h,v 1.1.1.1 2005-12-19 15:43:16 mneedham Exp $
#ifndef _MCITDEPOSITCREATOR_H_
#define _MCITDEPOSITCREATOR_H_

#include <vector>
#include <string>
#include <map>

#include "GaudiAlg/GaudiAlgorithm.h"

#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Event/MCSTDeposit.h"
#include "Event/MCHit.h"

class DeSTDetector;
class DeSTSector;
class ISTChargeSharingTool;
class ISTAmplifierResponse;
class ISTSignalToNoiseTool;
class ISTDepositedCharge;

class MCSTDepositCreator : public GaudiAlgorithm {

public:

  // Constructor: A constructor of this form must be provided.
  MCSTDepositCreator(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MCSTDepositCreator(); 

  StatusCode initialize();
  StatusCode execute();
 
  
private:

  /// create the deposits
  StatusCode createDeposits(const LHCb::MCHits* mcHitsCont, 
                            const double spillTime, 
                            LHCb::MCSTDeposits* depositCont);

  /// check can digitize this tracking hit
  bool hitToDigitize(const LHCb::MCHit* aHit) const;
 
  /// create vector of charge sites
  void distributeCharge(const double entryU, const double exitU, std::vector<double>& sites) const;
  void chargeSharing(const std::vector<double>& sites , const DeSTSector* aSector,
                     std::map<unsigned int,double>& stripMap) const;
  double chargeOnStrips(const std::map<unsigned int,double>& stripMap) const;

  double beetleResponse(const double time, const double capacitance, const std::string& type);

  std::string m_detType;
  DeSTDetector* m_tracker;

  std::string m_inputLocation;
  std::string m_outputLocation;

  ISTChargeSharingTool* m_chargeSharer;
  ISTDepositedCharge* m_depositedCharge;

  std::string  m_depChargeToolName;
  std::vector<ISTAmplifierResponse*> m_AmplifierResponse;

  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;
 
  std::vector<double> m_TOFVector;
  std::string m_chargeSharerName;
  std::vector<std::string> m_SpillVector;  // spills to digitize
  std::vector<std::string>  m_spillNames; // name of spills
  std::vector<double> m_spillTimes;
  std::vector<std::string> m_beetleResponseTypes;

  double m_minDistance;
  double m_tolerance;
  double m_siteSize;
  std::vector<double> m_xTalkParams;
  int m_maxNumSites;
  double m_scaling;

};

#endif








