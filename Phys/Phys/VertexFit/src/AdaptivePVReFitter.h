
#ifndef ADAPTIVEPVREFITTER_H
#define ADAPTIVEPVREFITTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IPVReFitter.h"            // Interface

#include "PVFitTrack.h"

class ITrackExtrapolator;

using namespace LHCb;

/** @class AdaptivePVReFitter AdaptivePVReFitter.h
 *
 *  A tool for PV refitting using adaptive method
 *
 *  @author Yuehong Xie
 *  @date   2007-11-05
 */

class AdaptivePVReFitter : public GaudiTool, virtual public IPVReFitter
{

private:

  typedef ROOT::Math::SMatrix<double, 10, 10,
                              ROOT::Math::MatRepSym<double,10> > SymMatrix10x10;

  typedef ROOT::Math::SMatrix<double, 8, 8,
                              ROOT::Math::MatRepSym<double,8> > SymMatrix8x8;

public:

  /// Standard constructor
  AdaptivePVReFitter( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  StatusCode initialize();

  /// refit PV
  virtual StatusCode reFit(LHCb::VertexBase* PV) const;

  /// remove track used for a (B) LHCb::Particle and refit PV
  virtual StatusCode remove(const LHCb::Particle* part,
                            LHCb::VertexBase* PV) const;

  virtual ~AdaptivePVReFitter( ); ///< Destructor

private:

  StatusCode fitPV(LHCb::RecVertex* PV,
                   std::vector<PVFitTrack>& fittracks) const;
  void getFinalTracks(const LHCb::Particle*,
                      LHCb::Track::ConstVector& tracks) const;
  double getNewWeight(const double x, const int i) const;

private:

  int m_maxIter;
  bool m_reweightTracks;
  std::vector<double> m_temperatures;
  double m_criticalPoint;
  double m_maxDeltaChi2;
  unsigned m_minTrNumber;

  ITrackExtrapolator* m_fullExtrapolator;
  std::string m_fullExtrapolatorName;
  ITrackExtrapolator* m_veloExtrapolator;
  std::string m_veloExtrapolatorName;

};
#endif // ADAPTIVEPVREFITTER_H
