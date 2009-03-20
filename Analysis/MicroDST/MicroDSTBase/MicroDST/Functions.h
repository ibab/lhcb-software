// $Id: Functions.h,v 1.4 2009-03-20 13:52:24 jpalac Exp $
#ifndef MICRODST_FUNCTIONS_H 
#define MICRODST_FUNCTIONS_H 1

// Include files
#include <iostream>
#include <string>
#include <map>
#include <GaudiKernel/StatusCode.h>

class IInterface;
class DataObject;

/** @namespace MicroDST Functions MicroDST/Functions.h
 *  
 *
 *  Free helper functions
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-28
 */
namespace MicroDST {

  /**
   * Get the TES location of an object.
   * 
   * @param pObject Pointer to he onject on the TES
   * @param it's location, if available, otherwise meaningful error names.
   *
   * @author Ulrich Kerzel
   *
   */
  const std::string objectLocation(const DataObject* pObject);

  /**
   * Remove leading "/Event" from TES locations.
   *
   * @author Juan Palacios juancho@nikhef.nl
   *
   */
  const std::string niceLocationName(const std::string& location);

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  StatusCode synchroniseProperty(const IInterface* parent,
                                 IInterface* daughter,
                                 const std::string& name  );
  
  /**
   *
   * Return the trunk of a location XX/YY/ZZ
   * Used to go from Phys/Selection/Particles to Phys/Selection
   * @param location the location XX/YY/ZZ
   * @return the top location, XX/YY
   * 
   * @date 20-03-2009
   * @author Juan Palacios juancho@nikhef.nl
   **/
  const std::string trunkLocation(const std::string& location);  

}
#endif // MICRODST_FUNCTIONS_H
