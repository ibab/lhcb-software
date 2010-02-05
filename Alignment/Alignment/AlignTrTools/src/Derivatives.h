
#ifndef ALIGNTRTOOLS_Derivatives_H
#define ALOGNTRTOOLS_Derivatives_H 1

#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"


// from Alignment/AlignmentInterfaces
#include "AlignmentInterfaces/IDerivatives.h"

/** @class Derivatives
 *
 *  Define the x position in the module of the hit of the
 *  track with the module
 *
 *  @author M.Deissenroth
 *  @date   10/10/2006
 */

class Derivatives : public GaudiTool, virtual public IDerivatives {

public:

  Derivatives( const std::string& type,
	       const std::string& name,
	       const IInterface* parent); ///< Standard constructor

virtual ~Derivatives(); ///< Destructor


  StatusCode initialize( const int &, const int &, const std::vector<bool>& );

  StatusCode SetGlobal( const Gaudi::XYZVector &, const Gaudi::XYZVector &,
                        const struct Point,
                        std::vector<double> &, 
			const int &, 
//			const double &, 
			const double & 
			);

  StatusCode SetLocal( std::vector<double> &, 
//			const int &, 
			const double &,
                        const double &  
//			const Gaudi::XYZVector, 
//			const Gaudi::XYZVector
			);




  double GetGlobalDerivative_X();
  double GetGlobalDerivative_Y();
  double GetGlobalDerivative_Z();
  double GetGlobalDerivative_RotZ();


  void SetTrueSlope(double slope[]);
  void SetTrueOrigin(double origin[]);
  void SetModulePos(double modulepos[]);
  void SetModuleRot(double modulerot[]);
  void SetModuleOffset(double moduleoff[]);
  void SetStereo(double);


private:
  bool m_MonteCarlo;
  double m_sign;
  int m_n_elements;
  int m_tr_pars;
  int m_n_dofs;
  std::vector<bool> m_dofs;
  std::vector<double> m_derLC;
  std::vector<double> m_derGB;

  double m_TrackSlope[3]; //array of track slope in x,y,z
  double m_ModulePos[3];  //array of pos.of the detector`s center in x,y,z
  double m_TrackOrigin[3]; //array of track origin in x,y,z
  double m_ModuleRot[3];  //array of rotation angles in xrot,yrot,zrot
  double m_ModuleOffset[3]; //array of offset of module in Dx, Dy, Dz
  double m_globalderiv_X; // value of global derivative delta X
  double m_globalderiv_Y; // value of global derivative delta Y, NO U V !!!
  double m_globalderiv_Z; // value of global derivative delta Z
  double m_globalderiv_RotZ; // value of global derivative rotation alpha
  double m_Stereo;

  
};



#endif // ALIGNTRTOOLS_Derivatives_H
