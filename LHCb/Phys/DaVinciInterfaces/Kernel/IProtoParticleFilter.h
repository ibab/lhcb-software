// $Id: IProtoParticleFilter.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_IProtoParticleFilter_H 
#define DAVINCIKERNEL_IProtoParticleFilter_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb
{
  class ProtoParticle;
}

/** @class IProtoParticleFilter IProtoParticleFilter.h Kernel/IProtoParticleFilter.h 
 *
 *  Interface Class for ProtoParticle filter tools. Given a ProtoParticle, returns a 
 *  yes/no decision based on the configured selection criteria.
 *
 *  @author Chris Jones
 *  @date   02/05/2006
 */

class GAUDI_API IProtoParticleFilter : virtual public IAlgTool
{

public:

  DeclareInterfaceID(IProtoParticleFilter, 2, 0);
  
  /// Test if filter is satisfied
  virtual bool isSatisfied( const LHCb::ProtoParticle* const & proto ) const = 0;

  /// Test if filter is satisfied
  inline bool operator()( const LHCb::ProtoParticle* const & proto ) const 
  { 
    return this->isSatisfied(proto); 
  }

};

#endif // DAVINCIKERNEL_IProtoParticleFilter_H


