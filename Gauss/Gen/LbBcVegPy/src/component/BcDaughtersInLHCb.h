// $Id: BcDaughtersInLHCb.h,v 1.3 2008-09-03 09:04:49 gcorti Exp $
#ifndef GENERATORS_BCDAUGHTERSINLHCB_H 
#define GENERATORS_BCDAUGHTERSINLHCB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Transform4DTypes.h"

#include "Generators/ExternalGenerator.h"
#include "MCInterfaces/IGenCutTool.h"

// Forward declaration
class IDecayTool ;

/** @class BcDaughtersInLHCb BcDaughtersInLHCb.h 
 *  
 *  Tool to keep events with daughters from B_c 
 *  in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */

class BcDaughtersInLHCb : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  BcDaughtersInLHCb( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~BcDaughtersInLHCb( ); ///< Destructor

  virtual StatusCode initialize( );   ///< Initialize method

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

  virtual StatusCode finalize( );   ///< Finalize method
  
protected:
  
  
  
private:
  /// Decay tool
  IDecayTool*  m_decayTool;

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
  
  /// Name of the decay tool to use
  std::string m_decayToolName ;

  int  m_sigBcPID        ;  ///< PDG Id of the B_c 

};
#endif // GENERATORS_BCDAUGHTERSINLHCB_H
