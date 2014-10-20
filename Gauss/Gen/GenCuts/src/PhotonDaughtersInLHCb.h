#ifndef GENERATORS_PHOTONDAUGHTERSINLHCB_H 
#define GENERATORS_PHOTONDAUGHTERSINLHCB_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Transform4DTypes.h"

#include "MCInterfaces/IGenCutTool.h"


// Forward declaration
class IDecayTool ;

/** @class PhotonDaughtersInLHCb PhotonDaughtersInLHCb.h 
 *  
 *  Tool to keep events with photon daughters
 *  in the LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Sean Benson     
 *  @date   2014-10-16
 */

class PhotonDaughtersInLHCb : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  PhotonDaughtersInLHCb( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~PhotonDaughtersInLHCb( ); ///< Destructor

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

  int     m_BsPID ;  ///< PDG Id of the Bs to consider (set by options)
};
#endif // GENERATORS_PHOTONDAUGHTERSINLHCB_H
