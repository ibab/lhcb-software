
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
#include "Kernel/RichSmartID.h"
#include "Kernel/ILHCbMagnetSvc.h"

// RichDet
#include "RichDet/DeRichBase.h"
#include "RichDet/Rich1DTabFunc.h"
#include "RichDet/Rich1DTabProperty.h"

// GSL
#include "gsl/gsl_math.h"

#include <bitset>

class ILHCbMagnetSvc;
// External declarations
extern const CLID CLID_DERichHPD;

/** @class DeRichHPD RichDet/DeRichHPD.h
 *
 * Class for generic info about HPDs
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-09-19
 */
class DeRichHPD: public DeRichBase
{

public:
  /**
   * Constructor for this class
   */
  DeRichHPD(const std::string & name = "");

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

  /**
   * Get the point on the centre of the HPD window on the inside surface
   * @return Inside window surface centre point
   */
  Gaudi::XYZPoint windowCentreIn() const
  {
    return geometry()->toGlobal( m_pvWindow->toMother(Gaudi::XYZPoint(0,0,sqrt(m_winInRsq))) );
  }

  /**
   * Get the point on the centre of the HPD window on the inside surface without misalignment
   * @return Inside window surface centre point without misalignment
   */
  Gaudi::XYZPoint windowCentreInIdeal() const
  {
    return geometry()->toGlobalMatrixNominal() * m_pvWindow->toMother(Gaudi::XYZPoint(0,0,sqrt(m_winInRsq)));
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
    return geometry()->toGlobal( m_pvWindow->toMother(Gaudi::XYZPoint(0,0,sqrt(m_winOutRsq))) );
  }

  /** @brief Converts a RichSmartID to a point in global coordinates.
   *  The point can be given either on the inside of the HPD window (photocathode) if
   *  photoCathodeSide=true or on the outside including refraction correction if
   *  photoCathodeSide=false
   *
   *  @param[in]  smartID     The RichSmartID pixel channel ID
   *  @param[out] detectPoint The position in global coordinates
   *  @param[in]  photoCathodeSide Set to false to include refraction on HPD window
   *  @return StatusCode indicating if the conversion was successful or not
   *  @retval StatusCoe::SUCCESS Conversion was successful
   *  @retval StatusCode::FAILURE Conversion failed
   */
  StatusCode detectionPoint ( const LHCb::RichSmartID smartID,
                              Gaudi::XYZPoint& detectPoint,
                              bool photoCathodeSide = false ) const;


  /** @brief Converts an x,y point from the anode to the photocathode in the
   * coordinate system of the HPD.
   *  The point can be given either on the inside of the HPD window (photocathode) if
   *  photoCathodeSide=true or on the outside including refraction correction if
   *  photoCathodeSide=false
   *
   *  @param[in]  anodePoint The <x,y> pair on the anode in pixel coordinates
   *  @param[out] detectPoint The position in HPD coordinates
   *  @param[in]  photoCathodeSide Set to false to include refraction on HPD window
   *  @return StatusCode indicating if the conversion was successful or not
   *  @retval StatusCoe::SUCCESS Conversion was successful
   *  @retval StatusCode::FAILURE Conversion failed
   */
  StatusCode detectionPoint ( double fracPixelCol, double fracPixelRow,
                              Gaudi::XYZPoint& detectPoint,
                              bool photoCathodeSide = true ) const;

  /** Converts a RichSmartID to a point on the anode in global coordinates.
   *  @param[in] smartID The RichSmartID for the HPD channel
   *  @return The detection point on the anode in global coordinates
   */
  Gaudi::XYZPoint detPointOnAnode( const LHCb::RichSmartID smartID ) const;

  /** Retrieves the demagnification interpolation function for the HPD R coordinate.
   *  For a given R on the HPD cathode returns the R on the anode.
   *  @return A pointer to the demagnification function for R(R)
   */
  inline const Rich::TabulatedFunction1D* demagnification_RtoR( int field=0 ) const
  {
    const unsigned int index = ( field > 0 ? 1 : 0 );
    return m_demagMapR[index];
  }

