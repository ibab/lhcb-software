// $Id: MCOTTimeCreator.h,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $

#ifndef OTSIMULATION_MCOTTIMECREATOR_H
#define OTSIMULATION_MCOTTIMECREATOR_H 1

//Event
#include "Event/MCOTDeposit.h"
#include "Event/MCOTTime.h"

// Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// OTSimulation
#include "OTSimulation/OTReadOutWindow.h"


/** @class MCOTTimeCreator MCOTTimeCreator.h "OTSimulation/MCOTTimeCreator.h"
 *
 *  Implementation of MCOTimeCreator
 *  
 *  @author J. Nardulli and J. van Tilburg 
 *  @date   10/6/2004
 */
class MCOTTimeCreator : public Algorithm

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

  /// finalize
  StatusCode finalize();

private:

  typedef std::vector<MCOTTime*> MCOTTimeVector;

  /// function to check if two deposits have the same channel ID
  bool keepAdding(const MCOTDeposit* firstDep,
                  const MCOTDeposit* secondDep) const;

  /// apply dead time
  StatusCode createTimes(MCOTTimes* times);

  /// Apply read out window and store the result in final Time container
  int calculateTDCTime( MCOTDeposit* firstDeposit);
  bool insideReadOutWindow( int tdcTime );

  std::vector<double> m_startReadOutGate; ///< start of readout gate
  double m_sizeOfReadOutGate;             ///< end of readout gate   
  double m_deadTime;                      ///< deadtime
  int m_countsPerBX;                      ///< counts per BX
  int  m_numberOfBX;                      ///< Number of BX
  double m_timePerBX;                     ///< time Per BX
  MCOTTimeVector*  m_tempTimeCont;        ///< temporary vector of digits
  
};

#endif // OTSIMULATION_MCOTTIMECREATOR_H
