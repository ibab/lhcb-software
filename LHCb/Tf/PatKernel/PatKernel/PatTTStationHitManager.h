#ifndef PATTTSTATIONHITMANAGER_H 
#define PATTTSTATIONHITMANAGER_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "PatKernel/PatTTHit.h"

#include "TfKernel/TTStationHitManager.h"

  /** @class PatTTStationHitManager PatTTStationHitManager.h
   *
   *  TT station hit manager for Pat.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01 Initial version
   */

  class PatTTStationHitManager : public Tf::TTStationHitManager <PatTTHit>
  {

  public:

    PatTTStationHitManager( const std::string& type,
                            const std::string& name,
                            const IInterface* parent) ;

    virtual ~PatTTStationHitManager( );

  };


#endif // PATTSTATIONHITMANAGER_H
