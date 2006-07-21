// $Id: MCOTTimeCreator.h,v 1.7 2006-07-21 08:05:07 janos Exp $

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

class MCOTTimeCreator : public GaudiAlgorithm

{

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
  bool insideReadOutWindow(int tdcTime) const;

  /// function to check if two deposits have the same channel ID
  bool AnalogDeadTime(const LHCb::MCOTDeposit* firstDep,
                      const LHCb::MCOTDeposit* secondDep) const;

  /// function to kill deposits due to the digital dead time
  bool DigitalDeadTime(const LHCb::MCOTDeposit* firstDep,
                       const LHCb::MCOTDeposit* secondDep) const;

  /// apply dead time
  StatusCode createTimes(LHCb::MCOTTimes* times);

  std::vector<double> m_startReadOutGate; ///< start of readout gate
  double m_sizeOfReadOutGate;             ///< end of readout gate   
  double m_deadTime;                      ///< Analogdeadtime
  int m_countsPerBX;                      ///< counts per BX
  int  m_numberOfBX;                      ///< Number of BX
  double m_timePerBX;                     ///< time Per BX
  bool m_singleHitMode;                   ///< Single Hit Mode   
};

inline int MCOTTimeCreator::calculateTDCTime(const LHCb::MCOTDeposit* firstDep) const
{
  /// center around zero
  unsigned stationNum = (firstDep->channel()).station();
  /// ns to tdc counts conversion
  double tdcConversion = double(m_countsPerBX)/m_timePerBX;
  /// Get deposit time apply sart of read out gate correction and convert
  /// to tdc counts
  double tdcTime = (firstDep->time()-m_startReadOutGate[stationNum-1])*tdcConversion ;
  
  return int(tdcTime);
}

inline bool MCOTTimeCreator::insideReadOutWindow(int tdcTime) const
{
  return (tdcTime < (m_countsPerBX * m_numberOfBX) && tdcTime >= 0 ) ;
}

inline bool MCOTTimeCreator::AnalogDeadTime(const LHCb::MCOTDeposit* firstDep,
                                            const LHCb::MCOTDeposit* secondDep) const 
{ 
  // check whether to continue adding deposits
  return (firstDep->channel() == secondDep->channel() && 
          calculateTDCTime(secondDep) - calculateTDCTime(firstDep) < m_deadTime);
}

inline bool MCOTTimeCreator::DigitalDeadTime(const LHCb::MCOTDeposit* firstDep,
                                             const LHCb::MCOTDeposit* secondDep) const 
{ 
  // check whether to continue killing deposits
  return (firstDep->channel() == secondDep->channel()  && 
          calculateTDCTime(secondDep) < (m_countsPerBX * m_numberOfBX));
}

#endif // OTSIMULATION_MCOTTIMECREATOR_H
