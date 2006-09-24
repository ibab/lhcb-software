
//---------------------------------------------------------------------------------
/** @file RichDAQDefinitions.h
 *
 *  Header file for RICH DAQ general definitions
 *
 *  CVS Log :-
 *  $Id: RichDAQDefinitions.h,v 1.8 2006-09-24 10:10:44 jonrob Exp $
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

/** @namespace RichDAQ
 *
 *  Namespace for definitions related to Rich DAQ processing
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
namespace RichDAQ
{

  //---------------------------------------------------------------------------------

  /** @class NumericType RichKernel/RichDAQDefinitions.h
   *
   *  Simple class with numeric characteristics but some additional type safety
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   11/11/2005
   */
  template < class TYPE >
  class NumericType
  {
  public:
    /// Constructor
    explicit NumericType ( const TYPE id = 0 ) : m_id(id) { }
    /// Retrieve the full value
    inline TYPE data() const { return m_id; }
    /// Operator ==
    inline bool operator== ( const RichDAQ::NumericType<TYPE>& id ) const
    { return id.data() == this->data() ; }
    /// Operator !=
    inline bool operator!= ( const RichDAQ::NumericType<TYPE>& id ) const
    { return id.data() != this->data() ; }
    /// Operator <
    inline bool operator<  ( const RichDAQ::NumericType<TYPE>& id ) const
    { return id.data() <  this->data() ; }
    /// Operator >
    inline bool operator>  ( const RichDAQ::NumericType<TYPE>& id ) const
    { return id.data() >  this->data() ; }
    /// Operator std::string
    inline operator std::string() const
    { return boost::lexical_cast<std::string>(data()); }
    /// Overload output to ostream
    friend inline std::ostream& operator << ( std::ostream& os, const NumericType<TYPE> & id )
    { return ( os << id.data() ); }
    /// Overloaded output to MsgStream
    friend inline MsgStream & operator << ( MsgStream & os, const NumericType<TYPE> & id )
    { return ( os << format("%4i",id.data()) ); }
  protected:
    /// Update the internal data
    inline void setData( const TYPE id ) { m_id = id; }
  private:
    TYPE m_id; ///< The data value
  };

  //---------------------------------------------------------------------------------

  /// DAQ long type definition
  typedef unsigned int                      LongType;

  /// DAQ short type definition
  typedef unsigned int                      ShortType;

  //---------------------------------------------------------------------------------

  /** @class Level0ID RichKernel/RichDAQDefinitions.h
   *
   *  Contains the Level 0 board number plus one bit identifying on of the two HPDs
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   11/11/2005
   */
  class Level0ID : public NumericType<RichDAQ::ShortType>
  {
  public :
    // Define the number of bits for each field
    static const RichDAQ::ShortType  BitsHPD =  1;  ///< Number of bits for HPD ID
    static const RichDAQ::ShortType  BitsL0  =  10; ///< Number of bits for L0 ID
    // Create the shift registers
    static const RichDAQ::ShortType  ShiftHPD = 0;
    static const RichDAQ::ShortType  ShiftL0  = ShiftHPD + BitsHPD;
    // Create the Masks
    static const RichDAQ::LongType  MaskHPD = ((1 << BitsHPD)-1) << ShiftHPD;
    static const RichDAQ::LongType  MaskL0  = ((1 << BitsL0)-1) << ShiftL0;
    // Create the max values that can be stored in each field
    static const RichDAQ::ShortType  MaxHPD = ( 1 << BitsHPD ) - 1;
    static const RichDAQ::ShortType  MaxL0  = ( 1 << BitsL0 ) - 1;
  public :
    /// Constructor from bit packed word
    explicit Level0ID ( const RichDAQ::ShortType id = 0 )
      : NumericType<RichDAQ::ShortType>(id) 
    { }
    /** Constructor from L0 and HPD number
     *  @param l0num  L0 board number
     *  @param hpdnum HPD bit number (0 or 1)
     */
    explicit Level0ID ( const RichDAQ::ShortType l0num,
                        const RichDAQ::ShortType hpdnum )
      : NumericType<RichDAQ::ShortType>(0) 
    {
      setHPD ( hpdnum );
      setL0  ( l0num  );
    }
    /// Return the HPD number
    inline RichDAQ::ShortType hpd() const
    {
      return ( (data() & MaskHPD) >> ShiftHPD );
    }
    /// Set the HPD number
    inline bool setHPD( const RichDAQ::ShortType hpd )
    {
      return ( dataInRange(hpd,MaxHPD) ?
               set( hpd, ShiftHPD, MaskHPD ) : false );
    }
    /// Return the L0 number
    inline RichDAQ::ShortType l0() const
    {
      return ( (data() & MaskL0) >> ShiftL0 );
    }
    /// Set the HPD number
    inline bool setL0( const RichDAQ::ShortType l0 )
    {
      return ( dataInRange(l0,MaxL0) ?
               set( l0, ShiftL0, MaskL0 ) : false );
    }
  private: // methods
    /// Set the data value for a given mask and shift value
    inline bool set( const RichDAQ::ShortType value,
                     const RichDAQ::ShortType shift,
                     const RichDAQ::LongType  mask )
    {
      setData( ((value << shift) & mask) | (data() & ~mask) );
      return true;
    }
    /// tests whether a given value is in range for a given data field
    inline bool dataInRange( const RichDAQ::ShortType value,
                             const RichDAQ::ShortType max ) const
    {
      return ( value <= max );
    }
  };

  //---------------------------------------------------------------------------------

  /** @class Level1ID RichKernel/RichDAQDefinitions.h
   *
   *  The Level 1 board ID.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   11/11/2005
   */
  class Level1ID : public NumericType<RichDAQ::ShortType>
  {
  public :
    /// Constructor
    explicit Level1ID ( const RichDAQ::ShortType id = 0 )
      : NumericType<RichDAQ::ShortType>(id) { }
  };

  /** @class Level1Input RichKernel/RichDAQDefinitions.h
   *
   *  The Level 1 board input number.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   11/11/2005
   */
  class Level1Input : public NumericType<RichDAQ::ShortType>
  {
  public :
    /// Constructor
    explicit Level1Input ( const RichDAQ::ShortType id = 0 )
      : NumericType<RichDAQ::ShortType>(id) { }
  };

  /** @class HPDHardwareID RichKernel/RichDAQDefinitions.h
   *
   *  The (numeric) HPD hardware ID. Unique to each HPD and can be
   *  used to locate its physical properties, such as Q.E. curves.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   11/11/2005
   */
  class HPDHardwareID : public NumericType<RichDAQ::ShortType>
  {
  public :
    /// Constructor
    explicit HPDHardwareID ( const RichDAQ::ShortType id = 0 )
      : NumericType<RichDAQ::ShortType>(id) { }
  };

  /** @class HPDL1InputID RichKernel/RichDAQDefinitions.h
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
  class HPDL1InputID : public NumericType<RichDAQ::ShortType>
  {
  public :
    // Define the number of bits for each field
    static const RichDAQ::ShortType  BitsIn =  8; ///< Number of bits for input number
    static const RichDAQ::ShortType  BitsB  =  8; ///< Number of bits for board number
    // Create the shift registers
    static const RichDAQ::ShortType  ShiftIn = 0;
    static const RichDAQ::ShortType  ShiftB  = ShiftIn + BitsIn;
    // Create the Masks
    static const RichDAQ::LongType  MaskIn = ((1 << BitsIn)-1) << ShiftIn;
    static const RichDAQ::LongType  MaskB  = ((1 << BitsB) -1) << ShiftB ;
    // Create the max values that can be stored in each field
    static const RichDAQ::ShortType  MaxIn = ( 1 << BitsIn ) - 1;
    static const RichDAQ::ShortType  MaxB  = ( 1 << BitsB ) - 1;
  public :
    /// Constructor from bit packed word
    explicit HPDL1InputID ( const RichDAQ::ShortType id = 0 )
      : NumericType<RichDAQ::ShortType>(id) { }
    /// Return the Level1 board number
    inline RichDAQ::Level1ID boardNumber() const
    {
      return RichDAQ::Level1ID( (data() & MaskB) >> ShiftB );
    }
    /// Set the Level1 board number
    inline bool setBoardNumber( const RichDAQ::Level1ID board )
    {
      return ( dataInRange(board.data(),MaxB) ?
               set( board.data(), ShiftB, MaskB ) : false );
    }
    /// Return the input number
    inline RichDAQ::Level1Input inputNumber() const
    {
      return RichDAQ::Level1Input( (data() & MaskIn) >> ShiftIn );
    }
    /// Set the input number
    inline bool setInputNumber( const RichDAQ::Level1Input input )
    {
      return ( dataInRange(input.data(),MaxIn) ?
               set( input.data(), ShiftIn, MaskIn ) : false );
    }
  private: // methods
    /// Set the data value for a given mask and shift value
    inline bool set( const RichDAQ::ShortType value,
                     const RichDAQ::ShortType shift,
                     const RichDAQ::LongType  mask )
    {
      setData( ((value << shift) & mask) | (data() & ~mask) );
      return true;
    }
    /// tests whether a given value is in range for a given data field
    inline bool dataInRange( const RichDAQ::ShortType value,
                             const RichDAQ::ShortType max ) const
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

  /// Hash Map for RICH HPD data, sorted by HPD number
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, LHCb::RichSmartID::Vector > PDHashMap;

  /// Map for RICH HPD data, sorted by HPD number
  typedef std::map< const LHCb::RichSmartID, LHCb::RichSmartID::Vector >            PDMap;

  /// Hash Map for RICH HPD data, sorted by Level1 board number
  typedef GaudiUtils::HashMap< const Level1ID, PDHashMap >                          L1Map;

  /// Mapping from Level1 ID to list of HPD RichSmartIDs
  typedef GaudiUtils::HashMap< const RichDAQ::Level1ID, LHCb::RichSmartID::Vector > L1ToSmartIDs;

  /// Mapping from Level1 ID to list of HPD RichSmartIDs
  typedef GaudiUtils::HashMap< const RichDAQ::Level1ID, RichDAQ::HPDHardwareIDs >   L1ToHardIDs;

  /// Maximum data block size (LHCb mode)
  static const RichDAQ::ShortType MaxDataSize = 32;

  /// Maximum data block size (ALICE mode)
  static const RichDAQ::ShortType MaxDataSizeALICE = 256;

  /// Number of bits per data word
  static const RichDAQ::ShortType BitsPerDataWord = 32;

  //---------------------------------------------------------------------------------

  /** @enum BankVersion
   *
   *  Enumeration for the RICH DAQ Level1 bank version
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
      /// Similar to LHCb4, but with "normal" headers
      LHCb3  = 127,
      /// Version compatible with real L1 firmware, as used in 2006 September testbeam
      LHCb4  = 128
    };

  //---------------------------------------------------------------------------------

}

//---------------------------------------------------------------------------------

// Hash functions
// Needed in order to allow these classes to be used as keys in Hash maps

#ifdef __GNUC__
namespace __gnu_cxx
{

  /// Level0ID hash function
  template <> struct hash<RichDAQ::Level0ID>
  { inline size_t operator() ( RichDAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID hash function
  template <> struct hash<RichDAQ::Level0ID&>
  { inline size_t operator() ( RichDAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID hash function
  template <> struct hash<const RichDAQ::Level0ID>
  { inline size_t operator() ( const RichDAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID hash function
  template <> struct hash<const RichDAQ::Level0ID&>
  { inline size_t operator() ( const RichDAQ::Level0ID id ) const { return (size_t)id.data(); } } ;

  /// Level1ID hash function
  template <> struct hash<RichDAQ::Level1ID>
  { inline size_t operator() ( RichDAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID hash function
  template <> struct hash<RichDAQ::Level1ID&>
  { inline size_t operator() ( RichDAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID hash function
  template <> struct hash<const RichDAQ::Level1ID>
  { inline size_t operator() ( const RichDAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID hash function
  template <> struct hash<const RichDAQ::Level1ID&>
  { inline size_t operator() ( const RichDAQ::Level1ID id ) const { return (size_t)id.data(); } } ;

  /// Level1Input hash function
  template <> struct hash<RichDAQ::Level1Input>
  { inline size_t operator() ( RichDAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input hash function
  template <> struct hash<RichDAQ::Level1Input&>
  { inline size_t operator() ( RichDAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input hash function
  template <> struct hash<const RichDAQ::Level1Input>
  { inline size_t operator() ( const RichDAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input hash function
  template <> struct hash<const RichDAQ::Level1Input&>
  { inline size_t operator() ( const RichDAQ::Level1Input id ) const { return (size_t)id.data(); } } ;

  /// HPDHardwareID hash function
  template <> struct hash<RichDAQ::HPDHardwareID>
  { inline size_t operator() ( RichDAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID hash function
  template <> struct hash<RichDAQ::HPDHardwareID&>
  { inline size_t operator() ( RichDAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID hash function
  template <> struct hash<const RichDAQ::HPDHardwareID>
  { inline size_t operator() ( const RichDAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID hash function
  template <> struct hash<const RichDAQ::HPDHardwareID&>
  { inline size_t operator() ( const RichDAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;

  /// HPDL1InputID hash function
  template <> struct hash<RichDAQ::HPDL1InputID>
  { inline size_t operator() ( RichDAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID hash function
  template <> struct hash<RichDAQ::HPDL1InputID&>
  { inline size_t operator() ( RichDAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID hash function
  template <> struct hash<const RichDAQ::HPDL1InputID>
  { inline size_t operator() ( const RichDAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID hash function
  template <> struct hash<const RichDAQ::HPDL1InputID&>
  { inline size_t operator() ( const RichDAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;

}

namespace GaudiUtils
{

  /// Level0ID Hash function
  template <> struct Hash<RichDAQ::Level0ID>
  { inline size_t operator() ( RichDAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID Hash function
  template <> struct Hash<RichDAQ::Level0ID&>
  { inline size_t operator() ( RichDAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID Hash function
  template <> struct Hash<const RichDAQ::Level0ID>
  { inline size_t operator() ( const RichDAQ::Level0ID id ) const { return (size_t)id.data(); } } ;
  /// Level0ID Hash function
  template <> struct Hash<const RichDAQ::Level0ID&>
  { inline size_t operator() ( const RichDAQ::Level0ID id ) const { return (size_t)id.data(); } } ;

  /// Level1ID Hash function
  template <> struct Hash<RichDAQ::Level1ID>
  { inline size_t operator() ( RichDAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID Hash function
  template <> struct Hash<RichDAQ::Level1ID&>
  { inline size_t operator() ( RichDAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID Hash function
  template <> struct Hash<const RichDAQ::Level1ID>
  { inline size_t operator() ( const RichDAQ::Level1ID id ) const { return (size_t)id.data(); } } ;
  /// Level1ID Hash function
  template <> struct Hash<const RichDAQ::Level1ID&>
  { inline size_t operator() ( const RichDAQ::Level1ID id ) const { return (size_t)id.data(); } } ;

  /// Level1Input Hash function
  template <> struct Hash<RichDAQ::Level1Input>
  { inline size_t operator() ( RichDAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input Hash function
  template <> struct Hash<RichDAQ::Level1Input&>
  { inline size_t operator() ( RichDAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input Hash function
  template <> struct Hash<const RichDAQ::Level1Input>
  { inline size_t operator() ( const RichDAQ::Level1Input id ) const { return (size_t)id.data(); } } ;
  /// Level1Input Hash function
  template <> struct Hash<const RichDAQ::Level1Input&>
  { inline size_t operator() ( const RichDAQ::Level1Input id ) const { return (size_t)id.data(); } } ;

  /// HPDHardwareID Hash function
  template <> struct Hash<RichDAQ::HPDHardwareID>
  { inline size_t operator() ( RichDAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID Hash function
  template <> struct Hash<RichDAQ::HPDHardwareID&>
  { inline size_t operator() ( RichDAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID Hash function
  template <> struct Hash<const RichDAQ::HPDHardwareID>
  { inline size_t operator() ( const RichDAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;
  /// HPDHardwareID Hash function
  template <> struct Hash<const RichDAQ::HPDHardwareID&>
  { inline size_t operator() ( const RichDAQ::HPDHardwareID id ) const { return (size_t)id.data(); } } ;

  /// HPDL1InputID Hash function
  template <> struct Hash<RichDAQ::HPDL1InputID>
  { inline size_t operator() ( RichDAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID Hash function
  template <> struct Hash<RichDAQ::HPDL1InputID&>
  { inline size_t operator() ( RichDAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID Hash function
  template <> struct Hash<const RichDAQ::HPDL1InputID>
  { inline size_t operator() ( const RichDAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;
  /// HPDL1InputID Hash function
  template <> struct Hash<const RichDAQ::HPDL1InputID&>
  { inline size_t operator() ( const RichDAQ::HPDL1InputID id ) const { return (size_t)id.data(); } } ;

}

#endif

//---------------------------------------------------------------------------------

#endif // RICHKERNEL_RICHDAQDEFINITIONS_H
