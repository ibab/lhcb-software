
#ifndef ITSTATIONHITMANAGER_H
#define ITSTATIONHITMANAGER_H 1

#include "TfKernel/TStationHitManager.h"

namespace Tf
{

  /** @class ITStationHitManager ITStationHitManager.h
   *  Base class for all IT station extended hit data managers.
   *  Can also be used directly.
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/

  template < class Hit >
  class ITStationHitManager : public TStationHitManager<Hit>
  {

  public:

    ITStationHitManager( const std::string& type,
                         const std::string& name,
                         const IInterface* parent )
      : TStationHitManager<Hit>( type, name, parent ) { }

    ~ITStationHitManager( ) { }

    virtual void prepareHits();
  };

}

template<class Hit>
void Tf::ITStationHitManager<Hit>::prepareHits()
{
  this->clearHits();
  this->prepareITHits();
}

#endif // ITSTATIONHITMANAGER_H
