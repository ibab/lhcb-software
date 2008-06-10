
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.h
 *
 *  Header file for tool : Rich::DAQ::RawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: RichRawDataFormatTool.h,v 1.29 2008-06-10 16:17:33 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHRAWDATAFORMATTOOL_H
#define RICHDAQ_RICHRAWDATAFORMATTOOL_H 1

// STD
#include <sstream>
#include <memory>

// Boost
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" // due to forward declaration in GaudiTool

// Base class
#include "RichKernel/RichToolBase.h"

// Kernel
#include "RichKernel/RichStatDivFunctor.h"

// Interfaces
#include "RichKernel/IRichRawDataFormatTool.h"
#include "Kernel/IEventTimeDecoder.h"

// local
#include "RichDAQVersions.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Event model
#include "Event/RawEvent.h"
#include "Event/ODIN.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class RawDataFormatTool RichRawDataFormatTool.h
     *
     *  Tool to encode and decode the Raw Event information for the RICH.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-18
     *
     *  @todo Remove backwards compatibility hacks when no longer needed
     *  @todo Pass Event ID number into data format when filling RAWBank from RichSmartIDs
     *  @todo Add ability to create ALICE mode data during filling RAWBank from RichSmartIDs
     *  @todo Make sure each L1 bank always has four ingress headers during encoding
     */
    //-----------------------------------------------------------------------------

    class RawDataFormatTool : public Rich::ToolBase,
                              virtual public IRawDataFormatTool,
                              virtual public IIncidentListener
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      RawDataFormatTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      /// Destructor
      virtual ~RawDataFormatTool( );

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

      /// Creates a bank data from a vector of RichSmartIDs
      void fillRawEvent( const LHCb::RichSmartID::Vector & smartIDs,
                         const Rich::DAQ::BankVersion version = Rich::DAQ::LHCb2 ) const;

      /// Decode all RICH RawBanks into RichSmartID identifiers
      void decodeToSmartIDs( Rich::DAQ::L1Map & decodedData ) const;

    private: // definitions

      // Summary data
      typedef std::pair< const Rich::DAQ::BankVersion, Rich::DAQ::Level1ID >            L1IDandV;
      typedef std::pair< unsigned long, std::pair< unsigned long, unsigned long > > L1CountAndSize;
      typedef Rich::Map< const L1IDandV, L1CountAndSize > L1TypeCount;

    private: // methods

      /// Decode a RawBank into RichSmartID identifiers
      void decodeToSmartIDs( const LHCb::RawBank & bank,
                             Rich::DAQ::L1Map & decodedData ) const;

      /** Creates a bank data of a given version from the given RichSmartID vector
       *
       *  NOTE : Ownership of the data object passes to the caller.
       *         It is their responsibility to delete when no longer needed.
       *
       *  @param smartIDs Vector of RichSmartIDs to use to create the data bank
       *  @param version  The RICH DAQ data bank version
       *  @param odin     Pointer to the ODIN data object
       */
      const HPDDataBank * createDataBank( const LHCb::RichSmartID::Vector & smartIDs,
                                          const Rich::DAQ::BankVersion version,
                                          const LHCb::ODIN * odin = NULL ) const;

      /** Creates a bank data from the given raw block of data
       *
       *  NOTE : Ownership of the data object passes to the caller.
       *         It is their responsibility to delete when no longer needed.
       *
       *  @param dataStart Pointer to the start of the raw data
       *  @param dataSize  The length of the data block (excluding header HPD word)
       *  @param version   The RICH DAQ data bank version
       */
      const HPDDataBank * createDataBank( const Rich::DAQ::LongType * dataStart,
                                          const unsigned int dataSize,
                                          const Rich::DAQ::BankVersion version ) const;

      /// Initialise for each event
      void InitEvent();

      /// Finalise for each event
      void FinishEvent();

      /// Retrieves the raw event.
      LHCb::RawEvent * rawEvent() const;

      /// Retrieves the ODIN data object
      const LHCb::ODIN * odin() const;

      /// Get the ODIN time tool
      const IEventTimeDecoder * timeTool() const;

      /** Final printout of Level 1 stats
       *
       *  @param count The Summary object to printout
       *  @param title The title to use
       */
      void printL1Stats( const L1TypeCount & count, const std::string & title ) const;

      /// Returns the RawBank version emun for the given bank
      Rich::DAQ::BankVersion bankVersion( const LHCb::RawBank & bank ) const;

      /** Print the given RawBank as a simple hex dump
       *  @param bank The RawBank to dump out
       *  @param os   The Message Stream to print to
       */
      void dumpRawBank( const LHCb::RawBank & bank,
                        MsgStream & os ) const;

      /// Test if a given bit in a word is set on
      bool isBitOn( const Rich::DAQ::LongType data, const Rich::DAQ::ShortType pos ) const;

      /// Decode a RawBank into RichSmartID identifiers
      /// Version with DC06 and DC04 compatibility
      void decodeToSmartIDs_DC0406( const LHCb::RawBank & bank,
                                    Rich::DAQ::L1Map & decodedData ) const;

      /// Decode a RawBank into RichSmartID identifiers
      /// Version compatible with 2006 testbeam
      void decodeToSmartIDs_2006TB( const LHCb::RawBank & bank,
                                    Rich::DAQ::L1Map & decodedData ) const;

      /// Decode a RawBank into RichSmartID identifiers
      /// Version compatible with first 2007 "final" L1 firmware
      void decodeToSmartIDs_2007( const LHCb::RawBank & bank,
                                  Rich::DAQ::L1Map & decodedData ) const;

      /// Print the given data word as Hex and as bits, to the given precision
      void rawDump( MsgStream & os, 
                    const LongType word,
                    const ShortType nBits = 32 ) const;

      /// Returns a default data map
      const Rich::DAQ::L1Map & dummyMap() const;

    private: // data

      /// Rich System detector element
      const DeRichSystem * m_richSys;

      /// Pointer to Raw Event
      mutable LHCb::RawEvent * m_rawEvent;

      /// Pointer to ODIN
      mutable LHCb::ODIN * m_odin;

      /// Pointer to ODIN (Event time) tool
      mutable const IEventTimeDecoder * m_timeTool;

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

      /// Max HPD Occupancy Cut
      unsigned int m_maxHPDOc;

      /// Flag to activate the raw printout of each Rawbank
      bool m_dumpBanks;

      /// Flag to turn on the use of zero suppression during encoding simulation (default)
      bool m_zeroSupp;

      /// Flag to turn the use of the extended HPD data format during encoding
      bool m_extendedFormat;

      /// Flag to turn on/off the use of the ODIN data bank during decoding for integrity checks
      bool m_decodeUseOdin;

      /// Turn on/off Event ID integrity checks
      bool m_checkEventsIDs;

      /// Turn on/off BX ID integrity checks
      bool m_checkBxIDs;

      /// Turn on/off data integrity checks
      bool m_checkDataIntegrity;

      /** @brief Turns on the use of a fake HPD RichSmartID for each HPD data data block.
       *
       *  Useful for deep debugging cases when the HPD L0 ID is missing in the database.
       *
       *  @attention If set to true, decoded data is not complete (RICH,HPD panel and HPD info is missing).
       *             Consequently, this option should only be used to test the data decoding and not if the
       *             RichSmartIDs are needed for analysis downstream.
       */
      bool m_useFakeHPDID;

      /// Map of the number of time each L1 board ID is found per event (debugging variable)
      mutable Rich::Map<Rich::DAQ::Level1ID,unsigned int> m_l1IdsDecoded;

      /** Turn on/off detailed error messages.
       *
       *  VERY verbose in case of frequent errors
       */
      bool m_verboseErrors;

    };

    inline void RawDataFormatTool::InitEvent()
    {
      m_rawEvent = NULL;
      m_odin     = NULL;
      m_hasBeenCalled = false;
    }

    inline void RawDataFormatTool::FinishEvent()
    {
      if ( m_hasBeenCalled ) ++m_evtCount;
    }

    inline Rich::DAQ::BankVersion
    RawDataFormatTool::bankVersion( const LHCb::RawBank & bank ) const
    {
      return static_cast< Rich::DAQ::BankVersion > ( bank.version() );
    }

    inline bool
    RawDataFormatTool::isBitOn( const Rich::DAQ::LongType data, const Rich::DAQ::ShortType pos ) const
    {
      return ( 0 != (data & (1<<pos)) );
    }

    inline LHCb::RawEvent * RawDataFormatTool::rawEvent() const
    {
      if ( !m_rawEvent )
      {
        m_rawEvent = get<LHCb::RawEvent>( m_rawEventLoc );
      }
      return m_rawEvent;
    }

    inline const LHCb::ODIN * RawDataFormatTool::odin() const
    {
      if ( !m_odin )
      {
        timeTool()->getTime();
        m_odin = get<LHCb::ODIN>( LHCb::ODINLocation::Default );
      }
      return m_odin;
    }

    inline const IEventTimeDecoder * RawDataFormatTool::timeTool() const
    {
      if (!m_timeTool) acquireTool( "OdinTimeDecoder", m_timeTool );
      return m_timeTool;
    }

  }
}

#endif // RICHDAQ_RICHRAWDATAFORMATTOOL_H
