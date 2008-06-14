// $Id: SamplerFactory.h,v 1.1 2008-06-14 09:15:04 jonrob Exp $
#ifndef RICH_SAMPLERFACTORY_H 
#define RICH_SAMPLERFACTORY_H 1

namespace Lester 
{

  /// Static factory method
  template < class SAMPLER >
  SAMPLER * getInstance()
  {
    static SAMPLER sampler;
    return &sampler;
  }

}

#endif // RICH_SAMPLERFACTORY_H
