
//-----------------------------------------------------------------------------
/** @file RichHighOccHPDSuppressionTool.h
 *
 *  Header file for tool : RichHighOccHPDSuppressionTool
 *
 *  CVS Log :-
 *  $Id: RichHighOccHPDSuppressionTool.h,v 1.1 2006-03-01 09:56:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHighOccHPDSuppressionTool_H
#define RICHDAQ_RichHighOccHPDSuppressionTool_H 1

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
/** @class RichHighOccHPDSuppressionTool RichHighOccHPDSuppressionTool.h
 *
 *  Tool for monitoring high occupancy HPDs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichHighOccHPDSuppressionTool : public RichToolBase,
                                      virtual public IRichPixelSuppressionTool
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichHighOccHPDSuppressionTool( const std::string& type,
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

private: // private methods


private: // private data

  /// Rich System detector element
  const DeRichSystem * m_richSys;

  /// Occupancy map
  typedef std::pair<long unsigned,double> Data;
  typedef Rich::HashMap< const LHCb::RichSmartID, Data > OccMap;
  mutable OccMap m_occMap;

  // Min number fills before applying occupancy cut
  unsigned int m_minFills;

  // absolute max HPD occupancy
  unsigned int m_overallMax;

  /// Occupancy scale cut
  double m_scale;

  /// Memory fraction
  double m_memory;

};

#endif // RICHDAQ_RichHighOccHPDSuppressionTool_H
