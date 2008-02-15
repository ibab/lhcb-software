// $Id: MCSTDepositCreator.h,v 1.2 2008-02-15 14:49:05 cattanem Exp $
#ifndef MCSTDEPOSITCREATOR_H
#define MCSTDEPOSITCREATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/MCSTDeposit.h"
#include "Event/MCHit.h"

class DeSTDetector;
class DeSTSector;
class ISharingTool;
class ISiAmplifierResponse;
class ISTSignalToNoiseTool;
class ISiDepositedCharge;

/** @class MCSTDepositCreator MCSTDepositCreator.h
 *
 *  Algorithm for creating MCSTDeposits from MCHits. The following processes
 *  are simulated:
 *  - For each available spill the MCHits are digitized (spill-over).
 *  - The total charge is calculated using the deposited charge tool.
 *  - The charge is distributed in space (bins) between the strips.
 *  - Charge sharing between the strips is applied by looping over these bins.
 *  - Capacitive coupling between the strips is applied using the xTalkParams.
 *  - The time sampling of the beetle chip is simulated using the available \
      ISiAmplifierResponse tools.
 *  - The number of electrons is converted into a ADC value using the S/N tool.
 *  - A deposit is created from the ADC value, the channel and the MCHit link.
 *  - The deposits are sorted by STChannelID.
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   05/01/2006
 */

class MCSTDepositCreator : public GaudiAlgorithm {

public:

  // Constructor: A constructor of this form must be provided.
  MCSTDepositCreator(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MCSTDepositCreator(); 

  StatusCode initialize();

  StatusCode execute();
  
private:

  /// create the deposits
  void createDeposits(const LHCb::MCHits* mcHitsCont, const double spillTime, 
                      LHCb::MCSTDeposits* depositCont);

  /// check can digitize this tracking hit
  bool hitToDigitize(const LHCb::MCHit* aHit) const;
 
  /// create vector of charge sites
  void distributeCharge(const double entryU, const double exitU, 
                        std::vector<double>& sites) const;

  void chargeSharing(const std::vector<double>& sites, 
                     const DeSTSector* aSector,
                     std::map<unsigned int,double>& stripMap, double& possibleCollectedCharge) const;


  double beetleResponse(const double time, const double capacitance,
                        const std::string& type);

  // Tools
  ISTChargeSharingTool* m_chargeSharer;  ///< Charge sharing tool 
  ISiDepositedCharge* m_depositedCharge; ///< Tool calculates accumulated charge
  ISTSignalToNoiseTool* m_sigNoiseTool;  ///< S/N tool

  std::string m_inputLocation;           ///< Input: MCHits
  std::string m_outputLocation;          ///< Output: MCSTDeposits
  std::vector<std::string> m_spillPaths; ///< Full path name of spills

  /// Pointer to the main Detector Element
  DeSTDetector* m_tracker;

  /// List of tools for different beetle responses
  std::vector<ISiAmplifierResponse*> m_amplifierResponse;
 
  // job options
  std::vector<double> m_tofVector;       ///< Time offsets for the stations
  std::vector<std::string> m_spillNames; ///< Names of spills (e.g. Next, Prev)
  std::vector<double> m_spillTimes;      ///< Corresponding time offsets
  double m_minDistance;                  ///< Min. pathlength through sensor
  std::string m_chargeSharerName;        ///< Charge charing tool name
  std::string  m_depChargeToolName;      ///< Name of tool to calculate charge
  double m_siteSize;                     ///< Binning for charge sharing
  int m_maxNumSites;                     ///< Max number of charge sharing bins
  std::vector<double> m_xTalkParams;     ///< Cross talk parameters
  std::string m_detType;                 ///< Detector type: IT or TT
  std::string m_sigNoiseToolName;        ///< Name of tool to calculate S/N 
  double m_scaling;                      ///< Scale the deposited charge
  /// Define tool names for different response types
  std::vector<std::string> m_beetleResponseTypes;
  bool m_useStatusConditions; ///< use dead strip info
  bool m_useSensDetID;

};

#endif // MCSTDEPOSITCREATOR_H
