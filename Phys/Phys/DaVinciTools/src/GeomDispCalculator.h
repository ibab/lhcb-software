// $Id: GeomDispCalculator.h,v 1.1 2002-03-27 20:35:00 gcorti Exp $
#ifndef GEOMDISPCALCULATOR_H
#define GEOMDISPCALCULATOR_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "CLHEP/Vector/LorentzVector.h"

// from DaVinciTools
#include "DaVinciTools/IGeomDispCalculator.h"

// Forward declarations

/** @class GeomDispCalculator GeomDispCalculator.h 
    Geometrical displacements calculator interface. 
  
   @author Miriam Gandelman
   @date 20/03/2002 
*/

class GeomDispCalculator : public AlgTool,
                   virtual public IGeomDispCalculator {

public:

  /// Standard Constructor
  GeomDispCalculator( const std::string& type, const std::string& name, 
              const IInterface* parent);

  /// Standard Destructor
  virtual ~GeomDispCalculator() { }

  /// Actual operator function 

  void calcImpactPar( const Particle& particle,
                            const Vertex& vertex, double& ip, double& ipErr );
  void calcImpactPar( const Particle& particle,
                            const Vertex& vertex, double& ip, double& ipErr,
                            Hep3Vector& ipVector, HepSymMatrix& errMatrix );
  void calcImpactPar( const Particle& particle,
                            HepPoint3D& point, double& ip , double& ipErr );
  void calcImpactPar( const Particle& particle,
                            HepPoint3D& point, double& ip , double& ipErr,
                            Hep3Vector& ipVector, HepSymMatrix& errMatrix );
  void calcCloseAppr( const Particle& particle1,
                            const Particle& particle2, double& dist, 
                            double& distErr );
  void calcVertexDis( const Vertex& vertex1,
                            const Vertex& vertex2, double& dist, 
                            double& distErr );
private:
  
};

#endif // GEOMDISPCALCULATOR_H
