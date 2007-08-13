
#ifndef OTSTATIONHITMANAGER_H
#define OTSTATIONHITMANAGER_H 1

#include "TfKernel/TStationHitManager.h"

namespace Tf
{

  /** @class OTStationHitManager OTStationHitManager.h
   *  Base class for all OT station extended hit data managers.
   *  Can also be used directly.
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/

  template < class Hit >
  class OTStationHitManager : public TStationHitManager<Hit>
  {

  public:

    OTStationHitManager( const std::string& type,
                         const std::string& name,
                         const IInterface* parent )
      : TStationHitManager<Hit>( type, name, parent ) { }

    ~OTStationHitManager( ) { }

    virtual void prepareHits();
  };

}

template<class Hit>
void Tf::OTStationHitManager<Hit>::prepareHits()
{
  this->clearHits();
  this->prepareOTHits();
}

#endif // OTSTATIONHITMANAGER_H
