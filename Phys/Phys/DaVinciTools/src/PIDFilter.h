// $Id: PIDFilter.h,v 1.1 2005-02-15 07:31:50 pkoppenb Exp $
#ifndef PIDFILTER_H 
#define PIDFILTER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DaVinciTools/DVAlgorithm.h"


/** @class PIDFilter PIDFilter.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-10-07
 */
class PIDFilter : public DVAlgorithm {
public: 
  /// Standard constructor
  PIDFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PIDFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::vector<std::string> m_particleNames;  ///< particle names
  std::vector<int> m_particleIDs;  ///< particle IDs
  bool m_reject ; ///< Reject these particles
   
};
#endif // PIDFILTER_H
