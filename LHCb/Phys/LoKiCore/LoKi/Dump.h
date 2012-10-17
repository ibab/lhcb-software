// ============================================================================
#ifndef LOKI_DUMP_H 
#define LOKI_DUMP_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
/** @file 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-11-18
 */
// ============================================================================
 namespace LoKi
{
  // ==========================================================================
  /** @class Dump
   *  helper class to implement the dumping 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-11-17
   */
  class GAUDI_API Dump 
  {
  public:
    // ========================================================================
    Dump  ( const std::string&  open   = ""   ,
            const std::string&  close  = "\n" ) ;
    Dump  ( const std::size_t   nmax          , 
            const std::string&  open   = ""   ,
            const std::string&  close  = "\n" ) ;
    Dump  ( const std::string&  open          ,
            const std::string&  close         ,
            const std::size_t   nmax          ) ;    
    Dump  ( const std::string&  open          ,
            const std::size_t   nmax          ,
            const std::string&  close  = "\n" ) ;
    virtual ~Dump() ;
    // ========================================================================
  public:
    // ========================================================================
    const std::string& open  () const { return  m_open  ; }
    const std::string& close () const { return  m_close ; }        
    std::size_t        nMax  () const { return  m_nmax  ; }        
    // ========================================================================
  private:
    // ========================================================================
    /// the opening 
    std::string   m_open   ;                                     // the opening
    /// the closing 
    std::string   m_close  ;                                     // the closing 
    /// max prints 
    std::size_t   m_nmax   ;                                     // max prints 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_DUMP_H
// ============================================================================
