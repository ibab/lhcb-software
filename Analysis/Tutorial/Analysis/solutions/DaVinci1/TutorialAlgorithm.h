// $Id: TutorialAlgorithm.h,v 1.1 2006-05-25 11:58:29 pkoppenb Exp $
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
};
#endif // TUTORIALALGORITHM_H
