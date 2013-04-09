
//=============================================================================
/** @file DeRichHPD.h
 *
 *  Header file for detector description class : DeRichHPD
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-09-19
 */
//=============================================================================

#ifndef RICHDET_DERICHHPD_H
#define RICHDET_DERICHHPD_H 1

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IPVolume.h"

// LHCbKernel
#include "Kernel/ILHCbMagnetSvc.h"

// RichDet
#include "RichDet/DeRichPD.h"

// GSL
#include "gsl/gsl_math.h"

// STL
#include <cmath>

// External declarations
extern const CLID CLID_DERichHPD;

//=============================================================================
/** @class DeRichHPD RichDet/DeRichHPD.h
 *
 * Class for generic info about HPDs
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-09-19
 */
//=============================================================================
class DeRichHPD : public DeRichPD
{

public:
  /**
   * Constructor for this class
   */
  explicit DeRichHPD( const std::string & name = "" );

  /**
   * Default destructor
   */
  virtual ~DeRichHPD();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const { return classID(); }

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

public:

  /**
   * Get the point on the centre of the HPD window on the inside surface
   * @return Inside window surface centre point
   */
  Gaudi::XYZPoint windowCentreIn() const
  {
    return geometry()->toGlobal(m_pvWindow->toMother(Gaudi::XYZPoint(0,0,m_winInR)) );
  }

  /**
   * Get the point on the centre of the HPD window on the inside surface without misalignment
   * @return Inside window surface centre point without misalignment
   */
  Gaudi::XYZPoint windowCentreInIdeal() const
  {
    return ( geometry()->toGlobalMatrixNominal() *
             m_pvWindow->toMother(Gaudi::XYZPoint(0,0,m_winInR)) );
  }

  /** Get the point on the centre of the HPD window on the inside surface in the mother
   *  (panel) coodinate system
   *  @return Inside window surface centre point in panel system
   */
  inline const Gaudi::XYZPoint hpdWinInsideCentreInMother() const
  {
    return m_windowInsideCentreMother;
  }

  /**
   * Get the point on the centre of the HPD window on the outside surface
   * @return Inside window surface centre point
   */
  Gaudi::XYZPoint windowCentreOut() const
  {
    return geometry()->toGlobal( m_pvWindow->toMother(Gaudi::XYZPoint(0,0,m_winOutR)) );
  }

  // @brief Converts a RichSmartID to a point in global coordinates.
  virtual StatusCode detectionPoint ( const LHCb::RichSmartID smartID,
                                      Gaudi::XYZPoint& detectPoint,
                                      bool photoCathodeSide = false ) const;


  // Converts an x,y point from the anode to the photocathode in the
  // coordinate system of the HPD.
  virtual StatusCode detectionPoint ( const double fracPixelCol,
                                      const double fracPixelRow,
                                      Gaudi::XYZPoint& detectPoint,
                                      const bool photoCathodeSide = true ) const;

  /** Converts a RichSmartID to a point on the anode in global coordinates.
   *  @param[in] smartID The RichSmartID for the HPD channel
   *  @return The detection point on the anode in global coordinates
   */
  Gaudi::XYZPoint detPointOnAnode( const LHCb::RichSmartID smartID ) const;

  /** Retrieves the demagnification interpolation function for the HPD R coordinate.
   *  For a given R on the HPD cathode returns the R on the anode.
   *  @return A pointer to the demagnification function for R(R)
   */
  inline const Rich::TabulatedFunction1D* demagnification_RtoR( const int field = 0 ) const
  {
    return m_demagMapR[ field > 0 ? 1 : 0 ];
  }

  /** Retrieves the demagnification interpolation function for the HPD phi coordinate.
   *  For a given R on the HPD cathode returns the phi on the anode.
   *  @return A pointer to the demagnification function for phi(R)
   */
  inline const Rich::TabulatedFunction1D* demagnification_RtoPhi( const int field = 0 ) const
  {
    return m_demagMapPhi[ field > 0 ? 1 : 0 ];
  }

  /** Retrieves the magnification interpolation function for the HPD R coordinate.
   *  For a given R on the HPD anode returns the R on the cathode.
   *  @return A pointer to the magnification function for R(R)
   */
  inline const Rich::TabulatedFunction1D* magnification_RtoR( const int field = 0 ) const
  {
    return m_magMapR[ field > 0 ? 1 : 0 ];
  }

  /** Retrieves the magnification interpolation function for the HPD phi coordinate.
   *  For a given R on the HPD anode returns the phi on the cathode.
   *  @return A pointer to the magnification function for phi(R)
   */
  inline const Rich::TabulatedFunction1D* magnification_RtoPhi( const int field = 0 ) const
  {
    return m_magMapPhi[ field > 0 ? 1 : 0 ];
  }

