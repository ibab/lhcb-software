// $Id: BasePlotTool.h,v 1.2 2008-12-10 16:42:17 pkoppenb Exp $
#ifndef BASEPLOTTOOL_H 
#define BASEPLOTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "Kernel/IPlotTool.h"            // Interface
#include "Kernel/ParticleProperty.h" 
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/Escape.h" 

/** @class BasePlotTool BasePlotTool.h
 *  
 *  Base class for all plot tools
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class BasePlotTool : public GaudiHistoTool, 
                     virtual public IPlotTool {
public: 
  /// Standard constructor
  BasePlotTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~BasePlotTool( ); ///< Destructor

//=============================================================================
/// Fill plots using a single Particle
StatusCode fillPlots(const LHCb::Particle*,const std::string trailer = ""){
    err() << "BasePlotTool::fillPlots should never be called " << trailer << endmsg ;
    return StatusCode::FAILURE;
};

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

  /// Fill mother mass plot
  StatusCode fillMother(const LHCb::Particle*,const std::string trailer = "");
  /// Fill final state PID plots
  StatusCode fillFinal(const LHCb::Particle*,const std::string trailer = ""){
    err() << "BasePlotTool::fillFinal  should never be called " << trailer << endmsg ;
    return StatusCode::FAILURE;
  }
  
  /// Fill final state PID plots
  std::string histoName(std::string var, const LHCb::ParticleProperty* pp, 
                        std::string trailer){
    if (0==pp) Exception("Null pp");
    return std::string(var+"_"+Decays::escape(pp->name())+"_"+trailer);
  }
  
  LHCb::IParticlePropertySvc* ppSvc()const{return m_ppSvc;}

private:
  LHCb::IParticlePropertySvc* m_ppSvc; ///< PPSvc.

  double m_massWin ; ///< Mass window 

};
#endif // BASEPLOTTOOL_H
