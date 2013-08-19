#ifndef GENERATORS_XICCDAUGHTERSINLHCB_H 
#define GENERATORS_XICCDAUGHTERSINLHCB_H 1

// Include files
#include "LbGenXicc/QQqBaryons.h"
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Transform4DTypes.h"

#include "Generators/ExternalGenerator.h"
#include "MCInterfaces/IGenCutTool.h"

// Forward declaration
class IDecayTool ;

/** @class XiccDaughtersInLHCbAndWithMinPT XiccDaughtersInLHCbAndWithMinPT.h 
 *  
 *  Tool to keep events with daughters from Xi_cc+
 *  in LHCb acceptance and cut on Xi_cc+ PT
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author L.Zhong, based on XiccDaughtersInLHCbAndWithMinPT.h
 *  @date   2013-08-19
 */

class XiccDaughtersInLHCbAndWithMinPT : public GaudiTool, public QQqBaryons, virtual public IGenCutTool {
 public:
  /// Standard constructor
  XiccDaughtersInLHCbAndWithMinPT( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~XiccDaughtersInLHCbAndWithMinPT( ); ///< Destructor

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

  std::string m_BaryonState;  ///< double heavy baryon to be looked for
  int  m_sigXiccPID        ;  ///< PDG Id of the double heavy baryon

  // Minimum value of Xicc+ PT
  double m_minXiccPT;

};
#endif // GENERATORS_XICCDAUGHTERSINLHCB_H
