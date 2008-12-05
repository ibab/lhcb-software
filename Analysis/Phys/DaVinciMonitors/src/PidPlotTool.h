// $Id: PidPlotTool.h,v 1.1.1.1 2008-12-05 16:41:05 pkoppenb Exp $
#ifndef PIDPLOTTOOL_H 
#define PIDPLOTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "Kernel/IPlotTool.h"            // Interface

class IParticlePopertySvc;
/** @class PidPlotTool PidPlotTool.h
 *  
 *  Fills PID plots
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class PidPlotTool : public GaudiHistoTool, virtual public IPlotTool {
public: 
  /// Standard constructor
  PidPlotTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~PidPlotTool( ); ///< Destructor

//=============================================================================
/// Fill plots using a single Particle
StatusCode fillPlots(const LHCb::Particle*,const std::string trailer = "");

/// Fill plots using a LHCb::Particle::ConstVector
StatusCode fillPlots(const LHCb::Particle::ConstVector& pv,
                     const std::string trailer = ""){
  for ( LHCb::Particle::ConstVector::const_iterator p = pv.begin();
        p != pv.end() ; ++p ){
    if (!fillPlots( *p, trailer )) return StatusCode::FAILURE ;
  }
  return StatusCode::SUCCESS ;
}

virtual StatusCode setPath ( const std::string& path ){
  setHistoDir ( path ) ;
  return StatusCode::SUCCESS;  
}
  StatusCode initialize();

protected:

private:
  StatusCode fillMother(const LHCb::Particle*,const std::string trailer = "");
  StatusCode fillFinal(const LHCb::Particle*,const std::string trailer = "");

private:
  LHCb::IParticlePropertySvc* m_ppSvc; ///< PPSvc.

private : 
  double m_massWin ; ///< Mass window per daughter

};
#endif // PIDPLOTTOOL_H
