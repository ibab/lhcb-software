// $Id: RichSmartIDTool.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_RICHSMARTIDTOOL_H
#define RICHDETTOOLS_RICHSMARTIDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichSmartIDTool.h"

// from RichKernel
#include "RichKernel/BoostArray.h"

// RichDet
#include "RichDet/DeRichHPDPanel.h"


/** @class RichSmartIDTool RichSmartIDTool.h
 *
 *  A tool to preform the manipulation of RichSmartIDs channel identifiers
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

class RichSmartIDTool : public RichToolBase,
                        virtual public IRichSmartIDTool {
public:
  /// Standard constructor
  RichSmartIDTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual ~RichSmartIDTool( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  /// Returns the position of a SmartID in global coordinates
  virtual StatusCode globalPosition ( const RichSmartID& inSmartID,
                                      HepPoint3D& outPosition) const;

  /// Returns the global position given a local position, rich and panel number
  virtual HepPoint3D globalPosition ( const HepPoint3D& localPoint,
                                      const Rich::DetectorType rich,
                                      const Rich::Side side ) const;

  /// Returns the SmartID for a given global position
  virtual StatusCode smartID ( const HepPoint3D& inPosition,
                               RichSmartID& outSmartID) const;

  /// Returns a list with all valid smartIDs
  virtual StatusCode readoutChannelList(std::vector<RichSmartID>&
                                        readoutChannels ) const;

  /// Converts a global position to the coordinate system of the
  /// photodetector panel
  virtual HepPoint3D globalToPDPanel( const HepPoint3D& globalPoint ) const;

protected:

private:

  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  boost::array<HPDPanelsPerRich, 2> m_photoDetPanels;

  typedef boost::array<double, 2> offsetPerRich;
  boost::array<offsetPerRich, 2> m_localOffset;
};
#endif // RICHDETTOOLS_RICHSMARTIDTOOL_H
