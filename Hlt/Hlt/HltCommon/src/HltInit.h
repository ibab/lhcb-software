// $Id: HltInit.h,v 1.5 2007-06-25 20:50:25 hernando Exp $
#ifndef HLTDATASTOREINIT_H 
#define HLTDATASTOREINIT_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/HltSummary.h"
#include "HltBase/HltTypes.h"

/** @class HltInit HltInit.h
 *  
 *  functionality:
 *         intialize a Hlt main algorithms
 *         creates a hltDataSvc
 *         store a HltConfiguration with enum infos
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-25
 */
class HltInit : public GaudiAlgorithm {
public:


  /// Standard constructor
  HltInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void saveConfiguration();

protected:

  std::string m_TCKName;  
  std::string m_dataSummaryLocation;

protected:

  IDataManagerSvc* m_hltSvc;
  Hlt::Configuration m_hltConfiguration;
  LHCb::HltSummary m_datasummary;

};
#endif // HLTDATASTOREINIT_H
