
/** @file DeRichHPD.h
 *
 *  Header file for detector description class : DeRichHPD
 *
 *  CVS Log :-
 *  $Id: DeRichHPD.h,v 1.11 2007-09-13 13:10:54 jpalac Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-09-19
 */

#ifndef RICHDET_DERICHHPD_H
#define RICHDET_DERICHHPD_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
//#include "DetDesc/TabulatedProperty.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichDet
#include "RichDet/Rich1DTabFunc.h"

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
    return geometry()->toGlobal( m_pvWindow->toMother(Gaudi::XYZPoint(0,0,sqrt(m_winInRsq))) );
  }

  /**
   * Get the point on the centre of the HPD window on the inside surface
   * @return Inside window surface centre point
   */
  Gaudi::XYZPoint windowCentreInIdeal() const
  {
    return geometry()->toGlobalMatrixNominal() * m_pvWindow->toMother(Gaudi::XYZPoint(0,0,sqrt(m_winInRsq)));
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
   *  The point is given on the inside of the HPD window, on the photocathode.
   *  @param[in]  smartID     The RichSmartID pixel channel ID
   *  @param[out] detectPoint The position in global coordinates
   *  @return StatusCode indicating if the conversion was successful or not
   *  @retval StatusCoe::SUCCESS Conversion was successful
   *  @retval StatusCode::FAILURE Conversion failed
   */
  StatusCode detectionPoint ( const LHCb::RichSmartID smartID,
                              Gaudi::XYZPoint& detectPoint ) const;


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

  /** Converts the given RichSmartID to the position on the silicon wafer,
   *  in the coordinate system of the wafer
   *  @param[in] smartID The RichSmartID
   *  @return The position on the wafer
   */
  inline Gaudi::XYZPoint pointOnSilicon ( const LHCb::RichSmartID smartID ) const
  {
    return Gaudi::XYZPoint( xOnSilicon(smartID), yOnSilicon(smartID), 0 );
  }

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

  /// Update the magnification abnd demagnification information
  StatusCode updateDemagProperties();

  StatusCode magnifyToGlobal_old( Gaudi::XYZPoint& detectPoint ) const;
  StatusCode magnifyToGlobal_new( Gaudi::XYZPoint& detectPoint ) const;

  //void init_mm( );
  //int number_range( int from, int to );
  //int number_mm( void );

  double Delta_Phi(const double, const double);
  double mag(const double , const double);
  double demag(const double, const double );

  StatusCode fillHpdDemagTable( ); ///< Initialise the interpolators for demagnification (cathode to anode)
  StatusCode fillHpdMagTable( );   ///< Initialise the interpolators for magnification (anode to cathode)

private: // data

  const IDetectorElement* m_deSiSensor;  ///< The silicon sensor detector element

  const IPVolume* m_pvWindow;      ///< The pv for the HPD quartz window

  std::string m_name;              ///< The name of this HPD
  int m_number;                    ///< HPD number (copy number)

  double m_winInRsq;          ///< Inner radius of HPD window square
  double m_winOutRsq;         ///< Outter radius of HPD window square

  /// The active HPD window radius (photocathode coverage)
  double m_activeRadius;

  Gaudi::Transform3D m_fromWindowToGlobal; ///< window to global coord system transform

  double m_pixelSize;           ///< The pixel size on the silicon sensor
  double m_siliconHalfLengthX;  ///< Si sensor half size in x
  double m_siliconHalfLengthY;  ///< Si sensor half size in y

  /// The demagnification factor of the HPD.  Element [0] is the linear
  /// term, and element[1] the non-linear term for small corrections.
  double m_deMagFactor[2];

  const Rich::TabulatedFunction1D * m_demagMapR;   ///< Interpolated function for HPD R for demagnification
  const Rich::TabulatedFunction1D * m_demagMapPhi; ///< Interpolated function for HPD phi for demagnification
  const Rich::TabulatedFunction1D * m_magMapR;     ///< Interpolated function for HPD R for magnification
  const Rich::TabulatedFunction1D * m_magMapPhi;   ///< Interpolated function for HPD phi for magnification

  /// Demagnification parameters condition
  SmartRef<Condition> m_demagCond;

  bool m_firstUpdate; ///< Flag to identify first UMS update

  std::vector<double> m_refactParams; ///< refraction parameters for quartz window

  /// Cache the local matrix for the sensor, for speed reasons
  Gaudi::Transform3D m_localMatrixInv;

  //int    rgiState[2+55];
  bool   m_UseHpdMagDistortions;
  bool   m_UseBFieldTestMap ;
  double m_LongitudinalBField ;

};

//=========================================================================
// Converts a RichSmartID to a point in global coordinates.
//=========================================================================
inline StatusCode DeRichHPD::detectionPoint ( const LHCb::RichSmartID smartID,
                                              Gaudi::XYZPoint& detectPoint ) const
{
  // convert pixel number to silicon coordinates
  // and transform the point to get the misalignment of the Si sensor
  //detectPoint = m_deSiSensor->geometry()->localMatrix().Inverse()*pointOnSilicon(smartID);
  detectPoint = m_localMatrixInv * pointOnSilicon(smartID);
  detectPoint.SetZ(0.0);
  return ( m_UseHpdMagDistortions ?
           magnifyToGlobal_new( detectPoint ) :  // M.Musy 07/09/2006
           magnifyToGlobal_old( detectPoint ) ); // old demag parameters
}

//=========================================================================
//  convert a RichSmartID to a point on the anode (global coord system)
//=========================================================================
inline Gaudi::XYZPoint DeRichHPD::detPointOnAnode( const LHCb::RichSmartID smartID ) const
{
  return ( m_deSiSensor->geometry()->toGlobal( pointOnSilicon(smartID) ) );
}

#endif    // RICHDET_DERICHHPD_H
