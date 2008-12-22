// $Id: TutorialAlgorithm.h,v 1.4 2008-12-22 18:08:36 pkoppenb Exp $
#ifndef TUTORIALALGORITHM_H 
#define TUTORIALALGORITHM_H 1 

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class TutorialAlgorithm TutorialAlgorithm.h
 *  
 *  Example of how to loop over particles to make a composite particle.
 *  Practically no-one ever does that. Use CombineParticles instead.
 *
 *  https://twiki.cern.ch/twiki/bin/view/LHCb/DaVinciTutorial2
 *  
 *  \sa CombineParticles
 *
 *  @author Patrick Koppenburg
 *  @date   2006-05-24
 */
class TutorialAlgorithm : public DVAlgorithm {
public: 
  /// Standard constructor
  TutorialAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TutorialAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Make a loop over muons
  StatusCode loopOnMuons(const LHCb::Particle::ConstVector&)const ;
  /// Plot quantities for muon
  StatusCode plotMuon(const LHCb::Particle*, const std::string& head)const ;
  /// Build J/psi
  StatusCode makeJpsi(const LHCb::Particle::ConstVector&) ;

  double m_jPsiMassWin ; ///< Mass window
  double m_jPsiChi2 ;    ///< Max J/psi chi^2
  LHCb::ParticleID  m_jPsiID ;         ///< J/psi ID
  double m_jPsiMass ;    ///< J/psi mass
};
#endif // TUTORIALALGORITHM_H
