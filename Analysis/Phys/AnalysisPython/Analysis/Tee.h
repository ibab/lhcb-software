// $Id$ 
// ============================================================================
#ifndef ANALYSIS_TEE_H 
#define ANALYSIS_TEE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STDL
// ============================================================================
#include <memory>
#include <ostream>
#include <streambuf>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
namespace Gaudi
{
  // ===========================================================================
  namespace Utils 
  {
    /** @class Tee Tee.h Analysis/Tee.h
     *  Helper utility for "tee"
     *  @see Gaudi::Utils::Mute 
     *  @author Vanya Belyaev
     *  @date   2013-07-07
     */
    class GAUDI_API Tee 
    {
    public: 
      // ======================================================================
      /// constructor from filename 
      Tee  ( const std::string&  filename   = "tee.out" ) ; 
      /// constructor from the stream 
      Tee  ( std::ostream& filestream ) ; 
      // destructor 
      ~Tee ( ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// copy     constructor is disabled 
      Tee ( const Tee& ) ;                   // copy    constructor is disabled 
      /// assignement operator is disabled 
      Tee& operator=( const Tee& );         // assignement operator is disabled 
      // ======================================================================
    public: // helper stuff to use it in python as Context Manager 
      // ======================================================================
      /** helper function to implement python's __enter__  
       *  the action is performed in constructor 
       */
      void   enter () ;
      /// helper function to implement python's __exit__
      void   exit  () ;
      // ======================================================================
    private: 
      // ======================================================================
      /// the file itself 
      std::auto_ptr<std::ostream>   m_file   ; // the file itself 
      /// is the file owned?
      bool                          m_own    ; // is the file owned?
      std::auto_ptr<std::streambuf> m_buffer ;
      std::streambuf*               m_keep   ; // keep the standard buffer 
      // ======================================================================
    };
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils 
  // ==========================================================================
} //                                                     end of namespace Gaudi 
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // ANALYSIS_TEE_H
// ============================================================================
