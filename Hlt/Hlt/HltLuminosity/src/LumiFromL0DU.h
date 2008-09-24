// $Id: LumiFromL0DU.h,v 1.3 2008-09-24 14:15:35 panmanj Exp $
#ifndef LUMIFROML0DU_H 
#define LUMIFROML0DU_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from LHCB
#include "Event/L0DUReport.h"
// Interfaces
#include "L0Interfaces/IL0DUFromRawTool.h"

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
  std::string m_OutputContainerName;

  const LHCb::L0DUReport* m_L0DUReport;
  std::string m_fromRawTool;
  IL0DUFromRawTool* m_fromRaw;

  std::string m_ValueName;
  std::string m_CounterName;
  int m_Counter;


private:

};
#endif // LUMIFROML0DU_H
