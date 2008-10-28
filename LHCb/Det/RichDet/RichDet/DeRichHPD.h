
//=============================================================================
/** @file DeRichHPD.h
 *
 *  Header file for detector description class : DeRichHPD
 *
 *  CVS Log :-
 *  $Id: DeRichHPD.h,v 1.16 2008-10-28 10:17:22 cattanem Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-09-19
 */
//=============================================================================

#ifndef RICHDET_DERICHHPD_H
#define RICHDET_DERICHHPD_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IPVolume.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichDet
#include "RichDet/Rich1DTabFunc.h"
#include "RichDet/Rich1DTabProperty.h"

#include <bitset>

// External declarations
extern const CLID CLID_DERichHPD;

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


  /** Converts a RichSmartID to a point on the anode in global coordinates.
   *  @param[in] smartID The RichSmartID for the HPD channel
   *  @return The detection point on the anode in global coordinates
   */
  Gaudi::XYZPoint detPointOnAnode( const LHCb::RichSmartID smartID ) const;

  /** Retrieves the demagnification interpolation function for the HPD R coordinate.
   *  For a given R on the HPD cathode returns the R on the anode.
   *  @return A pointer to the demagnification function for R(R)
   */
  inline const Rich::TabulatedFunction1D* demagnification_RtoR() const
  {
    return m_demagMapR;
  }

  /** Retrieves the demagnification interpolation function for the HPD phi coordinate.
   *  For a given R on the HPD cathode returns the phi on the anode.
   *  @return A pointer to the demagnification function for phi(R)
   */
  inline const Rich::TabulatedFunction1D* demagnification_RtoPhi() const
  {
    return m_demagMapPhi;
  }

  /** Retrieves the magnification interpolation function for the HPD R coordinate.
   *  For a given R on the HPD anode returns the R on the cathode.
   *  @return A pointer to the magnification function for R(R)
   */
  inline const Rich::TabulatedFunction1D* magnification_RtoR() const
  {
    return m_magMapR;
  }

  /** Retrieves the magnification interpolation function for the HPD phi coordinate.
   *  For a given R on the HPD anode returns the phi on the cathode.
   *  @return A pointer to the magnification function for phi(R)
   */
  inline const Rich::TabulatedFunction1D* magnification_RtoPhi() const
  {
    return m_magMapPhi;
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

  /** Returns the name of this particular HPD
   *  @return HPD name
   */
  inline const std::string& myName() const { return m_name; }

  /// Get parameters from Rich1
  StatusCode getParameters();

  /// Update the locally stored transformations
  StatusCode updateTransformations();

  /// Update the magnification and demagnification information
  StatusCode updateDemagProperties();

  // go from a point on silicon to a point on the photo-cathode
  StatusCode magnifyToGlobal( Gaudi::XYZPoint& detectPoint, bool photoCathodeSide ) const;

  /// Initialise the interpolators for demagnification (cathode to anode)
  StatusCode fillHpdDemagTable( );

  /// Initialise the interpolators for magnification (anode to cathode)
  StatusCode fillHpdMagTable( );

  double Delta_Phi(const double, const double);
  double mag(const double , const double);
  double demag(const double, const double );

private: // data

  const IDetectorElement* m_deSiSensor;  ///< The silicon sensor detector element

  const IPVolume* m_pvWindow;      ///< The pv for the HPD quartz window
  const ISolid* m_windowSolid;     ///< The HPD window solid

  const ISolid* m_kaptonSolid;     ///< Pointer to the kapton solid

  std::string m_name;              ///< The name of this HPD
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

  Rich::TabulatedFunction1D* m_demagMapR;   ///< Interpolated function for HPD R for demagnification
  Rich::TabulatedFunction1D* m_demagMapPhi; ///< Interpolated function for HPD phi for demagnification
  Rich::TabulatedFunction1D* m_magMapR;     ///< Interpolated function for HPD R for magnification
  Rich::TabulatedFunction1D* m_magMapPhi;   ///< Interpolated function for HPD phi for magnification

  ///< Interpolated property for HPD quantum efficiency
  const Rich::TabulatedProperty1D* m_hpdQuantumEffFunc;

  /// Demagnification parameters condition
  SmartRef<Condition> m_demagCond;

  /// binary flags; 1 for update of demag param, 2 for update of geometry; 6 for new hpdQE
  std::bitset<7> flags;
  
  std::vector<double> m_refactParams; ///< refraction parameters for quartz window

  //int    rgiState[2+55];
  bool   m_UseHpdMagDistortions;
  bool   m_UseBFieldTestMap ;
  double m_LongitudinalBField ;

  // Cached parameters for speed reasons.
  Gaudi::Transform3D m_SiSensorToHPDMatrix; ///< silicon to HPD transform
  Gaudi::Transform3D m_fromWindowToGlobal; ///< window to global coord system transform
  Gaudi::Transform3D m_fromPanelToWindow; ///< HPD panel to HPD window coord system transform
  Gaudi::Transform3D m_fromPanelToKapton; ///< HPD panel to kapton coord system transform
  Gaudi::Transform3D m_fromWindowToHPD; ///< window to global coord system transform
  Gaudi::Transform3D m_fromHPDToPanel; ///< HPD to HPD Panel transform
  /// The centre of the HPD window (inside) in the mother (panel) coordinate system
  Gaudi::XYZPoint m_windowInsideCentreMother;

};

//=========================================================================
// Converts a RichSmartID to a point in global coordinates.
//=========================================================================
inline StatusCode DeRichHPD::detectionPoint ( const LHCb::RichSmartID smartID,
                                              Gaudi::XYZPoint& detectPoint,
                                              bool photoCathodeSide ) const
{
  // convert pixel number to silicon coordinates
  // and transform the point to get the misalignment of the Si sensor
  detectPoint = m_SiSensorToHPDMatrix * pointOnSilicon(smartID);
  detectPoint.SetZ(0.0);
  return magnifyToGlobal( detectPoint, photoCathodeSide );
}

//=========================================================================
//  convert a RichSmartID to a point on the anode (global coord system)
//=========================================================================
inline Gaudi::XYZPoint DeRichHPD::detPointOnAnode( const LHCb::RichSmartID smartID ) const
{
  return ( m_deSiSensor->geometry()->toGlobal( pointOnSilicon(smartID) ) );
}

#endif    // RICHDET_DERICHHPD_H
