// $Id: HltInit.h,v 1.6 2008-01-22 09:56:32 hernando Exp $
#ifndef HLTDATASTOREINIT_H 
#define HLTDATASTOREINIT_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IDataManagerSvc.h" 
#include "HltBase/HltBaseAlg.h"

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
class HltInit : public HltBaseAlg {
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

  IDataManagerSvc* m_hltMan;

  Hlt::Data* m_HLTData;

  Hlt::Configuration* m_HLTConf;

};
#endif // HLTDATASTOREINIT_H
