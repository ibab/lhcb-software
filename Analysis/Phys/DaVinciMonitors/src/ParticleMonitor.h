// $Id: ParticleMonitor.h,v 1.1.1.1 2008-12-05 16:41:05 pkoppenb Exp $
#ifndef PARTICLEMONITOR_H 
#define PARTICLEMONITOR_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
/** @class ParticleMonitor ParticleMonitor.h
 *  
 *  Plot PID quantities for final state daughters
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-04
 */
class ParticleMonitor : public DVAlgorithm {
public: 
  /// Standard constructor
  ParticleMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  ///< Configure cut tools. That should go to DVAlgorithm
  StatusCode configure( LoKi::IHybridFactory* f, std::string & s, LoKi::Types::Cut& c)
  {
    StatusCode sc = f -> get ( s , c  ) ;
    if ( sc.isFailure () ){ return Error ( "Unable to  decode cut: " + s  , sc ) ; }  
    if ( msgLevel(MSG::DEBUG)) debug () << "The decoded cut is: " << s << endreq ;
    return sc;
  }
  ///< Fill plots
  StatusCode fillPlots(const LHCb::Particle* d, std::string where){
    for ( std::map<std::string,IPlotTool*>::iterator s = m_plotTools.begin() ;
          s != m_plotTools.end() ; ++s){
      if (msgLevel(MSG::VERBOSE)) verbose() << "Filling " << s->first << endmsg ;
      if (!s->second->fillPlots(d,where)) return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS ;
  }

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

};
#endif // PARTICLEMONITOR_H
