// $Id: LagrangeMassVertexFitter.h,v 1.1, 2002/03/08, modified from gcorti 
// Exp`s program $
#ifndef LAGRANGEMASSVERTEXFITTER_H
#define LAGRANGEMASSVERTEXFITTER_H 1

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
#include "DaVinciTools/IMassVertexFitter.h"
#include "DaVinciTools/IVertexFitter.h"
#include "DaVinciTools/IParticleTransporter.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"

// Forward declarations
class IParticlePropertySvc;

// Forward declarations
class IVertexFitter;
class IParticleTransporter;


/** @class LagrangeMassVertexFitter LagrangeMassVertexFitter.h 
 *  
 * A kinematical constrained fit using Lagrange multipliers method
 * for 2 or 3 tracks. Invariant mass and geometrical vertex constraint.
 * Adapted from Juan Saborido`s program ( written in fortran ). 
 * See the LHCb internal note LHCb/98-051 PHYS. 
 *
 * @author J.  de Mello 
 * @author C. Nunes.  
 *
 * @date 08/03/2002
 *
*/

class LagrangeMassVertexFitter : public AlgTool,
				 virtual public IMassVertexFitter {

public:

  /// Standard Constructor
  LagrangeMassVertexFitter( const std::string& type, const std::string& name, 
const IInterface* parent);

  /// Standard Destructor
  virtual ~LagrangeMassVertexFitter() { }

  /// Actual operator function
  StatusCode
    lagrangeFitter(const std::string& motherName, 
                   const ParticleVector&  pList, Vertex& constrVtx,
                   Particle& motherParticle);   

 private:

  // number of tracks ( = ntracks ) must be 2 or 3.
  int ntracks;

  // private methods used in lagrangeFitter.
  StatusCode itera(HepVector &e, HepSymMatrix &Ce, HepVector &mass,
                   double massConstr, int dimCe, bool final );
  StatusCode nwcov(HepVector &e, HepSymMatrix &Ce, double zcer,
                   HepVector &tpf, HepSymMatrix &Cx, int dimCe );
  StatusCode evalu(HepVector &e, HepVector &mass,
		   double& massConstrCalc,double diffZver);
  StatusCode chisq(HepVector &e0,HepVector &e, HepSymMatrix &Ce,
                   double &chis);
  
 
  // The Particle Property Service is set internally at creation. 
  IParticlePropertySvc* m_ppSvc;

  // The tool service
  IToolSvc* m_pToolSvc;
  IVertexFitter* m_pVertexUnconst; 
  IParticleTransporter* m_pTransporter;
};

#endif // LAGRANGEMASSVERTEXFITTER_H

