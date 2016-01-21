#ifndef KERNEL_STANDARDPACKER_H
#define KERNEL_STANDARDPACKER_H 1

// STL
#include <cmath>

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IRegistry.h"

class GaudiAlgorithm;

//---------------------------------------------------------------------------
/** @class StandardPacker StandardPacker.h Event/StandardPacker.h
 *
 *  This is a simple class to convert to int/short with standard factors
 *  various quantities.
 *
 *  @author Olivier Callot
 *  @date   2005-03-15
 */
//---------------------------------------------------------------------------

namespace Packer
{
  const double ENERGY_SCALE     = 1.0e2;  ///< .01 MeV steps
  const double POSITION_SCALE   = 1.0e4;  ///< 0.1 micron steps
  const double SLOPE_SCALE      = 1.0e8;  ///< full scale +- 20 radians
  const double FRACTION_SCALE   = 3.0e4;  ///< store in short int.
  const double TIME_SCALE       = 1.0e5;  ///< 0.0001 ns resolution
  const double DELTALL_SCALE    = 1.0e4;  ///< 0.0001 precision
  const double MASS_SCALE       = 1.0e3;  ///< 1 keV steps
}

class StandardPacker
{

public:

  //---------------------------------------------------------------------------

  // Default constructor
  StandardPacker() = default;

  /// Constructor with parent
  explicit StandardPacker( const GaudiAlgorithm * p ) : m_parent(p) { }

  //---------------------------------------------------------------------------

  /** returns an int for a double energy */
  inline int energy( const double e ) const
  {
    return packDouble( e * Packer::ENERGY_SCALE );
  }

  /** returns an int for a double position */
  inline int position( const double x ) const
  {
    return packDouble( x * Packer::POSITION_SCALE );
  }

  /** returns an int for a double slope */
  inline int slope( const double x ) const
  {
    return packDouble( x * Packer::SLOPE_SCALE );
  }

  /** returns an short int for a double fraction f */
  inline short int fraction( const double f ) const
  {
    return shortPackDouble( f * Packer::FRACTION_SCALE );
  }

  /** returns an short int for a double fraction top/bot */
  inline short int fraction( const double top, const double bot ) const
  {
    return fraction( fabs(bot) > 0 ? top/bot : 0.0 );
  }

  /** returns an int for a double time (TOF) value */
  inline int time( const double x ) const
  {
    return packDouble( x * Packer::TIME_SCALE );
  }

  /** returns an int for a double delta log likelihood value */
  inline int deltaLL( const double x ) const
  {
    return packDouble( x * Packer::DELTALL_SCALE );
  }

  /** returns an int for a double mass */
  int mass( const double mass ) const
  {
    return packDouble( mass * Packer::MASS_SCALE );
  }

  /** returns an int containing the float value of the double */
  inline int fltPacked( const double& x ) const
  {
    union fltInt { int i; float f; } convert;
    convert.f = (float)x;
    return convert.i;
  }

  //---------------------------------------------------------------------------

  /// Returns the 'LinkID' 
  inline long linkID( DataObject* out,
                      const DataObject* parent ) const
  {
    auto * myLink = out->linkMgr()->link(parent);
    if ( !myLink )
    {
      out->linkMgr()->addLink( parent->registry()->identifier(), parent );
      myLink = out->linkMgr()->link(parent);
    }
    return ( myLink ? myLink->ID() : 0 );
  }

  /// Returns the 'LinkID'
  inline long linkID( DataObject* out,
                      const std::string& targetName ) const
  {
    auto * myLink = out->linkMgr()->link( targetName );
    if ( !myLink )
    {
      out->linkMgr()->addLink( targetName, 0 );
      myLink = out->linkMgr()->link( targetName );
    }
    return ( myLink ? myLink->ID() : 0 );
  }
  
  //---------------------------------------------------------------------------

  /** returns an int for a Smart Ref.
   *  @arg  out : Output data object, to store the links
   *  @arg  parent : Pointer to the parent container of the SmartRef, method->parent()
   *  @arg  key    : returned by the method .linkID() of the SmartRef
   */
  int reference32( DataObject* out,
                   const DataObject* parent,
                   const int key ) const;

