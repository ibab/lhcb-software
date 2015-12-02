// $Id: IANSvc.h,v 1.1 2008-06-25 14:14:17 graven Exp $
#ifndef IANSVC_H 
#define IANSVC_H 1

// Include files
#include <string>
#include <vector>
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StringKey.h"


/** @class IANSvc IANSvc.h
 *
 * AN: Assigned Names 
 *  
 *  functionality:
 *         Interface for obtaining a list of names
 *
 *         Each list is labeled by a 'major' key,
 *         i.e. for each major key, there exists an individual
 *         list of names.
 *
 *  @author Gerhard Raven
 *  @date   2008-06-25
 */


class IANSvc : virtual public extend_interfaces<INamedInterface>  {
public:
  /// Return the interface ID
  DeclareInterfaceID(IANSvc,3,0);
  virtual ~IANSvc() = default;

  typedef Gaudi::StringKey                                major_key_type;
  typedef Gaudi::StringKey                                minor_key_type;

  /// introspection, i.e. access to what is available: is a major present?
  virtual bool hasMajor(const major_key_type& major) const = 0;
  ///                                                  return list of majors
  virtual std::vector<major_key_type> majors() const = 0;
  ///                                                  return minors for a given major
  virtual std::vector<minor_key_type> keys(const major_key_type& major) const = 0;
};
#endif // IANSVC_H
