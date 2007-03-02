
/** @file DeRichHPD.h
 *
 *  Header file for detector description class : DeRichHPD
 *
 *  CVS Log :-
 *  $Id: DeRichHPD.h,v 1.5 2007-03-02 14:26:56 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-09-19
 */

#ifndef RICHDET_DERICHHPD_H
#define RICHDET_DERICHHPD_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// External declarations
extern const CLID CLID_DERichHPD;

/** @namespace DeRichLocation
 *
 *  Namespace for the xml location of the detector elements
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-09-19
 */
namespace DeRichLocation 
{
  /// RichHPD location in transient detector store
  static const std::string& RichHPD = "HPD:";
}


/** @class DeRichHPD DeRichHPD.h
 *
 * Class for generic info about HPDs
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-09-19
 */
class DeRichHPD: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeRichHPD();

  /**
   * Default destructor
   */
  virtual ~DeRichHPD();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const {
    return classID();
  }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Get the point on the centre of the HPD window on the inside surface
   * @return Inside window surface centre point
   */
  Gaudi::XYZPoint windowCentreIn() const
  {
    return geometry()->toGlobal( m_HPDTopIn );
  }

  /**
   * Get the point on the centre of the HPD window on the inside surface
   * @return Inside window surface centre point
   */
  Gaudi::XYZPoint windowCentreInIdeal() const
  {
    return m_HPDTopInIdeal;
  }


  /**
   * Get the point on the centre of the HPD window on the outside surface
   * @return Inside window surface centre point
   */
  Gaudi::XYZPoint windowCentreOut() const
  {
    return geometry()->toGlobal( m_HPDTopOut );
  }

  /**
   * Converts a RichSmartID to a point in global coordinates. The point is
   * given on the inside of the HPD window, on the photocathode.
   *
   */
  StatusCode detectionPoint ( const LHCb::RichSmartID& smartID,
                              Gaudi::XYZPoint& detectPoint       );
  

  /**
   * Converts a RichSmartID to a point on the anode in global coordinates.
   *
   * @return The detection anode point in global coordinates
   */
  Gaudi::XYZPoint detPointOnAnode( const LHCb::RichSmartID& smartID ) const;

private: // functions

  /** Returns the name of this particular HPD
   *  @return HPD name
   */
  inline const std::string& myName() const { return m_name; }

  StatusCode getParameters();    ///< Get parameters from Rich1

  StatusCode updateDemagProperties();

  void init_mm( );
  int number_range( int from, int to );
  int number_mm( void );
  double Delta_Phi(double, const double);
  double mag(double , double);
  double demag(double, double );
  StatusCode fillHpdDemagTableSim( const std::vector<double>& );
  StatusCode fillHpdDemagTableRec( const std::vector<double>& );
  bool demagToCathode_new( Gaudi::XYZPoint& localDetectPoint );
  bool demagToCathode_old( Gaudi::XYZPoint& localDetectPoint );


private: // data

  IDetectorElement* deSiSensor;

  std::string m_name;           ///< The name of this HPD panel
  int m_number;                 ///< HPD number (copy number)
  
  Gaudi::XYZPoint m_HPDTopIn;     ///< The centre of the window on the inner side (local)
  Gaudi::XYZPoint m_HPDTopOut;    ///< The centre of the window on the outter side (local)
  Gaudi::XYZPoint m_HPDTopInIdeal; ///< The centre of the window on the inner side (global)

  double m_winInR;            ///< Inner radius of HPD window
  double m_winInRsq;          ///< Inner radius of HPD window square
  double m_winOutR;            ///< Outter radius of HPD window
  double m_winOutRsq;          ///< Outter radius of HPD window square

  unsigned int m_pixelRows;        /// Number of pixel rows
  unsigned int m_pixelColumns;     /// Number of pixel columns

  /// The active HPD window radius (photocathode coverage)
  double m_activeRadius;
  /// The active HPD window radius Squared
  double m_activeRadiusSq;

  Gaudi::Transform3D m_fromWindowToHPD; ///< window to HPD coord system transform

  double m_pixelSize;      ///< The pixel size on the silicon sensor
  double m_subPixelSize;   ///< The size of the subpixel (Alice mode)
  double m_siliconHalfLengthX;  ///< Si sensor half size in x
  double m_siliconHalfLengthY;  ///< Si sensor half size in y

  /// The demagnification factor of the HPD.  Element [0] is the linear
  /// term, and element[1] the non-linear term for small corrections.
  double m_deMagFactor[2];

  TabulatedProperty m_simDemag;
  TabulatedProperty m_recDemag;
  
  SmartRef<Condition> m_demagCond; 

  bool m_firstUpdate;
  
  int    rgiState[2+55]; 
  bool   m_UseHpdMagDistortions;
  bool   m_UseBFieldTestMap ;
  bool   m_UseRandomBField  ;
  double m_LongitudinalBField ;
  double m_RandomBFieldMinimum ;
  double m_RandomBFieldMaximum ;
  double m_RichHpdQWToSiMaxDist;

};

#endif    // RICHDET_DERICHHPD_H
