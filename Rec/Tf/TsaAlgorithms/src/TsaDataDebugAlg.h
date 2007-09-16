// $Id: TsaDataDebugAlg.h,v 1.2 2007-09-16 10:01:50 mneedham Exp $
#ifndef TSADATADEBUGALG_H
#define TSADATADEBUGALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include "TsaKernel_Tf/SeedStub.h"
#include "TsaKernel_Tf/SeedTrack.h"
#include "TsaKernel_Tf/SeedHit.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class DataDebugAlg TsaDataDebugAlg.h
     *
     *  Low level debug algorithm for Tsa data objects
     *
     *  @author Chris Jones
     *  @date   2007-07-17
     */

    class DataDebugAlg : public GaudiHistoAlg
    {

    public:

      /// Standard constructor
      DataDebugAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~DataDebugAlg( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution
      virtual StatusCode finalize  ();    ///< Algorithm finalization

    private: // methods

      /// Debug the Seed Hits
      StatusCode debugSeedHits();

    private: // data

      std::string m_seedHitLocation; ///< Location in TES for Seed Hits

    };

  }
}

#endif // TSADATADEBUGALG_H
