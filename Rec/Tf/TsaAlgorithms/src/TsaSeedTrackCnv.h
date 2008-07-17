// $Id: TsaSeedTrackCnv.h,v 1.5 2008-07-17 13:22:20 smenzeme Exp $
#ifndef _TsaSeedTrackCnv_H_
#define _TsaSeedTrackCnv_H_

#include "TsaBaseAlg.h"

#include <string>
#include <vector>

// TsaKernel
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedPnt.h"
#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/TsaFun.h"
#include "TsaKernel/ITsaSeedTrackCnvTool.h"

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
     *  Converter for Tsa Tracks to standard LHCb Tracks
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

      double m_likCut;
      std::string m_inputLocation;
      std::string m_outputLocation;

      Tf::Tsa::ITsaSeedTrackCnvTool* m_Tsa2TrackCnv;

    };

  }
}

#endif // _TsaToTrTrack_H_








