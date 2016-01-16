// $Id$
// ============================================================================
#ifndef LOKI_FIRSTN_H
#define LOKI_FIRSTN_H 1
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
 *  @date 2011-10-30
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class FirstN
   *  helper class to implement the selection of first N-elements in Hybdrids
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-10-20
   */
  class GAUDI_API FirstN
  {
    // ========================================================================
  public:
    // ========================================================================
    /// constructor
    FirstN ( const unsigned int N ) ;                            // constructor
    /// virtual destructor
    virtual ~FirstN() ;                                   // virtual destructor
  public:
    // ========================================================================
    /// get N
    unsigned int N() const { return m_N ; }                            // get N
    /// get N
    unsigned int n() const { return m_N ; }                            // get N
    /// printout
    virtual std::ostream& fillStream ( std::ostream& s ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    FirstN () ;                          // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// number of elements to be selected
    unsigned int m_N ;                     // number of elements to be selected
    // ========================================================================
  }; //                                               end of class LoKi::FirstN
  // ==========================================================================
  /** @class FirstN
   *  helper class to implement the selection of first N-elements in Hybrids
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-10-20
   */
  class GAUDI_API Reverse
  {
  public:
    // ========================================================================
    /// constructor
    Reverse() ;                                                  // constructor
    // ========================================================================
    /// virtual destructor
    virtual ~Reverse () ;                                 // virtual destructor
    // ========================================================================
    /// printout
    virtual std::ostream& fillStream ( std::ostream& s ) const ;
    // ========================================================================
  } ;
  // ==========================================================================
  /** @class Sort
   *  helper class to implement the sort and
   *  selection of first N-elements in Hybdrids
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-10-20
   */
  class GAUDI_API Sort
  {
  public:
    // ========================================================================
    /// constructor
    Sort ( const int  N         = -1   , // ALL
           const bool ascending = true ) ;
    // ========================================================================
    /// virtual destructor
    virtual ~Sort () ;                                 // virtual destructor
    // ========================================================================
    /// printout
    virtual std::ostream& fillStream ( std::ostream& s ) const ;
    // ========================================================================
  public:
    // ========================================================================
    bool ascending () const { return m_ascending ; }
    bool all       () const { return m_N < 0    ; }
    int  N         () const { return m_N        ; }
    int  n         () const { return m_N        ; }
    // ========== =============================================================
  private:
    // ========================================================================
    /// number of elements
    int  m_N         ;                                    // number of elements
    /// ascending order ?
    bool m_ascending ;                                     // ascending order ?
    // ========================================================================
  };
  // ==========================================================================
  /// output operator
  // ==========================================================================
  std::ostream& operator<< ( std::ostream& s , const FirstN&  o ) ;
  // ==========================================================================
  /// output operator
  // ==========================================================================
  std::ostream& operator<< ( std::ostream& s , const Reverse& o ) ;
  // ==========================================================================
  /// output operator
  // ==========================================================================
  std::ostream& operator<< ( std::ostream& s , const Sort&    o ) ;
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    // Dump as C++ objects
    GAUDI_API std::string toCpp ( const LoKi::FirstN&  f ) ;
    GAUDI_API std::string toCpp ( const LoKi::Sort&    s ) ;
    GAUDI_API std::string toCpp ( const LoKi::Reverse& r ) ;
    // ========================================================================
  } 
  // ==========================================================================
}
// ============================================================================
#endif // LOKI_FIRSTN_H
// ============================================================================

