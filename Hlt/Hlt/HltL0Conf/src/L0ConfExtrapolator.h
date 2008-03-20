// $Id: L0ConfExtrapolator.h,v 1.1 2008-03-20 14:15:07 albrecht Exp $
#ifndef L0CONFEXTRAPOLATOR_H 
#define L0CONFEXTRAPOLATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IL0ConfExtrapolator.h"            // Interface

#include "Event/Track.h"
#include "GaudiKernel/Point3DTypes.h"

/** @class L0ConfExtrapolator L0ConfExtrapolator.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2008-01-18
 */
class L0ConfExtrapolator : public GaudiTool, virtual public IL0ConfExtrapolator {
public: 
  /// Standard constructor
  L0ConfExtrapolator( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~L0ConfExtrapolator( ); ///< Destructor

  FwdHypothesis getFwdHypothesis( const LHCb::Track& veloTrack, 
                                  const LHCb::Track& caloTrack,
                                  double nSigma ) const;
  
  ParabolaHypothesis getParabolaHypothesis( const LHCb::Track& seedTrack, 
                                            double nSigma ) const;

  ParabolaHypothesis getParabolaHypothesis( const LHCb::State& aState, 
                                            double nSigma ) const;

  void muon2T( const LHCb::Track& muonTrack, LHCb::State& stateAtT ) const;
  
  void ecal2T( const LHCb::Track& ecalTrack, 
               LHCb::State& statePosAtT, LHCb::State& stateNegAtT ) const;
  
  void hcal2T( const LHCb::Track& ecalTrack, 
               LHCb::State& statePosAtT, LHCb::State& stateNegAtT ) const;
  
  int getCaloRegion( float stateX, float stateY, float stateZ ) const;
protected:

private:
  //for getParabolaHypothesis
  double m_curvFactor;
  
  //for getFwdHypothesis
  double m_zMagnet;
  double m_zRef;
  double m_axParam;
  double m_cxParam;
  double m_dxParam;
  double m_byParam;

  std::vector<double> m_fwdSigmaX2;
  std::vector<double> m_fwdSigmaY2;

  double m_ecalKick;

  double m_HECalKick[2], m_HCalKick[2];
  
  // FIXME: until this gets into StateParameters.h, we define a constant
  static const double zEndT3 = 9315.0;
  
  // this routine does the real work once the kick in x direction has
  // been figured out by [eh]cal2T rountines above
  void calo2T( const LHCb::State& aState, double xkick,
               LHCb::State& statePosAtT, LHCb::State& stateNegAtT ) const;
};
#endif // L0CONFEXTRAPOLATOR_H
