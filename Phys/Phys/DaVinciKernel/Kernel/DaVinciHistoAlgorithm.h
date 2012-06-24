#ifndef KERNEL_DaVinciHistoAlgorithm_H 
#define KERNEL_DaVinciHistoAlgorithm_H 1

// Local base class
#include "Kernel/DVCommonBase.h"

// Gaudi base class
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class DaVinciHistoAlgorithm Kernel/DaVinciHistoAlgorithm.h
 *  
 *  DaVinci Algorithm with histogramming
 *  
 *  @author Chris Jones
 *  @date   2012-06-22
 */
class DaVinciHistoAlgorithm : public DVCommonBase<GaudiHistoAlg>
{
  
public: 
  
  /// Standard constructor
  DaVinciHistoAlgorithm( const std::string& name,
                         ISvcLocator* pSvcLocator )
    : DVCommonBase<GaudiHistoAlg> ( name, pSvcLocator ) { }
  
};

#endif // KERNEL_DaVinciHistoAlgorithm_H
