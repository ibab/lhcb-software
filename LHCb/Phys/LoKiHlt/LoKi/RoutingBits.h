// $Id: RoutingBits.h,v 1.1 2010-05-17 16:01:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ROUTINGBITS_H 
#define LOKI_ROUTINGBITS_H 1
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IDataProviderSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb { class RawEvent ; }
// ============================================================================
/** @file
 *  set of helper function to deal with "hlt-routing-bits"
 *  @see Hlt::firedRoutingBits 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-17
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace HLT 
  {
    // ========================================================================
    /** @class RoutingBits
     *  simple class to check if the given routing bit is fired 
     *  @see LoKi::Cuts::ROUTINGBITS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-17
     */
    class RoutingBits : public LoKi::Functor<void,bool> 
    {
    public:
      // ======================================================================
      typedef std::vector<unsigned int>                                  Bits ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the bit 
      RoutingBits ( const unsigned short bit  ) ;
      /// constructor from the bits
      RoutingBits ( const unsigned short bit1 ,
                    const unsigned short bit2 ) ;
      /// constructor from the bits 
      RoutingBits ( const unsigned short bit1 ,
                    const unsigned short bit2 , 
                    const unsigned short bit3 ) ;
      /// constructor from the bits 
      RoutingBits ( const unsigned short bit1 ,
                    const unsigned short bit2 , 
                    const unsigned short bit3 ,
                    const unsigned short bit4 ) ;
      /// constructor from the bits 
      RoutingBits ( const std::vector<unsigned int>& bits ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RoutingBits () ;
      /// MANDATORY: clones method ("virtual contructor")
      virtual  RoutingBits* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( /* argument */ ) const ;
      /// OPTIONAL : the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private: 
      // ======================================================================
      /// the default constructor is disabled 
      RoutingBits () ;                    // the default consructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      const Bits& bits() const { return m_bits ; }      
      // ======================================================================
    private:
      // ======================================================================
      /// get the fired bits 
      std::size_t getFired () const ;                     // get the fired bits 
      // ======================================================================
    private:
      // ======================================================================
      /// the list of fired bits 
      mutable Bits m_fired ;                          // the list of fired bits 
      /// the bits to be checked 
      Bits         m_bits  ;                          //  the bit to be checked 
      // ======================================================================
    } ;
  } //                                               end of namespace LoKi::HLT 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef ROUTINBITS 
     *  simple (void)-predicate to check the fired routing bits 
     *  @see LoKi::HLT::RoutingBits 
     *  @see LoKi::HLT::RoutingBit
     *  @see LoKi::Cuts::ROUTINGBIT 
     *  @see Hlt::firedRoutingBits 
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-17
     */
    typedef LoKi::HLT::RoutingBits                                ROUTINGBITS ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ROUTINGBITS_H
