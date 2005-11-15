
//---------------------------------------------------------------------------------
/** @file RichDAQDefinitions.h
 *
 *  Header file for RICH DAQ general definitions
 *
 *  CVS Log :-
 *  $Id: RichDAQDefinitions.h,v 1.6 2005-11-15 13:01:54 jonrob Exp $
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
#include "GaudiKernel/stl_hash.h"

// Event model
#include "Event/DAQTypes.h"
#include "Event/RawBank.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/RichMap.h"
#include "RichKernel/RichSmartIDHashFuncs.h"

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
    /// Access the actual data value
    inline TYPE dataValue() const { return m_id; }
    /// Operator ==
    inline bool operator== ( const RichDAQ::NumericType<TYPE>& id ) const
    { return id.dataValue() == this->dataValue() ; }
    /// Operator !=
    inline bool operator!= ( const RichDAQ::NumericType<TYPE>& id ) const
    { return id.dataValue() != this->dataValue() ; }
    /// Operator <
    inline bool operator<  ( const RichDAQ::NumericType<TYPE>& id ) const
    { return id.dataValue() <  this->dataValue() ; }
    /// Operator >
    inline bool operator>  ( const RichDAQ::NumericType<TYPE>& id ) const
    { return id.dataValue() >  this->dataValue() ; }
    /// Operator std::string
    inline operator std::string() const
    { return boost::lexical_cast<std::string>(dataValue()); }
    /// Update the internal data
    inline void setData( const TYPE id ) { m_id = id; }
    /// Retrieve the full value
    inline TYPE data() const { return m_id; }
  private:
    TYPE m_id; ///< The data value
  };

  //---------------------------------------------------------------------------------

  /// DAQ long type definition
  typedef raw_int                                           LongType;

  /// DAQ short type definition
  typedef unsigned int                                      ShortType;

  //---------------------------------------------------------------------------------

  /** @class Level0ID RichKernel/RichDAQDefinitions.hHPD
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
    static const RichDAQ::ShortType  BitsHPD =  1; ///< Number of bits for HPD ID
    static const RichDAQ::ShortType  BitsL0  =  9; ///< Number of bits for L0 ID
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
      : NumericType<RichDAQ::ShortType>(id) { }
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

  /** @class Level1ID RichKernel/RichDAQDefinitions.hHPD
   *
   *  The Level 1 board ID
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

  /** @class HPDHardwareID RichKernel/RichDAQDefinitions.hHPD
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

  //---------------------------------------------------------------------------------

  /// Local definition of a RawBank constructed from 32bit ints
  typedef std::vector< LongType >                           RAWBank;

  /// Container of Rich RawBanks
  typedef std::vector< RawBank >                            RAWBanks;

  /// Vector of HPD Hardware IDs
  typedef std::vector< HPDHardwareID >                      HPDHardwareIDs;

  /// Vector of Level 1 IDs
  typedef std::vector< Level0ID >                           Level0IDs;

  /// Vector of Level 1 IDs
  typedef std::vector< Level1ID >                           Level1IDs;

  /// Container for RICH HPD data, sorted by HPD number
  typedef RichHashMap< const RichSmartID, RichSmartID::Collection >       PDMap;

  /// Container for RICH HPD data, sorted by Level1 board number
  typedef RichHashMap< const Level1ID, PDMap >                            L1Map;

  /// Mapping from Level1 ID to list of HPD RichSmartIDs
  typedef RichHashMap< const RichDAQ::Level1ID, RichSmartID::Collection > L1ToSmartIDs;

  /// Mapping from Level1 ID to list of HPD RichSmartIDs
  typedef RichHashMap< const RichDAQ::Level1ID, RichDAQ::HPDHardwareIDs > L1ToHardIDs;

  /// Maximum data block size
  static const RichDAQ::ShortType MaxDataSize = 32;

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
      LHCb0  = 0, ///< First LHCb mode version. Compatible with DC04
      LHCb1  = 1, ///< Second LHCb mode version. Same as LHCb0 with new header + Level1 grouping
      LHCb2  = 2  ///< Third LHCb mode version. Same as LHCb1 with new zero suppression format
    };

  //---------------------------------------------------------------------------------

}

//---------------------------------------------------------------------------------

/// overloaded output to MsgStream for RichDAQ::Level0ID
inline MsgStream& operator << ( MsgStream& os,
                                const RichDAQ::Level0ID & id )
{
  os << format( "%4i",id.dataValue() );
  return os;
}
/// overloaded output to MsgStream for RichDAQ::Level1ID
inline MsgStream& operator << ( MsgStream& os,
                                const RichDAQ::Level1ID & id )
{
  os << format( "%4i",id.dataValue() );
  return os;
}
/// overloaded output to MsgStream for RichDAQ::HPDHardwareID
inline MsgStream& operator << ( MsgStream& os,
                                const RichDAQ::HPDHardwareID & id )
{
  os << format( "%4i",id.dataValue() );
  return os;
}

//---------------------------------------------------------------------------------

/// overloaded output to std::ostream for RichDAQ::Level0ID
inline std::ostream& operator << ( std::ostream& os,
                                   const RichDAQ::Level0ID & id )
{
  os << id.dataValue();
  return os;
}
/// overloaded output to std::ostream for RichDAQ::Level1ID
inline std::ostream& operator << ( std::ostream& os,
                                   const RichDAQ::Level1ID & id )
{
  os << id.dataValue();
  return os;
}
/// overloaded output to std::ostream for RichDAQ::HPDHardwareID
inline std::ostream& operator << ( std::ostream& os,
                                   const RichDAQ::HPDHardwareID & id )
{
  os << id.dataValue();
  return os;
}

//---------------------------------------------------------------------------------

// Hash functions
// Needed in order to allow these classes to be used as keys in Hash maps

#ifdef __GNUC__
namespace __gnu_cxx
{
  /// Level0ID hash function
  template <> struct hash<const RichDAQ::Level0ID>
  { size_t operator() ( const RichDAQ::Level0ID id ) const { return (size_t)id.dataValue(); } } ;
  /// Level0ID hash function
  template <> struct hash<const RichDAQ::Level0ID&>
  { size_t operator() ( const RichDAQ::Level0ID id ) const { return (size_t)id.dataValue(); } } ;
  /// Level1ID hash function
  template <> struct hash<const RichDAQ::Level1ID>
  { size_t operator() ( const RichDAQ::Level1ID id ) const { return (size_t)id.dataValue(); } } ;
  /// Level1ID hash function
  template <> struct hash<const RichDAQ::Level1ID&>
  { size_t operator() ( const RichDAQ::Level1ID id ) const { return (size_t)id.dataValue(); } } ;
  /// HPDHardwareID hash function
  template <> struct hash<const RichDAQ::HPDHardwareID>
  { size_t operator() ( const RichDAQ::HPDHardwareID id ) const { return (size_t)id.dataValue(); } } ;
  /// HPDHardwareID hash function
  template <> struct hash<const RichDAQ::HPDHardwareID&>
  { size_t operator() ( const RichDAQ::HPDHardwareID id ) const { return (size_t)id.dataValue(); } } ;
}
#endif

//---------------------------------------------------------------------------------

#endif // RICHKERNEL_RICHDAQDEFINITIONS_H
