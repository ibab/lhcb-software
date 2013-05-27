#ifndef OTSIMULATION_MCOTTIMECREATOR_H
#define OTSIMULATION_MCOTTIMECREATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "LHCbMath/LHCbMath.h"

namespace LHCb 
{
  class MCOTDeposit;
  class MCOTTime;
}

/** @class MCOTTimeCreator MCOTTimeCreator.h "OTSimulation/MCOTTimeCreator.h"
 *
 *  Implementation of MCOTimeCreator
 *  
 *  @author J. Nardulli and J. van Tilburg 
 *  @date   10/6/2004
 */

class MCOTTimeCreator : public GaudiAlgorithm {

 public:
 
  // constructor
  MCOTTimeCreator(const std::string& name, ISvcLocator* pSvcLocator );
  
  /// destructor
  virtual ~MCOTTimeCreator();
  
  /// intialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:
  typedef std::vector<LHCb::MCOTTime*> MCOTTimeVec;
  
  /// calculate tdc time
  int calculateTDCTime(const LHCb::MCOTDeposit* firstDep) const;

  /// Apply read out window
  bool insideReadoutWindow(int tdcTime) const;

  /// function to check if two deposits have the same channel ID
  bool insideAnalogDeadTime(const LHCb::MCOTDeposit* firstDep,
			    const LHCb::MCOTDeposit* secondDep) const;

  /// function to kill deposits due to the digital dead time
  bool insideDigitalDeadTime(const LHCb::MCOTDeposit* firstDep,
			     const LHCb::MCOTDeposit* secondDep) const;

  /// apply dead time
  StatusCode createTimes(LHCb::MCOTTimes* times);

  std::vector<double> m_startReadOutGate;  ///< start of readout gate
  double              m_deadTime;          ///< Analogdeadtime
  int                 m_countsPerBX;       ///< counts per BX
  int                 m_numberOfBX;        ///< Number of BX
  double              m_timePerBX;         ///< time Per BX
  bool                m_singleHitMode;     ///< Single Hit Mode
  double              m_nsToTDCcounts;     ///< Conversion from ns to tdc counts;
  double              m_readoutWindow;     ///< Readout window = m_countsPerBX * m_numberOfBX = 192 
};

inline int MCOTTimeCreator::calculateTDCTime(const LHCb::MCOTDeposit* firstDep) const {
  /// center around zero
  unsigned stationNum = (firstDep->channel()).station();
  /// Get deposit time apply sart of read out gate correction and convert
  /// to tdc counts
  double tdcTime = (firstDep->time()-m_startReadOutGate[stationNum-1])*m_nsToTDCcounts;

  return LHCb::Math::round(tdcTime);
}

inline bool MCOTTimeCreator::insideReadoutWindow(int tdcTime) const {
  return (tdcTime < int(m_readoutWindow) && tdcTime >= 0 ) ;
}

inline bool MCOTTimeCreator::insideAnalogDeadTime(const LHCb::MCOTDeposit* firstDep,
						  const LHCb::MCOTDeposit* secondDep) const { 
  // check whether consecutive fall inside analog deatime of first deposit of same channel
  // same channel
  const bool sameChannel = ( firstDep->channel() ).sameGeometry( secondDep->channel() );
  // Time difference. Sorted according to increasing time
  const double deltaT = std::abs( secondDep->time() - firstDep->time() );
  return ( sameChannel && deltaT < m_deadTime );
}

inline bool MCOTTimeCreator::insideDigitalDeadTime(const LHCb::MCOTDeposit* firstDep,
						   const LHCb::MCOTDeposit* secondDep) const { 
  // check whether to continue killing deposits
  const bool sameChannel = ( firstDep->channel() ).sameGeometry( secondDep->channel() );
  return ( sameChannel  && insideReadoutWindow( calculateTDCTime(secondDep) ) );
}

#endif // OTSIMULATION_MCOTTIMECREATOR_H
