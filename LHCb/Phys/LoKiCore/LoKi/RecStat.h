// ============================================================================
#ifndef LOKI_RECSTAT_H 
#define LOKI_RECSTAT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TES.h"
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
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace TES
  {
    // ========================================================================
    /** @class HasRecSummary
     *  Check the existence of certain record in RecSummary object
     *  @see LHCb::RecSummary
     *  @see LHCb::RecSummary::hasInfo
     *  The functor has been asked by Sophie Redford and Andy Powell
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @daet 2011-03-24
     */
    class GAUDI_API HasRecSummary : public LoKi::TES::Exists
    {
    public:
      // ======================================================================
      /** Standard constructor from the key, location and 'rootInTES'
       *  @param key          the key to use 
       *  @param location     TES location of LHCb::RecSummary obejct
       *  @param useRootInTES flato to use/ignore root-in-tes 
       */
      HasRecSummary 
      ( const int          key                 , 
        const std::string& location     = ""   , 
        const bool         useRootInTES = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~HasRecSummary () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasRecSummary* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( /* argument v */ ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      HasRecSummary () ;                    // default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the key to be used for LHCb::RecSummary::hasInfo method 
      int m_key ; // the key to be used for LHCb::RecSummary::hasInfo method
      // ======================================================================
    } ;
    // ========================================================================
    /** @class RecSummary
     *  Check the content of certain record in RecSummary object
     *  @see LHCb::RecSummary
     *  @see LHCb::RecSummary::info
     *  The functor has been asked by Sophie Redford and Andy Powell
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @daet 2011-03-24
     */
    class GAUDI_API RecSummary : public LoKi::TES::Contains
    {
    public:
      // ======================================================================
      /** Standard constructor from the key, location and 'rootInTES'
       *  @param key          the key to use 
       *  @param bad          bad value  
       *  @param location     TES location of LHCb::RecSummary obejct
       *  @param useRootInTES flato to use/ignore root-in-tes 
       */
      RecSummary 
      ( const int          key                  , 
        const int          bad                  ,  
        const std::string& location     = ""    , 
        const bool         useRootInTES = true  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RecSummary () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecSummary* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( /* argument v */ ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      RecSummary () ;                       // default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the key to be used for LHCb::RecSummary::info method 
      int m_key ; // the key to be used for LHCb::RecSummary::info method
      /// the value to be used for LHCb::RecSummary::info method 
      int m_bad ; // the value to be used for LHCb::RecSummary::info method
      // ======================================================================
    } ;
    // ========================================================================
  } //                                               end of namespace LoKi::TES 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef HASRECSUMMARY 
     *  Check the record in LHCb::RecSummary
     *  The functors is designed for Sophie Redford and Andy Powell
     *
     *  @code
     * 
     *     HASRECSUMMARY ( 10 ) 
     *
     *  @endcode 
     *  @see LoKi::TES::HasRecSummary
     *  @author Vanya BELYAEV  Ivan.BElyaev@cern.ch
     *  @date 2011-03-24
     */
    typedef LoKi::TES::HasRecSummary                            HASRECSUMMARY ;
    // ========================================================================
    /** @typedef RECSUMMARY 
     *  Get the record from LHCb::RecSummary
     *  The functors is designed for Sophie Redford and Andy Powell
     *
     *  @code
     * 
     *     RECSUMMARY ( 10 , -1000 ) 
     *
     *  @endcode 
     *  @see LoKi::TES::RecSummary
     *  @author Vanya BELYAEV  Ivan.BElyaev@cern.ch
     *  @date 2011-03-24
     */
    typedef LoKi::TES::RecSummary                                  RECSUMMARY ;
    // ========================================================================
  } //                                             end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_RECSTAT_H
// ============================================================================
