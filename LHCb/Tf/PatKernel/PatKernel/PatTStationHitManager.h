#ifndef PATTSTATIONHITMANAGER_H 
#define PATTSTATIONHITMANAGER_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "PatKernel/PatForwardHit.h"

#include "TfKernel/TStationHitManager.h"


  /** @class PatTStationHitManager PatTStationHitManager.h
   *
   *  T station hit manager for Pat.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   */

  class PatTStationHitManager : public Tf::TStationHitManager<PatForwardHit>
  {

  public:

    PatTStationHitManager( const std::string& type,
                           const std::string& name,
                           const IInterface* parent) ;

    virtual ~PatTStationHitManager( );

    virtual void prepareHits() const;  
    
    virtual void prepareHitsInWindow(const Tf::IStationSelector & selector);

    void sortMyHits() const;

  };


#endif // PATTSTATIONHITMANAGER_H
