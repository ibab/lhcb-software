// $Id: TutorialTuple.h,v 1.1 2005-11-07 15:30:45 pkoppenb Exp $
#ifndef TUTORIALTUPLE_H 
#define TUTORIALTUPLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


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


};
#endif // TUTORIALTUPLE_H
