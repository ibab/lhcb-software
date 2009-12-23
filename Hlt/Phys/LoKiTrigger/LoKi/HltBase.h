// $Id: HltBase.h,v 1.3 2009-12-23 10:37:34 graven Exp $
// ============================================================================
#ifndef LOKI_HLTBASE_H 
#define LOKI_HLTBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiAlg/GaudiHistoAlg.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IANNSvc.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltSelection.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
// ============================================================================
namespace Hlt 
{
  // ==========================================================================
  /** @class Base LoKi/HltBase.h
   *  Helper base class for implementation of Hlt-algorithms 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2009-03-19
   */
  class Base : public GaudiHistoAlg
  {
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the algorithm 
    virtual StatusCode initialize () ;
    // ========================================================================
    /// finalize   the algorithm 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard contructor
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    Base ( const std::string& name ,            //      algorithm instance name 
           ISvcLocator*       pSvc ) ;          //   pointer to Service Locator 
    /// virtual and protected destructor 
    virtual ~Base() ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Base () ;                            // the default constructor is disabled 
    /// the copy constructor is disabled 
    Base ( const Base& ) ;                  // the copy constructor is disabled 
    /// the assignement operator is disabled 
    Base& operator=( const Base& ) ;    // the assignemen toperator is disabled
    // ========================================================================
  protected:
    // ========================================================================
    /// accessor to Hlt Registration Service 
    Hlt::IRegister* regSvc () const ;
    /// accessor to Hlt Data Service 
    Hlt::IData*     hltSvc () const ;
    /// accessor to Assigned Names & Numbers Service 
    IANNSvc*        annSvc () const ;
    // ========================================================================
  protected:
    // ========================================================================
    // name   -> number for "InfoID" using IANNSvc 
    int         hltInfoID   ( const std::string& name ) const  ;   
    // number -> name   for "InfoID" using IANNSvc 
    std::string hltInfoName ( const int          id   ) const  ;   
    // ========================================================================
  private:
    // ========================================================================
    /// Hlt registration service 
    mutable Hlt::IRegister*  m_regSvc ;            //  Hlt Registration Service 
    /// Hlt Data Service 
    mutable Hlt::IData*      m_hltSvc ;            //          Hlt Data Service 
    /// "Assigned Numbers & Names" service 
    mutable IANNSvc*         m_annSvc ;            //  Assigned Numbers & Names
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace Hlt
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTBASE_H
// ============================================================================
