// $Id: ReadFile.h,v 1.1 2007-07-06 08:10:42 ibelyaev Exp $
// ============================================================================
#ifndef LBPYTHIA_READFILE_H
#define LBPYTHIA_READFILE_H 1 
// ============================================================================
// Include files 
// ============================================================================
// LbPythia
// ============================================================================
#include "LbPythia/PythiaProduction.h"
// ============================================================================
namespace LbPythia
{
  // ==========================================================================
  /** @class ReadFile ReadFile.h
   *  The base class for various production tools which reads parton level events 
   *  from external files and perform showering using Pythia 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-10-03
   */
  class ReadFile : public PythiaProduction 
  {
  public:
    // ========================================================================
    /// tool initialization 
    virtual StatusCode initialize () ;
    /// tool finalization 
    virtual StatusCode finalize   () ;
    // ========================================================================
  public:
    // ========================================================================
    /// Read the event from the file @see IProductionTool::generateEvent
    virtual StatusCode generateEvent 
    ( HepMC::GenEvent*    event     , 
      LHCb::GenCollision* collision ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param type tool type (?)
     *  @param name tool name 
     *  @param parent tool parent 
     */
    ReadFile
    ( const std::string& type,
      const std::string& name,
      const IInterface* parent ) ;
    /// virtual and protected destructor 
    virtual ~ReadFile () ;                 // virtual and protected destructor 
    // ========================================================================
  public:
    // ========================================================================
    /// the name of the input file  
    const std::string& file() const  { return m_file ; }    
    // ========================================================================
  private:
    // ========================================================================
    // the default constructor is disabled 
    ReadFile () ;
    // the copy constructor is disabled 
    ReadFile ( const ReadFile & ) ;
    // assignement operator is disabled 
    ReadFile& operator=( const ReadFile& ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the name of input file 
    std::string m_file ;                              // the name of input file 
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                  end of namespace LbPythia 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LBPYTHIA_READFILE_H 1 
// ============================================================================
