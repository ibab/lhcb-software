
//---------------------------------------------------------------------------------
/** @file RichDAQDefinitions.h
 *
 *  Header file for RICH DAQ general definitions
 *
 *  CVS Log :-
 *  $Id: RichDAQDefinitions.h,v 1.12 2007-03-19 15:02:47 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//---------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHDAQDEFINITIONS_H
#define RICHKERNEL_RICHDAQDEFINITIONS_H 1

// from STL
#include <vector>

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/HashMap.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichSmartIDHashFuncs.h"

// Boost
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

namespace Rich
{
  namespace DAQ
  {

    //---------------------------------------------------------------------------------

    /// DAQ long type definition
    typedef unsigned int                      LongType;

    /// DAQ short type definition
    typedef unsigned int                      ShortType;

    //---------------------------------------------------------------------------------

    /// Maximum data block size (LHCb mode)
    static const ShortType MaxDataSize = 32;

    /// Number of Alice pixels per LHCb pixel
    static const ShortType NumAlicePixelsPerLHCbPixel = 8;

    /// Maximum data block size (ALICE mode)
    static const ShortType MaxDataSizeALICE = MaxDataSize * NumAlicePixelsPerLHCbPixel;

    /// Number of bits per data word
    static const ShortType BitsPerDataWord = 32;

    /// Number of Ingress's per L1 board
    static const ShortType NumIngressPerL1 = 4;

    /// Number of L1 inputs per ingress
    static const ShortType NumL1InputsPerIngress = 9;

    /// Total number of inputs to an L1 board
    static const ShortType MaxL1Inputs = NumIngressPerL1 * NumL1InputsPerIngress;

    /// Maximum ZS address value
    static const ShortType MaxZSAddress = 255;

    //---------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------
    /** @class NumericType RichDet/RichDAQDefinitions.h
     *
     *  Simple class with numeric characteristics but some additional type safety
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   11/11/2005
     */
    //---------------------------------------------------------------------------------
    template < class TYPE >
    class NumericType
    {
    public:
      /// Constructor
      explicit NumericType ( const TYPE id = 0 ) : m_id(id) { }
      /// Retrieve the full value
      inline TYPE data() const { return m_id; }
      /// Operator ==
      inline bool operator== ( const NumericType<TYPE>& id ) const
      { return id.data() == this->data() ; }
      /// Operator !=
      inline bool operator!= ( const NumericType<TYPE>& id ) const
      { return id.data() != this->data() ; }
      /// Operator <
      inline bool operator<  ( const NumericType<TYPE>& id ) const
      { return this->data() < id.data() ; }
      /// Operator >
      inline bool operator>  ( const NumericType<TYPE>& id ) const
      { return this->data() > id.data() ; }
      /// Operator std::string
      inline operator std::string() const
      { return boost::lexical_cast<std::string>(data()); }
      /// Overload output to ostream
      friend inline std::ostream& operator << ( std::ostream& os, const NumericType<TYPE> & id )
      { return os << id.data() ; }
      /// Overloaded output to MsgStream
      friend inline MsgStream & operator << ( MsgStream & os, const NumericType<TYPE> & id )
      { return os << boost::format("%4i") % id.data() ; }
      /// Operator ++   (prefix)
      inline NumericType<TYPE>& operator++()    { ++m_id; return *this; }
      /// Operator ++(int)  (postfix)
      inline NumericType<TYPE>& operator++(int) { NumericType<TYPE> tmp = *this; ++m_id; return tmp; }
      /// Operator --   (prefix)
      inline NumericType<TYPE>& operator--()    { --m_id; return *this; }
      /// Operator --(int)  (postfix)
      inline NumericType<TYPE>& operator--(int) { NumericType<TYPE> tmp = *this; --m_id; return tmp; }
    protected:
      /// Update the internal data
      inline void setData( const TYPE id ) { m_id = id; }
    private:
      TYPE m_id; ///< The data value
    };

    //---------------------------------------------------------------------------------

    /** @class Level0ID RichDet/RichDAQDefinitions.h
     *
     *  Contains the Level 0 board number plus one bit identifying on of the two HPDs
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   11/11/2005
     */
    class Level0ID : public NumericType<ShortType>
    {
    public :
      // Define the number of bits for each field
      static const ShortType  BitsHPD =  1;  ///< Number of bits for HPD ID
      static const ShortType  BitsL0  =  10; ///< Number of bits for L0 ID
      // Create the shift registers
      static const ShortType  ShiftHPD = 0;
      static const ShortType  ShiftL0  = ShiftHPD + BitsHPD;
      // Create the Masks
      static const LongType  MaskHPD = ((1 << BitsHPD)-1) << ShiftHPD;
      static const LongType  MaskL0  = ((1 << BitsL0)-1) << ShiftL0;
      // Create the max values that can be stored in each field
      static const ShortType  MaxHPD = ( 1 << BitsHPD ) - 1;
      static const ShortType  MaxL0  = ( 1 << BitsL0 ) - 1;
    public :
      /// Constructor from bit packed word
      explicit Level0ID ( const ShortType id = 0 )
        : NumericType<ShortType>(id)
      { }
      /** Constructor from L0 and HPD number
       *  @param l0num  L0 board number
       *  @param hpdnum HPD bit number (0 or 1)
       */
      explicit Level0ID ( const ShortType l0num,
                          const ShortType hpdnum )
        : NumericType<ShortType>(0)
      {
        setHPD ( hpdnum );
        setL0  ( l0num  );
      }
      /// Return the HPD number (0 or 1)
      inline ShortType hpd() const
      {
        return ( (data() & MaskHPD) >> ShiftHPD );
      }
      /// Set the HPD number (0 or 1)
      inline bool setHPD( const ShortType hpd )
      {
        return ( dataInRange(hpd,MaxHPD) ?
                 set( hpd, ShiftHPD, MaskHPD ) : false );
      }
      /// Return the L0 board number
      inline ShortType l0() const
      {
        return ( (data() & MaskL0) >> ShiftL0 );
      }
      /// Set the L0 board number
      inline bool setL0( const ShortType l0 )
      {
        return ( dataInRange(l0,MaxL0) ?
                 set( l0, ShiftL0, MaskL0 ) : false );
      }
    private: // methods
      /// Set the data value for a given mask and shift value
      inline bool set( const ShortType value,
                       const ShortType shift,
                       const LongType  mask )
      {
        setData( ((value << shift) & mask) | (data() & ~mask) );
        return true;
      }
      /// tests whether a given value is in range for a given data field
      inline bool dataInRange( const ShortType value,
                               const ShortType max ) const
      {
        return ( value <= max );
      }
    };

    //---------------------------------------------------------------------------------

    /** @class EventID RichDet/RichDAQDefinitions.h
     *
     *  The Event ID.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   24/01/2007
     */
    class EventID : public NumericType<ulonglong>
    {
    public:
      /// Constructor
      template<class NUMTYPE>
      explicit EventID ( const NUMTYPE   id    = 0,
                         const ShortType aBits = 8*sizeof(NUMTYPE) )
        : NumericType<ulonglong> ( (ulonglong)id ),
          m_nActiveBits          ( aBits         ) { }
      /// Return the number of active bits
      inline ShortType activeBits() const { return m_nActiveBits; }
      /// Set the number of active bits
      inline void setActiveBits(const ShortType bits) { m_nActiveBits = bits; }
    private:
      /// Number of sensitive bits in this EventID
      ShortType m_nActiveBits;
    };

    /** @class BXID RichDet/RichDAQDefinitions.h
     *
     *  The BX ID.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   24/01/2007
     */
    class BXID : public NumericType<LongType>
    {
    public:
      /// Constructor
      template<class NUMTYPE>
      explicit BXID ( const NUMTYPE   id    = 0,
                      const ShortType aBits = 8*sizeof(NUMTYPE) )
        : NumericType<LongType> ( (LongType)id ),
          m_nActiveBits         ( aBits        ) { }
      /// Return the number of active bits
      inline ShortType activeBits() const { return m_nActiveBits; }
      /// Set the number of active bits
      inline void setActiveBits(const ShortType bits) { m_nActiveBits = bits; }
    private:
      /// Number of sensitive bits in this BXID
      ShortType m_nActiveBits;
    };

    /** @class Level1ID RichDet/RichDAQDefinitions.h
     *
     *  The Level 1 board ID.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   11/11/2005
     */
    class Level1ID : public NumericType<ShortType>
    {
    public :
      /// Constructor
      explicit Level1ID ( const ShortType id = 0 )
        : NumericType<ShortType>(id) { }
    };

    /** @class L1IngressID RichDet/RichDAQDefinitions.h
     *
     *  The L1 ingress number (which of the four groups of inputs to each L1 board)
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   17/01/2007
     */
    class L1IngressID : public NumericType<ShortType>
    {
    public :
      /// Constructor
      explicit L1IngressID ( const ShortType id = 0 )
        : NumericType<ShortType>(id) { }
    };

    /** @class L1InputWithinIngress RichDet/RichDAQDefinitions.h
     *
     *  The input number within a given L1 ingress
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   17/01/2007
     */
    class L1InputWithinIngress : public NumericType<ShortType>
    {
    public :
      /// Constructor
      explicit L1InputWithinIngress ( const ShortType id = 0 )
        : NumericType<ShortType>(id) { }
    };

    /** @class Level1Input RichDet/RichDAQDefinitions.h
     *
     *  The Level 1 board input number.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   11/11/2005
     */
    class Level1Input : public NumericType<ShortType>
    {
    public :
      /// Constructor
      explicit Level1Input ( const ShortType id = 0 )
        : NumericType<ShortType>(id) { }
      /// Constructor from an Ingress ID and Ingress input number
      Level1Input( const L1IngressID ingress,       ///< The ingress ID
                   const L1InputWithinIngress input ///< Input number within an ingress
                   )
        : NumericType<ShortType>( ingress.data()*NumL1InputsPerIngress + input.data() )
      { }
      /// Returns the input number within a given ingress (0-8)
      inline L1InputWithinIngress l1InputWithinIngress() const
      {
        return L1InputWithinIngress( data() % NumL1InputsPerIngress );
      }
      /// Returns the L1 ingress number
      inline L1IngressID ingressID() const
      {
        return L1IngressID ( data() / NumL1InputsPerIngress );
      }
    };

    /** @class HPDHardwareID RichDet/RichDAQDefinitions.h
     *
     *  The (numeric) HPD hardware ID. Unique to each HPD and can be
     *  used to locate its physical properties, such as Q.E. curves.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   11/11/2005
     */
    class HPDHardwareID : public NumericType<ShortType>
    {
    public :
      /// Constructor
      explicit HPDHardwareID ( const ShortType id = 0 )
        : NumericType<ShortType>(id) { }
    };

    /** @class HPDL1InputID RichDet/RichDAQDefinitions.h
     *
     *  The HPD Level1 input ID. The Level1 board number and the input
     *  number on that board for a given HPD bit-packed into a single word.
     *
     *  The most significant 8 bits gives the L1 board number.
     *  The least significant 8 bits gives the L1 board input number.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   11/11/2005
     */
    class HPDL1InputID : public NumericType<LongType>
    {
    public :
      // Define the number of bits for each field
      static const ShortType  BitsIn =  8; ///< Number of bits for input number
      static const ShortType  BitsB  =  8; ///< Number of bits for board number
      // Create the shift registers
      static const ShortType  ShiftIn = 0;
      static const ShortType  ShiftB  = ShiftIn + BitsIn;
      // Create the Masks
      static const LongType  MaskIn = ((1 << BitsIn)-1) << ShiftIn;
      static const LongType  MaskB  = ((1 << BitsB) -1) << ShiftB ;
      // Create the max values that can be stored in each field
      static const ShortType  MaxIn = ( 1 << BitsIn ) - 1;
      static const ShortType  MaxB  = ( 1 << BitsB ) - 1;
    public :
      /// Constructor from bit packed word
      explicit HPDL1InputID ( const ShortType id = 0 )
        : NumericType<ShortType>(id) { }
      /// Constructor from a L1 ID and input number
      HPDL1InputID ( const Level1ID l1ID,    ///< The L1 board ID
                     const Level1Input input ///< L1 input number
                     )
        : NumericType<ShortType>(0)
      {
        setBoardNumber(l1ID);
        setInputNumber(input);
      }
      /// Return the Level1 board number
      inline Level1ID boardNumber() const
      {
        return Level1ID( (data() & MaskB) >> ShiftB );
      }
      /// Set the Level1 board number
      inline bool setBoardNumber( const Level1ID board )
      {
        return ( dataInRange(board.data(),MaxB) ?
                 set( board.data(), ShiftB, MaskB ) : false );
      }
      /// Return the input number
      inline Level1Input inputNumber() const
      {
        return Level1Input( (data() & MaskIn) >> ShiftIn );
      }
      /// Set the input number
      inline bool setInputNumber( const Level1Input input )
      {
        return ( dataInRange(input.data(),MaxIn) ?
                 set( input.data(), ShiftIn, MaskIn ) : false );
      }
    private: // methods
      /// Set the data value for a given mask and shift value
      inline bool set( const ShortType value,
                       const ShortType shift,
                       const LongType  mask )
      {
        setData( ((value << shift) & mask) | (data() & ~mask) );
        return true;
      }
      /// tests whether a given value is in range for a given data field
      inline bool dataInRange( const ShortType value,
                               const ShortType max ) const
      {
        return ( value <= max );
      }
    };

    //---------------------------------------------------------------------------------

    /// Local definition of a RawBank constructed from 32bit ints
    typedef std::vector< LongType >                           RAWBank;

    /// Vector of HPD Hardware IDs
    typedef std::vector< HPDHardwareID >                      HPDHardwareIDs;

    /// Vector of Level 1 IDs
    typedef std::vector< Level0ID >                           Level0IDs;

    /// Vector of Level 1 IDs
    typedef std::vector< Level1ID >                           Level1IDs;

    /// Vector of L1InputWithinIngress
    typedef std::vector< L1InputWithinIngress >               L1IngressInputs;

    //---------------------------------------------------------------------------------

    /** @enum BankVersion
     *
     *  Enumeration for the RICH DAQ Level1 bank versions
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   20/12/2004
     */
    enum BankVersion
      {
        /// Compatible with DC04
        LHCb0  = 0,
        /// Same as LHCb0 with new header + Level1 grouping
        LHCb1  = 1,
        /// Same as LHCb1 with new zero suppression format. Compatible with DC06
        LHCb2  = 2,
        /// Similar to LHCb4, but with "normal" (small) headers. Never used in practise
        LHCb3  = 127,
        /// Version compatible with first L1 firmware, as used in 2006 September testbeam
        LHCb4  = 128,
        /// Version compatible with the second (final?) version of the L1 firmware
        LHCb5  = 129
      };

    //---------------------------------------------------------------------------------

  }
}

