
//-----------------------------------------------------------------------------
/** @file IRichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool interface : Rich::DAQ::IRawBufferToSmartIDsTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHRAWBUFFERTOSMARTIDSTOOL_H
#define RICHKERNEL_IRICHRAWBUFFERTOSMARTIDSTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"

// RichKernel
#include "RichKernel/RichDecodedData.h"

/// Static Interface Identification
static const InterfaceID IID_IRichRawBufferToSmartIDsTool( "Rich::DAQ::IRawBufferToSmartIDsTool", 1, 0 );

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class IRawBufferToSmartIDsTool RichKernel/IRichRawBufferToSmartIDsTool.h
     *
     *  Interface for tool for decoding the Rich Raw Buffer
     *
     *  Provides back the raw data in a decoded format.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class IRawBufferToSmartIDsTool : public virtual IAlgTool
    {

    public:

      /// Type for Input location for RawEvent in TES
      using RawEventLocation  = std::string;
      /// Type for Input location(s) for RawEvent in TES
      using RawEventLocations = std::vector<RawEventLocation>;

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichRawBufferToSmartIDsTool; }

      /** Access all RichSmartIDs, for all configured TAE spills
       *  @return Reference to the decoded RICH data structure
       */
      virtual const Rich::DAQ::L1Map & allRichSmartIDs() const = 0;

      /** Access the vector of RichSmartIDs for the given HPD identifier, for all configured TAE spills
       *  @param hpdID The HPD identifier
       *  @param createIfMissing If data is missing for the given HPD, create the entry in the L1Map
       *  @return The vector of hits (RichSmartIDs) for the given HPD
       */
      virtual const LHCb::RichSmartID::Vector& richSmartIDs( const LHCb::RichSmartID hpdID,
                                                             const bool createIfMissing = true ) const = 0;

      /** Access all the RichSmartIDs, for the given list of TAE spills
       *  @param taeLocs The TAE location(s) ( "" for main event, "Prev1" for -25 ns etc. )
       *  @return Reference to the decoded RICH data structure
       */
      virtual const Rich::DAQ::L1Map & allRichSmartIDs( const RawEventLocations& taeLocs ) const = 0;

      /** Access the vector of RichSmartIDs for the given HPD identifier, for the given list of TAE spills
       *  @param taeLocs The TAE location(s) ( "" for main event, "Prev1" for -25 ns etc. )
       *  @param hpdID   The HPD identifier
       *  @param createIfMissing If data is missing for the given HPD, create the entry in the L1Map
       *  @return The vector of hits (RichSmartIDs) for the given HPD
       */
      virtual const LHCb::RichSmartID::Vector& richSmartIDs( const RawEventLocations& taeLocs,
                                                             const LHCb::RichSmartID hpdID,
                                                             const bool createIfMissing = true ) const = 0;

      /** Access all RichSmartIDs for the given TAE spill
       *  @param taeLoc The TAE location ( "" for main event, "Prev1" for -25 ns etc. )
       *  @return Reference to the decoded RICH data structure
       */
      inline const Rich::DAQ::L1Map & allRichSmartIDs( const RawEventLocation& taeLoc ) const
      {
        return this->allRichSmartIDs ( RawEventLocations(1,taeLoc) );
      }

      /** Access the vector of RichSmartIDs for the given HPD identifier, for the given TAE spill
       *  @param taeLoc The TAE location ( "" for main event, "Prev1" for -25 ns etc. )
       *  @param hpdID  The HPD identifier
       *  @param createIfMissing If data is missing for the given HPD, create the entry in the L1Map
       *  @return The vector of hits (RichSmartIDs) for the given HPD
       */
      inline const LHCb::RichSmartID::Vector& richSmartIDs( const RawEventLocation& taeLoc,
                                                            const LHCb::RichSmartID hpdID,
                                                            const bool createIfMissing = true ) const
      {
        return this->richSmartIDs ( RawEventLocations(1,taeLoc), hpdID, createIfMissing );
      }

      /** Access the number of RICH hits in the given detector, in the current event, 
       *  for all configured TAE spills
       *  @param rich The RICH detector (Rich::InvalidDetector means count both)
       *  @return The total number of hits for the configured TAE locations and the given RICH
       */
      virtual unsigned int nTotalHits( const Rich::DetectorType rich = Rich::InvalidDetector ) const = 0;

      /** Access the number of RICH hits in the given detector, the current event, 
       *  for the given list of TAE spills
       *  @param taeLocs The TAE location(s) ( "" for main event, "Prev1" for -25 ns etc. )
       *  @param rich The RICH detector (Rich::InvalidDetector means count both)
       *  @return The total number of hits for the configured TAE locations
       */
      virtual unsigned int nTotalHits( const RawEventLocations& taeLocs,
                                       const Rich::DetectorType rich = Rich::InvalidDetector ) const = 0;

      /** Access the number of RICH hits in the given RICH detecotr, in the current event, 
       *  for the given TAE spill
       *  @param taeLoc The TAE location ( "" for main event, "Prev1" for -25 ns etc. )
       *  @param rich The RICH detector (Rich::InvalidDetector means count both)
       *  @return The total number of hits for the configured TAE location
       */
      inline unsigned int nTotalHits( const RawEventLocation& taeLoc,
                                      const Rich::DetectorType rich = Rich::InvalidDetector )
      {
        return this->nTotalHits ( RawEventLocations(1,taeLoc), rich );
      }

    };

  }
}

#endif // RICHKERNEL_IRICHRAWBUFFERTOSMARTIDSTOOL_H
