// $Id: StandardPacker.h,v 1.2 2008-11-19 07:18:27 ocallot Exp $
#ifndef KERNEL_STANDARDPACKER_H 
#define KERNEL_STANDARDPACKER_H 1

// Include files
#include "math.h"   // get the log
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
  const double ENERGY_SCALE     = 1.e2;  ///< .01 MeV steps
  const double POSITION_SCALE   = 1.e4;  ///< 0.1 micron steps
  const double SLOPE_SCALE      = 1.e8;  ///< full scale +- 20 radians
  const double FRACTION_SCALE   = 3.e4;  ///< store in short int.
  const double SMALL_SCALE      = 1.e7;  ///< store the log with this accuracy
}

class StandardPacker {
public: 
  /// Standard constructor
  StandardPacker( ) {}; 

  ~StandardPacker( ) {}; ///< Destructor

  /** returns an int for a double energy */
  int energy( double e ) { 
    return packDouble( e * Packer::ENERGY_SCALE );  
  }
  
  /** returns an int for a double position */
  int position( double x ) { 
    return packDouble( x * Packer::POSITION_SCALE ); 
  }
  
  /** returns an int for a double slope */
  int slope( double x ) { 
    return packDouble( x * Packer::SLOPE_SCALE ); 
  }
  
  /** returns an short int for a double fraction, for covariance matrix */
  short int fraction( double f ) { 
    return shortPackDouble( f * Packer::FRACTION_SCALE );  
  }

  /** returns an int from a value with a log between -100 and 100 ! */
  int logPacked( double x  ) { 
    double tmp = 0.;
    if ( 0 == x ) return 0;
    if ( 0 < x ) {
      tmp = log( x ) + 100.;
      if ( 0 > tmp ) tmp = 0.;
    } else {
      tmp = - ( log( -x ) + 100. );
      if ( 0 < tmp ) tmp = 0.;
    }
    return packDouble( tmp * Packer::SMALL_SCALE ); 
  }

  /** returns an int for a Smart Ref.
   *  @arg  out : Output data object, to store the links
   *  @arg  parent : Pointer to the parent container of the SmartRef, method ->parent()
   *  @arg  key    : returned by the method .linkID() of the SmartRef
   */
  int reference( DataObject* out, const DataObject* parent, int key ) {
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

  void hintAndKey( int data, DataObject* source, DataObject* target, int& hint, int& key ) {
    int indx = data >> 28;
    key = data & 0x0FFFFFFF;
    hint = target->linkMgr()->addLink( source->linkMgr()->link( indx )->path(), 0 );
  }

  /** returns the energy as double from the int value */
  double energy( int k )         { return double(k) / Packer::ENERGY_SCALE; }
  
  /** returns the position as double from the int value */
  double position( int k )       { return double(k) / Packer::POSITION_SCALE; }
  
  /** returns the slope as double from the int value */
  double slope( int k )          { return double(k) / Packer::SLOPE_SCALE; }
  
  /** returns the fraction as double from the short int value */
  double fraction( short int k ) { return double(k) / Packer::FRACTION_SCALE; }  

  /** returns an double from a log packed value */
  double logPacked( int k  ) {
    if ( 0 == k ) return 0.;
    if ( 0 < k ) return exp( double( k )/ Packer::SMALL_SCALE - 100. );
    return -exp( double( -k ) / Packer::SMALL_SCALE -100. );
  }

protected:
  int packDouble ( double val ) {
    if (  2.e9 < val ) return  2000000000;  // saturate 31 bits
    if ( -2.e9 > val ) return -2000000000;  // idem
    if ( 0 < val ) return int( val + 0.5 ); // proper rounding
    return int( val - 0.5 );
  }

  short int shortPackDouble ( double val ) {
    if (  3.e4 < val ) return  30000;  // saturate 15 bits
    if ( -3.e4 > val ) return -30000;  // idem
    if ( 0 < val ) return int( val + 0.5 ); // proper rounding
    return int( val - 0.5 );
  }

private:

};
#endif // KERNEL_STANDARDPACKER_H
