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
 *  @author Stephanie Hansmann-Menzemer
 *  @date   2007-10-30
 */
class FastMomentumEstimate : public GaudiTool, virtual public ITrackMomentumEstimate{
public: 
  /// Standard construct
  FastMomentumEstimate( const std::string& type, 
			const std::string& name,
			const IInterface* parent);

  virtual ~FastMomentumEstimate( ); ///< Destructor

  StatusCode initialize();

  
    // Estimate the momentum P of a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* tState, double& qOverP, double& sigmaQOverP,
				bool tCubicFit ) const; 

  // Estimate the momentum P of a velo State and a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* veloState, const LHCb::State* tState,
				double& qOverP, double& sigmaQOverP,
				bool tCubicFit ) const;

protected:

private:
  ILHCbMagnetSvc*     m_magFieldSvc;
  /// Define the parameters of the Z dependance
  std::vector<double> m_paramsTCubic;
  std::vector<double> m_paramsTParab;
  std::vector<double> m_paramsVeloTCubic;
  std::vector<double> m_paramsVeloTParab;
  double m_tResolution;
  double m_veloPlusTResolution;

};
#endif // TRACKTOOLS_FASTMOMENTUMESTIMATE_H