  /** Converts the given RichSmartID to the position on the silicon wafer,
   *  in the coordinate system of the wafer
   *  @param[in] smartID The RichSmartID channel identifier
   *  @return The position on the wafer
   */
  inline Gaudi::XYZPoint pointOnSilicon ( const LHCb::RichSmartID smartID ) const
  {
    return Gaudi::XYZPoint( xOnSilicon(smartID), yOnSilicon(smartID), 0.0 );
  }

  /** Get the tranformation from the HPD panel coordinate system to the HPD quartz
   *  @return The HPD panel to the HPD quartz tranformation
   */
  inline const Gaudi::Transform3D& fromPanelToHPDWindow ( ) const
  {
    return m_fromPanelToWindow;
  }

  /** Get the tranformation from the HPD quartz window to the HPD
   *  @return The HPD window to HPD tranformation
   */
  inline const Gaudi::Transform3D& fromHPDWindowToHPD ( ) const
  {
    return m_fromWindowToHPD;
  }

  /** Get the tranformation from the HPD quartz window to the global LHCb coordinate system
   *  @return The HPD window to LHCb global tranformation
   */
  inline const Gaudi::Transform3D& fromHPDWindowToGlobal ( ) const
  {
    return m_fromWindowToGlobal;
  }

  /** Get the tranformation from the HPD coordinate system to the panel coordinate
   *  system including misalignment
   *  @return The HPD to panel tranformation
   */
  inline const Gaudi::Transform3D& fromHPDToPanel ( ) const
  {
    return m_fromHPDToPanel;
  }

  /** Get the solid for the HPD window (for intersection purposes)
   *  @return The quartz window solid
   */
  inline const ISolid* HPDWindowSolid ( ) const
  {
    return m_windowSolid;
  }

  /** Test for shadowing by the kapton shield of the given DeRichHPD
   *  @param[in] pInPanel Position in the HPD panel coordinate system
   *  @param[in] vInPanel Direction vedctor in the HPD panel coordinate system
   *  @return boolean indicating if the given direction intersects the HPD kapton shield
   *  @retval TRUE  Does intersect
   *  @retval FALSE Does not intersect
   */
  bool testKaptonShadowing( const Gaudi::XYZPoint&  pInPanel,
                            const Gaudi::XYZVector& vInPanel ) const;

private: // functions

  /// Clean up interpolators
  void cleanUpInterps();

  /** Returns the silicon x coordinate for the given RichSmartID
   *  @param smartID The RichSmartID
   *  @return The x coordinate
   */
  inline double xOnSilicon( const LHCb::RichSmartID smartID ) const
  {
    return smartID.pixelCol()*m_pixelSize + m_pixelSize/2.0 - m_siliconHalfLengthX;
  }

  /** Returns the silicon y coordinate for the given RichSmartID
   *  @param smartID The RichSmartID
   *  @return The y coordinate
   */
  inline double yOnSilicon( const LHCb::RichSmartID smartID ) const
  {
    return m_siliconHalfLengthY - smartID.pixelRow()*m_pixelSize - m_pixelSize/2.0;
  }

  /// parameterised extra radius for the defraction in the HPD window
  inline double extraRadiusForRefraction( const double r ) const
  {
    const double  rr = r*r;
    const double rrr = r*rr;
    return ( m_refactParams[3]*rrr +
             m_refactParams[2]*rr  +
             m_refactParams[1]*r   +
             m_refactParams[0]     );
  }

  /// Get parameters from Rich1
  StatusCode getParameters();

  /// Update the cached parameters based on geometry
  StatusCode updateGeometry();

  /// Update the magnification and demagnification information
  StatusCode updateDemagProperties();

  /// go from a point on silicon to a point on the photo-cathode with magnet ON
  StatusCode magnifyToGlobalMagnetON( Gaudi::XYZPoint& detectPoint, 
                                      const bool photoCathodeSide ) const;

  /// go from a point on silicon to a point on the photo-cathode with magnet OFF
  StatusCode magnifyToGlobalMagnetOFF( Gaudi::XYZPoint& detectPoint, 
                                       const bool photoCathodeSide ) const;

  /// Initialise the interpolators for demagnification (cathode to anode)
  StatusCode fillHpdDemagTable( const unsigned int field );

  /// Initialise the interpolators for magnification (anode to cathode)
  StatusCode fillHpdMagTable( const unsigned int field );

  /// Initialise the HPD quantum eff function
  StatusCode initHpdQuantumEff();

  /// Access magnetic field service on demand
  inline ILHCbMagnetSvc * magSvc() const
  {
    if ( !m_magFieldSvc ) { loadMagSvc(); }
    return m_magFieldSvc;
  }

  /// Load the Magnetic field service when required
  void loadMagSvc() const;

