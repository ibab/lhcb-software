#ifndef PRUTSTATIONHITMANAGER_H 
#define PRUTSTATIONHITMANAGER_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "PrKernel/PrUTHit.h"

#include "TfKernel/UTStationHitManager.h"

  /** @class PrUTStationHitManager PrUTStationHitManager.h
   *
   *  UT station hit manager for Pr.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01 Initial version
   */

  class PrUTStationHitManager : public Tf::UTStationHitManager <PrUTHit>
  {

  public:

    PrUTStationHitManager( const std::string& type,
                            const std::string& name,
                            const IInterface* parent) ;

    virtual ~PrUTStationHitManager( );

  };


#endif // PRUTSTATIONHITMANAGER_H
