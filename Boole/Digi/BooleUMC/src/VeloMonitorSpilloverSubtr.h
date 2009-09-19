// $Id: VeloMonitorSpilloverSubtr.h,v 1.1.1.1 2009-09-19 23:12:04 tskwarni Exp $
#ifndef VELOMONITORSPILLOVERSUBTR_H 
#define VELOMONITORSPILLOVERSUBTR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include <string>

/** @class VeloMonitorSpilloverSubtr VeloMonitorSpilloverSubtr.h
 *  
 *
 *  @author 
 *  @date   2009-09-11
 */
class VeloMonitorSpilloverSubtr : public GaudiHistoAlg {
public: 
  /// Standard constructor
  VeloMonitorSpilloverSubtr( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloMonitorSpilloverSubtr( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::string m_inputLocation;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;

};
#endif // VELOMONITORSPILLOVERSUBTR_H
