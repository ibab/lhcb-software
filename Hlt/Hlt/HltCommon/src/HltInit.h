// $Id: HltInit.h,v 1.7 2008-05-15 08:56:55 graven Exp $
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
  virtual StatusCode reinitialize();    ///< Algorithm reinitialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


private:

  IDataManagerSvc* m_hltMan;


};
#endif // HLTDATASTOREINIT_H
