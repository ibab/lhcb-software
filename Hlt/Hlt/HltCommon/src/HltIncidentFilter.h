// $Id: HltIncidentFilter.h,v 1.1 2008-07-14 07:15:49 graven Exp $
#ifndef HLTINCIDENTFILTER_H 
#define HLTINCIDENTFILTER_H 1

// Include files
// from Gaudi

#include "HltBase/HltAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentListener.h"

/** @class HltIncidentFilter HltIncidentFilter.h
 *  
 *  functionality:
 *      pass event if a given Incident has fired during
 *      event processing
 *
 *  Options:
 *
 *  @author Gerhard Raven
 *  @date   2008-07-07
 */
class HltIncidentFilter : public HltAlgorithm, virtual public IIncidentListener {
public: 

  /// Standard constructor
  HltIncidentFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltIncidentFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  void handle(const Incident& incident);
private:
  struct stat {
       stat() : request(0),accept(0) {} // make sure default c'tor zeros data...
       unsigned long request;
       unsigned long accept;
  };
  /// NOTE: we only keep requests if present in quota. 
  /// If you're not there, you will be ignored
  std::map<std::string,int>        m_quota; // note: this is PER PROCESS
  std::map<std::string,stat>       m_stat;
  std::string                      s_incident;
  bool                             m_keep;
};
#endif
