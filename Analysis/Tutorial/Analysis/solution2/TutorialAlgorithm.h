// $Id: TutorialAlgorithm.h,v 1.2 2004-11-09 14:59:01 pkoppenb Exp $
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
  double m_MassWin ; ///< Mass window
  double m_Chi2 ;    ///< Max chi^2
  std::string m_Name ; ///< Name of particle
  int m_ID ;         ///< ID
  double m_Mass ;    ///< mass
  int m_nFound ;     ///< number of selected particles
  int m_nEvents ;    ///< number of events
};
#endif // TutorialAlgorithm_H
