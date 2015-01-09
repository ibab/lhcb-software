#ifndef TRACKTOOLS_FASTMOMENTUMESTIMATE_H 
#define TRACKTOOLS_FASTMOMENTUMESTIMATE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ILHCbMagnetSvc.h"


// from TrackInterfaces
#include "TrackInterfaces/ITrackMomentumEstimate.h"

/** @class FastMomentumEstimate FastMomentumEstimate.h
 *
 *  Calculate momentum of a seed track or a match track using the states and a parametrisation
 *  The parameters for VeloTCubic can be obtained using the "MatchFitParams" algorithm.
 *
 *  Parameters:
 *
 *  - ParamsTCubic: Coefficients for momentum calculation using a state in the T stations when using a cubic fit.
 *  - ParamsTParabola: Coefficients for momentum calculation using a state in the T stations when using a parbolic fit.
 *  - ParamsVeloTCubic: Coefficients for momentum calculation using a state in the Velo and the T stations when using a cubic fit.
 *  - ParamsVeloTParabola: Coefficients for momentum calculation using a state in the Velo and the T stations when using a parbolic fit.
 *  - TResolution: Resolution for q/p, given as: sigma(q/p) = TResolution * (q/p)
 *  - VeloPlusTResolution: Resolution for q/p, given as: sigma(q/p) = VeloPlusTResolution * (q/p)
 *
 *  Note: When giving the momentum parameters / coefficients in the options file, all 4 vectors have to be given.
 *
 *
 *  @author Stephanie Hansmann-Menzemer
 *  @date   2007-10-30
 */
class FastMomentumEstimate : public GaudiTool, virtual public ITrackMomentumEstimate{
public: 
  /// Standard constructor
  FastMomentumEstimate( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);
  
  virtual ~FastMomentumEstimate( ); ///< Destructor
  
  StatusCode initialize();
  
  
  /// Estimate the momentum P of a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* tState, double& qOverP, double& sigmaQOverP,
                                bool tCubicFit ) const; 
  
  /// Estimate the momentum P of a velo State and a State in T (for matching)
  virtual StatusCode calculate( const LHCb::State* veloState, const LHCb::State* tState,
                                double& qOverP, double& sigmaQOverP,
                                bool tCubicFit ) const;
  
protected:

private:

  ILHCbMagnetSvc*     m_magFieldSvc;
  /// Momentum parameters (coefficients) for T state when using cubic fit
  std::vector<double> m_paramsTCubic;
  /// Momentum parameters (coefficients) for T state when using parabolic fit
  std::vector<double> m_paramsTParab;
  /// Momentum parameters (coefficients) for Velo and T state when using cubic fit
  std::vector<double> m_paramsVeloTCubic;
  /// Momentum parameters (coefficients) for Velo and T state when using parabolic fit
  std::vector<double> m_paramsVeloTParab;
  double m_tResolution;
  double m_veloPlusTResolution;

};
#endif // TRACKTOOLS_FASTMOMENTUMESTIMATE_H
