
/** @file RichSmartIDTool.h
 *
 *  Header file for tool : RichSmartIDTool
 *
 *  CVS Log :-
 *  $Id: RichSmartIDTool.h,v 1.7 2005-01-07 13:24:31 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.6  2004/10/27 14:41:03  jonrob
 *  Various updates
 *
 *  Revision 1.5  2004/10/12 09:58:26  papanest
 *  speed up globaltoPDpanel
 *
 *  Revision 1.4  2004/07/26 18:03:05  jonrob
 *  Various improvements to the doxygen comments
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

#ifndef RICHTOOLS_RICHSMARTIDTOOL_H
#define RICHTOOLS_RICHSMARTIDTOOL_H 1

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
  virtual HepPoint3D globalPosition ( const RichSmartID& smartid ) const;

  // Computes the global position coordinate for a given position in local
  virtual HepPoint3D globalPosition ( const HepPoint3D& localPoint, 
                                      const Rich::DetectorType rich, 
                                      const Rich::Side side ) const;

  // Converts a position in global coordinates to the corresponding RichSmartID
  virtual StatusCode smartID( const HepPoint3D&  globalPoint,
                              RichSmartID& smartid ) const;

  // Supplies a vector of all currently active and valid channels in the RICH detectors
  virtual const RichSmartID::Collection & readoutChannelList( ) const;

  /** Converts a position in global coordinates to the local coordinate system.
   *  The panel Rich::Left has positive x and panel Rich::Right has negative 
   *  x coordinates. Only the x and y coordinates are valid. z coordinate 
   *  is set to 0.
   */
  virtual HepPoint3D globalToPDPanel ( const HepPoint3D& globalPoint ) const;

private:

  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  boost::array<HPDPanelsPerRich, 2> m_photoDetPanels;

  typedef boost::array<double, 2> offsetPerRich;
  boost::array<offsetPerRich, 2> m_localOffset;

};
#endif // RICHDETTOOLS_RICHSMARTIDTOOL_H
