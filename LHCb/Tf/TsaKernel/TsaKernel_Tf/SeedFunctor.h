// $Id: SeedFunctor.h,v 1.1 2007-09-16 09:44:45 mneedham Exp $
#ifndef _SeedFunctor_H_
#define _SeedFunctor_H_

#include <functional>
#include <algorithm>

namespace Tf
{
  namespace Tsa
  {
    namespace SeedFunctor
    {

      template <class TYPE1, class TYPE2 = TYPE1 >
      class increasingX  {
      public:
        inline bool operator() ( TYPE1 first, TYPE2  second ) const{
          return ( 0 == second ? true : 0 == first ? false : first->x() < second->x());
        }
      };


      template <class TYPE1, class TYPE2 = TYPE1 >
      class decreasingLikelihood  {
      public:
        inline bool operator() ( TYPE1 first, TYPE2 second ) const{
          return ( 0 == second ? true : 0 == first ? false : first->lik() > second->lik());
        }
      };


      template <class TYPE1, class TYPE2 = TYPE1 >
      class increasingByXHits  {
      public:
        inline bool operator() ( TYPE1 first, TYPE2 second ) const{
          return ( 0 == second ? true : 0 == first ? false : first->nx() > second->nx());
        }
      };

      template <class TYPE1, class TYPE2 = TYPE1 >
      class increasingByHits  {
      public:
        inline bool operator() ( TYPE1 first, TYPE2 second ) const{
          return ( 0 == second ? true : 0 == first ? false : first->nHit() > second->nHit());
        }
      };

      template <class TYPE1, class TYPE2 = TYPE1 >
      class LessByXChi2  {
      public:
        inline bool operator() ( TYPE1 first, TYPE2  second ) const{
          return ( 0 == second ? true : 0 == first ? false : first->xChi2() < second->xChi2());
        }
      };

      template <class TYPE1, class TYPE2 = TYPE1 >
      class increasingSX  {
      public:
        inline bool operator() ( TYPE1 first, TYPE2  second ) const{
          return ( 0 == second ? true : 0 == first ? false : first->sx() < second->sx());
        }
      };

      template <class TYPE>
      class compBySX_LB: public std::binary_function<const TYPE, const double, bool>{
        double testVal;
      public:
        inline bool operator() (const TYPE& obj,const double& testVal) const{
          return ((!obj) ? false : testVal > obj->sx());
        }
      };

      template <class TYPE>
      class compByX_LB: public std::binary_function<const TYPE, const double, bool>{
        double testVal;
      public:
        inline bool operator() (const TYPE& obj,const double& testVal) const{
          return ((!obj) ? false : testVal > obj->x());
        }
      };

      template <class TYPE>
      class compByXMin_LB: public std::binary_function<const TYPE, const double, bool>{
        double testVal;
      public:
        inline bool operator() (const TYPE& obj,const double& testVal) const{
          return ((!obj) ? false : testVal > obj->xMin());
        }
      };

      template <class TYPE>
      class compByXMax_LB: public std::binary_function<const TYPE, const double, bool>{
        double testVal;
      public:
        inline bool operator() (const TYPE& obj,const double& testVal) const{
          return ((!obj) ? false : testVal > obj->xMax());
        }
      };

    }
  }
}

#endif //SeedFunctor
