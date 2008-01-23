// $Id: IANNSvc.h,v 1.1 2008-01-23 13:11:53 graven Exp $
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

  virtual boost::optional<int>           asInt(   const std::string& major, const std::string& minor) const = 0;
  virtual boost::optional<std::string>   asString(const std::string& major, int minor) const = 0;

  virtual bool hasMajor(const std::string& major) const = 0;
  virtual std::vector< std::string > majors() const = 0;

  virtual std::vector< std::pair<std::string,int> > items(const std::string& major) const = 0;
};
#endif // IANNSVC_H
