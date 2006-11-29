
//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.h
 *
 *  Header file for detector description class : DeRichHPDPanel
 *
 *  $Id: DeRichHPDPanel.h,v 1.39 2006-11-29 16:28:58 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHHPDPANEL_H
#define RICHDET_DERICHHPDPANEL_H 1

// Include files
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Plane3DTypes.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichTraceMode.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

// RichDet
//#include "RichDet/DeRich1.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichSystem.h"
#include "RichDet/DeRichHPD.h"

/** @namespace DeRichHPDPanelLocation
 *
 *  Namespace for the location of HPD panels in xml
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 *
 *  @todo Find a proper way to deal with RichSmartIDs that cannot be mapped to the
 *        entrance window in the method DeRichHPDPanel::detectionPoint
 */
namespace DeRichHPDPanelLocation {
  /// Location of Rich1 top panel
  static const std::string& Rich1Panel0_old = "/dd/Structure/LHCb/Rich1/PDPanel0";
  static const std::string& Rich1Panel0 =
  "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0";

  /// Location of Rich1 bottom panel
  static const std::string& Rich1Panel1_old = "/dd/Structure/LHCb/Rich1/PDPanel1";
  static const std::string& Rich1Panel1 =
  "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel1";

  /// Location of Rich2 left panel
  static const std::string& Rich2Panel0_old = "/dd/Structure/LHCb/Rich2/PDPanel0";
  static const std::string& Rich2Panel0 =
  "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem/HPDPanel0";

  /// Location of Rich2 right panel
  static const std::string& Rich2Panel1_old = "/dd/Structure/LHCb/Rich2/PDPanel1";
  static const std::string& Rich2Panel1 =
  "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem/HPDPanel1";
}

/** @class DeRichHPDPanel DeRichHPDPanel.h
 *
 * DeRichHPDPanel provides geometry information for the HPD panels, converts RichSmartIDs
 * to space points and finds intersections with the HPDs
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */
class DeRichHPDPanel: public DetectorElement {

public:

  /**
   * Constructor for this class
   */
  DeRichHPDPanel();

  /**
   * Default destructor
   */
  virtual ~DeRichHPDPanel();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const
  {
    return classID();
  }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This is where most of the geometry is read and variables initialised
   *
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();


  /**
   * Retrieves the detection plane of the HPD panel. The plane is defined
   * at the top of the HPDs (a plane resting on the HPDs "touching" the
   * INSIDE surface of the window).
   *
   * @return The detection plane
   */
  inline const Gaudi::Plane3D& detectionPlane() const
  {
    return m_detectionPlane;
  }

  /**
   * Returns the offset (y in Rich1, x in Rich2) so that the two panels of
   * each detector appear side-by-side using the globalToPanel method.
   *
   * @return The offset for the globalToPanel method
   */
  inline const double localOffset() const
  {
    return m_localOffset;
  }

  /**
   * Converts a Gaudi::XYZPoint in global coordinates to a RichSmartID.
   * The point is assumed to be on the actual detection volume
   * (silicon pixel sensor).
   *
   * @param globalPoint The point in global coordinates
   * @param id          The RichSmartID for the given point
   *
   * @return Status of conversion
   * @retval StatusCode::FAILURE Point outside silicon pixel sensor
   */
  virtual StatusCode smartID( const Gaudi::XYZPoint& globalPoint,
                              LHCb::RichSmartID& id ) const;

  /**
   * Converts a RichSmartID to a point in global coordinates. The point is
   * given on the inside of the HPD window, on the photocathode.
   *
   * @return The detection point in global coordinates
   */
  virtual Gaudi::XYZPoint detectionPoint( const LHCb::RichSmartID& smartID ) const;

  /**
   * Converts a RichSmartID to a point on the anode in global coordinates.
   *
   * @return The detection anode point in global coordinates
   */
  virtual Gaudi::XYZPoint detPointOnAnode( const LHCb::RichSmartID& smartID ) const;

