// $Id: ICloner.h,v 1.2 2010-08-11 12:45:09 jpalac Exp $
#ifndef MICRODST_ICLONER_H
#define MICRODST_ICLONER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace MicroDST
{

  /** @class ICloner ICloner.h MicroDST/ICloner.h
   *
   *
   *  @author Juan Palacios
   *  @date   2009-07-29
   */

  template <class T>
  class ICloner : virtual public IAlgTool
  {

  public:

    /// Type typedef
    typedef T Type;

  public:

    /// Clone operator
    virtual T* operator() ( const T* source ) = 0;

  public:

    /// Destructor
    virtual ~ICloner<T>() { }

  };

} // MicroDST namespace

#endif // MICRODST_ICLONER_H
