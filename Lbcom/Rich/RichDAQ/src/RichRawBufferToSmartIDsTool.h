
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool : RichRawBufferToSmartIDsTool
 *
 *  CVS Log :-
 *  $Id: RichRawBufferToSmartIDsTool.h,v 1.9 2006-01-25 10:38:32 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
#define RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichKernel/RichToolBase.h"

// RichDAQ utility classes
#include "RichHPDDataBank.h"

// from RichKernel
#include "RichKernel/RichSmartIDSorter.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/IRichRawDataFormatTool.h"

// event
#include "Event/RawBank.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

//-----------------------------------------------------------------------------
/** @class RichRawBufferToSmartIDsTool RichRawBufferToSmartIDsTool.h
 *
 *  Tool to create RichSmartIDs from the raw buffer.
 *  Uses the raw decoding tool and performs verious post processing tasks,
 *  such as sorting.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *
 *  @todo Add missing sorting and partitioning functionality for reconstruction
 */
//-----------------------------------------------------------------------------

class RichRawBufferToSmartIDsTool : public RichToolBase,
                                    virtual public IRichRawBufferToSmartIDsTool,
                                    virtual public IIncidentListener 
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichRawBufferToSmartIDsTool( const std::string& type,
                               const std::string& name,
                               const IInterface* parent );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from interface

  // Access all RichSmartIDs for the current Event
  const RichSmartID::Vector & allRichSmartIDs() const;

private: // private methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Fill the RichSmartID container
  void fillRichSmartIDs() const;

private: // private data

  /// Pointer to RICH raw data format tool
  const IRichRawDataFormatTool * m_rawFormatT;

  /// Flag to turn on the sorting of the RichSmartIDs
  bool m_sortIDs;

  /// RichSmartIDs
  mutable RichSmartID::Vector m_smartIDs;

  /// New event flag
  mutable bool m_newEvent;

  /// RichSmartID sorting utility object
  RichSmartIDSorter m_sorter;

};

inline void RichRawBufferToSmartIDsTool::InitNewEvent()
{
  m_newEvent = true;
  m_smartIDs.clear();
}

#endif // RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
