// $Id$
#ifndef FILTEREVENTLIST_H 
#define FILTEREVENTLIST_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <vector>
#include <utility>
#include <algorithm>


/** @class FilterEventList FilterEventList.h
 *  
 *  Algorithm that selects events with a given list
 *  of (event#,run#)
 *  
 *  OPTIONS:
 *  - EventRunList: list of pairs (Event,Run) to look for
 *  - SetFilterPassed: What to do with events in this list? 
 *                     Pass=true? or Fail=false? Default true
 *
 *  @author Rob Lambert
 *  @date   2009-08-14
 */
class FilterEventList : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterEventList( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterEventList( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef std::pair< int, int >EventRunPair;
  typedef std::vector< std::pair< int, int > > EventRunList;
  
  EventRunList m_list;
  bool m_pass;
  

};
#endif // FILTEREVENTLIST_H
