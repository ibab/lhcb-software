// $Id: VeloCluster2VeloDigitTool.h,v 1.2 2006-03-06 09:17:45 cattanem Exp $
#ifndef VELOCLUSTER2VELODIGITTOOL_H 
#define VELOCLUSTER2VELODIGITTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Event Model
#include "Event/VeloCluster.h"
#include "Kernel/IVeloCluster2VeloDigitTool.h"            // Interface

/** @class VeloCluster2VeloDigitTool VeloCluster2VeloDigitTool.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-01
 */
class VeloCluster2VeloDigitTool : public GaudiTool, virtual public IVeloCluster2VeloDigitTool {

public: 
  /// Standard constructor
  VeloCluster2VeloDigitTool( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);

  virtual ~VeloCluster2VeloDigitTool( ); ///< Destructor
  virtual SmartRefVector<LHCb::VeloDigit> getDigits(
                                          LHCb::VeloCluster* cluster,
                                          LHCb::VeloDigits* digitCont);

protected:

private:

};
#endif // VELOCLUSTER2VELODIGITTOOL_H
