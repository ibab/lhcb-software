// $Id: SimplePlots.h,v 1.1.1.1 2004-11-18 12:38:36 pkoppenb Exp $
#ifndef SIMPLEPLOTS_H 
#define SIMPLEPLOTS_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DaVinciTools/DVAlgorithm.h"


/** @class SimplePlots SimplePlots.h
 *  
 *  Simple algorithm that plots most common variables
 *  for all particles in the inputlocation depending
 *  on it's PID. One can define the histogram boundaries
 *  for all these variables or take the default ones.
 *  For the mass a value of -1 for both min and max
 *  sets the boundaries to a PID-dependent default.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-10-22
 */
class SimplePlots : public DVAlgorithm {
public: 
  /// Standard constructor
  SimplePlots( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SimplePlots( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

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
};
#endif // SIMPLEPLOTS_H
