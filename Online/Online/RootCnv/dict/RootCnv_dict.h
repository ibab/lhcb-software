#include "RootCnv/RootRefs.h"

#ifdef __POOL_COMPATIBILITY
#include "src/PoolClasses.h"

//typedef Gaudi::RootNTupleDescriptor PoolDbNTupleDescriptor;
struct PoolDbNTupleDescriptor {
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

// Add here addition include files for the dictionary generation
namespace RootCnv {
  struct __Instantiations   {
  };
}
