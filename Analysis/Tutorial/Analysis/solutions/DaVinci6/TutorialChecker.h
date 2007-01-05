// $Id: TutorialChecker.h,v 1.2 2007-01-05 17:08:47 pkoppenb Exp $
#ifndef TUTORIALCHECKER_H 
#define TUTORIALCHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

#include "Kernel/IBackgroundCategory.h"
#include "DaVinciAssociators/Particle2MCLinker.h"

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
  StatusCode fillTruth(Tuple&,const LHCb::Particle*);
  StatusCode fillTagging(Tuple&,const LHCb::Particle*);
  
private:
  IBackgroundCategory* m_background ;
  Particle2MCLinker* m_pLinker;
};
#endif // TUTORIALCHECKER_H
