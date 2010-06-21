// $Id: DaVinciStringUtils.h,v 1.3 2010-06-21 12:17:04 jpalac Exp $
#ifndef KERNEL_DAVINCISTRINGUTILS_H 
#define KERNEL_DAVINCISTRINGUTILS_H 1

// Include files

#include <string>
#include <vector>
/** @file StringUtils.h
 *
 *  Namespace for functions dealing with string manipulations for DaVinci
 *  components.
 * 
 *  @author Juan PALACIOS
 *  @date   2006-06-19
 */
namespace DaVinci 
{
  namespace StringUtils 
  {

    /** Strip particle name down to basic type.
     *  Strips off any "+", "-", "~", "anti-" from an std::string.
     *
     *  Example: mu+ -> mu, Delta++ -> Delta, anti-nu_tau -> nu_tau
     *
     *  @code
     *  std::string myParticleName("tau+");
     *  DaVinci::StringUtils::stripParticleName(myParticleName);
     *  @endcode
     *
     *  @param name (input) string to be stripped.
    */

    void stripParticleName(std::string& name);

    /**
     *
     * Add leading prefix +"/" string to input locations,
     * unless these contain a "/", in which case leave them untouched.
     * <b> Examples: </b> 
     *
     * "MyBsSelection" becomes "Phys/MyBsSelection" if prefix = "Phys"
     *
     * "Phys/MyBsSelection" remains unchanged.
     * "Anything/MyBsSelection" remains unchanged.
     * "/MyBsSelection" remains unchanged, but is most likely an error!
     * 
     *
     * @author Juan Palacios juan.palacios@nikhef.nl
     * @date 2008-10-31
     *
     */
    void expandLocations( std::vector<std::string>::iterator begin,
                          std::vector<std::string>::iterator end,
                          const std::string& prefix                );

    /**
     *
     * Add leading prefix +"/" string to input location,
     * unless these contain a "/", in which case leave them untouched.
     * <b> Examples: </b> 
     *
     * "MyBsSelection" becomes "Phys/MyBsSelection" if prefix = "Phys"
     *
     * "Phys/MyBsSelection" remains unchanged.
     * "Anything/MyBsSelection" remains unchanged.
     * "/MyBsSelection" remains unchanged, but is most likely an error!
     * 
     *
     * @author Juan Palacios juan.palacios@nikhef.nl
     * @date 2008-10-31
     *
     */
    void expandLocation( std::string& location,
                         const std::string& prefix);

    /**
     * @author Juan Palacios juan.palacios@cern.ch
     *
     * Remove end of string starting from substring beginning with <b>ending</b>
     *
     * <b> Examples:</b>
     * 
     * @code
     * std::string loc = "Phys/SomeAlgorithm/Particles"
     * removeEnding(loc, "/Part")
     * @endcode
     * transforms "loc" into "Phys/SomeAlgorithm"
     *
     * @date 2010-06-21
     *
     */
    void removeEnding(std::string& location, const std::string& ending);

  }
}
#endif // KERNEL_DAVINCISTRINGUTILS_H
