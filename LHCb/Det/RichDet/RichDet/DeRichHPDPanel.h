// $Id: DeRichHPDPanel.h,v 1.11 2004-02-16 14:11:52 papanest Exp $

#ifndef RICHDET_DERICHHPDPANEL_H
#define RICHDET_DERICHHPDPANEL_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

// LHCbKernel
#include "Kernel/CLHEPStreams.h"
#include "Kernel/RichSmartID.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

/** @class DeRichHPDPanel DeRichHPDPanel.h
 *
 * This is the definition of the Rich HPDPanel detector class
 *
 * @author Antonis Papanestis
 */

class DeRichHPDPanel: public DetectorElement {

public:

  /// traceMode is used in the methods detPlanePoint to set boundaries
  /// and in PDWindow point to set whether the returned point is actualy
  /// on the window surface, or a simple algorith is used.
  enum traceMode {
    loose = 0,
    tight,
    window = 0,
    circle
  };

  /**
   * Constructor for this class
   */
  DeRichHPDPanel();

  /**
   * Default destructor
   */
  virtual ~DeRichHPDPanel();

  /**
   * This is where most of the geometry is read
   * @return StatusCode
   */
  virtual StatusCode initialize();


  /**
   * Returns the detection plane of the PD panel.
   * @return The detection plane defined at the top of the
   * photo detectors (a plane resting on the PDs "touching" the 
   * INSIDE surface of the window).
   */
  virtual const HepPlane3D & detectionPlane() const = 0;

  /**
   * Returns the offset for the globalToPanel method.
   * @return The offset (y in Rich1, x in Rich2) so that the two panels of
   * each detector appear side-by-side using the globalToPanel method.
   */
  virtual const double localOffset() const = 0;


  /**
   * Converts a HepPoint3D in global coordinates to a RichSmartID.
   * @return StatusCode The point is assumed to be on the actual 
   * detection volume (silicon pixel sensor for the HPD).
   */
  virtual StatusCode smartID( const HepPoint3D& globalPoint,
                              RichSmartID& id );

  /**
   * Converts a RichSmartID to a point in global coordinates.
   * @return StatusCode  The point is given on the inside
   * of the HPD window, on the photocathode.
   */
  virtual StatusCode detectionPoint( const RichSmartID& smartID,
                                     HepPoint3D& windowHitGlobal ); 
  /**
   * Returns the intersection point with an PD window given a vector
   * and a point.
   * @return StatusCode With the "circle" option a quick check is performed
   * to test if there would be an intersection with a flat circle instead
   * of the HPD window.
   */

  virtual StatusCode PDWindowPoint( const HepVector3D& vGlobal, 
                                    const HepPoint3D& pGlobal, 
                                    HepPoint3D& windowPointGlobal, // return
                                    RichSmartID& smartID,
                                    traceMode mode = circle );

  /**
   * Returns the intersection point with the detector plane given a vector
   * and a point. If mode is tight, returns true only if point is within
   * the detector coverage.
   * @return bool
   */
  virtual bool detPlanePoint( const HepPoint3D& pGlobal,
                              const HepVector3D& vGlobal,
                              HepPoint3D& hitPosition,
                              traceMode mode = loose);

  /**
   * Converts a global position to the coordinate system of the
   * photodetector panel
   */
  virtual HepPoint3D globalToPDPanel( const HepPoint3D& globalPoint );
  
    //  {
    //    return ( geometry()->toLocal( globalPoint ) );
    //  }
  

  /**
   * Returns the global position given a local position and panel number
   */
  virtual HepPoint3D globalPosition( const HepPoint3D& localPoint,
                                     Rich::Side side) = 0;
  

  /**
   * Returns a list with all the available readout channels, in form of
   * RichSmartIDs.
   * @return 
   */
  virtual StatusCode readoutChannelList(std::vector<RichSmartID>&
                                        readoutChannels);


protected:

  /**  
   * Returns the number of HPDs in the panel
   */
  virtual unsigned int PDMax() = 0;

  /**  
   * Returns the HPD row in the panel, given the HPD number
   */
  virtual unsigned int PDRow(unsigned int PD) = 0;

  /**  
   * Returns the HPD column in the panel, given the HPD number
   */
  virtual unsigned int PDCol(unsigned int PD) = 0;

  /**  
   * Returns the HPD at the next row/column depending on panel configurartion 
   */
  virtual unsigned int HPDForNS() = 0;

  /**  
   * Returns an HPD number that can be used as the second point for the
   * detection plane.
   */
  virtual unsigned int HPDForB() = 0;

  /**  
   * Returns an HPD number that can be used as the third point for the
   * detection plane.
   */
  virtual unsigned int HPDForC() = 0;

  /**  
   * Converts an HPD row and column to a number corresponding
   * to the position of this physical volume in the physical volumes vector 
   */
  virtual unsigned int HPDRowColToNum(unsigned int HPDRow, 
                                      unsigned int HPDCol ) = 0;

  /**  
   * Finds the HPD row and column that corresponds to the x,y coordinates
   * of a point in the panel.
   * @returns true if the HPD is found, false if the point is outside the
   * coverage of the HPDs. The row and column are retuned in the smartID.
   */
  virtual bool findHPDRowCol(const HepPoint3D& inPanel, RichSmartID& id) = 0;


  /// the name of this HPD panel
  std::string m_name;

  /// number of HPD rows and columns
  unsigned int m_HPDRows;
  unsigned int m_HPDColumns;
  
  /// these two are used in the cdf type panel
  unsigned int m_HPDsInBigCol;
  unsigned int m_HPDsIn2Cols;

  /// number of pixel rows and columns
  unsigned int m_pixelRows;
  unsigned int m_pixelColumns;

  /// the active HPD window radius (photocathode coverage)
  double m_activeRadius;
  double m_activeRadiusSq; // Squared

  /// the radius of the sphere of the HPD window
  double m_winR;
  /// radius squared
  double m_winRsq;
  
  double m_detPlaneZ;

  double m_pixelSize;
  double m_subPixelSize;
  double m_siliconHalfLengthX;
  double m_siliconHalfLengthY;

  /// distance between HPD rows
  double m_rowPitch;
  /// distance between HPD columns
  double m_columnPitch;

  /// the demagnification factor of the HPD.  Element [0] is the linear
  /// term, and element[1] the non-linear term for small corrections.
  double m_deMagFactor[2];

  /// The following points are used to get the row and column pitch
  HepPoint3D m_HPD0Centre;
  HepPoint3D m_HPD1Centre;
  HepPoint3D m_HPDNSCentre;  ///next set: either next row or next column
  
  /// the top of the HPD window in silicon coordinates
  HepPoint3D m_HPDTop;  

  const ISolid* m_HPDPanelSolid;

  std::vector<HepPoint3D> m_HPDCentres;

  // from PDPanel
  /// detection plane in global coordinates
  HepPlane3D m_detectionPlane;

  /// detection plane in PDPanel coordinates
  HepPlane3D m_localPlane;
  HepVector3D m_localPlaneNormal;

  /// local plane for HPD row/column purposes
  HepPlane3D m_localPlane2;
  HepVector3D m_localPlaneNormal2;

  double m_detPlaneHorizEdge;
  double m_detPlaneVertEdge;

  HepTransform3D m_vectorTransf;

};

#endif    // RICHDET_DERICHHPDPANEL_H
