// $Id: RichDetector.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_RICHDETECTOR_H
#define RICHDETTOOLS_RICHDETECTOR_H 1

// Include files
#include <vector>

#include "RichDet/DeRichHPDPanel.h"
#include "Kernel/RichSmartID.h"

/** @class RichDetector RichDetector.h RichDetTools/RichDetector.h
 *
 *
 *  @author Artur Barczyk
 *  @date   18/05/2002
 */

// This class will act as the interface to the detection elements, aka pixels,
// thus representing such an element as HPD/MAPMT, or the whole panel of HPDs

class RichDetector {

public:

  typedef std::vector<RichDetector>                   Collection;
  typedef std::vector<RichDetector>::iterator         Iterator;
  typedef std::vector<RichDetector>::const_iterator   ConstIterator;
  typedef std::vector<RichDetector*>                  PointerCollection;
  typedef std::vector<RichDetector*>::iterator        PointerIterator;
  typedef std::vector<RichDetector*>::const_iterator  ConstPointerIterator;

  RichDetector( DeRichHPDPanel* dePDPanel )
    : m_dePDPanel(dePDPanel) { }

  ~RichDetector( ) {}; ///< Destructor

  /// Returns a SmartID given a global position
  StatusCode smartID( const HepPoint3D& globalPosition,
                      RichSmartID& pixelID );

  /// Returns a global position given a SmartID
  StatusCode detectionPoint( const RichSmartID& pixelID,
                             HepPoint3D& globalPosition );

  /// For a given direction from a point, returns the PD hit point and
  /// SmartID encoding the hit PD
  StatusCode PDWindowPoint( const HepPoint3D& position,
                            const HepVector3D& direction,
                            HepPoint3D& windowPoint,
                            RichSmartID& hpd,
                            DeRichHPDPanel::traceMode mode =
                            DeRichHPDPanel::window);

  /// For a given direction from a point, returns the intersecton with the
  /// detector plane. Mode controls the behaviour. Tight mode returns false
  /// if the point is outside the photodetector coverage area
  bool detPlanePoint( const HepPoint3D& position,
                      const HepVector3D& direction,
                      HepPoint3D& detPoint,
                      DeRichHPDPanel::traceMode mode = DeRichHPDPanel::loose);

  /// Returns the average detection plane for this arrary detector
  void detectionPlane( HepPlane3D& plane );

  inline StatusCode readoutChannelList( std::vector<RichSmartID>& channels );

protected:

  // Returns pointer to XML detector object
  DeRichHPDPanel* dePDPanel();

private:

  DeRichHPDPanel* m_dePDPanel;

};

inline DeRichHPDPanel* RichDetector::dePDPanel()
{
  return m_dePDPanel;
}

inline StatusCode
RichDetector::readoutChannelList( std::vector<RichSmartID>& channels )
{
  return dePDPanel()->readoutChannelList(channels);
}

inline StatusCode RichDetector::smartID ( const HepPoint3D& globalPosition,
                                          RichSmartID& pixelID )
{
  return dePDPanel()->smartID(globalPosition, pixelID);
}

inline StatusCode RichDetector::detectionPoint( const RichSmartID& pixelID,
                                                HepPoint3D& globalPosition )
{
  return dePDPanel()->detectionPoint(pixelID, globalPosition);
}

inline StatusCode RichDetector::PDWindowPoint( const HepPoint3D& position,
                                               const HepVector3D& direction,
                                               HepPoint3D& windowPoint,
                                               RichSmartID& hpd,
                                               DeRichHPDPanel::traceMode mode)
{
  return dePDPanel()->PDWindowPoint(direction, position, windowPoint,
                                    hpd, mode);
}

inline void RichDetector::detectionPlane(HepPlane3D& plane)
{
  plane = dePDPanel()->detectionPlane();
}

inline  bool RichDetector::detPlanePoint( const HepPoint3D& position,
                                          const HepVector3D& direction,
                                          HepPoint3D& detPoint,
                                          DeRichHPDPanel::traceMode mode )
{
  return dePDPanel()->detPlanePoint(position, direction, detPoint, mode);
}



#endif // RICHDETTOOLS_RICHDETECTOR_H
