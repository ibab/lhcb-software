#ifndef KERNEL_PARTICLE2MCMETHOD_H
#define KERNEL_PARTICLE2MCMETHOD_H 1

#include <string>
#include <array>
// Include files

/** @namespace Particle2MCMethod Particle2MCMethod.h Kernel/Particle2MCMethod.h
 *
 *  @author Philippe CHARPENTIER
 *  @author Juan PALACIOS
 *  @date   2006-10-11
 */
namespace Particle2MCMethod {

  /**  Enumeration for MC association method
   *
   *  @author Philippe CHARPENTIER
   *  @author Juan PALACIOS
   *  @date   2006-10-11
   */
  enum AssociationMethod {
    No = 0, WithChi2, Chi2, Links, Composite, ChargedPP, NeutralPP, Max
  } ;

  static const std::array<std::string,Max> extension= {
    "",
    "/WithChi2",
    "/Chi2",
    "/Links",
    "/Composite",
    "/PP2MC",
    "/PP2MC"
  };
  static const std::array<std::string,Max> algType = {
    "",
    "Particle2MCWithChi2",
    "Particle2MCChi2",
    "Particle2MCLinks",
    "CompositeParticle2MCLinks",
    "ChargedPP2MC",
    "NeutralPP2MC"
  };

}
#endif // KERNEL_PARTICLE2MCMETHOD_H
