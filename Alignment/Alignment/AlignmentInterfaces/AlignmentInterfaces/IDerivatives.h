#ifndef IDERIVATIVES_H
#define IDERIVATIVES_H 1

#include "GaudiKernel/IAlgTool.h"

// from LHCb/Kernel:
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

//MD due to struct def
#include "ITAConfigTool.h"
/** @class Derivatives 
 *
 *  Interface to the Derivatives tool
 *  
 *
 *  @author M.Deissenroth, J. Blouw
 *  @date   10/10/2006
 */


static const InterfaceID IID_IDerivatives( "IDerivatives", 1, 0);
//Forward Declaration
class IDerivatives : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IDerivatives;}

  virtual StatusCode initialize( const int &, const int &, const std::vector<bool>& ) = 0;

  virtual StatusCode SetGlobal( const Gaudi::XYZVector &, /// slope of track 
                                const Gaudi::XYZVector &, /// origin of track 
                                const struct Point,
                                std::vector<double> &, 
				const int &, 
//				const double &, 
				const double & ) = 0;
  
  virtual StatusCode SetLocal( std::vector<double> &, 
//				const int &, 
				const double &, 
				const double & 
//                                const Gaudi::XYZVector, 
//				const Gaudi::XYZVector 
				) = 0;

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
}; 



#endif // IDERIVATIVES_H
