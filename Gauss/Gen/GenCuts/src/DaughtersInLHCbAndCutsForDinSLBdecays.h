#ifndef GENCUTS_DAUGHTERSINLHCBANDCUTSFORDINSLBDECAYS_H 
#define GENCUTS_DAUGHTERSINLHCBANDCUTSFORDINSLBDECAYS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IGenCutTool.h"

/** @class DaughtersInLHCbAndCutsForDinSLBdecays DaughtersInLHCbAndCutsForDinSLBdecays.h 
 *  
 *  Tool to keep events with daughters from signal particles 
 *  in LHCb and with p and pt cuts on D0 daughters and muon.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Maurizio Martinelli
 *  @date   2013-08-19
 */
class DaughtersInLHCbAndCutsForDinSLBdecays : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  DaughtersInLHCbAndCutsForDinSLBdecays( const std::string& type, 
					 const std::string& name,
					 const IInterface* parent);
  
  virtual ~DaughtersInLHCbAndCutsForDinSLBdecays( ); ///< Destructor

  /** Accept events with daughters in LHCb and p/pt cuts on D0 daughters 
   *  (defined by min and max angles, different values for charged and neutrals)
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

  /** Check particle is in LHCb acceptance
   */
  bool InLHCbAcceptance( const HepMC::GenParticle * stable, const double firstpz ) const ;

  /** Momentum Cut function
   */
  bool momentumCut( const HepMC::GenParticle *, double ) const ;  

  /** Calculate proper time
   */
  double ProperTime( const HepMC::GenParticle * resonance ) const;

  /**  Minimum value of angle around z-axis for charged daughters
   */
  double m_chargedThetaMin ;

  /** Maximum value of angle around z-axis for charged daughters
   */
  double m_chargedThetaMax ;

  /** Minimum value of angle around z-axis for neutral daughters
   */
  double m_neutralThetaMin ;

  /** Maximum value of angle around z-axis for neutral daughters
   */
  double m_neutralThetaMax ;

  /** cut value of minimum (D0mu) invariant mass
   */
  double m_md0muMin;

  /** cut value of D0 pt
   */
  double m_d0PtMin ;

  /** cut on D0 ctau
   */
  double m_d0ctauMin ;

  /** cut value on daughters min pt
   */
  double m_daughtersPtMin ;

  /** cut value on daughters max pt
   */
  //  double m_daughtersPtMaxCut ;
  
  /** cut value on daughters min p
   */
  double m_daughtersPMin ;

  /** cut value on minimum muon p
   */
  double m_muonPMin ;

  /** cut value on minimum muon pt
   */
  double m_muonPtMin ;
};
#endif // GENCUTS_DAUGHTERSINLHCDANDCUTSFORDINSLBDECAYS_H
