
//=============================================================================================
/** @file RichDAQL1IngressHeader.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::L1IngressHeader
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   19/01/2007
 */
//=============================================================================================

#ifndef RICHDAQ_RichDAQL1IngressHeader_H
#define RICHDAQ_RichDAQL1IngressHeader_H 1

// STD
#include <bitset>

// Gaudi
#include "GaudiKernel/GaudiException.h"

// Event Model
#include "Event/ODIN.h"

// RichKernel
#include "RichKernel/RichDAQDefinitions.h"

//-----------------------------------------------------------------------------

namespace Rich
{
  namespace DAQ
  {

    //=============================================================================================
    /** @class L1IngressHeader RichDAQL1IngressHeader.h
     *
     *  The L1 Ingress header
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   19/01/2007
     */
    //=============================================================================================

    class L1IngressHeader
    {

    public: // Bit packing information

      // Number of bits for each data field in the word
      static const ShortType BitsEventID        = 8;  ///< Number of bits for Event ID
      static const ShortType BitsBXID           = 8;  ///< Number of bits for BX ID
      static const ShortType BitsActiveHPDs     = 12; ///< Number of bits for Active HPD flags
      static const ShortType BitsIngressID      = 2;  ///< Number of bits for ingress number
      static const ShortType BitsHPDsSuppressed = 1;  ///< Number of bits for supression flag

    private:

      // The shifts
      static const ShortType ShiftEventID        = 0;
      static const ShortType ShiftBXID           = ShiftEventID    + BitsEventID;
      static const ShortType ShiftActiveHPDs     = ShiftBXID       + BitsBXID;
      static const ShortType ShiftIngressID      = ShiftActiveHPDs + BitsActiveHPDs;
      static const ShortType ShiftHPDsSuppressed = ShiftIngressID  + BitsIngressID;

      // The masks
      static const ShortType MaskEventID         = (ShortType) ((1 << BitsEventID)-1)        << ShiftEventID;
      static const ShortType MaskBXID            = (ShortType) ((1 << BitsBXID)-1)           << ShiftBXID;
      static const ShortType MaskActiveHPDs      = (ShortType) ((1 << BitsActiveHPDs)-1)     << ShiftActiveHPDs ;
      static const ShortType MaskIngressID       = (ShortType) ((1 << BitsIngressID)-1)      << ShiftIngressID ;
      static const ShortType MaskHPDsSuppressed  = (ShortType) ((1 << BitsHPDsSuppressed)-1) << ShiftHPDsSuppressed;

      // the max values storable
      static const ShortType MaxEventID          = (ShortType) ( 1 << BitsEventID        ) - 1;
      static const ShortType MaxBXID             = (ShortType) ( 1 << BitsBXID           ) - 1;
      static const ShortType MaxActiveHPDs       = (ShortType) ( 1 << BitsActiveHPDs     ) - 1;
      static const ShortType MaxIngressID        = (ShortType) ( 1 << BitsIngressID      ) - 1;
      static const ShortType MaxHPDsSuppressed   = (ShortType) ( 1 << BitsHPDsSuppressed ) - 1;

    public:

      /// Default Constructor
      L1IngressHeader() = default;

      /// Constructor from raw LongType
      explicit L1IngressHeader( const LongType data  ) : m_data(data) { }

      /// Destructor
      ~L1IngressHeader( ) = default;

      /// Retrieve the full value
      inline LongType data() const noexcept { return m_data; }

    public: // getters

      /// Read data from ODIN
      inline void readFromODIN( const LHCb::ODIN * odin )
      {
        // set the event and bxID bits
        setEventID ( EventID ( odin ? odin->eventNumber() : 0 ) );
        setBXID    ( BXID    ( odin ? odin->orbitNumber() : 0 ) );
      }

      /// Access the Event ID
      inline EventID eventID() const
      {
        return EventID( ((data() & MaskEventID) >> ShiftEventID), BitsEventID );
      }

      /// Access the BX ID
      inline BXID bxID() const
      {
        return BXID( ((data() & MaskBXID) >> ShiftBXID), BitsBXID );
      }

      /// Access the ingress number
      inline L1IngressID ingressID() const
      {
        return L1IngressID( ((data() & MaskIngressID) >> ShiftIngressID) );
      }

