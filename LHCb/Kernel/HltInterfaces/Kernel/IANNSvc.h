#ifndef IANNSVC_H 
#define IANNSVC_H 1

// Include files
#include <string>
#include <vector>
#include <utility>
#include "Kernel/IANSvc.h"
#include "boost/optional.hpp"
#include "GaudiKernel/VectorMap.h"

/** @class IANNSvc IANNSvc.h
 *
 * ANN: Assigned Names and Numbers
 *   or Assigned Numbers and Names
 *  
 *  functionality:
 *         Interface for obtaining names for numbers,
 *         and numbers given names... similar to the
 *         mapping of IP addresses to names.
 *
 *         The code relies on an invertable relation between
 *         names and numbers, i.e. it represent a 'bijective' 
 *         mapping.
 *
 *         Each mapping is labeled by a 'major' key,
 *         i.e. for each major key, there exists an individual
 *         map from a set of numbers to a set of names, and 
 *         vice-versa.
 *
 *  @author Gerhard Raven
 *  @date   2008-01-02
 */


class IANNSvc : virtual public extend_interfaces<IANSvc>  {
public:
  DeclareInterfaceID(IANNSvc,3,0);
  /// Return the interface ID
  virtual ~IANNSvc() = default;

  typedef IANSvc::major_key_type                      major_key_type;
  typedef IANSvc::minor_key_type                      minor_key_type;
  typedef int                                         minor_mapped_type;
  typedef std::pair<minor_key_type,minor_mapped_type> minor_value_type;

  /// the two main functions: map 'major/minor key' -> minor key, value
  virtual boost::optional<minor_value_type>  value(const major_key_type& major, const std::string& minor) const = 0;
  ///                     and map 'major/minor value' -> minor key, value
  virtual boost::optional<minor_value_type>  value(const major_key_type& major, int minor) const = 0;

  /// introspection, i.e. access to what is available: is a major present?
  virtual bool hasMajor(const major_key_type& major) const = 0;
  ///                                                  return list of majors
  virtual std::vector< major_key_type > majors() const = 0;
  ///                                                  return minors for a given major
  virtual std::vector<minor_value_type> items(const major_key_type& major) const = 0;
  ///                                                  optimal return of map for later use without additional lookups
  virtual GaudiUtils::VectorMap< minor_value_type::first_type, minor_value_type::second_type > item_map(const major_key_type& major) const = 0;
};
#endif // IANNSVC_H