//---------------------------------------------------------------------------------

// Hash functions
// Needed in order to allow these classes to be used as keys in Hash maps
// CRJ : Are these really needed ?

#ifdef __GNUC__
namespace __gnu_cxx
{

  /// Level0ID hash function
  template <> struct hash<Rich::DAQ::Level0ID>
  { inline size_t operator() ( Rich::DAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID hash function
  template <> struct hash<Rich::DAQ::Level0ID&>
  { inline size_t operator() ( Rich::DAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID hash function
  template <> struct hash<const Rich::DAQ::Level0ID>
  { inline size_t operator() ( const Rich::DAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID hash function
  template <> struct hash<const Rich::DAQ::Level0ID&>
  { inline size_t operator() ( const Rich::DAQ::Level0ID id ) const { return (size_t)id.data(); } } ;

  /// Level1ID hash function
  template <> struct hash<Rich::DAQ::Level1ID>
  { inline size_t operator() ( Rich::DAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID hash function
  template <> struct hash<Rich::DAQ::Level1ID&>
  { inline size_t operator() ( Rich::DAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID hash function
  template <> struct hash<const Rich::DAQ::Level1ID>
  { inline size_t operator() ( const Rich::DAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID hash function
  template <> struct hash<const Rich::DAQ::Level1ID&>
  { inline size_t operator() ( const Rich::DAQ::Level1ID id ) const { return (size_t)id.data(); } } ;

  /// L1IngressID hash function
  template <> struct hash<Rich::DAQ::L1IngressID>
  { inline size_t operator() ( Rich::DAQ::L1IngressID id ) const { return (size_t)id.data(); } } ;
  /// L1IngressID hash function
  template <> struct hash<Rich::DAQ::L1IngressID&>
  { inline size_t operator() ( Rich::DAQ::L1IngressID id ) const { return (size_t)id.data(); } } ;
  /// L1IngressID hash function
  template <> struct hash<const Rich::DAQ::L1IngressID>
  { inline size_t operator() ( const Rich::DAQ::L1IngressID id ) const { return (size_t)id.data(); } } ;
  /// L1IngressID hash function
  template <> struct hash<const Rich::DAQ::L1IngressID&>
  { inline size_t operator() ( const Rich::DAQ::L1IngressID id ) const { return (size_t)id.data(); } } ;

  /// L1InputWithinIngress hash function
  template <> struct hash<Rich::DAQ::L1InputWithinIngress>
  { inline size_t operator() ( Rich::DAQ::L1InputWithinIngress id ) const { return (size_t)id.data(); } } ;
  /// L1InputWithinIngress hash function
  template <> struct hash<Rich::DAQ::L1InputWithinIngress&>
  { inline size_t operator() ( Rich::DAQ::L1InputWithinIngress id ) const { return (size_t)id.data(); } } ;
  /// L1InputWithinIngress hash function
  template <> struct hash<const Rich::DAQ::L1InputWithinIngress>
  { inline size_t operator() ( const Rich::DAQ::L1InputWithinIngress id ) const { return (size_t)id.data(); } } ;
  /// L1InputWithinIngress hash function
  template <> struct hash<const Rich::DAQ::L1InputWithinIngress&>
  { inline size_t operator() ( const Rich::DAQ::L1InputWithinIngress id ) const { return (size_t)id.data(); } } ;

  /// Level1Input hash function
  template <> struct hash<Rich::DAQ::Level1Input>
  { inline size_t operator() ( Rich::DAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input hash function
  template <> struct hash<Rich::DAQ::Level1Input&>
  { inline size_t operator() ( Rich::DAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input hash function
  template <> struct hash<const Rich::DAQ::Level1Input>
  { inline size_t operator() ( const Rich::DAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input hash function
  template <> struct hash<const Rich::DAQ::Level1Input&>
  { inline size_t operator() ( const Rich::DAQ::Level1Input id ) const { return (size_t)id.data(); } } ;

  /// HPDHardwareID hash function
  template <> struct hash<Rich::DAQ::HPDHardwareID>
  { inline size_t operator() ( Rich::DAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID hash function
  template <> struct hash<Rich::DAQ::HPDHardwareID&>
  { inline size_t operator() ( Rich::DAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID hash function
  template <> struct hash<const Rich::DAQ::HPDHardwareID>
  { inline size_t operator() ( const Rich::DAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID hash function
  template <> struct hash<const Rich::DAQ::HPDHardwareID&>
  { inline size_t operator() ( const Rich::DAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;

  /// HPDL1InputID hash function
  template <> struct hash<Rich::DAQ::HPDL1InputID>
  { inline size_t operator() ( Rich::DAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID hash function
  template <> struct hash<Rich::DAQ::HPDL1InputID&>
  { inline size_t operator() ( Rich::DAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID hash function
  template <> struct hash<const Rich::DAQ::HPDL1InputID>
  { inline size_t operator() ( const Rich::DAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID hash function
  template <> struct hash<const Rich::DAQ::HPDL1InputID&>
  { inline size_t operator() ( const Rich::DAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;

}

namespace GaudiUtils
{

  /// Level0ID Hash function
  template <> struct Hash<Rich::DAQ::Level0ID>
  { inline size_t operator() ( Rich::DAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID Hash function
  template <> struct Hash<Rich::DAQ::Level0ID&>
  { inline size_t operator() ( Rich::DAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID Hash function
  template <> struct Hash<const Rich::DAQ::Level0ID>
  { inline size_t operator() ( const Rich::DAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID Hash function
  template <> struct Hash<const Rich::DAQ::Level0ID&>
  { inline size_t operator() ( const Rich::DAQ::Level0ID id ) const { return (size_t)id.data(); } } ;

  /// Level1ID Hash function
  template <> struct Hash<Rich::DAQ::Level1ID>
  { inline size_t operator() ( Rich::DAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID Hash function
  template <> struct Hash<Rich::DAQ::Level1ID&>
  { inline size_t operator() ( Rich::DAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID Hash function
  template <> struct Hash<const Rich::DAQ::Level1ID>
  { inline size_t operator() ( const Rich::DAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID Hash function
  template <> struct Hash<const Rich::DAQ::Level1ID&>
  { inline size_t operator() ( const Rich::DAQ::Level1ID id ) const { return (size_t)id.data(); } } ;

  /// L1IngressID Hash function
  template <> struct Hash<Rich::DAQ::L1IngressID>
  { inline size_t operator() ( Rich::DAQ::L1IngressID id ) const { return (size_t)id.data(); } } ;
  /// L1IngressID Hash function
  template <> struct Hash<Rich::DAQ::L1IngressID&>
  { inline size_t operator() ( Rich::DAQ::L1IngressID id ) const { return (size_t)id.data(); } } ;
  /// L1IngressID Hash function
  template <> struct Hash<const Rich::DAQ::L1IngressID>
  { inline size_t operator() ( const Rich::DAQ::L1IngressID id ) const { return (size_t)id.data(); } } ;
  /// L1IngressID Hash function
  template <> struct Hash<const Rich::DAQ::L1IngressID&>
  { inline size_t operator() ( const Rich::DAQ::L1IngressID id ) const { return (size_t)id.data(); } } ;

  /// L1InputWithinIngress Hash function
  template <> struct Hash<Rich::DAQ::L1InputWithinIngress>
  { inline size_t operator() ( Rich::DAQ::L1InputWithinIngress id ) const { return (size_t)id.data(); } } ;
  /// L1InputWithinIngress Hash function
  template <> struct Hash<Rich::DAQ::L1InputWithinIngress&>
  { inline size_t operator() ( Rich::DAQ::L1InputWithinIngress id ) const { return (size_t)id.data(); } } ;
  /// L1InputWithinIngress Hash function
  template <> struct Hash<const Rich::DAQ::L1InputWithinIngress>
  { inline size_t operator() ( const Rich::DAQ::L1InputWithinIngress id ) const { return (size_t)id.data(); } } ;
  /// L1InputWithinIngress Hash function
  template <> struct Hash<const Rich::DAQ::L1InputWithinIngress&>
  { inline size_t operator() ( const Rich::DAQ::L1InputWithinIngress id ) const { return (size_t)id.data(); } } ;

  /// Level1Input Hash function
  template <> struct Hash<Rich::DAQ::Level1Input>
  { inline size_t operator() ( Rich::DAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input Hash function
  template <> struct Hash<Rich::DAQ::Level1Input&>
  { inline size_t operator() ( Rich::DAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input Hash function
  template <> struct Hash<const Rich::DAQ::Level1Input>
  { inline size_t operator() ( const Rich::DAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input Hash function
  template <> struct Hash<const Rich::DAQ::Level1Input&>
  { inline size_t operator() ( const Rich::DAQ::Level1Input id ) const { return (size_t)id.data(); } } ;

  /// HPDHardwareID Hash function
  template <> struct Hash<Rich::DAQ::HPDHardwareID>
  { inline size_t operator() ( Rich::DAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID Hash function
  template <> struct Hash<Rich::DAQ::HPDHardwareID&>
  { inline size_t operator() ( Rich::DAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID Hash function
  template <> struct Hash<const Rich::DAQ::HPDHardwareID>
  { inline size_t operator() ( const Rich::DAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID Hash function
  template <> struct Hash<const Rich::DAQ::HPDHardwareID&>
  { inline size_t operator() ( const Rich::DAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;

  /// HPDL1InputID Hash function
  template <> struct Hash<Rich::DAQ::HPDL1InputID>
  { inline size_t operator() ( Rich::DAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID Hash function
  template <> struct Hash<Rich::DAQ::HPDL1InputID&>
  { inline size_t operator() ( Rich::DAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID Hash function
  template <> struct Hash<const Rich::DAQ::HPDL1InputID>
  { inline size_t operator() ( const Rich::DAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID Hash function
  template <> struct Hash<const Rich::DAQ::HPDL1InputID&>
  { inline size_t operator() ( const Rich::DAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;

}

#endif


//----------------------------------------------------------------------------------------------------
namespace Rich
{
  namespace DAQ
  {

    /// Map for RICH HPD data, sorted by HPD number
    typedef std::map< const LHCb::RichSmartID, LHCb::RichSmartID::Vector >            PDMap;

    /// HPD Information
    typedef std::pair< const LHCb::RichSmartID, const Level1Input >                   HPDInfo;

    /// Hash Map for RICH HPD data, sorted by HPD number
    typedef GaudiUtils::HashMap< const HPDInfo, LHCb::RichSmartID::Vector >           HPDHashMap;

    /// type for data in a single L1 ingress
    typedef GaudiUtils::HashMap< const L1IngressID, HPDHashMap >                      IngressMap;

    /// Hash Map for RICH HPD data, sorted by Level1 board number
    typedef GaudiUtils::HashMap< const Level1ID, IngressMap >                         L1Map;

    /// Mapping from Level1 ID to list of HPD RichSmartIDs
    typedef GaudiUtils::HashMap< const Level1ID, LHCb::RichSmartID::Vector >          L1ToSmartIDs;

    /// Mapping from Level1 ID to list of HPD RichSmartIDs
    typedef GaudiUtils::HashMap< const Level1ID, HPDHardwareIDs >                     L1ToHardIDs;

  }
}
//----------------------------------------------------------------------------------------------------

#endif // RICHKERNEL_RICHDAQDEFINITIONS_H
