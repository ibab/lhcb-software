#ifndef KERNEL_DAVINCIALGORITHM_H 
#define KERNEL_DAVINCIALGORITHM_H 1

// Local base class
#include "Kernel/DVCommonBase.h"

// Gaudi base class
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class DaVinciAlgorithm Kernel/DaVinciAlgorithm.h
 *  
 *  DaVinci Algorithm
 *  
 *  @author Chris Jones
 *  @date   2012-06-22
 */
class DaVinciAlgorithm : public DVCommonBase<GaudiAlgorithm>
{
  
public: 
  
  /// Standard constructor
  DaVinciAlgorithm( const std::string& name,
                    ISvcLocator* pSvcLocator )
    : DVCommonBase<GaudiAlgorithm> ( name, pSvcLocator ) { }

};

#endif // KERNEL_DAVINCIALGORITHM_H
