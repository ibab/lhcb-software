// $Id: HltEntry.h,v 1.1.1.1 2006-01-19 10:01:59 hernando Exp $
#ifndef HLTENTRY_H 
#define HLTENTRY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/HltSummary.h"

/** @class HltEntry HltEntry.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-01-16
 */
class HltEntry : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltEntry( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltEntry( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  bool isEntry();

protected:

  int m_boxID;
  std::string m_l0Location;
  std::string m_hltSummaryLocation;
  bool m_filter;

  int m_entries;
  int m_passed;
  

private:

};
#endif // HLTENTRY_H
