// $
#ifndef LAGRANGEDIRECTIONFITTER_H
#define LAGRANGEDIRECTIONFITTER_H 1

// Include files
// from CLHEP
//#include "CLHEP/Geometry/Point3D.h"

// Include files from STL
#include <vector>
#include <string>

// Include files from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

// Include files from DaVinciTools
#include "DaVinciTools/IDirectionFitter.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"





/** @class LagrangeDirectionFitter LagrangeDirectionFitter.h 
 * 
 *  @author B.VIAUD
 *
*/

class LagrangeDirectionFitter : public AlgTool, 
                  virtual public IDirectionFitter {

public:

  /// Standard Constructor
  LagrangeDirectionFitter(const std::string& type, const std::string& name, 
            const IInterface* parent);

  virtual ~LagrangeDirectionFitter() { } ///< Destructor

  /// Actual operator function.
  // Performs the fit.

  StatusCode  fitWithDirection(Particle& particle,Vertex& vertex, 
                            Particle& fitParticle,Vertex& fitVtx);

  StatusCode  fitWithDirection(Particle& particle,Vertex& vertex,
                               Particle& fitParticle,Vertex& fitBV, 
                               Vertex& fitPV);

};

#endif // LAGRANGEDIRECTIONFITTER_H






