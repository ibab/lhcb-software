///@{
/** GaudiKernel includes */
#include "GaudiKernel/ISvcFactory.h"
///@}

/// ===========================================================================
/** @file SimSvc_load.cpp 
 * 
 *  Implementation of <Package>_load routine. This routine 
 *  is needed for forcing the linker to load all the components
 *  of the library.. 
 */
/// ===========================================================================


#define DLL_DECL_SERVICE(x)       extern const ISvcFactory& x##Factory; \
                                  x##Factory.addRef();

void SimSvc_load() 
{
  
  ///@{
  /** Declare services */ 
  DLL_DECL_SERVICE (SimulationSvc);
  ///@}

};


extern "C" void SimSvc_loadRef() { SimSvc_load(); }







