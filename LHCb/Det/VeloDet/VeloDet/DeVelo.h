// $Id: DeVelo.h,v 1.1 2001-10-25 06:13:26 ocallot Exp $
#ifndef       VELODET_DEVELO_H
#define       VELODET_DEVELO_H 1
// ============================================================================
/// from STL
#include <iostream>
#include <vector>

/// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

/// GaudiKernel
#include "GaudiKernel/MsgStream.h"

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"

/// from Det/VeloDet
#include "VeloDet/CLIDDeVelo.h"

/// forwad declarations
class MsgStream;


/**  
 * Auxilliary class to handle each wafer.
 */

class VeloWafer {
  
public:
  VeloWafer() {};
  ~VeloWafer() {};

  double z()                    const { return m_z;      }
  void   setZ( double z )             { m_z = z;         }
  
  int    type()                 const { return m_type;   }
  void   setType( int type )          { m_type = type;   }
  
  int    number()               const { return m_number; }
  void   setNumber( int num )         { m_number = num;  }
  
private:
  int m_number;   ///< station number
  int m_type;     ///< sector type: 0 = R, 1 = Phi droite, 2 = Phi gauche
  double m_z;     ///< Z position of silicium center
};



/** @class DeVelo DeVelo.h "VeloDet/DeVelo.h" 
 *
 *  Velo detector element class.
 *
 *  @author Olivier Callot Olivier.Callot@cern.ch
 */

class DeVelo: public DetectorElement {

public:
  
  /// Constructors
  DeVelo( const std::string& name    = "" );
  
  /// (dirtual) Destructor
  virtual ~DeVelo() ;
  
  /// object identification
  static  const CLID& classID ()       { return CLID_DeVelo ; } 
  /// object identification
  virtual const CLID& clID    () const ;
  
  /// initialization method 
  virtual StatusCode initialize(); 

  /// return the wafer number for a point
  int waferNumber( HepPoint3D& point );

  /// return the number of wafers
  int nbWafer()  const { return m_wafer.size(); };
  
  /// return the (floating) strip number for this wafer;
  double stripNumber( unsigned int waferNumber, 
                      HepPoint3D& point, 
                      double& pitch );

  /// return the space point and sigma for a given pair of strips.

  bool getSpacePoint( unsigned int RWaferNumber, 
                      double       RStripNumber,
                      unsigned int PhiWaferNumber, 
                      double       PhiStripNumber,
                      HepPoint3D& point, 
                      double&  rPitch,
                      double&  phiPitch );
    
protected: 

private:
  
  double m_innerRadius;
  double m_outerRadius;
  // R detector
  double m_fixPitchRadius;
  double m_middleRRadius;
  double m_innerPitch;
  double m_outerPitch;
  double m_nbRInner;
  
  // Phi detector
  double m_phiBoundRadius;
  double m_innerTilt;
  double m_outerTilt;
  double m_phiOrigin;
  double m_nbPhiInner;
  
  // auxilliary variables

  double m_pitchSlope;
  double m_logPitchStep;
  double m_nbRFixedPitch;
  double m_halfAngle;
  double m_quarterAngle;
    
  double m_innerTiltRadius;
  double m_outerTiltRadius;
  double m_phiAtBoundary;
  double m_phiPitchInner;
  double m_phiPitchOuter;
  double m_phiInnerTilt;
  double m_phiOuterTilt;

  // Local storage for geometry computation
  std::vector<VeloWafer*> m_wafer;

};

// ============================================================================
#endif  //    VELODET_DEVELO_H
// ============================================================================
