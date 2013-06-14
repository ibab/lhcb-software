// $Id: ICloneTagger.h,v 1.1 2009-11-17 15:52:36 jpalac Exp $
#ifndef MICRODST_ICLONETAGGER_H 
#define MICRODST_ICLONETAGGER_H 1

// Include files
// from STL
#include <string>

// from MicroDST
#include "MicroDST/ICloner.h"
namespace LHCb 
{
  class Tagger;
}

/** @class ICloneTagger MicroDST/ICloneTagger.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-11-17
 */
class GAUDI_API ICloneTagger : virtual public MicroDST::ICloner<LHCb::Tagger>
{

public: 

  /// Interface ID
  DeclareInterfaceID(ICloneTagger, 1, 0 );

  /// Destructor
  virtual ~ICloneTagger() { }

};

#endif // MICRODST_ICLONETAGGER_H
