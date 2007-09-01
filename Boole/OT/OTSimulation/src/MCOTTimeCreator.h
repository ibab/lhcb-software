// $Id: MCOTTimeCreator.h,v 1.8 2007-09-01 18:18:29 janos Exp $

#ifndef OTSIMULATION_MCOTTIMECREATOR_H
#define OTSIMULATION_MCOTTIMECREATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

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
  bool AnalogDeadTime(const LHCb::MCOTDeposit* firstDep,
                      const LHCb::MCOTDeposit* secondDep) const;

  /// function to kill deposits due to the digital dead time
  bool DigitalDeadTime(const LHCb::MCOTDeposit* firstDep,
                       const LHCb::MCOTDeposit* secondDep) const;

  /// apply dead time
  StatusCode createTimes(LHCb::MCOTTimes* times);

  std::vector<double> m_startReadOutGate;  ///< start of readout gate
  double              m_sizeOfReadOutGate; ///< end of readout gate   
  double              m_deadTime;          ///< Analogdeadtime
  int                 m_countsPerBX;       ///< counts per BX
  int                 m_numberOfBX;        ///< Number of BX
  double              m_timePerBX;         ///< time Per BX
  bool                m_singleHitMode;     ///< Single Hit Mode
  double              m_nsToTDCcounts;     ///< Conversion from ns to tdc counts;
  double              m_readoutWindow;     ///< Readout window = m_countsPerBX * m_numberOfBX = 192 ns 
};

inline int MCOTTimeCreator::calculateTDCTime(const LHCb::MCOTDeposit* firstDep) const {
  /// center around zero
  unsigned stationNum = (firstDep->channel()).station();
  /// Get deposit time apply sart of read out gate correction and convert
  /// to tdc counts
  double tdcTime = (firstDep->time()-m_startReadOutGate[stationNum-1])*m_nsToTDCcounts;
  
  return int(tdcTime);
}

inline bool MCOTTimeCreator::insideReadoutWindow(int tdcTime) const {
  return (tdcTime < m_readoutWindow && tdcTime >= 0 ) ;
}

inline bool MCOTTimeCreator::AnalogDeadTime(const LHCb::MCOTDeposit* firstDep,
                                            const LHCb::MCOTDeposit* secondDep) const { 
  // check whether to continue adding deposits
  return (firstDep->channel() == secondDep->channel() && 
          calculateTDCTime(secondDep) - calculateTDCTime(firstDep) < m_deadTime);
}

inline bool MCOTTimeCreator::DigitalDeadTime(const LHCb::MCOTDeposit* firstDep,
                                             const LHCb::MCOTDeposit* secondDep) const { 
  // check whether to continue killing deposits
  return (firstDep->channel() == secondDep->channel()  && 
          calculateTDCTime(secondDep) < m_readoutWindow);
}

#endif // OTSIMULATION_MCOTTIMECREATOR_H
