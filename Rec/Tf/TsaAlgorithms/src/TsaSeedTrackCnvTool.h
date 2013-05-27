#ifndef TSASEEDTRACKCNVTOOL_H 
#define TSASEEDTRACKCNVTOOL_H 1

// Include files
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITrackMomentumEstimate.h"

// TsaKernel
#include "TsaKernel/ITsaSeedTrackCnvTool.h"            // Interface
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedPnt.h"
#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/TsaFun.h"

//  Event
#include "Event/State.h"
#include "Event/Track.h"

//gsl
#include "gsl/gsl_math.h"


namespace Tf
{
  namespace Tsa
  {
    /** @class TsaSeedTrackCnvTool TsaSeedTrackCnvTool.h
     *  
     *
     *  @author M.Needham
     *  @date   30/04/2004
     *
     *  2007-10-31 : Johannes Albrecht - convert to tool
     */
    class SeedTrackCnvTool : public GaudiTool, virtual public ITsaSeedTrackCnvTool {
    public: 
      /// Standard constructor
      SeedTrackCnvTool( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

      virtual ~SeedTrackCnvTool( ); ///< Destructor

      virtual StatusCode initialize();

      LHCb::Track* convert(const SeedTrack* aTrack) const;
  
    protected:

    private:

      void addState(const SeedTrack* aTrack,
                    LHCb::Track* lTrack, const double z) const;

      ITrackMomentumEstimate* m_momentumTool;
      
      double m_EX2;
      double m_EY2;
      double m_ETx2;
      double m_ETy2;
      double m_EQdivP2;

      double m_curvFactor;
      bool m_pFromPtKick;
      bool m_pFromCurvature;
      bool m_largeErrors;

      std::string m_inputLocation;
      std::string m_outputLocation;
      std::vector<double> m_zStates;
 
    };
  }
}

#endif // TSASEEDTRACKCNVTOOL_H
