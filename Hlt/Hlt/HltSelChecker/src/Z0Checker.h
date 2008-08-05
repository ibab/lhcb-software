// $Id: Z0Checker.h,v 1.1 2008-08-05 09:37:43 pkoppenb Exp $
#ifndef Z0CHECKER_H 
#define Z0CHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class Z0Checker Z0Checker.h
 *  
 *  Test
 *
 *  @author Patrick Koppenburg
 *  @date   2008-06-18
 */
class Z0Checker : public DVAlgorithm {
public: 
  /// Standard constructor
  Z0Checker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Z0Checker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::vector<std::string> m_locations ;
  
};
#endif // Z0CHECKER_H
