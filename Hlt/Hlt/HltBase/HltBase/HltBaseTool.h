// $Id: HltBaseTool.h,v 1.1 2008-01-22 09:34:58 hernando Exp $
#ifndef HLTBASE_HLTBASETOOL_H 
#define HLTBASE_HLTBASETOOL_H 1

// Include files

#include "GaudiAlg/GaudiHistoTool.h"
#include "HltBase/HltBase.h"

/** @class HltBaseAlg 
 */

class HltBaseTool : public HltBase<GaudiHistoTool> {
public:
  
  HltBaseTool( const std::string& type,  const std::string& name,
               const IInterface* pSvcLocator );
  
  virtual ~HltBaseTool( ); 

  virtual StatusCode initialize();

  virtual StatusCode finalize();

};
#endif // HLTBASE_HLTBASEALG_H
