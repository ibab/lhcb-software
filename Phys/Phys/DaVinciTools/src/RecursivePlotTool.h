// $Id: RecursivePlotTool.h,v 1.1 2005-01-10 09:57:17 pkoppenb Exp $
#ifndef RECURSIVEPLOTTOOL_H 
#define RECURSIVEPLOTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IPlotTool.h"            // Interface

/** @class RecursivePlotTool RecursivePlotTool.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-01-06
 */
class RecursivePlotTool : public GaudiTool, virtual public IPlotTool {
public: 
  /// Standard constructor
  RecursivePlotTool( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~RecursivePlotTool( ); ///< Destructor
  /// initialize
  StatusCode initialize ();
  /// finalize
  StatusCode finalize ();

  /// Define plot directory
  StatusCode setPath(const std::string&);

   /// Fill plots using a ParticleVector
  StatusCode fillPlots(const ParticleVector&);

  /// Fill plots using a single Particle
  StatusCode fillPlots(const Particle*);  
 

protected:

private:

  std::vector<std::string> m_variables ; ///< Variables to plot
  std::vector<double> m_minima ;    ///< Lower bounds for this variable
  std::vector<double> m_maxima ;    ///< Upper bounds for this variable

  IPlotTool* m_simplePlotTool ;
  

};
#endif // RECURSIVEPLOTTOOL_H
