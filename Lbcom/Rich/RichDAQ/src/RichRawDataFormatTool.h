
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.h
 *
 *  Header file for tool : RichRawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: RichRawDataFormatTool.h,v 1.15 2006-07-14 13:32:03 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHRAWDATAFORMATTOOL_H
#define RICHDAQ_RICHRAWDATAFORMATTOOL_H 1

// STD
#include <sstream>

// Boost
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// Base class
#include "RichKernel/RichToolBase.h"

// Kernel
#include "RichKernel/RichStatDivFunctor.h"

// Interfaces
#include "RichKernel/IRichRawDataFormatTool.h"

// local
#include "RichNonZeroSuppData.h"
#include "RichNonZeroSuppALICEData.h"
#include "RichZeroSuppData.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Event model
#include "Event/RawEvent.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

//-----------------------------------------------------------------------------
/** @class RichRawDataFormatTool RichRawDataFormatTool.h
 *
 *  Tool to encode and decode the Raw Event information for the RICH.
 *
 *  The following versions are included :-
 *
 *   Version 0 : DC04 compatibile. Can only decode this version, not encode
 *
 *   Version 1 : Same HPD bank format as version 0, but corrected header word bug
 *               plus incorporate Level 1 board groupings
 *
 *   Version 2 : Same as 1 but using a new zero suppressed HPD bank format
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 *
 *  @todo Remove DC04 hacks when no longer needed
 */
//-----------------------------------------------------------------------------

class RichRawDataFormatTool : public RichToolBase,
                              virtual public IRichRawDataFormatTool,
                              virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichRawDataFormatTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// Destructor
  virtual ~RichRawDataFormatTool( );

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

  // Creates a bank data from a vector of RichSmartIDs
  void fillRawEvent( const LHCb::RichSmartID::Vector & smartIDs,
                     const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const;

  // Decode a RawBank into RichSmartID identifiers
  void decodeToSmartIDs( const RawBank & bank,
                         RichDAQ::PDMap & smartIDs ) const;

  // Decode all RICH RawBanks into RichSmartID identifiers
  void decodeToSmartIDs( RichDAQ::PDMap & smartIDs ) const;

private: // definitions

  // Summary data
  typedef std::pair< const RichDAQ::BankVersion, RichDAQ::Level1ID >            L1IDandV;
  typedef std::pair< unsigned long, std::pair< unsigned long, unsigned long > > L1CountAndSize;
  typedef Rich::Map< const L1IDandV, L1CountAndSize > L1TypeCount;

private: // methods

  /** Creates a bank data of a given version from the given RichSmartID vector
   *
   *  NOTE : Ownership of the data object passes to the caller.
   *         It is their responsibility to delete when no longer needed.
   *
   *  @param smartIDs Vector of RichSmartIDs to use to create the data bank
   *  @param version   The RICH DAQ data bank version
   */
  const RichHPDDataBank * createDataBank( const RichSmartID::Vector & smartIDs,
                                          const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const;

  /** Creates a bank data from the given raw block of data
   *
   *  NOTE : Ownership of the data object passes to the caller.
   *         It is their responsibility to delete when no longer needed.
   *
   *  @param dataStart Pointer to the start of the raw data
   *  @param dataSize  The length of the data block (excluding header HPD word)
   *  @param version   The RICH DAQ data bank version
   */
  const RichHPDDataBank * createDataBank( const RichDAQ::LongType * dataStart,
                                          const unsigned int dataSize,
                                          const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const;

  /// Initialise for each event
  void InitEvent();

  /// Finalise for each event
  void FinishEvent();

  /// Retrieves the raw event.
  RawEvent * rawEvent() const;

  /** Final printout of Level 1 stats
   *
   *  @param count The Summary object to printout
   *  @param title The title to use
   */
  void printL1Stats( const L1TypeCount & count, const std::string & title ) const;

  /// Returns the RawBank version emun for the given bank
  RichDAQ::BankVersion bankVersion( const LHCb::RawBank & bank ) const;

  /** Print the given RawBank as a simple hex dump
   *  @param bank The RawBank to dump out
   *  @param os   The Message Stream to print to
   */
  void dumpRawBank( const LHCb::RawBank & bank,
                    MsgStream & os ) const;

  /// Test if a given bit in a word is set on
  bool
  isBitOn( const RichDAQ::LongType data, const RichDAQ::ShortType pos ) const;

private: // data

  /// Rich System detector element
  const DeRichSystem * m_richSys;

  /// Pointer to Raw Event
  mutable RawEvent * m_rawEvent;

  /// Input location for RawEvent in TES
  std::string m_rawEventLoc;

  /// The number of hits marking the transistion between zero and non-zero suppressed data
  /// Used by version 0 of the data banks
  unsigned int m_zeroSuppresCut;

  /// Flag to turn on and off the summary information
  bool m_summary;

  mutable L1TypeCount m_l1decodeSummary; ///< Summary object for decoding
  mutable L1TypeCount m_l1encodeSummary; ///< Summary object for encoding

  /// Number of events processed
  mutable unsigned int m_evtCount;

  /// Flag to indicate if the tool has been used in a given event
  mutable bool m_hasBeenCalled;

  /// starting map
  RichDAQ::L1Map m_dummyMap;

  /// Max HPD Occupancy Cut
  unsigned int m_maxHPDOc;

  /// Flag to activate the raw printout of each Rawbank
  bool m_dumpBanks;

};

inline void RichRawDataFormatTool::InitEvent()
{
  m_rawEvent = 0;
  m_hasBeenCalled = false;
}

inline void RichRawDataFormatTool::FinishEvent()
{
  if ( m_hasBeenCalled ) ++m_evtCount;
}

inline RichDAQ::BankVersion
RichRawDataFormatTool::bankVersion( const LHCb::RawBank & bank ) const
{
  return static_cast< RichDAQ::BankVersion > ( bank.version() );
}

inline bool
RichRawDataFormatTool::isBitOn( const RichDAQ::LongType data, const RichDAQ::ShortType pos ) const
{
  return ( 0 != (data & (1<<pos)) );
}

#endif // RICHDAQ_RICHRAWDATAFORMATTOOL_H
