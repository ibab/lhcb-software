// $Id: MCOTTimeCreator.h,v 1.4 2005-11-09 16:52:25 jnardull Exp $

#ifndef OTSIMULATION_MCOTTIMECREATOR_H
#define OTSIMULATION_MCOTTIMECREATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/MCOTTime.h"


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
  typedef std::vector<MCOTTime*> MCOTTimeVector;

  /// function to check if two deposits have the same channel ID
  bool AnalogDeadTime(const MCOTDeposit* firstDep,
                      const MCOTDeposit* secondDep) const;

  /// function to kill deposits due to the digital dead time
  bool DigitalDeadTime(const MCOTDeposit* firstDep,
                       const MCOTDeposit* secondDep) const;

  /// apply dead time
  StatusCode createTimes(MCOTTimes* times);

  /// Apply read out window and store the result in final Time container
  int calculateTDCTime( const MCOTDeposit* firstDeposit) const;
  bool insideReadOutWindow( int tdcTime ) const;

  std::vector<double> m_startReadOutGate; ///< start of readout gate
  double m_sizeOfReadOutGate;             ///< end of readout gate   
  double m_deadTime;                ///< Analogdeadtime
  int m_countsPerBX;                      ///< counts per BX
  int  m_numberOfBX;                      ///< Number of BX
  double m_timePerBX;                     ///< time Per BX
  bool m_singleHitMode;                   ///< Single Hit Mode   
  MCOTTimeVector*  m_tempTimeCont;        ///< temporary vector of digits
  
};

#endif // OTSIMULATION_MCOTTIMECREATOR_H
