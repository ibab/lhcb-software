#ifndef GENCUTS_DAUGHTERSINLHCBANDCUTSFORLC3PI_H
#define GENCUTS_DAUGHTERSINLHCBANDCUTSFORLC3PI_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IGenCutTool.h"

/** @class DaughtersInLHCbAndCutsForLc DaughtersInLHCbAndCutsForLc3pi.h
 *
 *  Tool to keep events with daughters from signal particles
 *  in LHCb and with p and pt cuts on Lc daughters.
 *  Concrete implementation of IGenCutTool.
 *
 *  @author Patrick Robbe
 *  @date   2012-02-07
 */
class DaughtersInLHCbAndCutsForLc3pi : public GaudiTool, virtual public IGenCutTool {
public:
  /// Standard constructor
  DaughtersInLHCbAndCutsForLc3pi( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent);

  virtual ~DaughtersInLHCbAndCutsForLc3pi( ); ///< Destructor

  /** Accept events with daughters in LHCb and p/pt cuts on Lc daughters
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

  /** Momentum Cut function
   *
   */
  bool momentumCut( const HepMC::GenParticle *, double ) const ;

  // Minimum value of angle around z-axis for charged daughters
  double m_chargedThetaMin ;

  // Maximum value of angle around z-axis for charged daughters
  double m_chargedThetaMax ;

  // Minimum value of angle around z-axis for neutral daughters
  double m_neutralThetaMin ;

  // Maximum value of angle around z-axis for neutral daughters
  double m_neutralThetaMax ;

  // cut value of Lc pt
  double m_lcptCut ;

  // cut value on daughters min pt
  double m_daughtersptminCut ;

  // cut value on daughters max pt
  double m_daughtersptmaxCut ;

  // cut value on daughters min p
  double m_daughterspminCut ;

 };
#endif // GENCUTS_DAUGHTERSINLHCDANDCUTSFORLC3PI_H
