// $Id: FilterByRunEvent.h,v 1.3 2008-04-25 11:58:53 graven Exp $
#ifndef FILTERBYRUNEVENT_H 
#define FILTERBYRUNEVENT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <vector>
#include <utility>

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

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

  std::vector<std::pair<int,int> > m_events; ///< Run/event number pairs
  //FIXME: needs to be: std::vector<std::pair<unsigned int,ulonglong> > m_events; ///< Run/event number pairs
  bool m_passSelect;		///< If true, will pass list events; false, fail

};
#endif // FILTERBYRUNEVENT_H
