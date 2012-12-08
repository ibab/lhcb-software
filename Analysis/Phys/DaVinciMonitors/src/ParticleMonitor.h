// $Id: ParticleMonitor.h,v 1.3 2009-04-07 16:25:12 pkoppenb Exp $
#ifndef PARTICLEMONITOR_H 
#define PARTICLEMONITOR_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"

/** @class ParticleMonitor ParticleMonitor.h
 *  
 *  Plot PID quantities for final state daughters
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-04
 */
class ParticleMonitor : public DaVinciAlgorithm
{

public: 
  /// Standard constructor
  ParticleMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// Configure cut tools.
  StatusCode configureCut( LoKi::IHybridFactory* f, std::string & s, LoKi::Types::Cut& c);

  /// Fill plots
  StatusCode fillPlots(const LHCb::Particle* d, const std::string & where);

private:

  LoKi::Types::Cut  m_mother       ; ///< Selection cut applied to all mothers
  //  LoKi::Types::Cut  m_final        ; ///< Selection cut applied when plotting
  LoKi::Types::Cut  m_peak         ; ///< Selection cut applied to peak mothers
  LoKi::Types::Cut  m_sideband     ; ///< Selection cut applied to sideband mothers
  std::string  m_motherCut         ; ///< Selection cut applied to all mothers
  //  std::string  m_finalCut          ; ///< Selection cut applied when plotting
  std::string  m_peakCut           ; ///< Selection cut applied to peak mothers
  std::string  m_sidebandCut       ; ///< Selection cut applied to sideband mothers
  std::map<std::string,IPlotTool*> m_plotTools ; ///< Plot tools to be used
  std::vector<std::string>  m_plotToolNames; ///< names for plottols, only for initialize
  std::string m_massPlotToolName ; ///< Mass plot tool is special
};
#endif // PARTICLEMONITOR_H
