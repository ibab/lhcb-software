// $Id: SeedFun.h,v 1.3 2007-10-09 18:03:39 smenzeme Exp $
#ifndef _SeedFun_H_
#define _SeedFun_H_

#include <functional>
#include <algorithm>

#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedHit.h"

namespace Tf
{
  namespace Tsa
  {

    namespace SeedFun{

      std::vector<SeedHit*>::iterator startX(std::vector<SeedHit*>& hits, const double x1,
                                             const double z1, const double sx,
                                             const double tol);


      std::vector<SeedHit*>::iterator stopX(std::vector<SeedHit*>& hits, const double x1,
                                            const double z1, const double sx,
                                            const double tol);

      std::vector<SeedHit*>::iterator startStereo(std::vector<SeedHit*>& hits, const double x, const double tol );
      std::vector<SeedHit*>::iterator endStereo(std::vector<SeedHit*>& hits, const double x, const double tol );

    }

    inline std::vector<SeedHit*>::iterator SeedFun::startX(std::vector<SeedHit*>& hits, const double x1,
                                                           const double z1, const double sx, const double tol) {

      if (hits.empty() == true) return hits.end();

      // we don't know which double layer first hit is - hence 10 mm
      const double xTest = x1 + (sx*(hits.front()->z() - z1)) - tol - fabs(10.0*sx) ;
      return std::lower_bound(hits.begin(),hits.end(), xTest , SeedFunctor::compByX_LB<const SeedHit*>());
    }

    inline std::vector<SeedHit*>::iterator SeedFun::stopX(std::vector<SeedHit*>& hits, const double x1,
                                                          const double z1, const double sx, const double tol) {

      if (hits.empty() == true) return hits.end();

      // we don't know which double layer first hit is - hence 10 mm
      const double xTest = x1 + (sx*(hits.front()->z() - z1)) + tol + fabs(10.0*sx) ;
      return std::lower_bound(hits.begin(),hits.end(), xTest , SeedFunctor::compByX_LB<const SeedHit*>());
    }

    inline  std::vector<SeedHit*>::iterator SeedFun::startStereo(std::vector<SeedHit*>& hits,
                                                                 const double x,
                                                                 const double tol){
      const double xTest = x - tol;
      return std::lower_bound(hits.begin(),hits.end(), xTest , SeedFunctor::compByXMin_LB<const SeedHit*>());
    }

    inline  std::vector<SeedHit*>::iterator SeedFun::endStereo(std::vector<SeedHit*>& hits,
                                                               const double x,
                                                               const double tol){
      const double xTest = x + tol;
      return std::lower_bound(hits.begin(),hits.end(), xTest , SeedFunctor::compByXMax_LB<const SeedHit*>());
    }

  }
}

#endif

