#ifndef IDERIVATIVES_H
#define IDERIVATIVES_H 1

// from std
#include <vector>

#include "GaudiKernel/IAlgTool.h"

// from LHCb/Kernel:
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

/** @class Derivatives 
 *
 *  Interface to the Derivatives tool
 *  
 *
 *  @author F. MACIUC, M.Deissenroth, J. Blouw
 *  @date   10/10/2006
 */


static const InterfaceID IID_IDerivatives( "IDerivatives", 1, 0);

class IDerivatives : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IDerivatives;}

  virtual StatusCode initialize( int, int, const std::vector<bool>& ) = 0;

//  virtual StatusCode SetGlobal( const Gaudi::XYZVector &, /// slope of track 
  virtual StatusCode SetGlobal( const Gaudi::XYZVector &, /// slope of track 
                                const Gaudi::XYZVector &, /// origin of track 
                                const Gaudi::Transform3D &, /// matrix representing shifts & rotations
                                std::vector<double> &, int, double, double ) = 0;
 
  virtual StatusCode SetLocal( std::vector<double> &, /*int,*/ double, /*double,*/ double ) = 0;

  virtual double GetGlobalDerivative_X() = 0;
  virtual double GetGlobalDerivative_Y() = 0;
  virtual double GetGlobalDerivative_Z() = 0;
  virtual double GetGlobalDerivative_RotZ() = 0;
  
  virtual void SetTrueSlope(double slope[]) = 0;
  virtual void SetTrueOrigin(double origin[]) = 0;
  virtual void SetModulePos(double modulepos[]) = 0;
  virtual void SetModuleRot(double modulerot[]) = 0;
  virtual void SetModuleOffset(double moduleoff[]) = 0;
  virtual void SetStereo(double) = 0;



  virtual bool SetGlobalN( const std::vector<double> &,
			   const std::vector<double> &,  
			   const Gaudi::XYZPoint &,  
			   const Gaudi::XYZPoint &,
			   std::vector<double> &, 
			   int, 
			   const Gaudi::XYZVector &, 
			   const Gaudi::XYZVector & ) = 0;

  virtual bool SetLocalN( std::vector<double> &, 
        // int, 
			  const std::vector<double> &, 
			  const std::vector<double> &, 
			  const Gaudi::XYZPoint &, 
			  const Gaudi::XYZPoint &, 
			  const Gaudi::XYZVector &, 
				const Gaudi::XYZVector & ) = 0;

  virtual  double Model( const std::vector<double> &, 
			 const std::vector<double> &,  
			 const Gaudi::XYZVector &, 
			 const Gaudi::XYZVector &, 
			 const Gaudi::XYZVector &,
			 const Gaudi::XYZVector & ) = 0;

}; 



#endif // IDERIVATIVES_H
