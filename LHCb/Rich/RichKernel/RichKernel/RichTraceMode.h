// $Id: RichTraceMode.h,v 1.1 2004-07-01 11:12:09 papanest Exp $
#ifndef RICHKERNEL_RICHTRACEMODE_H 
#define RICHKERNEL_RICHTRACEMODE_H 1

// Include files
// Gaudi
#include "GaudiKernel/MsgStream.h"

/** @class RichTraceMode RichTraceMode.h RichKernel/RichTraceMode.h
 *  
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

// Namespace for definitions related to RichTraceMode
namespace RichTraceModeNames {
  
  // Type for dataword
  typedef unsigned int ShortType;

  static const ShortType BitsForcedSide   = 1; ///< number of bits for this flag
  static const ShortType BitsOutMirBound  = 1; ///< number of bits outside mirror boundary
  static const ShortType BitsDetPrecision = 1; ///< number of bits for detection precision
  static const ShortType BitsDetPlaneBound = 1; ///< number of bits for detection plane boundary
  static const ShortType BitsMirrorSegBound = 1; ///< number of bits mirror segment boundary

  // Shifts
  static const ShortType ShiftForcedSide   = 0;
  static const ShortType ShiftOutMirBound  = BitsOutMirBound + ShiftForcedSide;
  static const ShortType ShiftDetPrecision = BitsDetPrecision + ShiftOutMirBound;
  static const ShortType ShiftDetPlaneBound = BitsDetPlaneBound + ShiftDetPrecision;
  static const ShortType ShiftMirrorSegBound = BitsMirrorSegBound + ShiftDetPlaneBound;

  // Create the Masks
  static const ShortType MaskForcedSide = ((1 << BitsForcedSide)-1) << ShiftForcedSide;
  static const ShortType MaskOutMirBound = ((1 << BitsOutMirBound)-1) << ShiftOutMirBound;
  static const ShortType MaskDetPrecision = ((1 << BitsDetPrecision)-1) << ShiftDetPrecision;
  static const ShortType MaskDetPlaneBound = ((1 << BitsDetPlaneBound)-1) << ShiftDetPlaneBound;
  static const ShortType MaskMirrorSegBound = ((1 << BitsMirrorSegBound)-1) << ShiftMirrorSegBound;

}

class RichTraceMode {
public: 

  enum detectPrecision {
    circle = 0,
    window
  };

  enum detPlaneBoundary {
    loose = 0,
    tight
  };
    

  /// Standard constructor
  RichTraceMode() : m_data(0) {}

  RichTraceMode( detectPrecision detPrec ) : m_data( 0 ) {
    setDetPrecision( detPrec );
  }
  
  RichTraceMode( detPlaneBoundary bound ) : m_data( 0 ) {
    setDetPlaneBound( bound );
  }
  

  virtual ~RichTraceMode() {} ///< Destructor

  /// Retrieve the full value
  inline RichTraceModeNames::ShortType data() const { return m_data; }

  /// Update the internal data
  inline void setData( const RichTraceModeNames::ShortType data ) { m_data = data; }

  /// Set the ForcedSide flag
  inline void setForcedSide( const bool forced ) {
    RichTraceModeNames::ShortType i = ( forced ? 1 : 0 );
    set( i, 
         RichTraceModeNames::ShiftForcedSide, 
         RichTraceModeNames::MaskForcedSide );
    return;
  }

  /// Retrieve the ForcedSide flag
  inline bool forcedSide() const {
    return ( 0 != ( (data() & RichTraceModeNames::MaskForcedSide) 
                    >> RichTraceModeNames::ShiftForcedSide ) );
  }

  /// Set the flag for respecting the outter boundaries of the mirrors
  inline void setOutMirrorBoundary( const bool outMirBound ) {
    RichTraceModeNames::ShortType i = ( outMirBound ? 1 : 0 );
    set( i, 
         RichTraceModeNames::ShiftOutMirBound, 
         RichTraceModeNames::MaskOutMirBound );
    return;
  }

  /// Retrieve the flag for respecting the outter boundaries of the mirrors
  inline bool outMirrorBoundary() const {
    return ( 0 != ( (data() & RichTraceModeNames::MaskOutMirBound) 
                    >> RichTraceModeNames::ShiftOutMirBound ) );
  }

  /// Set the flag for detection precision
  inline void setDetPrecision( const detectPrecision precision ) {
    set( precision, 
         RichTraceModeNames::ShiftDetPrecision, 
         RichTraceModeNames::MaskDetPrecision );
    return;
  }

  /// Retrieve the flag for detection precision
  inline detectPrecision detPrecision() const {
    if ( 0 == ((data() & RichTraceModeNames::MaskDetPrecision) 
               >> RichTraceModeNames::ShiftDetPrecision) )
      return circle;
    else
      return window;
    
  }

  /// Set the flag for respecting the detection plane boundary
  inline void setDetPlaneBound( const detPlaneBoundary boundary ) {
    set( boundary, 
         RichTraceModeNames::ShiftDetPlaneBound, 
         RichTraceModeNames::MaskDetPlaneBound );
    return;
  }

  /// Retrieve the flag for respecting the detection plane boundary
  inline detPlaneBoundary detPlaneBound() const {
    if ( 0 == ((data() & RichTraceModeNames::MaskDetPlaneBound) 
               >> RichTraceModeNames::ShiftDetPlaneBound) )
      return loose;
    else
      return tight;
    
  }

  /// Set the flag for respecting the mirror segment boundaries
  inline void setMirrorSegBoundary( const bool mirrorBound ) {
    RichTraceModeNames::ShortType i = ( mirrorBound ? 1 : 0 );
    set( i, 
         RichTraceModeNames::ShiftMirrorSegBound, 
         RichTraceModeNames::MaskMirrorSegBound );
    return;
  }

  /// Retrieve the flag for respecting the mirror segment boundaries
  inline bool mirrorSegBoundary() const {
    return ( 0 != ( (data() & RichTraceModeNames::MaskMirrorSegBound) 
                    >> RichTraceModeNames::ShiftMirrorSegBound ) );
  }

protected:

private:

  RichTraceModeNames::ShortType m_data;
  
  inline bool set( const RichTraceModeNames::ShortType value,
                   const RichTraceModeNames::ShortType shift,
                   const RichTraceModeNames::ShortType mask ) {
    setData( ((value << shift) & mask) | (data() & ~mask) );
    return true;
  }

};

/// overloaded output to MsgStream
inline MsgStream& operator << ( MsgStream& os,
                                const RichTraceMode& mode )
{
  std::string prec;
  if ( mode.detPrecision() == RichTraceMode::window )
    prec = "window";
  else
    prec = "circle";

  std::string bound;
  if ( mode.detPlaneBound() == RichTraceMode::loose )
    bound = "loose";
  else
    bound = "tight";
  
  os << "ForcedSide = " << mode.forcedSide()
     << ", Respect Outside Mirror Boundaries = " << mode.outMirrorBoundary()
     << ", Respect Mirror Seg Boundaries = " << mode.mirrorSegBoundary()
     << ", Detection Precision: " << prec
     << ", Detection Plane Boundary: " << bound;
  return os;
}
#endif // RICHKERNEL_RICHTRACEMODE_H
