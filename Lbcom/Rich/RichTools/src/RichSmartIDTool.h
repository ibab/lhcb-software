// $Id: RichSmartIDTool.h,v 1.2 2004-07-15 15:44:40 jonrob Exp $
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
 *  A tool to preform the manipulation of RichSmartID channel identifiers
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

class RichSmartIDTool : public RichToolBase,
                        virtual public IRichSmartIDTool {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichSmartIDTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual ~RichSmartIDTool( ); ///< Destructor

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize  ();

public: // methods (and doxygen comments) inherited from interface

  // Converts a RichSmartID channel indentification into a position in
  virtual StatusCode globalPosition ( const RichSmartID& smartid, 
                                      HepPoint3D& position ) const;

  // Computes the global position coordinate for a given position in local
  virtual HepPoint3D globalPosition ( const HepPoint3D& localPoint, 
                                      const Rich::DetectorType rich, 
                                      const Rich::Side side ) const;

  // Converts a position in global coordinates to the corresponding
  virtual StatusCode smartID( const HepPoint3D& inPosition,
                              RichSmartID& outSmartID ) const;

  // Supplies a vector of all currently active and valid channels in the RICH detectors
  virtual StatusCode readoutChannelList ( std::vector<RichSmartID>& readoutChannels ) const;

  // Converts a position in global coordinates to the local coordinate system
  virtual HepPoint3D globalToPDPanel ( const HepPoint3D& globalPoint ) const;

private:

  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  boost::array<HPDPanelsPerRich, 2> m_photoDetPanels;

  typedef boost::array<double, 2> offsetPerRich;
  boost::array<offsetPerRich, 2> m_localOffset;

};
#endif // RICHDETTOOLS_RICHSMARTIDTOOL_H
