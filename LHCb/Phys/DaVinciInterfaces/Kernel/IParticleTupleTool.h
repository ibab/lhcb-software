// $Id: IParticleTupleTool.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DECAYTUPLE_IPARTICLETUPLETOOL_H
#define DECAYTUPLE_IPARTICLETUPLETOOL_H  1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Tuples{
  class Tuple;
};
namespace LHCb {
  class Particle;
};

/** @class IParticleTupleTool
 * 
 * Fill some particle related variables into a tuple.
 * 
 * \sa IEventTupleTool
 * 
 * @author Jeremie Borel
 * @date November 2007
 */
class GAUDI_API IParticleTupleTool : virtual public IAlgTool {
 public:

  DeclareInterfaceID(IParticleTupleTool, 2, 0);
  
  virtual ~IParticleTupleTool(){};

  //! Fill the tuple. 

  //! - \b top  : may be used to provide additional information about \b part, like the top particle of the decay.
  //! - \b part : the particle about which some info are filled.
  //! - \b head : prefix for the tuple column name.
  //! - \b tuple: the tuple to fill
  virtual StatusCode fill( const LHCb::Particle* top
                           , const LHCb::Particle* part
                           , const std::string& head
                           , Tuples::Tuple& tuple )=0;
};

#endif // DECAYTUPLE_IPARTICLETUPLETOOL_H
