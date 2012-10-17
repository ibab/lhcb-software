// ============================================================================
#ifndef LOKI_DECAYCHAINBASE_H 
#define LOKI_DECAYCHAINBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Colors.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/KinTypes.h"
// ============================================================================
/** @file LoKi/DecayChainBase.h
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
  /** @class DecayChainBase
   *  
   *  Helper class for implementation of decay-chain printers 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
   *  @date   2011-06-03
   */
  class GAUDI_API DecayChainBase : public virtual LoKi::AuxFunBase 
  {
    // ========================================================================
  public:
    // ========================================================================
    // modes to print 4-vectors 
    enum Mode    
      { 
        LV_STD    = 0 , // default: as 4-values (E/px/py/pz) 
        LV_MASS       , //          as 5 values (M/E/px/py/pz)
        LV_WITHPT     , //          as 6 values (M/PT/E/px/py/pz) 
        LV_ONLYP      , //          as 3 values (px/py/pz) 
        LV_SHORT      , //          as 3 values (M/PT/E) 
        LV_MPTYPHI    , //          as 4 values (M/PT/Y/PHI) 
        LV_MPTETAPHI  , //          as 4 values (M/PT/ETA/PHI) 
        LV_MPTY       , //          as 3 values (M/PT/Y) 
        LV_MPTETA       //          as 3 values (M/PT/ETA) 
      } ;
    // ========================================================================
  public:
    // ========================================================================
    /** Standard constructor
     *  @param maxDepth  maximal decay depth 
     *  @param vertex    vertex info to be printed 
     *  @param mode      mode for printout of 4-vectors 
     *  @param fg        color for foreground for 'marked' lines 
     *  @param bg        color for background for 'marked' lines 
     */
    DecayChainBase 
    ( const size_t          maxDepth = 5            , 
      const bool            vertex   = false        , 
      const Mode            mode     = LV_WITHPT    ,
      const MSG::Color&     fg       = MSG::YELLOW  ,
      const MSG::Color&     bg       = MSG::RED     ) ;
    /// virtual destructor 
    virtual ~DecayChainBase () ;                         // virtual destructor 
    // ========================================================================
  public:
    // ========================================================================    
    /// max depth 
    size_t            maxDepth    () const { return m_maxDepth ; }
    void              setMaxDepth (  const size_t      value )  
    { m_maxDepth   = value ; }
    // ========================================================================
    /// print vertex info ?
    bool              vertex      () const { return m_vertex   ; }
    void              setVertex   (  const bool value ) 
    { m_vertex     = value ; }
    // ========================================================================
    /// color for foreground (for marked entities)
    const MSG::Color& fg          () const { return m_fg ; }
    void              setFG       (  const MSG::Color& value ) 
    { m_fg         = value ; }
    // ========================================================================
    /// color for background (for marked entities)
    const MSG::Color& bg          () const { return m_bg ; }
    void              setBG       (  const MSG::Color& value ) 
    { m_bg         = value ; }
    Mode              mode        () const { return m_mode ; }
    void              setMode     (  const Mode&  value ) 
    { m_mode = value ; }
    // ========================================================================    
    void setFmt_M  ( const std::string& value ) { m_fmt_m  = value ; }
    void setFmt_P  ( const std::string& value ) { m_fmt_p  = value ; }
    void setFmt_PT ( const std::string& value ) { m_fmt_pt = value ; }
    void setFmt_V  ( const std::string& value ) { m_fmt_v  = value ; }
    void setFmt_D  ( const std::string& value ) { m_fmt_d  = value ; }
    void setFmt_I  ( const std::string& value ) { m_fmt_i  = value ; }
    // ========================================================================    
  protected:
    // ========================================================================
    /// particle name by ID
    std::string name     ( const LHCb::ParticleID&    p ) const ;
    /// long   as string
    std::string toString ( const long                 v ) const ;
    /// int    as string
    std::string toString ( const int                  v ) const ;
    /// double as string
    std::string toString ( const double               v ) const ;
    /// Lorentz vector as string
    std::string toString ( const LoKi::LorentzVector& v , 
                           const Mode&                m ) const ;
    /// Lorentz vector as string
    std::string toString ( const LoKi::LorentzVector& v ) const ;
    /// 3D-point as string
    std::string toString ( const LoKi::Point3D&       v ) const ;
    /// 3D-vector as string
    std::string toString ( const LoKi::Vector3D&      v ) const ;
    // ========================================================================    
  protected:
    // ========================================================================
    /// the maximal depth 
    size_t                m_maxDepth ; // the maximal depth 
    /// print (main) vertex information 
    bool                  m_vertex   ; // print (main) vertex information  
    /// The default color for  foreground 
    MSG::Color            m_fg       ; // The default color for  foreground 
    /// The default color for  background 
    MSG::Color            m_bg       ; // The default color for  background 
    /// printout mode 
    Mode                  m_mode     ; // the printout mode 
    // format for "mass" 
    std::string           m_fmt_m    ; 
    // format for "PT" 
    std::string           m_fmt_pt   ;
    // format for "P/E" 
    std::string           m_fmt_p    ;
    // format for vertices 
    std::string           m_fmt_v    ;
    // general format for doubles 
    std::string           m_fmt_d    ;
    // general format for integers  
    std::string           m_fmt_i    ;
    // ========================================================================
  protected:
    // ========================================================================
    /// the default indentation string
    static std::string blank ( const int len )  ; 
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_DECAYCHAINBASE_H
// ============================================================================
