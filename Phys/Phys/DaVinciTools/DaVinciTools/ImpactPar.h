// $Id: ImpactPar.h,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#ifndef DAVINCITOOLS_IMPACTPARTOOL_H
#define DAVINCITOOLS_IMPACTPARTOOL_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "GaudiKernel/AlgTool.h"

// Forward declarations
class AxPartCandidate;
class VtxCandidate;

//--------------------------------------------------------------------
//
//  ClassName  : ImpactPar
// 
//  Description: Calculate impact parameter of a "track" with respect
//               to a "vertex": take AxPartCandidate and MyAxVertex as
//               input arguments.
//               
//
//  Author     : G.Corti  
//
//--------------------------------------------------------------------

class ImpactPar : public AlgTool {

public:

  // Standard Constructor
  ImpactPar(const std::string& type, const std::string& name, 
            const IInterface* parent);
  

  // Standard Destructor
  virtual ~ImpactPar() { }

  // Actual operator function
  double calculate( AxPartCandidate* candidate, VtxCandidate* vertex ); 

};

#endif // DAVINCITOOLS_IMPACTPARTOOL_H
