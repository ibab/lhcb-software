// $Id: StandardPacker.h,v 1.9 2009-11-10 10:27:14 jonrob Exp $
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

namespace Packer {
  const double ENERGY_SCALE     = 1.0e2;  ///< .01 MeV steps
  const double POSITION_SCALE   = 1.0e4;  ///< 0.1 micron steps
  const double SLOPE_SCALE      = 1.0e8;  ///< full scale +- 20 radians
  const double FRACTION_SCALE   = 3.0e4;  ///< store in short int.
  const double TIME_SCALE       = 1.0e5;  ///< 0.0001 ns resolution
  const double DELTALL_SCALE    = 1.0e4;  ///< 0.0001 precision
}

class StandardPacker {
public:

  /// Standard constructor
  StandardPacker( ) {};

  ~StandardPacker( ) {}; ///< Destructor

  /** returns an int for a double energy */
  int energy( const double e ) const {
    return packDouble( e * Packer::ENERGY_SCALE );
  }

  /** returns an int for a double position */
  int position( const double x ) const {
    return packDouble( x * Packer::POSITION_SCALE );
  }

  /** returns an int for a double slope */
  int slope( const double x ) const {
    return packDouble( x * Packer::SLOPE_SCALE );
  }

  /** returns an short int for a double fraction */
  short int fraction( const double f ) const {
    return shortPackDouble( f * Packer::FRACTION_SCALE );
  }

  /** returns an int for a double time (TOF) value */
  int time( const double x ) const {
    return packDouble( x * Packer::TIME_SCALE );
  }

  /** returns an int for a double delta log likelihood value */
  int deltaLL( const double x ) const {
    return packDouble( x * Packer::DELTALL_SCALE );
  }

  /** returns an int containing the float value of the double */
  int fltPacked( const double x  ) const {
    union fltInt { int i; float f; } convert;
    convert.f = (float)x;
    return convert.i;
  }

  /** returns an int for a Smart Ref.
   *  @arg  out : Output data object, to store the links
   *  @arg  parent : Pointer to the parent container of the SmartRef, method ->parent()
   *  @arg  key    : returned by the method .linkID() of the SmartRef
   */
  int reference( DataObject* out, const DataObject* parent, const int key ) const 
  {
    LinkManager::Link* myLink = out->linkMgr()->link( parent );
    if ( NULL == myLink ) {
      out->linkMgr()->addLink( parent->registry()->identifier(),
                               parent );
    }
    if ( key != (key & 0x0FFFFFFF ) ) {
      std::cout << "************************* Key over 28 bits in StandardPacker ***********************" << std::endl;
    }
    int myLinkID = out->linkMgr()->link( parent )->ID() << 28;
    return key + myLinkID;
  }

  void hintAndKey( const int data, 
                   const DataObject* source, 
                   DataObject* target, 
                   int& hint, int& key ) const {
    const int indx = data >> 28;
    key = data & 0x0FFFFFFF;
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

  /** returns an double from a int containing in fact a float */
  double fltPacked( const int k  ) const {
    union fltInt { int i; float f; } convert;
    convert.i = k;
    return double(convert.f);
  }

protected:

  int packDouble ( double val ) const 
  {
    if (  2.e9 < val ) return  2000000000;  // saturate 31 bits
    if ( -2.e9 > val ) return -2000000000;  // idem
    if ( 0 < val ) return int( val + 0.5 ); // proper rounding
    return int( val - 0.5 );
  }

  short int shortPackDouble ( double val ) const {
    if (  3.e4 < val ) return  30000;  // saturate 15 bits
    if ( -3.e4 > val ) return -30000;  // idem
#ifdef __INTEL_COMPILER         // Disable ICC remark
  #pragma warning(disable:2259) // non-pointer conversion from "int" to "short" may lose significant bits
  #pragma warning(push)
#endif
    if ( 0 < val ) return int( val + 0.5 ); // proper rounding
    return int( val - 0.5 );
#ifdef __INTEL_COMPILER         // Re-enable ICC remark 2259
  #pragma warning(pop)
#endif
  }

};
#endif // KERNEL_STANDARDPACKER_H
