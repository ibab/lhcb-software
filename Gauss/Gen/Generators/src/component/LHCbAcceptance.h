// $Id: LHCbAcceptance.h,v 1.5 2006-10-01 22:43:38 robbep Exp $
#ifndef GENERATORS_LHCBACCEPTANCE_H 
#define GENERATORS_LHCBACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IGenCutTool.h"

/** @class LHCbAcceptance LHCbAcceptance.h 
 *  
 *  Tool to keep events with particles only in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class LHCbAcceptance : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  LHCbAcceptance( const std::string& type, const std::string& name,
                  const IInterface* parent);
  
  virtual ~LHCbAcceptance( ); ///< Destructor

  /** Accept events in LHCb acceptance (defined by angle)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * thGeneEvent , 
                         const LHCb::GenCollision * theCollision , 
                         IDecayTool * theDecayTool , bool cpMixture ,
                         const HepMC::GenParticle * theSignalAtRest ) const ;
  
 private:
  /// Maximum value of angle around z-axis (set by options) 
  double m_thetaMax ;
};
#endif // GENERATORS_LHCBACCEPTANCE_H
