#ifndef GENCUTS_DAUGHTERSINLHCBKEEPONLYSIGNAL_H 
#define GENCUTS_DAUGHTERSINLHCBKEEPONLYSIGNAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IGenCutTool.h"

// from LHCb
#include "Kernel/ParticleID.h"

namespace LHCb { class IParticlePropertySvc ; }

/** @class DaughtersInLHCbKeepOnlySignal DaughtersInLHCbKeepOnlySignal.h 
 *  
 *  Tool to keep events with daughters from signal particles 
 *  in LHCb acceptance.
 *  Remove also all particles form the event except the signal particle
 *  This allows to embed J/psi, D, etc... in minimum bias events
 * 
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2015-08-01
 */
class DaughtersInLHCbKeepOnlySignal : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  DaughtersInLHCbKeepOnlySignal( const std::string& type, 
                                 const std::string& name,
                                 const IInterface* parent);
  
  virtual ~DaughtersInLHCbKeepOnlySignal( ); ///< Destructor

  virtual StatusCode initialize( );   ///< Initialize method

  virtual StatusCode finalize( );   ///< Finalize method

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Remove all particles except the signal
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

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

  int     m_signalPID        ;  ///< PDG Id of the signal to consider (set by options)
  mutable int  m_nSignalBeforeCut    ;  ///< Counter of generated signal

  LHCb::ParticleID::Quark m_signalQuark ; 

  LHCb::IParticlePropertySvc * m_ppSvc ;

  /// Create a new event with a given particle
  HepMC::GenVertex * copyHepMCParticle( const HepMC::GenParticle * theSignal ,
                                        std::vector< HepMC::GenVertex * > &vV ) const ;
  
  /// Make an HepMC tree from a particle
  void fillTree(  HepMC::GenParticle * theNewParticle ,
                  const HepMC::GenParticle * theOldParticle ,
                  std::vector< HepMC::GenVertex * > &vV ) const ;
};
#endif // GENCUTS_DAUGHTERSINLHCBKEEPONLYSIGNAL_H
