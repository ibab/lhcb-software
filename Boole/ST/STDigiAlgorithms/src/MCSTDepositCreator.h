// $Id: MCSTDepositCreator.h,v 1.6 2010-04-07 09:27:37 mneedham Exp $
#ifndef MCSTDEPOSITCREATOR_H
#define MCSTDEPOSITCREATOR_H 1

// Gaudi
#include "Kernel/STAlgBase.h"

// Event
#include "Event/MCSTDeposit.h"
#include "Event/MCHit.h"
#include "Kernel/ILHCbMagnetSvc.h"

class DeSTSensor;
class ISTChargeSharingTool;
class ISiAmplifierResponse;
class ISiDepositedCharge;

/** @class MCSTDepositCreator MCSTDepositCreator.h
 *
 *  Algorithm for creating MCSTDeposits from MCHits. The following processes
 *  are simulated:
 *  - For each available spill the MCHits are digitized (spill-over).
 *  - The total charge is calculated using the deposited charge tool.
 *  - The entry or exit point is tilted due to Lorentz force.
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

class MCSTDepositCreator : public ST::AlgBase {

public:

  // Constructor: A constructor of this form must be provided.
  MCSTDepositCreator(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MCSTDepositCreator(); 

  StatusCode initialize();

  StatusCode execute();
  
private:

  /// create the deposits
  void createDeposits(const LHCb::MCHits* mcHitsCont, const double spillTime, 
                      std::vector<LHCb::MCSTDeposits*>& depositCont);

  /// check can digitize this tracking hit
  bool hitToDigitize(const LHCb::MCHit* aHit) const;
 
  /// create vector of charge sites
  void distributeCharge(const double entryU, const double exitU, 
                        std::vector<double>& sites) const;

  void chargeSharing(const std::vector<double>& sites, 
                     const DeSTSensor* aSensor,
                     std::map<unsigned int,double>& stripMap, 
                     double& possibleCollectedCharge) ;


  double beetleResponse(const double time, const double capacitance,
                        const std::string& type);

  void lorentzShift(const DeSTSensor* sensor,
                    const Gaudi::XYZPoint& midPoint,
                    Gaudi::XYZPoint& entry,  
                    Gaudi::XYZPoint& exit ) const ;

  // Tools

  /// List of tools for the different charge sharing parameters
  std::vector<ISTChargeSharingTool*> m_chargeSharer; 
  ISiDepositedCharge* m_depositedCharge; ///< Tool calculates accumulated charge

  std::string m_inputLocation;           ///< Input: MCHits
  std::string m_outputLocation;          ///< Output: MCSTDeposits
  std::vector<std::string> m_spillPaths; ///< Full path name of spills
  std::vector<std::string> m_outPaths; /// < Output paths
 
  /// List of tools for different beetle responses
  std::vector<ISiAmplifierResponse*> m_amplifierResponse;

  // job options
  std::vector<double> m_tofVector;       ///< Time offsets for the stations
  std::vector<std::string> m_spillNames; ///< Names of spills (e.g. Next, Prev)
  std::vector<double> m_spillTimes;      ///< Corresponding time offsets
  std::vector<std::string> m_sampleNames; ///< Names of spills (e.g. Next, Prev)
  std::vector<double> m_sampleTimes;      ///< Corresponding time offsets
  double m_minDistance;                  ///< Min. pathlength through sensor
  std::string m_chargeSharerName;        ///< Charge charing tool name
  std::string  m_depChargeToolName;      ///< Name of tool to calculate charge
  double m_siteSize;                     ///< Binning for charge sharing
  int m_maxNumSites;                     ///< Max number of charge sharing bins
  std::vector<double> m_xTalkParamsRightEven;///< Cross talk parameters
  std::vector<double> m_xTalkParamsLeftEven; ///< Cross talk parameters
  std::vector<double> m_xTalkParamsRightOdd; ///< Cross talk parameters
  std::vector<double> m_xTalkParamsLeftOdd;  ///< Cross talk parameters

  /// Defines tool names for the different charge sharing tools
  std::vector<std::string> m_chargeSharerTypes;

  double m_scaling;                      ///< Scale the deposited charge
  bool m_applyScaling;                   ///< Flag to apply the scaling
  /// Define tool names for different response types
  std::vector<std::string> m_beetleResponseTypes;
  bool m_useStatusConditions; ///< use dead strip info
  bool m_useSensDetID;
  double m_pMin; ///< min momentum particle to digitize

  const ILHCbMagnetSvc* m_fieldSvc ; ///< Pointer to the magnetic field service
  bool m_applyLorentzCorrection;
  double m_lorentzFactor;

};

#endif // MCSTDEPOSITCREATOR_H
