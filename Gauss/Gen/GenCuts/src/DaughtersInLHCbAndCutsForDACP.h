#ifndef GENCUTS_DAUGHTERSINLHCBANDCUTSFORDACP_H 
#define GENCUTS_DAUGHTERSINLHCBANDCUTSFORDACP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IGenCutTool.h"

/** @class DaughtersInLHCbAndCutsForDACP DaughtersInLHCbAndCutsForDACP.h 
 *  
 *  Tool to keep events with daughters from signal particles in LHCb
 *  and with cuts on:
 *     (Dst p, D0 pT) plane
 *     (Dst p - D0 p, Dst pT - D0 pT) plane
 *     D0 tau
 *     D0 flight distance from origin vertex
 *     D0 daughter p and pT
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Chris Thomas, based on DaughtersInLHCbAndCutsForDstar
 *  @date   2012-05-14
 */
class DaughtersInLHCbAndCutsForDACP : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  DaughtersInLHCbAndCutsForDACP( const std::string& type, 
                                 const std::string& name,
                                 const IInterface* parent);
  
  virtual ~DaughtersInLHCbAndCutsForDACP( ); ///< Destructor

  /** Accept events with daughters in LHCb and cuts outlined above
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

 private:
  /** Study a particle a returns true when all stable daughters
   *  are in LHCb AndCutsForDACP
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

  //
  //Cuts on (D* p, D* pT) plane
  //

  double m_scaleDst ;
  double m_uDst ;
  double m_vDst ;
  double m_sinthetaDst ;
  double m_costhetaDst ;
  double m_kDst ;  
  
  //
  //Cuts on (D* p - D0 p, D* pT - D0 pT) plane
  //

  double m_scaleDiff ;
  double m_uDiff ;
  double m_vDiff ;
  double m_sinthetaDiff ;
  double m_costhetaDiff ;
  double m_kDiff ;  
  
  //
  //Cut on D0 tau
  //

  double m_D0_TAU ;
  
  //
  //Cut on D0 flight distance from origin vertex
  //

  double m_D0_FD_ORIVX ;
  
  //
  //Cuts on D0 daughter minimum pT, maximum pT and minimum p
  //

  double m_D0_daugHiPT ;
  double m_D0_daugLoPT ;
  double m_D0_daugLoP ;
  
};
#endif // GENCUTS_DAUGHTERSINLHCDANDCUTSFORDACP_H
