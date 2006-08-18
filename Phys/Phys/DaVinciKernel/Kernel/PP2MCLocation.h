// $Id: PP2MCLocation.h,v 1.1 2006-08-18 12:00:26 jpalac Exp $
#ifndef KERNEL_PP2MCLOCATION_H 
#define KERNEL_PP2MCLOCATION_H 1

// Include files

/** @namespace ProtoParticle2MCLocation PP2MCLocation.h Kernel/PP2MCLocation.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-08-18
 */
namespace LHCb 
{
  
  namespace ProtoParticle2MCLocation {

    const std::string Charged  = "Relations/" + LHCb::ProtoParticleLocation::Charged ;
    const std::string Neutrals = "Relations/" + LHCb::ProtoParticleLocation::Neutrals ;
  }
}
#endif // KERNEL_PP2MCLOCATION_H
