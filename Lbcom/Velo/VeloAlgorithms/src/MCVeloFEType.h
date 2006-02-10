// $Id: MCVeloFEType.h,v 1.2 2006-02-10 14:02:49 cattanem Exp $
#ifndef MCVELOFETYPE_H 
#define MCVELOFETYPE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// LHCbKernel
#include "Kernel/IMCVeloFEType.h"            // Interface

#include "Event/MCVeloFE.h"

/** @class MCVeloFEType MCVeloFEType.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-11-16
 */

class MCVeloFEType : public GaudiTool, virtual public IMCVeloFEType {
public: 
  
  /// Standard constructor
  MCVeloFEType( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~MCVeloFEType( ); ///< Destructor

  virtual void FEType(const LHCb::MCVeloFE* fe, int& feType);

protected:

private:

};
#endif // MCVELOFETYPE_H
