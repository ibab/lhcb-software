// $ Exp $
#ifndef _TsaStereoBase_H
#define _TsaStereoBase_H

#include "TsaKernel/ITsaSeedStep.h"
#include "GaudiAlg/GaudiTool.h"
#include <algorithm>

#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedTrack.h"

namespace Tf
{
  namespace Tsa
  {

    class SeedLineFit;

    /** @class StereoBase TsaStereoBase.h
     * Follow track and pick up hits
     * @author M. Needham
     **/

    class StereoBase:  public GaudiTool,
                       virtual public ITsaSeedStep
    {

    public:

      /// constructor
      StereoBase(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

      /// destructor
      virtual ~StereoBase();

      StatusCode initialize();
      StatusCode finalize();

      virtual StatusCode execute(LHCb::State& hint, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

      virtual StatusCode execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

    protected:

      int sector() const;

      void cleanup(std::vector<SeedHit*> hits[6]);

      unsigned int collectHits(SeedTrack* seed, std::vector<SeedHit*> hits[6],
                               std::vector<SeedHit*> yHits[6] );


      double m_syCut;
      double m_syMin;
      double m_syMax;
      double m_win;
      double m_nWin;
      double m_outlierCut;
      double m_yTol;
      double m_nY;

      SeedLineFit* m_fitLine;

    private:

      int m_sector;
      double m_nSigmaTy;
      std::string m_seedHitLocation;

    };

    inline int StereoBase::sector() const{
      return m_sector;
    }

  }
}

#endif  // _TsaStereoBase_H



