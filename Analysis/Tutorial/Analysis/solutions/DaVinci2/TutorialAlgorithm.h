// $Id: TutorialAlgorithm.h,v 1.3 2008-06-03 09:58:38 pkoppenb Exp $
#ifndef TUTORIALALGORITHM_H 
#define TUTORIALALGORITHM_H 1 

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class TutorialAlgorithm TutorialAlgorithm.h
 *  
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
  StatusCode loopOnMuons(const LHCb::Particle::ConstVector&)const ;
  StatusCode plotMuon(const LHCb::Particle*, const std::string& head)const ;
  StatusCode makeJpsi(const LHCb::Particle::ConstVector&) ;

  double m_jPsiMassWin ; ///< Mass window
  double m_jPsiChi2 ;    ///< Max J/psi chi^2
  LHCb::ParticleID  m_jPsiID ;         ///< J/psi ID
  double m_jPsiMass ;    ///< J/psi mass
};
#endif // TUTORIALALGORITHM_H
