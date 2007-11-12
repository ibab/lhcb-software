
#ifndef ADAPTIVEPVREFITTER_H 
#define ADAPTIVEPVREFITTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IPVReFitter.h"            // Interface

#include "PVFitTrack.h"

class ITrackExtrapolator;

using namespace LHCb;

typedef ROOT::Math::SMatrix<double, 10, 10,
                            ROOT::Math::MatRepSym<double,10> > SymMatrix10x10;

typedef ROOT::Math::SMatrix<double, 8, 8,
                            ROOT::Math::MatRepSym<double,8> > SymMatrix8x8;

/** @class AdaptivePVReFitter AdaptivePVReFitter.h
 *  
 *  A tool for PV refitting using adaptive method
 *
 *  @author Yuehong Xie
 *  @date   2007-11-05
 */

class AdaptivePVReFitter : public GaudiTool, virtual public IPVReFitter {
public: 
  /// Standard constructor
  AdaptivePVReFitter( const std::string& type, 
              const std::string& name,
              const IInterface* parent);
  
  StatusCode initialize();

  /// refit PV
  virtual StatusCode reFit(LHCb::VertexBase* );

  /// remove track used for a (B) LHCb::Particle and refit PV
  virtual StatusCode remove(LHCb::Particle*,  LHCb::VertexBase*);

  virtual ~AdaptivePVReFitter( ); ///< Destructor

private:

  StatusCode fitPV(LHCb::RecVertex*, std::vector<PVFitTrack> &);
  void getFinalTracks(LHCb::Particle*, LHCb::Track::ConstVector &);
  double getNewWeight(double&, int&);

private:
      
  int m_maxIter;
  bool m_reweightTracks;
  std::vector<double> m_temperatures;
  double m_criticalPoint;
  double m_maxDeltaChi2;
  int m_minTrNumber;
  
  ITrackExtrapolator* m_fullExtrapolator;
  std::string m_fullExtrapolatorName;
  ITrackExtrapolator* m_veloExtrapolator;
  std::string m_veloExtrapolatorName;

};
#endif // ADAPTIVEPVREFITTER_H