  /** It returns the rotation angle \Delta\phi [rad] as a function of the
   * radial entrance
   * window coordinate r [mm] and the axial magnetic flux density B [G].
   * first calculate the coefficients a, b, c from the
   * value of the axial magnetic flux density
   * these are called \Delta\phi_0, \Delta\phi_2, \Delta\phi_3 in the
   * referenced thesis at the beginning
   */
  double Delta_Phi( const double r, const double B );

  /** function mag returns the r coordinate given rho and B as arguments
   * coefficients of the fits for the dependence of the mag law
   * from the magnetic field
   */
  double mag( const double rho, const double B );

  /**
   * Radial mapping function and rotation function
   * from Gianluca Aglieri Rinella and Ann Van Lysebetten
   * for the description and correction of the magnetic distortions
   * induced in the axially symmetric case on the HPD detector.
   * See for explanation and reference
   * CERN-THESIS-2005-060 on pages 92-93
   * http://cdsweb.cern.ch/search.py?recid=916449
   *
   * function demag returns the rho coordinate given r and B as arguments
   * r [mm] is the radial coordinate of the led on the entrance window
   * B [G] is the magnetic flux density field (axial)
   * rho [mm] is the radial coordinate on the anode plane from the centre of
   * the cathode image
   */
  double demag( const double r, const double B );

private: // data

  IDetectorElement* m_deSiSensor; ///< The silicon sensor detector element

  const IPVolume* m_pvWindow;      ///< The pv for the HPD quartz window
  const ISolid* m_windowSolid;     ///< The HPD window solid

  const ISolid* m_kaptonSolid;     ///< Pointer to the kapton solid

  int m_number;                    ///< HPD number (should be the same as copy number)

  double m_winInR;            ///< Inner radius of HPD window squared
  double m_winOutR;           ///< Outter radius of HPD window squared
  double m_winInRsq;          ///< Inner radius of HPD window squared
  double m_winOutRsq;         ///< Outter radius of HPD window squared

  /// The active HPD window radius (photocathode coverage)
  double m_activeRadius;

  double m_pixelSize;           ///< The pixel size on the silicon sensor
  double m_siliconHalfLengthX;  ///< Si sensor half size in x
  double m_siliconHalfLengthY;  ///< Si sensor half size in y

  /// The demagnification factor of the HPD. Element [0] is the linear
  /// term, and element[1] the non-linear term for small corrections.
  double m_deMagFactor[2];
  double m_magnificationCoef1;
  double m_magnificationCoef2;

  /// Interpolated function for HPD R for demagnification
  std::vector<Rich::TabulatedFunction1D*> m_demagMapR;
  /// Interpolated function for HPD phi for demagnification
  std::vector<Rich::TabulatedFunction1D*> m_demagMapPhi;
  /// Interpolated function for HPD R for magnification
  std::vector<Rich::TabulatedFunction1D*> m_magMapR;
  /// Interpolated function for HPD phi for magnification
  std::vector<Rich::TabulatedFunction1D*> m_magMapPhi;

  /// Demagnification parameters condition
  std::vector< SmartRef<Condition> > m_demagConds;

  /// Flag indicating if this HPD owns its HPD QE function
  mutable bool m_ownHPDQEFunc;

  std::vector<double> m_refactParams; ///< refraction parameters for quartz window

  /// Flag to indicate the full treatment of magnetic distortions should be performed
  bool m_UseHpdMagDistortions;

  /// Turns on the use of a test field map
  bool m_UseBFieldTestMap;

  /// magnitude of the longitudinal B field
  double m_LongitudinalBField;

  /// version of MDMS corrections, for both field polarities
  int m_MDMS_version[2];

  // Cached parameters for speed reasons.
  Gaudi::Transform3D m_SiSensorToHPDMatrix; ///< silicon to HPD transform
  Gaudi::Transform3D m_fromWindowToGlobal;  ///< window to global coord system transform
  Gaudi::Transform3D m_fromPanelToWindow;   ///< HPD panel to HPD window coord system transform
  Gaudi::Transform3D m_fromPanelToKapton;   ///< HPD panel to kapton coord system transform
  Gaudi::Transform3D m_fromWindowToHPD;     ///< window to global coord system transform
  Gaudi::Transform3D m_fromHPDToPanel;      ///< HPD to HPD Panel transform

  /// The centre of the HPD window (inside) in the mother (panel) coordinate system
  Gaudi::XYZPoint m_windowInsideCentreMother;

  /// Rotional centre for MDMS corrections
  Gaudi::XYZVector m_MDMSRotCentre;

  /// pointer to the magnetic field service
  mutable ILHCbMagnetSvc * m_magFieldSvc;

};

//=========================================================================
//  convert a RichSmartID to a point on the anode (global coord system)
//=========================================================================
inline Gaudi::XYZPoint
DeRichHPD::detPointOnAnode( const LHCb::RichSmartID smartID ) const
{
  return ( m_deSiSensor->geometry()->toGlobal( pointOnSilicon(smartID) ) );
}

#endif    // RICHDET_DERICHHPD_H
