// $Id: TutorialChecker.h,v 1.6 2008-04-24 12:46:16 pkoppenb Exp $
#ifndef TUTORIALCHECKER_H 
#define TUTORIALCHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

#include "Kernel/IBackgroundCategory.h"
#include "Kernel/Particle2MCLinker.h"

/** @class TutorialChecker TutorialChecker.h
 *   
 *
 *  @author Patrick Koppenburg
 *  @date   2006-12-21
 */
class TutorialChecker : public DVAlgorithm {
public: 
  /// Standard constructor
  TutorialChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TutorialChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  StatusCode fillReco(Tuple&,const LHCb::Particle*);
  StatusCode fillTruth(Tuple&,const LHCb::Particle*, Particle2MCLinker&);
  StatusCode fillTagging(Tuple&,const LHCb::Particle*);
  StatusCode fillTrigger(Tuple&);
  StatusCode fillHeader(Tuple&);
  
private:
  IBackgroundCategory* m_background ;
};
#endif // TUTORIALCHECKER_H
