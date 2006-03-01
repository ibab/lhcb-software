
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusterSuppressionTool.h
 *
 *  Header file for tool : RichHPDPixelClusterSuppressionTool
 *
 *  CVS Log :-
 *  $Id: RichHPDPixelClusterSuppressionTool.h,v 1.1 2006-03-01 09:56:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHPDPixelClusterSuppressionTool_H
#define RICHDAQ_RichHPDPixelClusterSuppressionTool_H 1

// STD
#include <sstream>

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichKernel/RichToolBase.h"

// Interfaces
#include "RichKernel/IRichPixelSuppressionTool.h"

// numberings
#include "RichDet/RichDAQDefinitions.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

//-----------------------------------------------------------------------------
/** @class RichHPDPixelClusterSuppressionTool RichHPDPixelClusterSuppressionTool.h
 *
 *  Tool for monitoring high occupancy HPDs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichHPDPixelClusterSuppressionTool : public RichToolBase,
                                           virtual public IRichPixelSuppressionTool
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichHPDPixelClusterSuppressionTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Monitor the occupancy of the given HPD
  bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                              LHCb::RichSmartID::Vector & smartIDs ) const;

private: // private data

  /// Rich System detector element
  const DeRichSystem * m_richSys;

  /// Pointer to another suppression tool
  const IRichPixelSuppressionTool * m_overallCheck;

};

#endif // RICHDAQ_RichHPDPixelClusterSuppressionTool_H
