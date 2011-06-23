// $Id: DaughtersInLHCb.h,v 1.4 2008-05-29 14:21:59 gcorti Exp $
#ifndef GENCUTS_DAUGHTERSINLHCB_H 
#define GENCUTS_DAUGHTERSINLHCB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IGenCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

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
#endif // GENCUTS_DAUGHTERSINLHCB_H
