// $Id: SimplePlotTool.h,v 1.4 2005-06-08 16:15:32 pkoppenb Exp $
#ifndef SIMPLEPLOTTOOL_H 
#define SIMPLEPLOTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "Kernel/IPlotTool.h"            // Interface

#include "Event/Particle.h"

/** @class SimplePlotTool SimplePlotTool.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-01-05
 */
class IParticlePropertySvc;
class IOnOffline;
class IGeomDispCalculator;
class SimplePlotTool : public GaudiHistoTool, virtual public IPlotTool {
public: 
  /// Standard constructor
  SimplePlotTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~SimplePlotTool( ); ///< Destructor

  /// initialize
  StatusCode initialize ();

  /// Define plot directory
  StatusCode setPath(const std::string&);

  /// Fill plots using a ParticleVector
  StatusCode fillPlots(const ParticleVector&,
                       const std::string trailer = "");

  /// Fill plots using a single Particle
  StatusCode fillPlots(const Particle*,
                       const std::string trailer = "");  

protected:

private:

  StatusCode firstInitialize(); ///< Initialize everything
  StatusCode reInitialize(); ///< Re-Initialize whatever can be

  // histo definition container
  class MyHisto{
  public:
    MyHisto(void){ m_var = "" ; m_min = 0 ;  m_max = 0 ; return ;};
    ~MyHisto(){};
    std::string getVar(void){return m_var ;}
    double getMin(void){return m_min ;}
    double getMax(void){return m_max ;}
    bool setHisto(const std::string& var, const double& mn, const double& mx ){
      if (!setHisto(var)) return false ;
      m_min = mn ;  m_max = mx ;
      return true ;};
    bool setHisto(const std::string& var );

  private:
    std::string m_var ;
    double m_min ;
    double m_max ;
  };
  StatusCode doPlot(const Particle*,MyHisto&,std::string) ;

  std::vector<MyHisto> m_histos ; ///< All histogram definitions
  bool m_allDefault ; ///< All histograms are default

  std::vector<std::string> m_variables ; ///< Variables to plot
  std::vector<double> m_minima ;    ///< Lower bounds for this variable
  std::vector<double> m_maxima ;    ///< Upper bounds for this variable

  /// Accessor for ParticlePropertySvc
  IParticlePropertySvc* m_ppSvc ;
  /// IP tool
  IGeomDispCalculator* m_geomTool;
  /// PV tool
  IOnOffline* m_onOfflineTool;

  /// Initialisation flag
  bool m_isInitialised;
  
  
};
#endif // SIMPLEPLOTTOOL_H
