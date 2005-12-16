// $Id: TutorialTuple.h,v 1.5 2005-12-16 15:26:39 pkoppenb Exp $
#ifndef TUTORIALTUPLE_H 
#define TUTORIALTUPLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#ifdef MCCheck
#include "DaVinciAssociators/Particle2MCLink.h"
#include "DaVinciMCTools/IBackgroundCategory.h"
#endif

/** @class TutorialTuple TutorialTuple.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-11-07
 */
class TutorialTuple : public DVAlgorithm {
public: 
  /// Standard constructor
  TutorialTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TutorialTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  StatusCode fillBs(Tuple&,const Particle*);
  StatusCode fillTrigger(Tuple&);
  StatusCode fillTagging(Tuple&,const Particle*);
  StatusCode fillTruth(Tuple&,const Particle*);

private:
#ifdef MCCheck
  bool m_truth ; ///< Use MC truth
  Particle2MCLink* m_pAsct;   ///<  Composite associator
  std::vector<std::string> m_containers ; ///< Containers for associators
  IBackgroundCategory* m_background ;
#endif
  
};
#endif // TUTORIALTUPLE_H
