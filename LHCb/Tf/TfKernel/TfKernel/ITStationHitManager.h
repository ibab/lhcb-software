
//-----------------------------------------------------------------------------
/** @file ITStationHitManager.h
 *
 *  Header file for class : Tf::ITStationHitManager
 *
 *  $Id: ITStationHitManager.h,v 1.2 2007-08-16 12:54:00 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_ITSTATIONHITMANAGER_H
#define TFKERNEL_ITSTATIONHITMANAGER_H 1

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

    /// Constructor
    ITStationHitManager( const std::string& type,
                         const std::string& name,
                         const IInterface* parent )
      : TStationHitManager<Hit>( type, name, parent ) { }

    /// Destructor
    ~ITStationHitManager( ) { }

  protected:

    /// Re-implement the method to prepare hits to only do IT
    virtual void prepareHits() const;

  };

}

template<class Hit>
void Tf::ITStationHitManager<Hit>::prepareHits() const
{
  this->clearHits();
  this->prepareITHits();
  this->setAllHitsPrepared(true);
}

#endif // TFKERNEL_ITSTATIONHITMANAGER_H
