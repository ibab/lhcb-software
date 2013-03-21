// $Id: BcDaughtersInLHCb.h,v 1.3 2008-09-03 09:04:49 gcorti Exp $
#ifndef GENERATORS_BCDAUGHTERSINLHCBANDMASSCUT_H 
#define GENERATORS_BCDAUGHTERSINLHCBANDMASSCUT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Transform4DTypes.h"

#include "MCInterfaces/IGenCutTool.h"

// Forward declaration
class IDecayTool ;

/** @class BcDaughtersInLHCbAndMassCut BcDaughtersInLHCbAndMassCut.h 
 *  
 *  Tool to keep events with daughters from B_c 
 *  in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */

class BcDaughtersInLHCbAndMassCut : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  BcDaughtersInLHCbAndMassCut( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~BcDaughtersInLHCbAndMassCut( ); ///< Destructor

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

  /** PDG ID of the two particle pair used to compute Mass
   * (set by options) 
   */
  int m_LeptonOneID ;
  int m_LeptonTwoID ;

  /** Switch for Minimal PT cut on the two daughters 
   * (defined by m_LeptonOneID, m_LeptonOTwoID) and cut value
   * (set by options) 
   */
  bool m_PreselDausPT ;
  double m_DausPTMin ;

  /** Switch for minimal mass cut on the two daughters pair 
   * (defined by m_LeptonOneID, m_LeptonOTwoID) and cut value
   * (set by options) 
   */
  bool m_PreselMinMass ;
  double m_mmMinMass ;

  /** Switch for maximal PT cut on the two daughters pair 
   * (defined by m_LeptonOneID, m_LeptonOTwoID) and cut value
   * (set by options) 
   */
  bool m_PreselMaxMass ;
  double m_mmMaxMass ;

  int  m_sigBcPID        ;  ///< PDG Id of the B_c 

};
#endif // GENERATORS_BCDAUGHTERSINLHCBANDMASSCUT_H
