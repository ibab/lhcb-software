// $Id: LumiReadBackFSR.h,v 1.1 2009-06-12 08:17:50 panmanj Exp $
#ifndef LUMIREADBACKFSR_H 
#define LUMIREADBACKFSR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// event model
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"
#include "Event/LumiIntegral.h"

// local
#include "LumiIntegrator.h"

/** @class LumiReadBackFSR LumiReadBackFSR.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2009-02-27
 */
class LumiReadBackFSR : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LumiReadBackFSR( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiReadBackFSR( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  std::string m_ToolName;                       // name of tool for normalization

private:
  ILumiIntegrator *m_integratorTool;            // tool to integrate luminosity

};
#endif // LUMIREADBACKFSR_H
