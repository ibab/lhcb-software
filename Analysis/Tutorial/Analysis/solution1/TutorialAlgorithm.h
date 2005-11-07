// $Id: TutorialAlgorithm.h,v 1.1 2005-11-07 15:30:45 pkoppenb Exp $
#ifndef TutorialAlgorithm_H 
#define TutorialAlgorithm_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DaVinciTools/DVAlgorithm.h"


/** @class TutorialAlgorithm TutorialAlgorithm.h
 *  
 *  Solution for exercise 1
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-11-04
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
  double m_jPsiMassWin ; ///< Mass window
  double m_jPsiChi2 ;    ///< Max J/psi chi^2
  int m_jPsiID ;         ///< J/psi ID
  double m_jPsiMass ;    ///< J/psi mass
  int m_nJPsis ;         ///< number of selected J/psis
  int m_nEvents ;        ///< number of events
};
#endif // TutorialAlgorithm_H
