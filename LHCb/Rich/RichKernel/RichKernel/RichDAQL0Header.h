
//=============================================================================================
/** @file RichDAQL0Header.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::RichDAQL0Header
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/01/2007
 */
//=============================================================================================

#ifndef RICHDAQ_RichDAQL0Header_H
#define RICHDAQ_RichDAQL0Header_H 1

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

namespace Rich
{
  namespace DAQ
  {

    //=============================================================================================
    /** @class L0Header RichDAQL0Header.h RichKernel/RichDAQL0Header.h
     *
     *  The overall RICH L0 header for each HPD. 
     *  
     *  Documentation of the data format is available here
     *
     *  https://edms.cern.ch/document/696530
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   23/01/2007
     */
    //=============================================================================================

    class L0Header
    {

    public: // Each word

      //=============================================================================================
      /** @class Word0 RichDAQL0Header.h RichKernel/RichDAQL0Header.h
       *
       *  The first word of the RICH L0 header
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   23/01/2007
       */
      //=============================================================================================
      class Word0
      {

      public:

        /// Default Constructor
        Word0() = default;

        /// Constructor from raw LongType
        explicit Word0( const LongType data ) : m_data(data) { }

        /// Retrieve the full value
        inline LongType data() const { return m_data; }

        // bit packing info
        //-----------------------------------------------------------------------------------------
      private:

        // Define the number of bits for each field
        static const ShortType BitsEventID        = 8;  ///< Number of bits for Event ID
        static const ShortType BitsBxID           = 12; ///< Number of bits for BX ID
        static const ShortType BitsParityBit      = 1;  ///< Number of bits for the parity flag

        // Create the shift registers
        static const ShortType ShiftEventID       = 0;
        static const ShortType ShiftUnusedBits1   = 8;
        static const ShortType ShiftBxID          = ShiftEventID + BitsEventID + ShiftUnusedBits1;
        static const ShortType ShiftUnusedBits2   = 3;
        static const ShortType ShiftParityBit     = ShiftBxID + BitsBxID + ShiftUnusedBits2;

        // Create the Masks
        static const LongType MaskEventID         = (LongType) ((1 << BitsEventID)-1)   << ShiftEventID;
        static const LongType MaskBxID            = (LongType) ((1 << BitsBxID)-1)      << ShiftBxID;
        static const LongType MaskParityBit       = (LongType) ((1 << BitsParityBit)-1) << ShiftParityBit;

        //-----------------------------------------------------------------------------------------

      public: // accessors

        /// Retrieve the event ID word
        inline EventID eventID() const
        {
          return EventID( ((data() & MaskEventID) >> ShiftEventID), BitsEventID );
        }

        /// Access the BX ID
        inline BXID bxID() const
        {
          return BXID( ((data() & MaskBxID) >> ShiftBxID), BitsBxID );
        }

        /// Access the parity bit
        inline bool parityBit() const
        {
          return ( ((data() & MaskParityBit) >> ShiftParityBit) != 0 );
        }

      public:

        /// overloaded output to std::ostream
        friend inline std::ostream & operator << ( std::ostream & os,
                                                   const L0Header::Word0 & word )
        {
          return  os << "L0Header::Word0 [ " << word.eventID() << " " << word.bxID() 
                     << " parityBit=" << word.parityBit() << " ]";
        }

      private:

        /// The data word
        LongType m_data{0};

      };

      //=============================================================================================
      /** @class Word1 RichDAQL0Header.h RichKernel/RichDAQL0Header.h
       *
       *  The second word of the RICH L0 header
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   23/01/2007
       */
      //=============================================================================================
      class Word1
      {

      public:

        /// Default Constructor
        Word1() = default;

        /// Constructor from raw LongType
        explicit Word1( const LongType data ) : m_data(data) { }

        /// Retrieve the full value
        inline LongType data() const { return m_data; }

        // bit packing info
        //------------------------------------------------------------------------------------------------------
      private:

        // Define the number of bits for each field
        static const ShortType BitsL0ID            = 11;  ///< Number of bits for the Level0 ID
        static const ShortType BitsNWordsBIDFIF    = 4;   ///< Number of bits for the # words in BID FIF()
        static const ShortType BitsBIDFIFEmpty     = 1;   ///< Number of bits for BID FIF() empty flag
        static const ShortType BitsBIDFIFFull      = 1;   ///< Number of bits for BID FIF() full flag
        static const ShortType BitsTestPat         = 1;   ///< Number of bits for test pattern flag
        static const ShortType BitsCalibEvent      = 1;   ///< Number of bits for calibration event flag
        static const ShortType BitsRunMode         = 1;   ///< Number of bits for run mode (0=LHCb 1=ALICE)
        static const ShortType BitsOtherGOLStatus  = 1;   ///< Number of bits for the status of the other GOL
        static const ShortType BitsParityBit       = 1;   ///< Number of bits for the parity flag (bits 0-20)

        // Create the shift registers
        static const ShortType ShiftL0ID           = 0;
        static const ShortType ShiftNWordsBIDFIF   = ShiftL0ID + BitsL0ID;
        static const ShortType ShiftBIDFIFEmpty    = ShiftNWordsBIDFIF + BitsNWordsBIDFIF;
        static const ShortType ShiftBIDFIFFull     = ShiftBIDFIFEmpty + BitsBIDFIFEmpty;
        static const ShortType ShiftTestPat        = ShiftBIDFIFFull + BitsBIDFIFFull;
        static const ShortType ShiftCalibEvent     = ShiftTestPat + BitsTestPat;
        static const ShortType ShiftRunMode        = ShiftCalibEvent + BitsCalibEvent;
        static const ShortType ShiftOtherGOLStatus = ShiftRunMode + BitsRunMode;
        static const ShortType ShiftUnusedBits1    = 10;
        static const ShortType ShiftParityBit      = ShiftOtherGOLStatus + BitsOtherGOLStatus + ShiftUnusedBits1;

