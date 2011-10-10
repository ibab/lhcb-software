
//-----------------------------------------------------------------------------
/** @file TsaTStationHitManager.h
 *
 *  Header file for class : Tf::Tsa::TStationHitManager
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TSAALGORITHMS_TSATSTATIONHITMANAGER_H
#define TSAALGORITHMS_TSATSTATIONHITMANAGER_H 1

#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // Tf::Tsa::ITsaSeedStep::execute only partially overridden
  #pragma warning(push)
#endif
#include "TfKernel/TStationHitManager.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

#include "TsaKernel/TsaSeedingHit.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class TStationHitManager TsaTStationHitManager.h
     *
     *  Derived implementation of an OT hit manager for Tsa.
     *
     *  @author Chris Jones
     *  @date   2007-08-16
     */
    class TStationHitManager : public Tf::TStationHitManager<Tf::Tsa::SeedingHit>
    {

    public:

      /// Standard constructor
      TStationHitManager( const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

      virtual ~TStationHitManager( ); ///< Destructor

    protected:

      /** Create a new hit object from an OTHit
       *  Re-implement here to allow custom Tsa specific things to be done
       */
      virtual Tf::Tsa::SeedingHit * createHit( const Tf::OTHit & othit ) const;

    };

  }
}

#endif // TSAALGORITHMS_TSATSTATIONHITMANAGER_H
