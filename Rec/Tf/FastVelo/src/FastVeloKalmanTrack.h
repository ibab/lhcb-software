#ifndef FastVeloKalmanTrack_H
#define FastVeloKalmanTrack_H

namespace LHCb
{
  class Track ;
  class State ;
}

class FastVeloTrack ;
class FastVeloHit ;

class FastVeloKalmanTrack
{
 private:
  const FastVeloTrack* m_track ;
  bool m_hitsIncreasingZ ;
  std::vector<const FastVeloHit*> m_hits ;
  std::vector<double>  kalmanScatteringNoiseParameters;
private:
  enum Direction { Forward = +1, Backward = -1 } ;
  void fit(LHCb::State& state,
	   double& chi2,
	   int& ndof,
	   const int direction,
	   const float noise2PerLayer) const ;
 public:
  // constructor
  FastVeloKalmanTrack( const FastVeloTrack& track ) ;
  // single member function
  void addStates(LHCb::Track& lhcbtrack,
		 const LHCb::State& basestate,
		 double transverseMomentumForScattering,
     std::vector<double>  kalmanScatteringNoiseParameters,
		 bool addStateBeamLine,
		 bool addStateLastMeasurement,
		 bool addStateEndVelo) const ;
} ;

#endif
