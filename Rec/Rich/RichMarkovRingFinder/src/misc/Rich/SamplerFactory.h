// $Id: SamplerFactory.h,v 1.1 2008-06-20 09:54:23 jonrob Exp $
#ifndef RICH_SAMPLERFACTORY_H 
#define RICH_SAMPLERFACTORY_H 1

namespace Lester 
{

  /// Static factory method
  template < class CLASS >
  CLASS * getInstance()
  {
    static CLASS instance;
    return &instance;
  }

}

#endif // RICH_SAMPLERFACTORY_H