        // Create the Masks
        static const LongType MaskL0ID             = (LongType) ((1 << BitsL0ID)-1)           << ShiftL0ID;
        static const LongType MaskNWordsBIDFIF     = (LongType) ((1 << BitsNWordsBIDFIF)-1)   << ShiftNWordsBIDFIF;
        static const LongType MaskBIDFIFEmpty      = (LongType) ((1 << BitsBIDFIFEmpty)-1)    << ShiftBIDFIFEmpty;
        static const LongType MaskBIDFIFFull       = (LongType) ((1 << BitsBIDFIFFull)-1)     << ShiftBIDFIFFull;
        static const LongType MaskTestPat          = (LongType) ((1 << BitsTestPat)-1)        << ShiftTestPat;
        static const LongType MaskCalibEvent       = (LongType) ((1 << BitsCalibEvent)-1)     << ShiftCalibEvent;
        static const LongType MaskRunMode          = (LongType) ((1 << BitsRunMode)-1)        << ShiftRunMode;
        static const LongType MaskOtherGOLStatus   = (LongType) ((1 << BitsOtherGOLStatus)-1) << ShiftOtherGOLStatus;
        static const LongType MaskParityBit        = (LongType) ((1 << BitsParityBit)-1)      << ShiftParityBit;

        //------------------------------------------------------------------------------------------------------

      public:

        /// Retrieve the Level0 ID
        inline Level0ID l0ID() const
        {
          return Level0ID( (data() & MaskL0ID) >> ShiftL0ID );
        }

        /// Access the number of words in the BID FIF()
        inline ShortType nWordsBIDFIF() const
        {
          return ( (data() & MaskNWordsBIDFIF) >> ShiftNWordsBIDFIF );
        }

        /// Access the BID FIF() empty flag
        inline bool BIDFIFEmpty() const
        {
          return ( (data() & MaskBIDFIFEmpty) >> ShiftBIDFIFEmpty ) != 0;
        }

        /// Access the BID FIF() full flag
        inline bool BIDFIFFull() const
        {
          return ( (data() & MaskBIDFIFFull) >> ShiftBIDFIFFull ) != 0;
        }

        /// Was a test pattern sent ?
        inline bool testPattern() const
        {
          return ( (data() & MaskTestPat) >> ShiftTestPat ) != 0;
        }

        /// Was this a calibration event ?
        inline bool calibrationEvent() const
        {
          return ( (data() & MaskCalibEvent) >> ShiftCalibEvent ) != 0;
        }

        /// ALICE Run mode ? (true=ALICE, false=LHCb)
        inline bool aliceRunMode() const
        {
          return ( (data() & MaskRunMode) >> ShiftRunMode ) != 0;
        }

        /// Acess the ready status of the other GOL
        inline bool otherGOLReady() const
        {
          return ( (data() & MaskOtherGOLStatus) >> ShiftOtherGOLStatus ) != 0;
        }

        /// Access the parity bit
        inline bool parityBit() const
        {
          return ( ((data() & MaskParityBit) >> ShiftParityBit) != 0 );
        }

     public:

        /// overloaded output to std::ostream
        friend inline std::ostream & operator << ( std::ostream & l0head_os,
                                                   const L0Header::Word1 & word )
        {
          return l0head_os << "L0Header::Word1 [ " << word.l0ID() 
                           << " nWordsBIDFIF=" << word.nWordsBIDFIF() 
                           << " BIDFIFEmpty=" << word.BIDFIFEmpty() 
                           << " BIDFIFFull=" << word.BIDFIFFull()
                           << " testPattern=" << word.testPattern()
                           << " calibrationEvent=" << word.calibrationEvent()
                           << " aliceRunMode=" << word.aliceRunMode()
                           << " otherGOLReady=" << word.otherGOLReady()
                           << " parityBit=" << word.parityBit() 
                           << " ]";
        }

      private:

        /// The data word
        LongType m_data{0};

      };

    public:

      /// Constructor from raw LongType words
      explicit L0Header( const Word0& word0,
                         const Word1& word1 )
        : m_word0(word0),
          m_word1(word1) { }

      /// Move Constructor from raw LongType words
      explicit L0Header( Word0&& word0,
                         Word1&& word1 )
        : m_word0(std::move(word0)),
          m_word1(std::move(word1)) { }

      /// Retrieve the first word
      inline const Word0& word0() const { return m_word0; }

      /// Retrieve the second word
      inline const Word1& word1() const { return m_word1; }

    public:

      /// Write this header to a RAWBank
      inline void fillRAWBank( RAWBank & bank ) const
      {
        bank.push_back( word0().data() );
        bank.push_back( word1().data() );
      }

    public:
      
      /// overloaded output to std::ostream
      friend inline std::ostream & operator << ( std::ostream & os,
                                                 const L0Header & header )
      {
        return os << "[ " << header.word0() << " " << header.word1() << " ]";
      }

    private:

      /// The first data word
      Word0 m_word0;

      /// The second data word
      Word1 m_word1;

    };

  }
}

#endif // RICHDAQ_RichDAQL0Header_H