  /** Retrieves the demagnification interpolation function for the HPD phi coordinate.
   *  For a given R on the HPD cathode returns the phi on the anode.
   *  @return A pointer to the demagnification function for phi(R)
   */
  inline const Rich::TabulatedFunction1D* demagnification_RtoPhi( int field=0 ) const
  {
    const unsigned int index = ( field > 0 ? 1 : 0 );
    return m_demagMapPhi[index];
  }

  /** Retrieves the magnification interpolation function for the HPD R coordinate.
   *  For a given R on the HPD anode returns the R on the cathode.
   *  @return A pointer to the magnification function for R(R)
   */
  inline const Rich::TabulatedFunction1D* magnification_RtoR( int field=0 ) const
  {
    const unsigned int index = ( field > 0 ? 1 : 0 );
    return m_magMapR[index];
  }

  /** Retrieves the magnification interpolation function for the HPD phi coordinate.
   *  For a given R on the HPD anode returns the phi on the cathode.
   *  @return A pointer to the magnification function for phi(R)
   */
  inline const Rich::TabulatedFunction1D* magnification_RtoPhi( int field=0 ) const
  {
    const unsigned int index = ( field > 0 ? 1 : 0 );
    return m_magMapPhi[index];
  }

  // Anatoly Solomin 2007-10-26
  /** Retrieves the HPD Quantum Efficiency interpolation function.
   * For a given Photon Momentum in eV
   * returns the HPD Quantum Efficiency percentage.
   * @return pointer to the interpolation function for QuantumEff(PhotMom)
   */
  inline const Rich::TabulatedProperty1D* hpdQuantumEff() const
  {
    return m_hpdQuantumEffFunc;
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
  inline double extraRadiusForRefraction( double rCathode ) const
  {
    return ( m_refactParams[3]*gsl_pow_3(rCathode) +
             m_refactParams[2]*gsl_pow_2(rCathode) +
             m_refactParams[1]*rCathode +
             m_refactParams[0] );
  }

  /// Get parameters from Rich1
  StatusCode getParameters();

  /// Update the locally stored transformations
  StatusCode updateTransformations();

  /// Update the magnification and demagnification information
  StatusCode updateDemagProperties();

  // go from a point on silicon to a point on the photo-cathode with magnet ON
  StatusCode magnifyToGlobalMagnetON( Gaudi::XYZPoint& detectPoint, bool photoCathodeSide ) const;
  // go from a point on silicon to a point on the photo-cathode with magnet OFF
  StatusCode magnifyToGlobalMagnetOFF( Gaudi::XYZPoint& detectPoint, bool photoCathodeSide ) const;

  /// Initialise the interpolators for demagnification (cathode to anode)
  StatusCode fillHpdDemagTable( unsigned int  field );

  /// Initialise the interpolators for magnification (anode to cathode)
  StatusCode fillHpdMagTable( unsigned int  field );

  double Delta_Phi(const double, const double);
  double mag(const double , const double);
  double demag(const double, const double );

private: // data

  const IDetectorElement* m_deSiSensor;  ///< The silicon sensor detector element

  const IPVolume* m_pvWindow;      ///< The pv for the HPD quartz window
  const ISolid* m_windowSolid;     ///< The HPD window solid

  const ISolid* m_kaptonSolid;     ///< Pointer to the kapton solid

  int m_number;                    ///< HPD number (should be the same as copy number)

  double m_winInRsq;          ///< Inner radius of HPD window square
  double m_winOutRsq;         ///< Outter radius of HPD window square

  /// The active HPD window radius (photocathode coverage)
  double m_activeRadius;

  double m_pixelSize;           ///< The pixel size on the silicon sensor
  double m_siliconHalfLengthX;  ///< Si sensor half size in x
  double m_siliconHalfLengthY;  ///< Si sensor half size in y

  /// The demagnification factor of the HPD.  Element [0] is the linear
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

  ///< Interpolated property for HPD quantum efficiency
  const Rich::TabulatedProperty1D* m_hpdQuantumEffFunc;

  /// Demagnification parameters condition
  std::vector< SmartRef<Condition> > m_demagConds;

  /// binary flags; 1 for update of demag param, 2 for update of geometry; 6 for new hpdQE
  std::bitset<7> flags;

  std::vector<double> m_refactParams; ///< refraction parameters for quartz window

  //int    rgiState[2+55];
  ///< Force the use of MDMS corrections code even if the field is OFF
  ///< When FALSE the magnetic field service is used to decide.
  bool   m_UseHpdMagDistortions;
  bool   m_UseBFieldTestMap ;
  double m_LongitudinalBField ;

  ///< version of MDMS corrections
  int m_MDMS_version;

  // Cached parameters for speed reasons.
  Gaudi::Transform3D m_SiSensorToHPDMatrix; ///< silicon to HPD transform
  Gaudi::Transform3D m_fromWindowToGlobal; ///< window to global coord system transform
  Gaudi::Transform3D m_fromPanelToWindow; ///< HPD panel to HPD window coord system transform
  Gaudi::Transform3D m_fromPanelToKapton; ///< HPD panel to kapton coord system transform
  Gaudi::Transform3D m_fromWindowToHPD; ///< window to global coord system transform
  Gaudi::Transform3D m_fromHPDToPanel; ///< HPD to HPD Panel transform
  /// The centre of the HPD window (inside) in the mother (panel) coordinate system
  Gaudi::XYZPoint m_windowInsideCentreMother;

  /// Rotional centre for MDMS corrections
  Gaudi::XYZVector m_MDMSRotCentre;

  /// pointer to the magnetic field service
  ILHCbMagnetSvc* m_magFieldSvc;
};

//=========================================================================
// Converts a RichSmartID to a point in global coordinates.
//=========================================================================
inline StatusCode DeRichHPD::detectionPoint ( const LHCb::RichSmartID smartID,
                                              Gaudi::XYZPoint& detectPoint,
                                              bool photoCathodeSide ) const
{
  detectPoint = pointOnSilicon(smartID);
  return ( (fabs( m_magFieldSvc->signedRelativeCurrent() ) > 0.5 || m_UseHpdMagDistortions) ?
           magnifyToGlobalMagnetON( detectPoint, photoCathodeSide ) :
           magnifyToGlobalMagnetOFF( detectPoint, photoCathodeSide ) );
}

//=========================================================================
// Converts a pair to a point in global coordinates.
//=========================================================================
inline StatusCode DeRichHPD::detectionPoint ( double fracPixelCol,
                                              double fracPixelRow,
                                              Gaudi::XYZPoint& detectPoint,
                                              bool photoCathodeSide ) const
{
  if ( fracPixelCol < 0.0 || fracPixelRow < 0.0 )
  {
    error() << "Negative pixel coordinate " << fracPixelCol << ","
            << fracPixelRow << endmsg;
    return StatusCode::FAILURE;
  }

  detectPoint = Gaudi::XYZPoint( fracPixelCol*m_pixelSize - m_siliconHalfLengthX,
                                 m_siliconHalfLengthY - fracPixelRow*m_pixelSize,
                                 0.0 );
  StatusCode sc = ( fabs( m_magFieldSvc->signedRelativeCurrent() ) > 0.5 || m_UseHpdMagDistortions) ?
    magnifyToGlobalMagnetON( detectPoint, photoCathodeSide ) :
    magnifyToGlobalMagnetOFF( detectPoint, photoCathodeSide );
  detectPoint = geometry()->toLocal(detectPoint);
  return sc;
}

//=========================================================================
//  convert a RichSmartID to a point on the anode (global coord system)
//=========================================================================
inline Gaudi::XYZPoint DeRichHPD::detPointOnAnode( const LHCb::RichSmartID smartID ) const
{
  return ( m_deSiSensor->geometry()->toGlobal( pointOnSilicon(smartID) ) );
}

#endif    // RICHDET_DERICHHPD_H