  /**
   * Returns the intersection point with an HPD window given a vector
   * and a point. With the "circle" option a quick check is performed
   * to test if there would be an intersection with a flat circle instead
   * of the HPD window.
   *
   * @param vGlobal  The intersection direction
   * @param pGlobal  The intersection start point
   * @param windowPointGlobal The return point on the HPD window
   * @param smartID  The returned smartID with hit HPD info
   * @param mode The ray-tracing configuration mode
   *
   * @return Status of intersection
   */
  virtual StatusCode PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                                    const Gaudi::XYZPoint& pGlobal,
                                    Gaudi::XYZPoint& windowPointGlobal, // return
                                    LHCb::RichSmartID& smartID,          // return
                                    const LHCb::RichTraceMode mode ) const;

  /**
   * Returns the intersection point with the detector plane given a vector
   * and a point. If mode is tight, returns true only if point is within
   * the detector coverage.
   *
   * @param vGlobal  The intersection direction
   * @param pGlobal  The intersection start point
   * @param hitPosition The intersection point on the HPD plane
   * @param mode The ray-tracing configuration mode
   *
   * @return Intersection status
   * @retval false Intersection fell outside acceptance, as defined by mode
   */
  virtual bool detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                              const Gaudi::XYZVector& vGlobal,
                              Gaudi::XYZPoint& hitPosition,
                              const LHCb::RichTraceMode mode ) const;

  /**
   * Converts a global position to the coordinate system of the
   * photodetector panel. The local coordinate system is shifted to allow
   * placing panels side by side
   *
   * @param globalPoint The point in global coordinates
   *
   * @return Local (panel) point
   */
  virtual Gaudi::XYZPoint globalToPDPanel( const Gaudi::XYZPoint& globalPoint ) const;


  /**
   * Returns the global position given a local position and panel number.
   * Assumes a shifted panel as previous method
   *
   * @param localPoint The point in local coordinates
   * @param side The detector side
   *
   * @return Global point.
   */
  virtual Gaudi::XYZPoint globalPosition( const Gaudi::XYZPoint& localPoint,
                                          const Rich::Side side) const;


  /**
   * Adds to the given vector all the available readout channels in this HPD panel
   *
   * @param readoutChannels Vector of valid pixel IDs
   *
   * @return Status code
   */
  StatusCode readoutChannelList( LHCb::RichSmartID::Vector& readoutChannels ) const;

  /// Access the RICH detector type
  inline Rich::DetectorType rich() const { return m_rich; }

  /// Access the HPD panel side
  inline Rich::Side side() const { return m_side; }

  /// sensitive volume identifier
  virtual const int sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const;

protected:

  /**
   * Finds the HPD row and column that corresponds to the x,y coordinates
   * of a point in the panel. The row and column are retuned in the smartID.
   *
   * @return Status of request
   * @retval true   HPD is found
   * @retval false  The point is outside the coverage of the HPDs.
   */
  virtual bool findHPDColAndPos(const Gaudi::XYZPoint& inPanel, LHCb::RichSmartID& id) const;

  /** Returns the name of this particular HPD panel
   *  @return HPD panel name
   */
  inline const std::string & myName() const { return m_name; }

  /** Convert the HPD number (number in the panel) to a copy number
   *  (which is unique in the Rich system).
   *  @return The Copy Number
   */
  int copyNumber( unsigned int HPDNumber ) const;


  // data

  std::string m_name;           ///< The name of this HPD panel
  unsigned int m_HPDColumns;    ///< Number of HPD columns in the panel
  unsigned int m_HPDNumInCol;   ///< Number of HPDs in each column
  unsigned int m_HPDMax;        ///< Total number of HPDs in this panel

  double m_HPDPitch;               ///< distance between HPDs
  double m_HPDColPitch;            ///< distance between HPD columns

  unsigned int m_pixelRows;        /// Number of pixel rows
  unsigned int m_pixelColumns;     /// Number of pixel columns

  /// The active HPD window radius (photocathode coverage)
  double m_activeRadius;
  /// The active HPD window radius Squared
  double m_activeRadiusSq;

  /// The radius of the sphere of the HPD window
  double m_winR;
  /// Radius squared
  double m_winRsq;

  double m_winOutR;
  double m_winOutRsq;

  /// The z position of the detection plane in an HPD panel
  double m_detPlaneZ;
  /// the z difference between the planes defined at the top and bottom of HPD window
  double m_detPlaneZdiff;

  double m_pixelSize;      ///< The pixel size on the silicon sensor
  double m_subPixelSize;   ///< The size of the subpixel (Alice mode)
  double m_siliconHalfLengthX;     ///< Half size (x) of silicon sensor
  double m_siliconHalfLengthY;     ///< Half size (y) of silicon sensor

  /// The demagnification factor of the HPD.  Element [0] is the linear
  /// term, and element[1] the non-linear term for small corrections.
  double m_deMagFactor[2];

  /// The following points are used to get the row and column pitch
  /// Centre of HPD 0
  Gaudi::XYZPoint m_HPD0Centre;

  /// the top of the HPD window in silicon coordinates
  Gaudi::XYZPoint m_HPDTop;

  const ISolid* m_HPDPanelSolid;   ///< The solid (box) that contains the HPDs

  std::vector<Gaudi::XYZPoint> m_HPDCentres; ///< The centres of all HPDs

  Gaudi::Plane3D m_detectionPlane;    ///< detection plane in global coordinates
  Gaudi::Plane3D m_localPlane;        ///< detection plane in PDPanel coordinates
  Gaudi::XYZVector m_localPlaneNormal; ///< The normal vector of det plane in local coordinates

  /// Plane2 is defined ging through all HPDs at the edge of photocathode coverage on
  /// HPD window. It is used for HPD row/column purposes
  Gaudi::Plane3D m_localPlane2;
  Gaudi::XYZVector m_localPlaneNormal2;  ///< Normal vector of plane2


  Gaudi::Transform3D m_vectorTransf;  ///< Transform from global to panel coordinates

  double m_panelColumnSideEdge;    ///< Edge of the panel along the columns
  double m_panelStartColPosEven;   ///< Bottom/Start point of the even HPD columns
  double m_panelStartColPosOdd;    ///< Bottom/Start point of the odd HPD columns
  /// abs max of even and odd start points used as the edge across columns
  double m_panelStartColPos;
  double m_localOffset;           ///< offset applied in the global to panel coordinates
  
