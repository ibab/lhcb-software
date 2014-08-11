#include "RootCnv/RootRefs.h"

#ifdef __POOL_COMPATIBILITY
#include "RootCnv/PoolClasses.h"
#if 0
//typedef Gaudi::RootNTupleDescriptor PoolDbNTupleDescriptor;
class PoolDbNTupleDescriptor {
 public:
  /// Description string
  std::string   description;
  /// Optional description
  std::string   optional;
  /// Identifier of description
  std::string   container;
  /// Class ID of the described object
  unsigned long clid;
  /// Standard constructor
  PoolDbNTupleDescriptor() {}
  /// Standard destructor
  virtual ~PoolDbNTupleDescriptor() {}
};
#endif
#endif

// Add here addition include files for the dictionary generation
namespace RootCnv {
  struct __Instantiations   {
  };
}
