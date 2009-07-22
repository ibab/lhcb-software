// $Id: IProtoParticleFilter.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
#ifndef DAVINCIKERNEL_IProtoParticleFilter_H 
#define DAVINCIKERNEL_IProtoParticleFilter_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb
{
  class ProtoParticle;
}

static const InterfaceID IID_IProtoParticleFilter("IProtoParticleFilter", 1 , 0); 

/** @class IProtoParticleFilter IProtoParticleFilter.h Kernel/IProtoParticleFilter.h 
 *
 *  Interface Class for ProtoParticle filter tools. Given a ProtoParticle, returns a 
 *  yes/no decision based on the configured selection criteria.
 *
 *  @author Chris Jones
 *  @date   02/05/2006
 */

class IProtoParticleFilter : virtual public IAlgTool
{

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IProtoParticleFilter; }

  /// Test if filter is satisfied
  virtual bool isSatisfied( const LHCb::ProtoParticle* const & proto ) const = 0;

  /// Test if filter is satisfied
  inline bool operator()( const LHCb::ProtoParticle* const & proto ) const 
  { 
    return this->isSatisfied(proto); 
  }

};

#endif // DAVINCIKERNEL_IProtoParticleFilter_H


