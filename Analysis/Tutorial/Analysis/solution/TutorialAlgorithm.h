// $Id: TutorialAlgorithm.h,v 1.1 2004-11-08 14:52:52 pkoppenb Exp $
#ifndef TutorialAlgorithm_H 
#define TutorialAlgorithm_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DaVinciTools/DVAlgorithm.h"


/** @class TutorialAlgorithm TutorialAlgorithm.h
 *  
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
  double m_JPsiMassWin ; ///< Mass window
  double m_JPsiChi2 ;    ///< Max J/psi chi^2
  int m_JPsiID ;         ///< J/psi ID
  double m_JPsiMass ;    ///< J/psi mass
  int m_nJPsis ;         ///< number of selected J/psis
  int m_nEvents ;        ///< number of events
};
#endif // TutorialAlgorithm_H
