// $Id: TsaSeedTrackCnv.h,v 1.2 2007-09-16 10:01:51 mneedham Exp $
#ifndef _TsaSeedTrackCnv_H_
#define _TsaSeedTrackCnv_H_

#include "TsaBaseAlg.h"

#include <string>
#include <vector>

#include "TrackInterfaces/ITrackPtKick.h"

// TsaKernel
#include "TsaKernel_Tf/SeedTrack.h"
#include "TsaKernel_Tf/SeedPnt.h"
#include "TsaKernel_Tf/TsaConstants.h"
#include "TsaKernel_Tf/TsaFun.h"

//  Event
#include "Event/State.h"
#include "Event/Track.h"

//gsl
#include "gsl/gsl_math.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class SeedTrackCnv TsaSeedTrackCnv.h
     *
     *  Converter for Tsa TYracks to standard LHCb Tracks
     *
     *  @author M.Needham
     *  @date   30/04/2004
     */

    class SeedTrackCnv: public BaseAlg {

    public:

      // Constructors and destructor
      SeedTrackCnv(const std::string& name,
                      ISvcLocator* pSvcLocator);
      virtual ~SeedTrackCnv();

      virtual StatusCode initialize();

      virtual StatusCode execute();


    private:

      LHCb::Track* convert(const SeedTrack* aTrack) const;
      void addState(const SeedTrack* aTrack,
                    LHCb::Track* lTrack, const double z) const;

      ITrackPtKick* m_ptKickTool;

      double m_EX2;
      double m_EY2;
      double m_ETx2;
      double m_ETy2;
      double m_EQdivP2;

      double m_likCut;
      double m_curvFactor;
      bool m_pFromCurvature;
      bool m_largeErrors;

      std::string m_inputLocation;
      std::string m_outputLocation;

    };

  }
}

#endif // _TsaToTrTrack_H_








