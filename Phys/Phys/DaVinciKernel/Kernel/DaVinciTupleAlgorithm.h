#ifndef KERNEL_DaVinciTupleAlgorithm_H 
#define KERNEL_DaVinciTupleAlgorithm_H 1

// Local base class
#include "Kernel/DVCommonBase.h"

// Gaudi base class
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class DaVinciAlgorithm Kernel/DaVinciAlgorithm.h
 *  
 *  DaVinci Algorithm with histogramming and tupling
 *  
 *  @author Chris Jones
 *  @date   2012-06-22
 */
class DaVinciTupleAlgorithm : public DVCommonBase<GaudiTupleAlg>
{
  
public: 
  
  /// Standard constructor
  DaVinciTupleAlgorithm( const std::string& name,
                         ISvcLocator* pSvcLocator )
    : DVCommonBase<GaudiTupleAlg> ( name, pSvcLocator ) { }
  
};

#endif // KERNEL_DaVinciTupleAlgorithm_H
