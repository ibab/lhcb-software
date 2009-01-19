// $Id: IMCParticleTupleTool.h,v 1.1 2009-01-19 18:05:52 pkoppenb Exp $
#ifndef DECAYTUPLE_IMCPARTICLETUPLETOOL_H
#define DECAYTUPLE_IMCPARTICLETUPLETOOL_H  1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMCParticleTupleTool ( "IMCParticleTupleTool", 1, 0 );

namespace Tuples{
  class Tuple;
};
namespace LHCb {
  class MCParticle;
};

/** @class IMCParticleTupleTool
 * 
 * Fill some mc-particle related variables into a tuple.
 * 
 * \sa IEventTupleTool
 * \sa IParticleTupleTool
 * 
 * @author patrick.koppenburg@cern.ch
 * @date 19 January 2009
 */
class IMCParticleTupleTool : virtual public IAlgTool {
 public:
  virtual ~IMCParticleTupleTool(){};
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMCParticleTupleTool; }

  //! Fill the tuple. 

  //! - \b top  : may be used to provide additional information about \b part, like the top particle of the decay.
  //! - \b part : the particle about which some info are filled.
  //! - \b head : prefix for the tuple column name.
  //! - \b tuple: the tuple to fill
  virtual StatusCode fill( const LHCb::MCParticle* top
			   , const LHCb::MCParticle* part
			   , const std::string& head
			   , Tuples::Tuple& tuple )=0;
};

#endif // DECAYTUPLE_IMCPARTICLETUPLETOOL_H
