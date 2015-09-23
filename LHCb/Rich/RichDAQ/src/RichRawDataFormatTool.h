
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.h
 *
 *  Header file for tool : Rich::DAQ::RawDataFormatTool
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
#include <set>

// Boost
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEventTimeDecoder.h"

// Base class
#include "RichKernel/RichToolBase.h"

// Kernel
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichHashMap.h"

// Interfaces
#include "RichKernel/IRichRawDataFormatTool.h"

// local
#include "RichDAQVersions.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichPoolMap.h"
#include "RichKernel/RichHashMap.h"

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

    public:

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
      void decodeToSmartIDs( const RawEventLocations & taeLocations,
                             Rich::DAQ::L1Map & decodedData ) const;

    private: // definitions

      /** @class L1IDandV RichRawDataFormatTool.h
       *
       *  Utility Class
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   01/06/2009
       */
      class L1IDandV
      {
      public:
        /// Default Constructor
        L1IDandV( )
          : bankVersion  ( Rich::DAQ::UndefinedBankVersion ) { }
        /// Constructor from arguments
        L1IDandV( const Rich::DAQ::BankVersion _version,
                  const Rich::DAQ::Level1HardwareID& _id )
          : bankVersion  ( _version ),
            l1HardwareID ( _id      ) { }
      public:
        /// Sorting operator
        inline bool operator < ( const L1IDandV& id ) const
        { return this->l1HardwareID < id.l1HardwareID ; }
      public:
        Rich::DAQ::BankVersion      bankVersion;  ///< Bank version
        Rich::DAQ::Level1HardwareID l1HardwareID; ///< L1 hardwareID
      };

      /** @class L1CountAndSize RichRawDataFormatTool.h
       *
       *  Utility Class
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   01/06/2009
       */
      class L1CountAndSize
      {
      public:
        /// Default Constructor
        L1CountAndSize( const unsigned long long _nHPDs  = 0,
                        const unsigned long long _nHits  = 0,
                        const unsigned long long _nWords = 0,
                        const unsigned long long _nFills = 0 )
          : nHPDs  ( _nHPDs  ),
            nHits  ( _nHits  ),
            nWords ( _nWords ),
            nFills ( _nFills ) { }
      public:
        unsigned long long nHPDs;  ///< Number of HPDs
        unsigned long long nHits;  ///< Number of Hits
        unsigned long long nWords; ///< Number of 32 bit words
        unsigned long long nFills; ///< Number of data entries
      };

      /// Summary data per L1 board
      typedef Rich::PoolMap< const L1IDandV, L1CountAndSize > L1TypeCount;

      /// Summary for each TAE location
      typedef Rich::HashMap< const std::string, L1TypeCount > L1TypeCountTAE;

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

      /** Creates a bank data from the given raw block of data
       *
       *  Uses static objects to avoid recreating.
       *
       *  @param dataStart Pointer to the start of the raw data
       *  @param dataSize  The length of the data block (excluding header HPD word)
       *  @param version   The RICH DAQ data bank version
       */
      const HPDDataBank * createDataBank_static( const Rich::DAQ::LongType * dataStart,
                                                 const unsigned int dataSize,
                                                 const Rich::DAQ::BankVersion version ) const;

      /// Initialise for each event
      void InitEvent();

      /// Finalise for each event
      void FinishEvent();

      /// Retrieves the raw event for the current TAE event
      LHCb::RawEvent * rawEvent() const;

      /** Retrieves the ODIN data object
       */
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
      template < class TYPE >
      bool isBitOn( const TYPE data, const Rich::DAQ::ShortType pos ) const;

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

      /// Decode a RawBank into RichSmartID identifiers
      /// MaPMT0 version
      void decodeToSmartIDs_MaPMT0( const LHCb::RawBank & bank,
                                      Rich::DAQ::L1Map & decodedData ) const;

      /// Print the given data word as Hex and as bits, to the given precision
      template < class TYPE >
      void rawDump( MsgStream & os,
                    const TYPE word,
                    const ShortType nBits = 32 ) const;

      /// Returns a default data map
      const Rich::DAQ::L1Map & dummyMap() const;

      /// Suppress hot pixels
      void suppressHotPixels( const LHCb::RichSmartID& hpdID,
                              LHCb::RichSmartID::Vector & newids ) const;

    private: // data

      /// Rich System detector element
      const DeRichSystem * m_richSys;

      /// Pointer to Raw Event
      mutable Rich::HashMap< const std::string, LHCb::RawEvent * > m_rawEvent;

      /// Pointer to ODIN
      mutable Rich::HashMap< const std::string, LHCb::ODIN * > m_odin;

      /// Current TAE type
      mutable std::string m_currentTAE;

      /// Pointer to ODIN (Event time) tool
      mutable const IEventTimeDecoder * m_timeTool;

      /// The number of hits marking the transistion between zero and non-zero suppressed data
      /// Used by version 0 of the data banks
      unsigned int m_zeroSuppresCut;

      /// Flag to turn on and off the summary information
      bool m_summary;

      mutable L1TypeCountTAE m_l1decodeSummary; ///< Summary object for decoding
      mutable L1TypeCount m_l1encodeSummary; ///< Summary object for encoding

      /// Number of events processed
      mutable unsigned long long m_evtCount;

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

      /// Turn on/off Event ID integrity checks between the RICH ingress headers and the ODIN
      bool m_checkODINEventsIDs;

      /// Turn on/off internal RICH Event ID integrity checks between the ingress and HPD header words
      bool m_checkRICHEventsIDs;

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
      mutable Rich::Map<Rich::DAQ::Level1HardwareID,unsigned int> m_l1IdsDecoded;

      /** Turn on/off detailed error messages.
       *
       *  VERY verbose in case of frequent errors
       */
      bool m_verboseErrors;

      /// Perform check of HPD L1 hardward ID to that expected by the DB
      bool m_hpdL1check;

      /// Flag to turn on/off decoding of each RICH detector (default is both on)
      std::vector<bool> m_richIsActive;

      /** Flag to turn on/off the purging off data from HPDs that fail the data
       *  integrity checks (default is on) */
      bool m_purgeHPDsFailIntegrity;

      typedef std::vector<LHCb::RichSmartID::KeyType> HotPixelListType;

      /** Software suppression of hot channels. List of RichSmartIDs (as unsigned ints) 
       * to suppress in the data.
       */
      HotPixelListType m_hotChannels;

      /// Boolean to indicate if there are any pixels that need suppressing
      bool m_pixelsToSuppress;

      /// Type for Storage of pixels to mask for each HPD
      typedef Rich::HashMap< LHCb::RichSmartID, std::set<LHCb::RichSmartID> > HPDHotPixels;

      /// Storage of pixels to mask for each HPD
      HPDHotPixels m_hotPixels;

      /// List of locations in the transient store to search the RawEvent object.
      std::vector<std::string> m_rawEventLocations;

    };

    inline void RawDataFormatTool::InitEvent()
    {
      m_rawEvent.clear();
      m_odin.clear();
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

    template < class TYPE >
    inline bool
    RawDataFormatTool::isBitOn( const TYPE data, const Rich::DAQ::ShortType pos ) const
    {
      return ( 0 != (data & (1<<pos)) );
    }

    inline const IEventTimeDecoder * RawDataFormatTool::timeTool() const
    {
      if (!m_timeTool) { acquireTool( "OdinTimeDecoder", m_timeTool ); }
      return m_timeTool;
    }

    inline const LHCb::ODIN * RawDataFormatTool::odin() const
    {
      LHCb::ODIN *& odin = m_odin[m_currentTAE];
      if ( !odin )
      {
        timeTool()->getTime(); // Needed to make sure ODIN object is in TES (Strange but true)
        odin = get<LHCb::ODIN>( m_currentTAE+LHCb::ODINLocation::Default );
      }
      return odin;
    }

    // Print the given data word as Hex and as bits, to the given precision
    template < class TYPE >
    inline void RawDataFormatTool::rawDump( MsgStream & os,
                                            const TYPE word,
                                            const ShortType nBits ) const
    {
      std::ostringstream hexW;
      hexW << std::hex << word;
      std::string tmpW = hexW.str();
      if ( tmpW.size() < 8 ) { tmpW = std::string(8-tmpW.size(),'0')+tmpW; }
      os << tmpW << "  |";
      for ( int iCol = nBits-1; iCol >= 0; --iCol )
      {
        os << "  " << isBitOn( word, iCol );
      }
    }

  }
}

#endif // RICHDAQ_RICHRAWDATAFORMATTOOL_H
