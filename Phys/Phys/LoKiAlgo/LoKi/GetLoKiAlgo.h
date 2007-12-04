// $Id: GetLoKiAlgo.h,v 1.1 2007-12-04 17:24:49 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GETLOKIALGO_H 
#define LOKI_GETLOKIALGO_H 1
// ============================================================================
// Include files
// ============================================================================
// forward declarations 
// ============================================================================
class IAlgContextSvc ;
namespace LoKi 
{ 
  class Algo     ; 
  class ILoKiSvc ;
} // end of namespace LoKi 
// ============================================================================
namespace Gaudi
{
  namespace Utils 
  {
    // ========================================================================
    /** get the last LoKi algorithm form the context service
     *  @see LoKi::Algo
     *  @see IAlgContextSvc
     *  @param svc pointer to algorithm context service
     *  @return the last LoKi-algortihm
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-04 
     */ 
    LoKi::Algo* getLoKiAlgo ( const IAlgContextSvc* svc     ) ;
    // ========================================================================
    /** get the last LoKi algorithm from the context service
     *  @see LoKi::Algo
     *  @see LoKi::ILoKiSvc
     *  @param svc pointer to LoKi service 
     *  @return the last LoKi-algortihm
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-04 
     */ 
    LoKi::Algo* getLoKiAlgo ( const LoKi::ILoKiSvc* svc = 0 ) ;
    // ======================================================================== 
  } // end of namespace Gaudi::Utils 
} // end of namespace Gaudi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** get the last LoKi algorithm form the context service
   *  @see LoKi::Algo
   *  @see IAlgContextSvc
   *  @param svc pointer to algorithm context service
   *  @return the last LoKi-algortihm
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-04 
   */ 
  inline LoKi::Algo* getAlgo ( const IAlgContextSvc* svc     ) 
  { return Gaudi::Utils::getLoKiAlgo ( svc ) ; }
  // ==========================================================================
  /** get the last LoKi algorithm from the context service
   *  @see LoKi::Algo
   *  @see LoKi::ILoKiSvc
   *  @param svc pointer to LoKi service 
   *  @return the last LoKi-algortihm
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-04 
   */ 
  inline LoKi::Algo* getAlgo ( const LoKi::ILoKiSvc* svc = 0 ) 
  { return Gaudi::Utils::getLoKiAlgo ( svc ) ; }
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETLOKIALGO_H
// ============================================================================
