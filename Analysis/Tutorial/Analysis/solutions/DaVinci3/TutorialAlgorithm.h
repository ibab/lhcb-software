// $Id: TutorialAlgorithm.h,v 1.1 2006-05-30 16:17:25 pkoppenb Exp $
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
  StatusCode loopOnDaughters(const LHCb::Particle::ConstVector&)const ;
  StatusCode plotDaughter(const LHCb::Particle*, const std::string& head)const ;
  StatusCode makeMother(const LHCb::Particle::ConstVector&) ;

  double m_motherMassWin ; ///< Mass window
  double m_motherChi2 ;    ///< Max Mother chi^2
  int m_motherID ;         ///< Mother ID
  double m_motherMass ;    ///< Mother mass
  int m_nMothers ;         ///< number of selected mothers
  int m_nEvents ;        ///< number of events
  std::string m_motherName ; ///< Mother name




};
#endif // TUTORIALALGORITHM_H
