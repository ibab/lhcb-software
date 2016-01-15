
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool : Rich::DAQ::RawBufferToSmartIDsTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
#define RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" // due to forward declaration in GaudiTool

// base class
#include "RichKernel/RichToolBase.h"

// Rich::DAQ utility classes
#include "RichHPDDataBank.h"

// Kernel
#include "RichKernel/RichHashMap.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/IRichRawDataFormatTool.h"

// RichDet
#include "RichDet/DeRichSystem.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class RawBufferToSmartIDsTool RichRawBufferToSmartIDsTool.h
     *
     *  Tool to create RichSmartIDs from the raw buffer.
     *  Uses the raw decoding tool and performs verious post processing tasks.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     *
     *  @todo Review if this tool is really needed
     */
    //-----------------------------------------------------------------------------

    class RawBufferToSmartIDsTool : public ToolBase,
                                    virtual public IRawBufferToSmartIDsTool,
                                    virtual public IIncidentListener
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      RawBufferToSmartIDsTool( const std::string& type,
                               const std::string& name,
                               const IInterface* parent );

      // Initialization of the tool after creation
      StatusCode initialize();

      /** Implement the handle method for the Incident service.
       *  This is used to inform the tool of software incidents.
       *
       *  @param incident The incident identifier
       */
      void handle( const Incident& incident );

    public: // methods (and doxygen comments) inherited from interface

      // Access all RichSmartIDs for the current Event
      const Rich::DAQ::L1Map & allRichSmartIDs() const;
      
      // Access the vector of RichSmartIDs for the given HPD identifier
      const LHCb::RichSmartID::Vector& richSmartIDs( const LHCb::RichSmartID hpdID,
                                                     const bool createIfMissing = true ) const;

      // Access all RichSmartIDs for the current Event
      const Rich::DAQ::L1Map & allRichSmartIDs( const IRawBufferToSmartIDsTool::RawEventLocations& taeLocs ) const;

      // Access the vector of RichSmartIDs for the given HPD identifier
      const LHCb::RichSmartID::Vector& richSmartIDs( const IRawBufferToSmartIDsTool::RawEventLocations& taeLocs,
                                                     const LHCb::RichSmartID hpdID,
                                                     const bool createIfMissing = true ) const;

      // Access the number of RICH hits in the given detector, in the current event, 
      unsigned int nTotalHits( const Rich::DetectorType rich = Rich::InvalidDetector ) const;

      // Access the number of RICH hits in the given detector, the current event, 
      unsigned int nTotalHits( const RawEventLocations& taeLocs,
                               const Rich::DetectorType rich = Rich::InvalidDetector ) const;
      
    private: // private methods

      /// Initialise for a new event
      void InitNewEvent();

      /// Make a unique string for a list of locations
      inline std::string taeKey( const IRawBufferToSmartIDsTool::RawEventLocations& taeLocs ) const
      {
        std::string loc;
        for ( const auto& L : taeLocs ) { loc += L; }
        return loc;
      }

      /// Find the given HPD data from the given L1Map
      const LHCb::RichSmartID::Vector& richSmartIDs( const LHCb::RichSmartID hpdID,
                                                     const Rich::DAQ::L1Map & data,
                                                     const bool createIfMissing ) const;

      /// Dummy vector of hits
      const LHCb::RichSmartID::Vector & dummyVector() const;

      /// Count the total number of hits in a L1Map
      unsigned int countTotalHits( const Rich::DAQ::L1Map & data,
                                   const Rich::DetectorType rich ) const;

    private: // private data

      /// Rich System detector element
      const DeRichSystem * m_richSys = nullptr;

      /// Pointer to RICH raw data format tool
      const IRawDataFormatTool * m_rawFormatT = nullptr;

      /// The decoded data for particular TAE event(s)
      mutable Rich::HashMap< const std::string, Rich::DAQ::L1Map > m_richDataTAE;

      /// Input location(s) for RawEvent in TES
      IRawDataFormatTool::RawEventLocations m_rawEventLocs;

      // Cached variables

      /// The TAE event key
      std::string m_taeKey;

    };

    inline void RawBufferToSmartIDsTool::InitNewEvent()
    {
      m_richDataTAE.clear();
    }

  }
}

#endif // RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
