// $Id: IANNSvc.h,v 1.2 2008-01-23 15:06:53 graven Exp $
#ifndef IANNSVC_H 
#define IANNSVC_H 1

// Include files
#include <string>
#include <vector>
#include <utility>
#include "GaudiKernel/INamedInterface.h"
#include "boost/optional.hpp"


/** @class IANNSvc IANNSvc.h
 *
 * ANN: Assigned Names and Numbers
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
 *  @author Gerhard Raven
 *  @date   2007-01-02
 */


class IANNSvc : virtual public INamedInterface  {
public:
  /// Return the interface ID
  static const InterfaceID& interfaceID();
  virtual ~IANNSvc();

  typedef std::string                                 major_key_type;
  typedef std::string                                 minor_key_type;
  typedef int                                         minor_mapped_type;
  typedef std::pair<minor_key_type,minor_mapped_type> minor_value_type;

  virtual boost::optional<int>           asInt(   const major_key_type& major, const std::string& minor) const = 0;
  virtual boost::optional<std::string>   asString(const major_key_type& major, int minor) const = 0;



  virtual bool hasMajor(const major_key_type& major) const = 0;
  virtual std::vector< major_key_type > majors() const = 0;



  virtual std::vector<minor_value_type> items(const std::string& major) const = 0;
};
#endif // IANNSVC_H
