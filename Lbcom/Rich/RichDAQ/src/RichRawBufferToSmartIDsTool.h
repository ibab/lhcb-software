
/** @file RichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool : RichRawBufferToSmartIDsTool
 *
 *  CVS Log :-
 *  $Id: RichRawBufferToSmartIDsTool.h,v 1.1 2004-10-30 19:13:05 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
#define RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichKernel/RichToolBase.h"

// RichDAQ utility classes
#include "RichDAQDefinitions.h"
#include "RichDAQHeaderPD.h"
#include "RichZSHitTriplet.h"
#include "RichDAQLinkNumber.h"
#include "RichNonZeroSuppData.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

// CLHEP
//#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRawBufferToSmartIDsTool RichRawBufferToSmartIDsTool.h
 *
 *  Tool performing MC truth associations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRawBufferToSmartIDsTool : public RichToolBase,
                                    virtual public IRichRawBufferToSmartIDsTool,
                                    virtual public IIncidentListener {

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

private: // definitions


private: // private methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Retrieves the raw event. If not available tries to build one from RawBuffer
  RawEvent * rawEvent() const;

  /// Fill the RichSmartID container
  void fillRichSmartIDs() const;

  /// Decode a zero suppress data block
  void decodeZeroSuppressedBank( const RawBank & bank ) const;

  /// Decode a non-zero suppress data block
  void decodeNonZeroSuppressedBank( const RawBank & bank ) const;

private: // private data

  /// Input location for RawEvent in TES
  std::string m_rawEventLoc;

  /// RichSmartIDs
  mutable RichSmartID::Vector m_smartIDs;

  /// New event flag
  mutable bool m_newEvent;

  /// Pointer to Raw Event
  mutable RawEvent * m_rawEvent;

};

inline void RichRawBufferToSmartIDsTool::InitNewEvent()
{
  m_newEvent = true;
  m_rawEvent = 0;
}

#endif // RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
