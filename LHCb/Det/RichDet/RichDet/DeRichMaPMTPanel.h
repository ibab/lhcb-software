// $Id: DeRichMaPMTPanel.h,v 1.1 2003-08-29 08:29:42 papanest Exp $

#ifndef DERICHMAPMTPANEL_H
#define DERICHMAPMTPANEL_H 1


// Include files
#include "RichDet/DeRichPDPanel.h"

/** @class DeRichMaPMTPanel DeRichMaPMTPanel.h
 *
 * This is the definition of the Rich MaPMTPanel detector class
 *
 * @author Antonis Papanestis
 */
class DeRichMaPMTPanel: public  DeRichPDPanel {

public:
  /**
   * Constructor for this class
   */
  DeRichMaPMTPanel();

  /**
   * Default destructor
   */
  virtual ~DeRichMaPMTPanel();

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
   * This is where most of the geometry is read
   * @return StatusCode
   */

  virtual StatusCode initialize();


  /**
   * Returns a RichSmartID for a given point in global coordinates.
   * @return StatusCode
   */
  virtual StatusCode smartID( const HepPoint3D& globalPoint,
                              RichSmartID& id );

  /**
   * Returns the detection point given a smartID. The point is on the inside
   * of the MaPMT window, on the photocathode.
   * @return StatusCode
   */
  virtual StatusCode detectionPoint( const RichSmartID& smartID,
                                     HepPoint3D& windowHitGlobal ); 
  /**
   * Returns the intersection point with an PD window given a vector
   * and a point.
   * @return StatusCode
   */

  virtual StatusCode PDWindowPoint( const HepVector3D& vGlobal, 
                                    const HepPoint3D& pGlobal, 
                                    HepPoint3D& windowPointGlobal, // return
                                    RichSmartID& smartID );
  /**
   * Returns the detection plane of the HPD panel, defined at the top of the 
   * HPDs (a plane resting on the HPDs touching the window).
   * @return HepPlane3D
   */
  inline HepPlane3D detectionPlane() const { return m_detectionPlane; }
  


  /**
   * Returns a list with all the available readout channels, in form of
   * RichSmartIDs.
   * @return StatusCode
   */
  virtual StatusCode readoutChannelList(std::vector<RichSmartID>&
                                        readoutChannels);


protected:

  unsigned int m_PDRows;
  unsigned int m_PDColumns;

  unsigned int m_PDMax;

  unsigned int m_pixelRows;
  unsigned int m_pixelCols;  

private:

  /// half sizes of the MaPMT quartz window
  double m_quartzWinHalfLengthX;
  double m_quartzWinHalfLengthY;
  double m_quartzWinHalfLengthZ;

  /// The pitch the MaPMTs are placed in the panel
  double m_rowPitch;
  double m_columnPitch;
  double m_pixelPitch;
  

  /// detection plane
  HepPlane3D m_detectionPlane;

  /// Panel edges and their absolute values
  double m_panelHorizEdge;
  double m_panelVerticalEdge;
  double m_abs_panelHorizEdge;
  double m_abs_panelVerticalEdge;

  double m_pixelHorizEdge;
  double m_pixelVerticalEdge;
  
  HepTransform3D m_vectorTransf;
  const ISolid* m_MaPMTPanelSolid;
  std::vector<const IPVolume*> m_pvMaPMTMasters;
  std::vector<const IPVolume*> m_pvMaPMTSMasters;
  std::vector<const IPVolume*> m_pvWindows;
  std::vector<const ISolid*>   m_windowSolids;
  std::vector<HepTransform3D>  m_vectorTransfMaPMT2s;

};

#endif    // DERICHMAPMTPANEL_H
