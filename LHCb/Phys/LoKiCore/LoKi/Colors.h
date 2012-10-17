// ============================================================================
#ifndef LOKI_COLORS_H 
#define LOKI_COLORS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
// ============================================================================
/** @file LoKi/Colors.h
 *  Helper file with functions to provide colorized output 
 * 
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Colors
   *  set of primitive utilities for colorized output 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace Colors
  {
    // ========================================================================
    /** set the colors for output stream (not active for WIN32)
     *  @param  stream  reference for stream 
     *  @param  fg      color for foreground 
     *  @param  bg      color for background 
     *  @return true if color are changed 
     */
    GAUDI_API 
    bool setColor 
    ( std::ostream&     stream              , 
      const MSG::Color& fg     = MSG::BLACK , 
      const MSG::Color& bg     = MSG::WHITE ) ;
    // ========================================================================
    /** set the colors for output stream (not active for WIN32)
     *  @param  stream  reference for stream 
     *  @param  fg      color for foreground 
     *  @param  bg      color for background 
     *  @return true if color are changed 
     */
    GAUDI_API
    bool setColor 
    ( MsgStream&        stream              , 
      const MSG::Color& fg     = MSG::BLACK , 
      const MSG::Color& bg     = MSG::WHITE ) ;
    // ========================================================================
    /** reset the colors for output stream (not active for WIN32)
     *  @param  stream  reference for stream 
     *  @return true if color are changed 
     */
    GAUDI_API 
    bool resetColor ( std::ostream&     stream ) ;
    // ========================================================================
    /** reset the colors for output stream (not active for WIN32)
     *  @param  stream  reference for stream 
     *  @return true if color are changed 
     */
    GAUDI_API
    bool resetColor ( MsgStream&        stream ) ;
    // ========================================================================
  }
  // ==========================================================================  
  /** @class MarkStrem 
   *  helper utility class to "mark" the stream 
   *  @author Vanya BELYAEV ibelyaev@phsyics.syr.edu
   */
  template <class STREAM> 
  class MarkStream
  {
  public:
    // ========================================================================
    MarkStream ( STREAM&           stream , 
                 const bool        mark   ,
                 const MSG::Color& fg     , 
                 const MSG::Color& bg     )
      : m_stream ( stream )
      , m_mark   ( mark   )
      , m_fg     ( fg     )
      , m_bg     ( bg     )
    { if ( m_mark )  {  LoKi::Colors::setColor ( m_stream , m_fg , m_bg ) ; } }
    //
    ~MarkStream  () { if( m_mark ) { LoKi::Colors::resetColor ( m_stream ) ; } }
    // ========================================================================
  private:
    // ========================================================================
    MarkStream   ()       ;
    // ========================================================================
  private:
    // ========================================================================
    STREAM&     m_stream  ;
    bool        m_mark    ;
    MSG::Color  m_fg      ;
    MSG::Color  m_bg      ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_COLORS_H
// ============================================================================
