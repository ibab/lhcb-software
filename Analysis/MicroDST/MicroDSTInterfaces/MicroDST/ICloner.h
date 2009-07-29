// $Id: ICloner.h,v 1.1 2009-07-29 21:28:16 jpalac Exp $
#ifndef MICRODST_ICLONER_H 
#define MICRODST_ICLONER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"


/** @class ICloner ICloner.h MicroDST/ICloner.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2009-07-29
 */
namespace MicroDST 
{
template <class T>
class ICloner : virtual public IAlgTool {
public: 
  
  virtual T* operator() (const T* source) = 0;
  

};
} // MicroDST namespace

#endif // MICRODST_ICLONER_H