public:
  /**
   * Prints coordinates on anode and cathode: for test only porposes.
   *
   * @param HPDNumber x y z Bfield
   *
   * @return void
   */
  void testdemagnification( int HPDNumber, double,double,double, double B );


private:

  Rich::DetectorType m_rich;
  Rich::Side m_side;
  LHCb::RichSmartID m_panelRichID;
  DeRichSystem* m_deRichS;
  const ISolid* m_kaptonSolid;

  std::vector<const IPVolume*> m_pvHPDMaster;
  std::vector<const IPVolume*> m_pvHPDSMaster;
  std::vector<const IPVolume*> m_pvSilicon;
  std::vector<const IPVolume*> m_pvWindow;
  std::vector<const IPVolume*> m_pvKapton;
  std::vector<Gaudi::Transform3D> m_HPDWindowToGlobal;
  std::vector<Gaudi::Transform3D> m_panelToSilicon;

  std::vector<DeRichHPD*> m_DeHPDs; ///< Container for the HPDs as Det Elem

  std::vector<double>  m_refactParams;

  StatusCode updateDemagProperties();
  int m_nstart, m_nstop;

  int    rgiState[2+55];
  bool   m_UseHpdMagDistortions;
  bool   m_UseBFieldTestMap ;
  bool   m_UseRandomBField  ;
  double m_LongitudinalBField ;
  double m_RandomBFieldMinimum ;
  double m_RandomBFieldMaximum ;
  double m_RichHpdQWToSiMaxDist;
  int m_Rich1TotNumHpd ;
  int m_Rich2TotNumHpd ;
  void init_mm( );
  int number_range( int from, int to );
  int number_mm( void );

  std::string m_XmlHpdDemagPath;
  SmartRef<Condition> m_demagCond;
  double Delta_Phi(double, const double);
  double mag(double , double);
  double demag(double, double );
  StatusCode fillHpdDemagTableSim( std::string , std::vector<double>& , int& );
  StatusCode fillHpdDemagTableRec( std::string , std::vector<double>& , int& );
  Gaudi::XYZPoint demagToCathode_new( int , double , double ) const;
  Gaudi::XYZPoint demagToCathode_old( double , double ) const;
  Gaudi::XYZPoint demagToAnode_test ( int , double , double );

};

#endif    // RICHDET_DERICHHPDPANEL_H
