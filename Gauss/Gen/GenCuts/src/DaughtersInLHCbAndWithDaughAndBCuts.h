// $Id: DaughtersInLHCbAndWithKSCutsAndWithMinPAndPT.h,v 1.4 2008-05-29 14:21:59 gcorti Exp $
#ifndef GENCUTS_DAUGHTERSINLHCBAndWithDaughAndBCuts_H 
#define GENCUTS_DAUGHTERSINLHCBAndWithDaughAndBCuts_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IGenCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

/** @class DaughtersInLHCbAndWithDaughAndBCuts DaughtersInLHCbAndWithDaughAndBCuts.h 
 *  
 *  Tool to keep events with daughters from signal particles 
 *  in LHCb AndWithMinP.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Alex Shires
 *  @date   2011-03-02
 */
class DaughtersInLHCbAndWithDaughAndBCuts : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  DaughtersInLHCbAndWithDaughAndBCuts( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  virtual ~DaughtersInLHCbAndWithDaughAndBCuts( ); ///< Destructor

  /** Accept events with daughters in LHCb AndWithMinP (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

 private:
  /** Study a particle a returns true when all stable daughters
   *  are in LHCb AndWithMinP
   */
  bool passCuts( const HepMC::GenParticle * theSignal ) const ;  

  /** Flight Distance Cut function
   *
   */
  bool flightCut( const HepMC::GenParticle * , double ) const ;

  /** Momentum Cut function
   *
   */
  bool momentumCut( const HepMC::GenParticle *p, const double pMin, double sumP = 0) const ;  

  /** Transverse Momentum Cut function
   *
   */
  bool transverseMomentumCut( const HepMC::GenParticle *p, const double pTMin, double sumPt = 0) const ;

  // Minimum value of angle around z-axis for charged daughters
  double m_chargedThetaMin ;

  // Maximum value of angle around z-axis for charged daughters
  double m_chargedThetaMax ;

  // Minimum value of angle around z-axis for neutral daughters
  double m_neutralThetaMin ;

  // Maximum value of angle around z-axis for neutral daughters
  double m_neutralThetaMax ;

  // Minimum value of angle around z-axis for a KS/Lambda
  double m_llThetaMin ;

  // Maximum value of angle around z-axis for a KS/Lambda
  double m_llThetaMax ;

  //min value of momentum
  double m_minMuonP;
  double m_minTrackP;
  double m_minLongLivedP;
	double m_minLongLivedDaughP;
  double m_minBP;

  //min value of trnasverse momentum
  double m_minMuonPT;
  double m_minTrackPT;
  double m_minLongLivedPT;
double m_minLongLivedDaughPT;
  double m_minBPT;
  
  //flight distance
  double m_minBFD;

	// sum of Pt and P
	double m_minSumP;
  double m_minSumPT;
  

};
#endif // GENCUTS_DAUGHTERSINLHCBAndWithDaughAndBCuts_H
