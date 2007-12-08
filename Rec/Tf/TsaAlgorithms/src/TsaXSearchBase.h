// $ Exp $
#ifndef _TsaXSearchBase_H
#define _TsaXSearchBase_H

#include "TsaKernel/ITsaSeedStep.h"
#include "GaudiAlg/GaudiTool.h"
#include <algorithm>
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedFun.h"
#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedTrack.h"

namespace Tf
{
  namespace Tsa
  {
    // forward delclaration
    class SeedParabolaFit;

    /** @class XSearchBase TsaXSearchBase.h
     * Follow track and pick up hits
     * @author M. Needham
     **/
    class XSearchBase:  public GaudiTool,
                        virtual public ITsaSeedStep {

    public:

      /// constructer
      XSearchBase(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

      // destructer
      virtual ~XSearchBase();

      virtual StatusCode initialize();
      virtual StatusCode finalize();

      virtual StatusCode execute(LHCb::State& hint, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0) ;

      virtual StatusCode execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

    protected:

      typedef void (XSearchBase::*collectFun)(std::vector<SeedHit*>& hits,
                                              const double x,
                                              const double z,
                                              const double sx,
                                              const double win, std::vector<SeedHit*>& selected) const;

      void collectXHits(std::vector<SeedHit*>& hits, std::vector<SeedHit*>::iterator& start,
                        const double x, const double z, double win, const double sx,
                        std::vector<SeedHit*>& selected) const;

      void collectXHitsLinear(std::vector<SeedHit*>& hits,
                              const double x,
                              const double z,
                              const double sx,
                              const double win, std::vector<SeedHit*>& selected) const;

      void collectXHitsCurved(std::vector<SeedHit*>& hits,
                              const double x,
                              const double z,
                              const double sx,
                              const double win, std::vector<SeedHit*>& selected) const;

      int sector() const;

      void cleanup(std::vector<SeedHit*> hits[6]);

      void tagUsedHits(SeedHit* hit1, SeedHit* hit2,  std::vector<SeedHit*> select[3]) const;

      bool goodTrack(const SeedTrack* aTrack) const;

      double m_deltaX;
      double m_sxCut;
      double m_sxMin;
      double m_sxMax;

      double m_xsCut;
      double m_x0Cut;
      double m_sx2Cut;
      double m_dthCut;
      double m_tdCut;
      double m_win;
      double m_win2;
      int m_nWin;
      int m_nWin2;
      int m_nxCut;
      int m_nT2Conf;
      double m_xsParam;
      double m_arrow;
      double m_outlierCut;

      std::string m_collectPolicy;
      collectFun m_collectFun;

      SeedParabolaFit* m_parabolaFit;

    private:

      double dx(const double x, const double z, const double sx, const SeedHit* hit ) const;

      int m_sector;
      double m_nSigmaTx;
      std::string m_seedHitLocation;

    };

    inline void XSearchBase::collectXHits(std::vector<SeedHit*>& hits, std::vector<SeedHit*>::iterator& start,
                                          const double x, const double z, double win, const double sx,
                                          std::vector<SeedHit*>& selected) const{

      std::vector<SeedHit*>::iterator it = start;
      if (win > 6.0 && fabs(sx) <0.4) win = 6.0;
      if (it !=  hits.end()){
        ++it;

        while ( it != hits.end() && (*it)->x() - x < win ) {

          if (sx > 0){
            if ((*it)->z() > z) selected.push_back( *it );
          }
          else {
            if ((*it)->z() < z) selected.push_back( *it );
          }
          ++it;
        }
      }

      it = start;
      if (it != hits.begin()){
        --it;
        while ( it != hits.begin() && x - (*it)->x() < win) {

          if (sx < 0){
            if ((*it)->z() > z) selected.push_back( *it );
          }
          else {
            if ((*it)->z() < z) selected.push_back( *it );
          }
          --it;
        }
      }
    }


    inline double XSearchBase::dx(const double x, const double z, const double sx, const SeedHit* hit ) const{
      return  hit->x() - x - sx*( hit->z() - z );
    }


    inline int XSearchBase::sector() const{
      return m_sector;
    }


    inline bool XSearchBase::goodTrack(const SeedTrack* aTrack) const{

      return (aTrack->nx() > m_nxCut &&  fabs( aTrack->x0() ) < m_x0Cut
              && fabs( aTrack->sx())  < m_sx2Cut  &&  fabs( 53000.0*aTrack->tx() - aTrack->dth() ) < m_tdCut);
    }

  }
}

#endif  // _TsaTsaXSearchBase_H



