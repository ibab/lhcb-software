// $Id: ImpactPar.h,v 1.1 2002-02-08 18:25:42 gcorti Exp $
#ifndef IMPACTPARTOOL_H
#define IMPACTPARTOOL_H 1

// Include files
// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IImpactPar.h"

/** @class ImpactPar ImpactPar.h 
 *  Impact parameter Algorithm Tool
 *  Calculate impact parameter of a "track" with respect
 *  to a "vertex": take AxPartCandidate and MyAxVertex as
 *  input arguments.
 * 
 *  @author G. Corti
 *
*/

class ImpactPar : public AlgTool, 
                  virtual public IImpactPar {

public:

  /// Standard Constructor
  ImpactPar(const std::string& type, const std::string& name, 
            const IInterface* parent);

  virtual ~ImpactPar() { } ///< Destructor

  /// Actual operator function
  double calculate( AxPartCandidate* candidate, VtxCandidate* vertex ); 

};

#endif // IMPACTPARTOOL_H
