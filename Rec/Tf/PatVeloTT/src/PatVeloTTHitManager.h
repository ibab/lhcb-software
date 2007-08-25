#ifndef PATVELOTTTHITMANAGER_H 
#define PATVELOTTTHITMANAGER_H  1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/TTStationHitManager.h"

#include "PatVeloTTHit.h"

namespace Tf {

  /** @class PatVeloTTHitManager PatTStationHitManager.h
   *
   *  Velo station hit manager for Pat.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   */

  class PatVeloTTHitManager : public TTStationHitManager<PatVeloTTHit>
  {

  public:

    PatVeloTTHitManager( const std::string& type,
                      const std::string& name,
                      const IInterface* parent) ;

    virtual ~PatVeloTTHitManager( );

  };
}

#endif // PATVELOTTTHITMANAGER_H 
