#ifndef KERNEL_STANDARDPACKER_H
#define KERNEL_STANDARDPACKER_H 1

// STL
#include <cmath>

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IRegistry.h"

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

  /// Standard constructor
  StandardPacker( ) {}

  ~StandardPacker( ) {} ///< Destructor

  //---------------------------------------------------------------------------

  /** returns an int for a double energy */
  int energy( const double e ) const
  {
    return packDouble( e * Packer::ENERGY_SCALE );
  }

  /** returns an int for a double position */
  int position( const double x ) const
  {
    return packDouble( x * Packer::POSITION_SCALE );
  }

  /** returns an int for a double slope */
  int slope( const double x ) const
  {
    return packDouble( x * Packer::SLOPE_SCALE );
  }

  /** returns an short int for a double fraction f */
  short int fraction( const double f ) const
  {
    return shortPackDouble( f * Packer::FRACTION_SCALE );
  }

  /** returns an short int for a double fraction top/bot */
  short int fraction( const double top, const double bot ) const
  {
    return fraction( fabs(bot) > 0 ? top/bot : 0.0 );
  }

  /** returns an int for a double time (TOF) value */
  int time( const double x ) const
  {
    return packDouble( x * Packer::TIME_SCALE );
  }

  /** returns an int for a double delta log likelihood value */
  int deltaLL( const double x ) const
  {
    return packDouble( x * Packer::DELTALL_SCALE );
  }

  /** returns an int for a double mass */
  int mass( const double mass ) const
  {
    return packDouble( mass * Packer::MASS_SCALE );
  }

  /** returns an int containing the float value of the double */
  int fltPacked( const double& x ) const
  {
    union fltInt { int i; float f; } convert;
    convert.f = (float)x;
    return convert.i;
  }

  //---------------------------------------------------------------------------

  /// Returns the 'LinkID' 
  long linkID( DataObject* out,
               const DataObject* parent ) const
  {
    LinkManager::Link* myLink = out->linkMgr()->link( parent );
    if ( NULL == myLink )
    {
      out->linkMgr()->addLink( parent->registry()->identifier(),
                               parent );
    }
    return out->linkMgr()->link(parent)->ID();
  }

  /// Returns the 'LinkID'
  long linkID( DataObject* out,
               const std::string& targetName ) const
  {
    LinkManager::Link* myLink = out->linkMgr()->link( targetName );
    if ( NULL == myLink )
    {
      out->linkMgr()->addLink( targetName, 0 );
      myLink = out->linkMgr()->link( targetName );
    }
    return myLink->ID();
  }

  //---------------------------------------------------------------------------

  /** returns an int for a Smart Ref.
   *  @arg  out : Output data object, to store the links
   *  @arg  parent : Pointer to the parent container of the SmartRef, method->parent()
   *  @arg  key    : returned by the method .linkID() of the SmartRef
   */
  int reference( DataObject* out,
                 const DataObject* parent,
                 const int key ) const
  {
    if ( key != (key & 0x0FFFFFFF) )
    {
      std::cout << "************************* Key over 28 bits in StandardPacker ***********************" 
                << " " << key
                << std::endl;
    }
    const int rawLinkID = (int)linkID(out,parent);
    if ( rawLinkID != (rawLinkID & 0x0000000F) )
    {
      std::cout << "************************* LinkID over 4 bits in StandardPacker ***********************" 
                << " " << rawLinkID
                << std::endl;
    }
    const int myLinkID = rawLinkID << 28;
    return key + myLinkID;
  }

  /** returns an int for a Smart Ref.
   *  @arg  out : Output data object, to store the links
   *  @arg  targetName : Name of the target
   *  @arg  key : returned by the method .linkID() of the SmartRef
   */
  int reference( DataObject* out,
                 const std::string& targetName,
                 const int key ) const
  {
    if ( key != (key & 0x0FFFFFFF) )
    {
      std::cout << "************************* Key over 28 bits in StandardPacker ***********************" 
                << " " << key
                << std::endl;
    }
    const int ID = (int)linkID(out,targetName);
    if ( ID != (ID & 0x0000000F) )
    {
      std::cout << "************************* LinkID over 4 bits in StandardPacker ***********************" 
                << " " << ID
                << std::endl;
    }
    const int myLinkID = (ID << 28);
    return key + myLinkID;
  }

  /// Extracts the key and index from a packed data word
  void indexAndKey( const int data,
                    int& indx,
                    int& key ) const
  {
    indx = data >> 28;
    key  = data & 0x0FFFFFFF;
  }

  /// Extracts the key and hint from a packed data word
  void hintAndKey( const int data,
                   const DataObject* source,
                   DataObject* target,
                   int& hint,
                   int& key ) const
  {
    int indx(0);
    indexAndKey(data,indx,key);
    hint = target->linkMgr()->addLink( source->linkMgr()->link(indx)->path(), 0 );
  }

  //---------------------------------------------------------------------------

  /** returns a long long for a Smart Ref, with small key and large links.
   *  @arg  out    : Output data object, to store the links
   *  @arg  parent : Pointer to the parent container of the SmartRef, method ->parent()
   *  @arg  key    : returned by the method .linkID() of the SmartRef
   */
  long long reference64( DataObject* out,
                         const DataObject* parent,
                         const int key ) const
  {
    const long long ID = (long long)linkID(out,parent);
    const long long myLinkID = (ID << 32);
    return (long long)key + myLinkID;
  }

  /// Extracts the key and index from a packed 64-bit data word
  void indexAndKey64( const long long data,
                      int& indx,
                      int& key ) const
  {
    indx = data >> 32;
    const long long mask = 0x00000000FFFFFFFF;
    key  = data & mask;
  }

  /// Extracts the key and hint from a packed 64-bit data word
  void hintAndKey64( const long long data,
                     const DataObject* source,
                     DataObject* target,
                     int& hint,
                     int& key ) const
  {
    int indx(0);
    indexAndKey64(data,indx,key);
    hint = target->linkMgr()->addLink( source->linkMgr()->link(indx)->path(), 0 );
  }

  //---------------------------------------------------------------------------

  /** returns the energy as double from the int value */
  double energy( const int k )         const { return double(k) / Packer::ENERGY_SCALE; }

  /** returns the position as double from the int value */
  double position( const int k )       const { return double(k) / Packer::POSITION_SCALE; }

  /** returns the slope as double from the int value */
  double slope( const int k )          const { return double(k) / Packer::SLOPE_SCALE; }

  /** returns the fraction as double from the short int value */
  double fraction( const short int k ) const { return double(k) / Packer::FRACTION_SCALE; }

  /** returns the time as double from the int value */
  double time( const int k )           const { return double(k) / Packer::TIME_SCALE; }

  /** returns the delta Log Likelihood as double from the int value */
  double deltaLL( const int k )        const { return double(k) / Packer::DELTALL_SCALE; }

  /** returns the mass as double from the int value */
  double mass( const int k )           const { return double(k) / Packer::MASS_SCALE; }

  /** returns an double from a int containing in fact a float */
  double fltPacked( const int k  ) const
  {
    union fltInt { int i; float f; } convert;
    convert.i = k;
    return double(convert.f);
  }

  //---------------------------------------------------------------------------

private:

  /// Pack a double to an int
  int packDouble ( const double val ) const
  {
    return ( 2.e9  < val ?  2000000000         : // saturate 31 bits
             -2.e9 > val ? -2000000000         : // idem
             0     < val ? (int) ( val + 0.5 ) : // proper rounding
             (int) ( val - 0.5 )               );
  }

  /// Pack a double to a short int
  short int shortPackDouble ( const double val ) const
  {
    return ( 3.e4  < val ? (short int)  30000        : // saturate 15 bits
             -3.e4 > val ? (short int) -30000        : // idem
             0     < val ? (short int) ( val + 0.5 ) : // proper rounding
             (short int) ( val - 0.5 )               );
  }

  //---------------------------------------------------------------------------

};

#endif // KERNEL_STANDARDPACKER_H
