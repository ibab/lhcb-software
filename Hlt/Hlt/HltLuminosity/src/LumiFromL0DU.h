// $Id: LumiFromL0DU.h,v 1.2 2008-08-25 10:59:04 panmanj Exp $
#ifndef LUMIFROML0DU_H 
#define LUMIFROML0DU_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from LHCB
#include "Event/L0DUReport.h"

/** @class LumiFromL0DU LumiFromL0DU.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2008-07-21
 */
class LumiFromL0DU : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LumiFromL0DU( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiFromL0DU( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  std::string m_InputSelectionName;
  const LHCb::L0DUReport* m_L0DUReport;
  std::string m_ValueName;
  std::string m_CounterName;
  int m_Counter;

  std::string m_OutputContainerName;

private:

};
#endif // LUMIFROML0DU_H
