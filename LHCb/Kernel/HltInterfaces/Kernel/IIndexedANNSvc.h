#ifndef IINDEXEDANNSVC_H 
#define IINDEXEDANNSVC_H 1

// Include files
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/VectorMap.h"


/** @class ITCKANNSvc ITCKANNSvc.h
 *
 * ANN: Assigned Names and Numbers, for a given index (eg. a TCK)
 *  
 *  functionality:
 *         Interface for obtaining, for a specified index ( which
 *         in this context is a plain, unsigned 32bit number, and
 *         which in practice could be a TCK),
 *         and a 'major' key an (invertable) map of int -> string 
 *
 *  Intended for decoding purposes, where typically a compact int 
 *  representation needs to be 'unpacked' into strings
 *
 *  @author Gerhard Raven
 *  @date   2014-05-29
 */


class IIndexedANNSvc : virtual public extend_interfaces<INamedInterface>  {
public:
  /// Return the interface ID
  DeclareInterfaceID(IIndexedANNSvc,2,0);
  virtual ~IIndexedANNSvc() = default;

  virtual GaudiUtils::VectorMap<unsigned int, Gaudi::StringKey> i2s(unsigned int index, const Gaudi::StringKey& major) const = 0;
};
#endif // IINDEXEDANNSVC_H
