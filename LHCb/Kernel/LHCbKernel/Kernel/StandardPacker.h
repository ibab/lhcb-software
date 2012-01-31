#ifndef KERNEL_STANDARDPACKER_H
#define KERNEL_STANDARDPACKER_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IRegistry.h"

/** @class StandardPacker StandardPacker.h Kernel/StandardPacker.h
 *  This is a simple class to convert to int/short with standard factors
 *  various quantities.
 *
 *  @author Olivier Callot
 *  @date   2005-03-15
 */

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

  /// Standard constructor
  StandardPacker( ) {};

  ~StandardPacker( ) {}; ///< Destructor

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

  /** returns an short int for a double fraction */
  short int fraction( const double f ) const
  {
    return shortPackDouble( f * Packer::FRACTION_SCALE );
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
  int fltPacked( const double x  ) const
  {
    union fltInt { int i; float f; } convert;
    convert.f = (float)x;
    return convert.i;
  }

  /** returns an int for a Smart Ref.
   *  @arg  out : Output data object, to store the links
   *  @arg  parent : Pointer to the parent container of the SmartRef, method ->parent()
   *  @arg  key    : returned by the method .linkID() of the SmartRef
   */
  int reference( DataObject* out, 
                 const DataObject* parent, 
                 const int key ) const
  {
    LinkManager::Link* myLink = out->linkMgr()->link( parent );
    if ( NULL == myLink )
    {
      out->linkMgr()->addLink( parent->registry()->identifier(),
                               parent );
    }
    if ( key != (key & 0x0FFFFFFF ) )
    {
      std::cout << "************************* Key over 28 bits in StandardPacker ***********************" << std::endl;
    }
    int myLinkID = out->linkMgr()->link( parent )->ID() << 28;
    return key + myLinkID;
  }

  int reference( DataObject* out, 
                 const std::string& targetName, 
                 const int key ) const
  {
    LinkManager::Link* myLink = out->linkMgr()->link( targetName );
    if ( NULL == myLink ) {
      out->linkMgr()->addLink( targetName, 0 );
      myLink = out->linkMgr()->link( targetName );
    }
    int myLinkID = myLink->ID() << 28;
    return key + myLinkID;
  }  

  void hintAndKey( const int data,
                   const DataObject* source,
                   DataObject* target,
                   int& hint, int& key ) const
  {
    const int indx = data >> 28;
    key = data & 0x0FFFFFFF;
    hint = target->linkMgr()->addLink( source->linkMgr()->link( indx )->path(), 0 );
  }

  /** returns an int for a Smart Ref, with small key and large links.
   *  @arg  out : Output data object, to store the links
   *  @arg  parent : Pointer to the parent container of the SmartRef, method ->parent()
   *  @arg  key    : returned by the method .linkID() of the SmartRef
   */
  int referenceLong( DataObject* out, const DataObject* parent, const int key ) const {
    LinkManager::Link* myLink = out->linkMgr()->link( parent );
    if ( NULL == myLink ) {
      out->linkMgr()->addLink( parent->registry()->identifier(), parent );
    }
    if ( key != (key & 0x0000FFFF ) ) {
      std::cout << "************ Key over 16 bits in StandardPacker::referenceLong ************" << std::endl;
    }
    int myLinkID = out->linkMgr()->link( parent )->ID() << 16;
    return key + myLinkID;
  }

  void hintAndKeyLong( const int data,
                       const DataObject* source,
                       DataObject* target,
                       int& hint, int& key ) const {
    const int indx = data >> 16;
    key = data & 0x0000FFFF;
    hint = target->linkMgr()->addLink( source->linkMgr()->link( indx )->path(), 0 );
  }

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

protected:

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

};

#endif // KERNEL_STANDARDPACKER_H
