// $Id: HltGEC.h,v 1.3 2010-08-19 09:51:40 graven Exp $
// ============================================================================
#ifndef HLTCOMMON_HLTGEC_H 
#define HLTCOMMON_HLTGEC_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ICheckTool.h"
#include "Kernel/IAccept.h"
// ============================================================================
// OTDAQ
// ============================================================================
#include "OTDAQ/IOTRawBankDecoder.h"
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  /** @class GEC HltGEC.h
   *  Simple tool to check the number of hits in OT  
   *
   *  @author Jose Angel Hernando Morata
   *  @author Vanya Belyaev@nikhef.nl
   *  @date   2010-08-03
   */
  class GEC : public extends2<GaudiTool,IAccept,ICheckTool>
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class ToolFactory<Hlt::GEC> ;
    // ========================================================================
  public: 
    // ========================================================================
    /** check 
     *  @see ICheckTool
     *  @see ICheckTool::check
     */
    virtual StatusCode check() ;
    // ========================================================================
    /** accept 
     *  @see IAccept
     *  @see IAccept::accept
     */
    virtual bool accept () const ;  
    // ========================================================================
  public: 
    // ========================================================================
    /// standard initialization 
    virtual StatusCode initialize () ;
    /// standard finalization 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** Standard constructor
     *  @param type the tol type (???)
     *  @param name the tool instance name 
     *  @param parent the tool parent 
     */
    GEC ( const std::string& type   ,   // the tool type  
          const std::string& name   ,   // the tool instance name 
          const IInterface*  parent ) ; // the tool parent 
    /// virtual & protected destructor 
    virtual ~GEC() ;                    // virtual & protected destructor 
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    GEC () ;                            //   the default contructor is disabled    
    /// the copy contructor is disabled 
    GEC ( const GEC& ) ;                //      the copy contructor is disabled 
    /// the assignement operator is disabled 
    GEC& operator=( const GEC& ) ;      // the assignement operator is disabled    
    // ========================================================================
  private:
    // ========================================================================
    /// the maximum number of OT-hits 
    int m_maxOTHits;                  // the maximum number of OT-hits 
    /// the maximum number of IT-hits 
    int m_maxITHits;                  // the maximum number of IT-hits
    /// the maximum number of Velo-hits 
    int m_maxVeloHits; 
    /// the minimum number of OT-hits 
    int m_minOTHits;                  // the minimum number of OT-hits 
    /// the minimum number of IT-hits 
    int m_minITHits;                  // the minimum number of IT-hits
    /// the minimum number of Velo-hits 
    int m_minVeloHits;                  // the minimum number of Velo-hits    
    /// the decodetr tool 
    IOTRawBankDecoder* m_rawBankDecoder; // the decodetr tool 
    //is it a upper GEC or an activity trigger
    bool m_isActivity;
    // ========================================================================    
  };
  // ==========================================================================
} // end of namespace Hlt
// ============================================================================
// The END 
// ============================================================================
#endif // HLTCOMMON_HLTGEC_H
// ============================================================================
