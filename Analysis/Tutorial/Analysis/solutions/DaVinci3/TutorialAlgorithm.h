// $Id: TutorialAlgorithm.h,v 1.5 2010-05-27 15:02:30 jpalac Exp $
#ifndef TUTORIALALGORITHM_H 
#define TUTORIALALGORITHM_H 1 

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"

/** @class TutorialAlgorithm TutorialAlgorithm.h
 *  
 *  Solution of problem sheet 3 
 *
 *  https://twiki.cern.ch/twiki/bin/view/LHCb/DaVinciTutorial3
 *
 *  @author Patrick Koppenburg
 *  @date   2006-05-24
 */
class TutorialAlgorithm : public DaVinciTupleAlgorithm {
public: 
  /// Standard constructor
  TutorialAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TutorialAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  StatusCode loopOnDaughters(const LHCb::Particle::Range&)const ;
  StatusCode plotDaughter(const LHCb::Particle*, const std::string& head)const ;
  StatusCode makeMother(const LHCb::Particle::Range&) ;

  double m_motherMassWin ; ///< Mass window
  double m_motherChi2 ;    ///< Max Mother chi^2
  LHCb::ParticleID m_motherID ;         ///< Mother ID
  double m_motherMass ;    ///< Mother mass
  std::string m_motherName ; ///< Mother name




};
#endif // TUTORIALALGORITHM_H
