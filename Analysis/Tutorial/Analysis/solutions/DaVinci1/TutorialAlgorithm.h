// $Id: TutorialAlgorithm.h,v 1.2 2008-12-22 18:08:36 pkoppenb Exp $
#ifndef TUTORIALALGORITHM_H 
#define TUTORIALALGORITHM_H 1 

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"


/** @class TutorialAlgorithm TutorialAlgorithm.h
 *  
 *  Simple algorithm that loops on muons and plots some quantities of interest.
 *
 *  See https://twiki.cern.ch/twiki/bin/view/LHCb/DaVinciTutorial
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
  StatusCode loopOnMuons(const LHCb::Particle::Range&)const ;
};
#endif // TUTORIALALGORITHM_H
