//===============================================================================
/** @file RichTraceMode.h
 *
 *  Header file for utility class : RichTraceMode
 *
 *  CVS History :-
 *  $Id: RichTraceMode.h,v 1.1 2005-12-19 16:52:16 cattanem Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//===============================================================================

#ifndef KERNEL_RICHTRACEMODE_H
#define KERNEL_RICHTRACEMODE_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

//===============================================================================
/** @namespace RichTraceModeNames
 *
 *  Namespace for bit-packing parameters used by the utility class RichTraceMode
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//===============================================================================
namespace RichTraceModeNames 
{

  // Type for dataword
  typedef unsigned int ShortType;

  // Number of bits for each field
  static const ShortType BitsForcedSide     = 1; ///< Number of bits for the forced side flag
  static const ShortType BitsOutMirBound    = 1; ///< Number of bits for outside mirror boundary flag
  static const ShortType BitsDetPrecision   = 1; ///< Number of bits for detection precision flag
  static const ShortType BitsDetPlaneBound  = 1; ///< Number of bits for detection plane boundary flag
  static const ShortType BitsMirrorSegBound = 1; ///< Number of bits mirror segment boundary flag

  // Shifts
  static const ShortType ShiftForcedSide     = 0;
  static const ShortType ShiftOutMirBound    = BitsOutMirBound + ShiftForcedSide;
  static const ShortType ShiftDetPrecision   = BitsDetPrecision + ShiftOutMirBound;
  static const ShortType ShiftDetPlaneBound  = BitsDetPlaneBound + ShiftDetPrecision;
  static const ShortType ShiftMirrorSegBound = BitsMirrorSegBound + ShiftDetPlaneBound;

  // Create the Masks
  static const ShortType MaskForcedSide     = ((1 << BitsForcedSide)-1) << ShiftForcedSide;
  static const ShortType MaskOutMirBound    = ((1 << BitsOutMirBound)-1) << ShiftOutMirBound;
  static const ShortType MaskDetPrecision   = ((1 << BitsDetPrecision)-1) << ShiftDetPrecision;
  static const ShortType MaskDetPlaneBound  = ((1 << BitsDetPlaneBound)-1) << ShiftDetPlaneBound;
  static const ShortType MaskMirrorSegBound = ((1 << BitsMirrorSegBound)-1) << ShiftMirrorSegBound;

}

//===============================================================================
/** @class RichTraceMode RichTraceMode.h RichKernel/RichTraceMode.h
 *
 *  Helper class used to configure the ray tracing options in
 *  the IRichRayTracing tools
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//===============================================================================
class RichTraceMode {

public:

  //=============================================================================
  /** Enumeration for the RichTraceMode to flag the precision mode to use
   *  when calculating the acceptance on the HPD panels
   *
   *  @author Antonis Papanestis a.papanestis@rl.ac.uk
   *  @date   2004-06-18
   */
  //=============================================================================
  enum detectPrecision
    {
      circle = 0,   ///< Approximate the HPD window acceptance by circles (fast)
      window        ///< Use the full detector description for the HPDs (slower)
    };

  //=============================================================================
  /** Enumeration for the RichTraceMode to flag how to treat the HPD
   *  panel boundaries
   *
   *  @author Antonis Papanestis a.papanestis@rl.ac.uk
   *  @date   2004-06-18
   */
  //=============================================================================
  enum detPlaneBoundary
    {
      loose = 0,    ///< Dis-regard the physical HPD boundaries (assume infinite plane)
      tight         ///< Respect the average physical boundaries of the HPDs on the panel plane
    };


  /// Constructor from explicit mode information
  RichTraceMode
  ( const detectPrecision  detPrec         = circle, ///< Precision to use on HPD panel
    const detPlaneBoundary bound           = loose,  ///< HPD panel boundary mode
    const bool             forcedSide      = false,  ///< Force the ray-tracing to follow the given RICH side
    const bool             respectOuter    = false,  ///< Respect the outer boundaries of the mirrors
    const bool             respectMirrSegs = false   ///< Respect the gaps between the individual mirror segments
    ) : m_data( 0 )
  {
    setDetPrecision      ( detPrec         );
    setDetPlaneBound     ( bound           );
    setForcedSide        ( forcedSide      );
    setOutMirrorBoundary ( respectOuter    );
    setMirrorSegBoundary ( respectMirrSegs );
  }

  /// Copy Constructor
  RichTraceMode ( const RichTraceMode & mode    ///< Source mode object to copy properties from
                  ) : m_data( mode.data() ) { }

  /// Destructor
  ~RichTraceMode() {}

  /** Access to the full bit-packed data word
   *
   *  @return the bit-packed data word
   */
  inline RichTraceModeNames::ShortType data() const
  {
    return m_data;
  }

  /** Set the full bit-packed data word
   */
  inline void setData ( const RichTraceModeNames::ShortType data )
  {
    m_data = data;
  }

  /** Set the flag to force the side to use in the ray-tracing
   */
  inline void setForcedSide( const bool forced )
  {
    const RichTraceModeNames::ShortType i = ( forced ? 1 : 0 );
    set( i,
         RichTraceModeNames::ShiftForcedSide,
         RichTraceModeNames::MaskForcedSide );
  }

  /** Retrieve the ForcedSide flag
   *
   * @return the forced side flag
   * @retval true  The side of the detector will be forced
   * @retval false The side will not be forced, but allow to follow the true detector optics
   */
  inline bool forcedSide() const
  {
    return ( 0 != ( (data() & RichTraceModeNames::MaskForcedSide)
                    >> RichTraceModeNames::ShiftForcedSide ) );
  }

  /// Set the flag for respecting the outer boundaries of the mirrors
  inline void setOutMirrorBoundary( const bool outMirBound )
  {
    const RichTraceModeNames::ShortType i = ( outMirBound ? 1 : 0 );
    set( i,
         RichTraceModeNames::ShiftOutMirBound,
         RichTraceModeNames::MaskOutMirBound );
  }

  /** Retrieve the flag for respecting the outer boundaries of the mirrors
   *
   * @return the outer mirror boundary flag
   * @retval true  The outer physical boundaries will be respected
   * @retval false The mirror will be assumed to have infinite extent
   */
  inline bool outMirrorBoundary() const
  {
    return ( 0 != ( (data() & RichTraceModeNames::MaskOutMirBound)
                    >> RichTraceModeNames::ShiftOutMirBound ) );
  }

  /// Set the flag for detection precision
  inline void setDetPrecision( const detectPrecision precision )
  {
    set( precision,
         RichTraceModeNames::ShiftDetPrecision,
         RichTraceModeNames::MaskDetPrecision );
  }

  /// Retrieve the detection precision mode
  inline detectPrecision detPrecision() const
  {
    return ( ( 0 == ((data() & RichTraceModeNames::MaskDetPrecision)
                     >> RichTraceModeNames::ShiftDetPrecision) ) ?
             circle : window );
  }

  /// Set the flag for respecting the detection plane boundary
  inline void setDetPlaneBound( const detPlaneBoundary boundary )
  {
    set( boundary,
         RichTraceModeNames::ShiftDetPlaneBound,
         RichTraceModeNames::MaskDetPlaneBound );
  }

  /// Retrieve the detection plane boundary mode
  inline detPlaneBoundary detPlaneBound() const
  {
    return ( ( 0 == ((data() & RichTraceModeNames::MaskDetPlaneBound)
                     >> RichTraceModeNames::ShiftDetPlaneBound) ) ?
             loose : tight );
  }

  /// Set the flag for respecting the mirror segment boundaries
  inline void setMirrorSegBoundary( const bool mirrorBound )
  {
    const RichTraceModeNames::ShortType i = ( mirrorBound ? 1 : 0 );
    set( i,
         RichTraceModeNames::ShiftMirrorSegBound,
         RichTraceModeNames::MaskMirrorSegBound );
  }

  /** Retrieve the flag for respecting the mirror segment boundaries
   *
   * @return the mirror segment boundary flag
   * @retval true  The mirror segment boundaries will be respected
   * @retval false The mirror will be treated as a single continuous object
   */
  inline bool mirrorSegBoundary() const
  {
    return ( 0 != ( (data() & RichTraceModeNames::MaskMirrorSegBound)
                    >> RichTraceModeNames::ShiftMirrorSegBound ) );
  }

private: // methods

  inline void set( const RichTraceModeNames::ShortType value,
                   const RichTraceModeNames::ShortType shift,
                   const RichTraceModeNames::ShortType mask )
  {
    setData( ((value << shift) & mask) | (data() & ~mask) );
  }

private: // data

  /// The bit-pack data word
  RichTraceModeNames::ShortType m_data;

};

/// overloaded output to MsgStream
inline MsgStream& operator << ( MsgStream& os,
                                const RichTraceMode& mode )
{
  const std::string prec = ( mode.detPrecision() == RichTraceMode::window ? "window" : "circle" );
  const std::string bound = ( mode.detPlaneBound() == RichTraceMode::loose ? "loose" : "tight" );
  os << "ForcedSide = " << mode.forcedSide()
     << ", Respect Outside Mirror Boundaries = " << mode.outMirrorBoundary()
     << ", Respect Mirror Seg Boundaries = " << mode.mirrorSegBoundary()
     << ", Detection Precision: " << prec
     << ", Detection Plane Boundary: " << bound;
  return os;
}

#endif // KERNEL_RICHTRACEMODE_H
