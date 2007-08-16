
//-----------------------------------------------------------------------------
/** @file OTStationHitManager.h
 *
 *  Header file for class : Tf::OTStationHitManager
 *
 *  $Id: OTStationHitManager.h,v 1.2 2007-08-16 12:54:00 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_OTSTATIONHITMANAGER_H
#define TFKERNEL_OTSTATIONHITMANAGER_H 1

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

    /// Constructor
    OTStationHitManager( const std::string& type,
                         const std::string& name,
                         const IInterface* parent )
      : TStationHitManager<Hit>( type, name, parent ) { }

    /// Destructor
    ~OTStationHitManager( ) { }

  protected:

    /// Re-implement the method to prepare hits to only do OT
    virtual void prepareHits() const;
 
  };

}

template<class Hit>
void Tf::OTStationHitManager<Hit>::prepareHits() const
{
  this->clearHits();
  this->prepareOTHits();
  this->setAllHitsPrepared(true);
}

#endif // TFKERNEL_OTSTATIONHITMANAGER_H
