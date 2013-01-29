#ifndef PAUTTSTATIONHITMANAGER_H 
#define PAUTTSTATIONHITMANAGER_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "PatKernel/PatTTHit.h"

#include "TfKernel/UTStationHitManager.h"

  /** @class PatUTStationHitManager PatUTStationHitManager.h
   *
   *  UT station hit manager for Pat.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01 Initial version
   */

  class PatUTStationHitManager : public Tf::UTStationHitManager <PatTTHit>
  {

  public:

    PatUTStationHitManager( const std::string& type,
                            const std::string& name,
                            const IInterface* parent) ;

    virtual ~PatUTStationHitManager( );

  };


#endif // PAUTSTATIONHITMANAGER_H
