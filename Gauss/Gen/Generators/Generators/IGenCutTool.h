// $Id: IGenCutTool.h,v 1.2 2005-11-17 15:54:02 robbep Exp $
#ifndef GENERATORS_IGENCUTTOOL_H 
#define GENERATORS_IGENCUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace HepMC {
  class GenParticle ; 
  class GenEvent ;
}  
class HardInfo ;

/** @class IGenCutTool IGenCutTool.h Generators/IGenCutTool.h
 *  
 *  Abstract interface to generator level cut
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IGenCutTool( "IGenCutTool" , 1 , 0 ) ;

class IGenCutTool : virtual public IAlgTool {
public:
  typedef std::vector< HepMC::GenParticle * > ParticleVector ;
  static const InterfaceID& interfaceID() { return IID_IGenCutTool ; }
  
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theGenEvent ,
                         const HardInfo * theHardInfo ) const = 0 ;
};
#endif // GENERATORS_ICUTTOOL_H
