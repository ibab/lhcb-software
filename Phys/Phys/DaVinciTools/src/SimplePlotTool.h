// $Id: SimplePlotTool.h,v 1.1 2005-01-06 10:37:47 pkoppenb Exp $
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
class IPVLocator;
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
  StatusCode fillPlots(const ParticleVector&);

  /// Fill plots using a single Particle
  StatusCode fillPlots(const Particle*);  

protected:

private:
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
  StatusCode doPlot(const Particle*,MyHisto&) ;

  std::vector<MyHisto> m_histos ; ///< All histogram definitions
  bool m_allDefault ; ///< All histograms are default

  std::vector<std::string> m_variables ; ///< Variables to plot
  std::vector<double> m_minima ;    ///< Lower bounds for this variable
  std::vector<double> m_maxima ;    ///< Upper bounds for this variable

  /// Accessor for ParticlePropertySvc
  IParticlePropertySvc* m_ppSvc ;
  /// IP tool
  IGeomDispCalculator* m_ipTool;
  /// PV tool
  IPVLocator* m_pv;
};
#endif // SIMPLEPLOTTOOL_H
