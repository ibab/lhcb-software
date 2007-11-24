// $Id: FilterByRunEvent.h,v 1.1 2007-11-24 18:08:52 gligorov Exp $
#ifndef FILTERBYRUNEVENT_H 
#define FILTERBYRUNEVENT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class FilterByRunEvent FilterByRunEvent.h
 *   
 *
 *  @author Patrick Spradlin
 *  @date   2007-01-18
 */
class FilterByRunEvent : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterByRunEvent( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterByRunEvent( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::vector<long> m_runnum;	///< Vector of run numbers
  std::vector<long> m_evnum;	///< Vector of event numbers
  bool m_passSelect;		///< If true, will pass list events; false, fail

};
#endif // FILTERBYRUNEVENT_H
