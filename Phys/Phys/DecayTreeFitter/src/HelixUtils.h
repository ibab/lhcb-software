#ifndef __VTK_HELIXUTILS_HH__
#define __VTK_HELIXUTILS_HH__

#include <string>
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"

typedef Gaudi::XYZVector BField ;

namespace DecayTreeFitter
{
  
  class VtkHelixUtils
  {
  public:
    enum VertexCoor {in_x=0,in_y,in_z,in_px,in_py,in_pz} ;
    enum HelixCoor  {ex_d0=0,
		     ex_phi0=1, 
		     ex_omega=2,
		     ex_z0=3, 
		     ex_tanDip=4,
		     ex_flt=5} ;
    
    static void vertexFromHelix(const HepVector& helixpar,
				const BField& fieldmap,
				HepVector& vertexpar, int& charge) ;
    
    static void helixFromVertex(const HepVector& vertexpar, int charge,
				const BField& fieldmap,
				HepVector& helixpar, HepMatrix& jacobian) ;

    static void helixFromVertexNumerical(const HepVector& vertexpar, int charge,
					 const BField& fieldmap,
					 HepVector& helixpar, HepMatrix& jacobian) ;
    
    static std::string helixParName(int i) ;
    static std::string vertexParName(int i) ;
    static void printHelixPar(const HepVector& helixpar) ;
    static void printVertexPar(const HepVector& vertexpar, int charge) ;

    static double helixPoca(const HepVector& helixpar1,
			    const HepVector& helixpar2,
			    double& flt1, double& flt2, 
			    Gaudi::XYZPoint& v, bool parallel=false) ; 
    static double helixPoca(const HepVector& helixpar,const Gaudi::XYZPoint& point,
			    double& flt) ;
    static double phidomain(const double phi) ;
  } ;

}


#endif

