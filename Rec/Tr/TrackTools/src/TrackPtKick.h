#ifndef TRACKTOOLS_TRACKPTKICK_H 
#define TRACKTOOLS_TRACKPTKICK_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCbKernel
#include "Kernel/IBIntegrator.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackMomentumEstimate.h"

/** @class TrackPtKick TrackPtKick.h TrackTools/TrackPtKick.h
 *  
 *  @author M. Needham
 *  @date   2000-08-16
 */
class TrackPtKick : public GaudiTool, virtual public ITrackMomentumEstimate {
public: 
  /// Standard constructor
  TrackPtKick( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~TrackPtKick( ); ///< Destructor

  StatusCode initialize();

  // Estimate the momentum P of a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* tState, double& qOverP, double& sigmaQOverP,
				bool tCubicFit=0 ) const; 

  // Estimate the momentum P of a velo State and a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* veloState, const LHCb::State* tState,
				double& qOverP, double& sigmaQOverP,
				bool tCubicFit=0 ) const;// Estimate the momentum P of a State

protected:

private:
  StatusCode determineFieldPolarity();

  IBIntegrator* m_bIntegrator;

  int m_FieldPolarity;
  
  /// Define the parameters of the Z dependance
  std::vector<double> m_ParabolicCorrection;
  std::vector<double> m_resParams;
  double m_Constant;

};
#endif // TRACKTOOLS_TRACKPTKICK_H
