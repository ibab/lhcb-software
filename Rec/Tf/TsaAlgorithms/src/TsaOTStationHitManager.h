
//-----------------------------------------------------------------------------
/** @file TsaOTStationHitManager.h
 *
 *  Header file for class : Tf::Tsa::OTStationHitManager
 *
 *  $Id: TsaOTStationHitManager.h,v 1.1 2007-08-16 12:55:55 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TSAALGORITHMS_TSAOTSTATIONHITMANAGER_H
#define TSAALGORITHMS_TSAOTSTATIONHITMANAGER_H 1

#include "TfKernel/OTStationHitManager.h"

#include "TsaKernel/TsaSeedingHit.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class OTStationHitManager TsaOTStationHitManager.h
     *
     *  Derived implementation of an OT hit manager for Tsa.
     *
     *  @author Chris Jones
     *  @date   2007-08-16
     */
    class OTStationHitManager : public Tf::OTStationHitManager<Tf::Tsa::SeedingHit>
    {

    public:

      /// Standard constructor
      OTStationHitManager( const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

      virtual ~OTStationHitManager( ); ///< Destructor

    protected:

      /** Create a new hit object from an OTHit
       *  Re-implement here to allow custom Tsa specific things to be done
       */
      virtual Tf::Tsa::SeedingHit * createHit( const Tf::OTHit & othit ) const;

    };

  }
}

#endif // TSAALGORITHMS_TSAOTSTATIONHITMANAGER_H
