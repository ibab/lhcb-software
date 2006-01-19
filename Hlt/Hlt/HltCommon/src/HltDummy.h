// $Id: HltDummy.h,v 1.1.1.1 2006-01-19 10:01:59 hernando Exp $
#ifndef HLTDUMMY_H 
#define HLTDUMMY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/HltSummary.h"

/** @class HltDummy HltDummy.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-01-16
 */
class HltDummy : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltDummy( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDummy( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_hltSummaryLocation;
  int m_boxID;
  bool m_filter;

private:

  int m_entries;
  int m_passed;

};
#endif // HLTDUMMY_H