      /// Access the Active HPD bits
      inline ShortType activeHPDbits() const
      {
        return ( (data() & MaskActiveHPDs) >> ShiftActiveHPDs );
      }

      /// Access the suppression flag
      inline bool hpdsSuppressed() const
      {
        return ( 0 != (data() & MaskHPDsSuppressed) >> ShiftHPDsSuppressed );
      }

      /// Test if a given HPD is active
      inline bool isHPDActive( const L1InputWithinIngress input ) const
      {
        return isBitOn( activeHPDbits(), input.data() );
      }

    public: // setters

      /// Set the event ID
      inline void setEventID( const EventID & evID )
      {
        // Don't test range, as we know we are only taking the lowest bits
        //dataInRange(evID,MaxEventID);
        set( (ShortType)evID.data(), ShiftEventID, MaskEventID );
      }

      /// Set the BX ID
      inline void setBXID( const BXID & bxID )
      {
        // Don't test range, as we know we are only taking the lowest bits
        //dataInRange(bxID,MaxBXID);
        set( (ShortType)bxID.data(), ShiftBXID, MaskBXID );
      }

      /// Set the given Ingress input number as active
      inline void setHPDActive( const L1InputWithinIngress input )
      {
        ShortType tmp = activeHPDbits();
        setBit( tmp, input.data() );
        set( tmp, ShiftActiveHPDs, MaskActiveHPDs );
      }

      /// Set the ingress number
      inline void setIngressID( const L1IngressID num )
      {
        set( num.data(), ShiftIngressID, MaskIngressID  );
      }

      /// Set the suppression flag
      inline void setHPDsSuppressed( const bool supp )
      {
        const ShortType i = ( supp ? 1 : 0 );
        set( i, ShiftHPDsSuppressed, MaskHPDsSuppressed );
      }

    public:

      /** For a given set of HPD Ids, set each of them as active
       *  @param inputs  HPD Ingress Input Numbers
       */
      void setHPDsActive( const L1IngressInputs & inputs );

      /// Returns a vector of the active HPD input numbers in this ingress
      void activeHPDInputs( L1IngressInputs & inputs ) const;

      /// Returns a vector of the inactive HPD input numbers in this ingress
      void inactiveHPDInputs( L1IngressInputs & inputs ) const;

      /// Returns the number of active HPDs (0-9) in this particular ingress
      inline ShortType numActiveHPDs() const
      {
        const std::bitset<NumL1InputsPerIngress> bits(activeHPDbits());
        return bits.count();
      }

    public:

      /// Write this header to a RAWBank
      inline void fillRAWBank( RAWBank & bank ) const
      {
        bank.push_back( this->data() );
      }

    private:

      /// Update the internal data
      inline void setData( const LongType data ) { m_data = data;  }

      /// Set the data value for a given mask and shift value
      inline void set( const ShortType value,
                       const ShortType shift,
                       const LongType  mask )
      {
        setData( ((value << shift) & mask) | (data() & ~mask) );
      }

      /// tests whether a given value is in range for a given data field
      inline void dataInRange( const ShortType value,
                               const ShortType max ) const
      {
        if ( value > max )
        { throw GaudiException( "Data out of range",
                                "*RichDAQL1IngressHeader*",
                                StatusCode::FAILURE ); }
      }

      /// Set a given bit in a data word on
      inline void
      setBit( Rich::DAQ::ShortType & data,
              const Rich::DAQ::ShortType pos,
              const Rich::DAQ::ShortType value = 1 ) const
      {
        data |= value<<pos;
      }

      /// Test if a given bit in a word is set on
      inline bool
      isBitOn( const Rich::DAQ::LongType data, const Rich::DAQ::ShortType pos ) const
      {
        return ( 0 != (data & (1<<pos)) );
      }

      /// Print in a human readable way
      std::ostream& fillStream( std::ostream& os ) const;

    public:

      /// Overloaded printout to ostream
      friend inline std::ostream& operator << ( std::ostream& os,
                                                const L1IngressHeader & head )
      {
        return head.fillStream(os);
      }

    private:

      /// The data word
      LongType m_data{0};

    };

  }
}

#endif // RICHDAQ_RichDAQL1IngressHeader_H
