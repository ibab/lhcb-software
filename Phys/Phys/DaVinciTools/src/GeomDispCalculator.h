// $Id: GeomDispCalculator.h,v 1.4 2003-06-26 16:51:51 gcorti Exp $
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
class IParticleTransporter;

/** @class GeomDispCalculator GeomDispCalculator.h 
 *  Geometrical displacements calculator. 
 *
 * @author Miriam Gandelman
 * @date 20/03/2002 
*/

class GeomDispCalculator : public AlgTool,
                   virtual public IGeomDispCalculator {

public:

  /// Standard Constructor
  GeomDispCalculator( const std::string& type, const std::string& name, 
              const IInterface* parent);

  /// Standard Destructor
  virtual ~GeomDispCalculator() { }

  /// Retrieve the necessary tools
  StatusCode initialize();    

  /// Release the tools
  StatusCode finalize();    

  /// Calculates the Impact Parameter and its error. 
  /// Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const Particle& particle,
                            const Vertex& vertex, double& ip, double& ipErr );

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const Particle& particle,
                            const Vertex& vertex, double& ip, double& ipErr,
                            Hep3Vector& ipVector, HepSymMatrix& errMatrix );

  /// Calculates the Impact Parameter and its error.
  /// Inputs: a particle and a space point.
  StatusCode calcImpactPar( const Particle& particle,
                            HepPoint3D& point, double& ip , double& ipErr );

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a space point.
  StatusCode calcImpactPar( const Particle& particle,
                            HepPoint3D& point, double& ip , double& ipErr,
                            Hep3Vector& ipVector, HepSymMatrix& errMatrix );
  /// Calculates the distance of closest approach between two particles
  /// and its error.
  StatusCode calcCloseAppr( const Particle& particle1,
       
                     const Particle& particle2, double& dist, 
                            double& distErr );

  /// Calculates the distance between two vertices and its error.
  StatusCode calcVertexDis( const Vertex& vertex1,
                            const Vertex& vertex2, double& dist, 
                            double& distErr );

private:
  IParticleTransporter* m_pTransporter;  ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      

};

#endif // GEOMDISPCALCULATOR_H
