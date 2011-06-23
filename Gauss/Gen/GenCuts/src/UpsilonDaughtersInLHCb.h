// $Id: UpsilonDaughtersInLHCb.h,v 1.2 2010-09-19 09:03:47 robbep Exp $
#ifndef GENCUTS_UPSILONDAUGHTERSINLHCB_H 
#define GENCUTS_UPSILONDAUGHTERSINLHCB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Transform4DTypes.h"

#include "MCInterfaces/IGenCutTool.h"

// Forward declaration
class IDecayTool ;

/** @class UpsilonDaughtersInLHCb UpsilonDaughtersInLHCb.h 
 *  
 *  Tool to keep events with daughters from Upsilon 
 *  in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2009-02-26
 */

class UpsilonDaughtersInLHCb : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  UpsilonDaughtersInLHCb( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);
  
  virtual ~UpsilonDaughtersInLHCb( ); ///< Destructor

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

  int     m_sigUpsilonPID        ;  ///< PDG Id of the Upsilon to consider (set by options)
  mutable int  m_nUpsilonBeforeCut    ;  ///< Counter of generated upsilon
};
#endif // GENERATORS_BCDAUGHTERSINLHCB_H
