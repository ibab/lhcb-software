// $Id: DaughtersInLHCb.h,v 1.3 2007-05-08 13:42:00 robbep Exp $
#ifndef GENERATORS_DAUGHTERSINLHCB_H 
#define GENERATORS_DAUGHTERSINLHCB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IGenCutTool.h"
#include "Kernel/Transform4DTypes.h"

/** @class DaughtersInLHCb DaughtersInLHCb.h 
 *  
 *  Tool to keep events with daughters from signal particles 
 *  in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class DaughtersInLHCb : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  DaughtersInLHCb( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  virtual ~DaughtersInLHCb( ); ///< Destructor

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

 private:
  /** Study a particle a returns true when all stable daughters
   *  are in LHCb acceptance
   */
  bool passCuts( const HepMC::GenParticle * theSignal ) const ;  

  /** Minimum value of angle around z-axis for charged daughters
   * (set by options) 
   */
  double m_chargedThetaMin ;

  /** Maximum value of angle around z-axis for charged daughters
   * (set by options) 
   */
  double m_chargedThetaMax ;

  /** Minimum value of angle around z-axis for neutral daughters
   * (set by options) 
   */
  double m_neutralThetaMin ;

  /** Maximum value of angle around z-axis for neutral daughters
   * (set by options) 
   */
  double m_neutralThetaMax ;
};
#endif // GENERATORS_DAUGHTERSINLHCB_H
