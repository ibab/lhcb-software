// $Id: TutorialTuple.h,v 1.3 2005-12-14 12:30:23 pkoppenb Exp $
#ifndef TUTORIALTUPLE_H 
#define TUTORIALTUPLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCLink.h"
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "Kernel/IParticleDescendants.h" // TMP
#include "DaVinciMCTools/IBackgroundCategory.h"

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
  bool m_truth ; ///< Use MC truth
  Particle2MCLink* m_pAsct;   ///<  Composite associator

  std::vector<std::string> m_containers ; ///< Containers for associators
  IParticleDescendants* m_TMP;
  IBackgroundCategory* m_background ;
  
};
#endif // TUTORIALTUPLE_H