  /** returns an int for a Smart Ref.
   *  @arg  out : Output data object, to store the links
   *  @arg  targetName : Name of the target
   *  @arg  key : returned by the method .linkID() of the SmartRef
   */
  int reference32( DataObject* out,
                   const std::string& targetName,
                   const int key ) const;
  
  /// Extracts the key and index from a packed data word
  inline void indexAndKey32( const int data,
                             int& indx,
                             int& key ) const
  {
    indx = data >> 28;
    key  = data & 0x0FFFFFFF;
  }
  
  /// Extracts the key and hint from a packed data word
  bool hintAndKey32( const int data,
                     const DataObject* source,
                     DataObject* target,
                     int& hint,
                     int& key ) const;
  
  //---------------------------------------------------------------------------

  /** returns a long long for a Smart Ref, with small key and large links.
   *  @arg  out    : Output data object, to store the links
   *  @arg  parent : Pointer to the parent container of the SmartRef, method ->parent()
   *  @arg  key    : returned by the method .linkID() of the SmartRef
   */
  inline long long reference64( DataObject* out,
                                const DataObject* parent,
                                const int key ) const
  {
    const long long ID( linkID(out,parent) );
    const long long myLinkID = (ID << 32);
    return (long long)key + myLinkID;
  }

  /// Extracts the key and index from a packed 64-bit data word
  inline void indexAndKey64( const long long data,
                             int& indx,
                             int& key ) const
  {
    indx = data >> 32;
    constexpr long long mask = 0x00000000FFFFFFFF;
    key  = data & mask;
  }

  /// Extracts the key and hint from a packed 64-bit data word
  bool hintAndKey64( const long long data,
                     const DataObject* source,
                     DataObject* target,
                     int& hint,
                     int& key ) const;

  //---------------------------------------------------------------------------

  /** returns the energy as double from the int value */
  inline double energy( const int k )         const { return double(k) / Packer::ENERGY_SCALE; }

  /** returns the position as double from the int value */
  inline double position( const int k )       const { return double(k) / Packer::POSITION_SCALE; }

  /** returns the slope as double from the int value */
  inline double slope( const int k )          const { return double(k) / Packer::SLOPE_SCALE; }

  /** returns the fraction as double from the short int value */
  inline double fraction( const short int k ) const { return double(k) / Packer::FRACTION_SCALE; }

  /** returns the time as double from the int value */
  inline double time( const int k )           const { return double(k) / Packer::TIME_SCALE; }

  /** returns the delta Log Likelihood as double from the int value */
  inline double deltaLL( const int k )        const { return double(k) / Packer::DELTALL_SCALE; }

  /** returns the mass as double from the int value */
  inline double mass( const int k )           const { return double(k) / Packer::MASS_SCALE; }

  /** returns an double from a int containing in fact a float */
  inline double fltPacked( const int k  ) const
  {
    union fltInt { int i; float f; } convert;
    convert.i = k;
    return double(convert.f);
  }

  //---------------------------------------------------------------------------

private:

  /// Pack a double to an int
  inline int packDouble ( const double val ) const
  {
    return ( 2.e9  < val ?  2000000000         : // saturate 31 bits
             -2.e9 > val ? -2000000000         : // idem
             0     < val ? (int) ( val + 0.5 ) : // proper rounding
             (int) ( val - 0.5 )               );
  }

  /// Pack a double to a short int
  inline short int shortPackDouble ( const double val ) const
  {
    return ( 3.e4  < val ? (short int)  30000        : // saturate 15 bits
             -3.e4 > val ? (short int) -30000        : // idem
             0     < val ? (short int) ( val + 0.5 ) : // proper rounding
             (short int) ( val - 0.5 )               );
  }

  /// Print a warning message
  void Warning( const std::string & mess ) const;

  //---------------------------------------------------------------------------

public:

  /// Access the parent algorithm
  const GaudiAlgorithm * parent() const { return m_parent; }

  //---------------------------------------------------------------------------

private:

  /// Pointer to parent algorithm
  const GaudiAlgorithm * m_parent = nullptr;

  //---------------------------------------------------------------------------

};

#endif // KERNEL_STANDARDPACKER_H
