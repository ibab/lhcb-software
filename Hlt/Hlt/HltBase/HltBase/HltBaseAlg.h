// $Id: HltBaseAlg.h,v 1.7 2008-01-22 09:34:58 hernando Exp $
#ifndef HLTBASE_HLTBASEALG_H 
#define HLTBASE_HLTBASEALG_H 1

// Include files

#include "GaudiAlg/GaudiHistoAlg.h"
#include "HltBase/HltBase.h"

/** @class HltBaseAlg 
 *  @author Hugo Ruiz Perez
 *  @author Jose Angel Hernando Morata
 *  @date   2006-06-15
 */

class HltBaseAlg : public HltBase<GaudiHistoAlg> {
public:

  /// Standard constructor
  HltBaseAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltBaseAlg( ); ///< Destructor

  virtual StatusCode initialize();

  virtual StatusCode finalize();

};
#endif // HLTBASE_HLTBASEALG_H
