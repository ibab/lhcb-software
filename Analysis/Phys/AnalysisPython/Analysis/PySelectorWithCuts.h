// $Id$ 
// ============================================================================
#ifndef ANALYSIS_PYSELECTORWITHCUTS_H 
#define ANALYSIS_PYSELECTORWITHCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// Local
// ============================================================================
#include "Analysis/PySelector.h"
// ============================================================================
namespace Analysis
{
  // ==========================================================================
  class Formula ;
  // ==========================================================================
  /** @class PySelectorWithCuts Analysis/PySelectorWithCuts.h
   *  
   *
   *  @author Vanya Belyaev
   *  @date   2013-05-06
   */
  class GAUDI_API SelectorWithCuts : public Selector 
  {
    // ========================================================================
  public:
    // ========================================================================
    ClassDef ( Analysis::SelectorWithCuts , 1 ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor 
    SelectorWithCuts
    ( TTree*             tree = 0  , 
      const std::string& cuts = "" , 
      PyObject*          self = 0  ) ;
    /// virtual destructor 
    virtual ~SelectorWithCuts () ; // virtual destructor 
    // ========================================================================
  public:
    // ========================================================================
    virtual Bool_t Notify       () ;
    virtual void   Init         ( TTree*   tree  ) ;
    virtual void   Begin        ( TTree*   tree  ) ;
    virtual void   SlaveBegin   ( TTree*   tree  ) ;
    virtual Bool_t Process      ( Long64_t entry ) ;
    // ========================================================================
  public:
    // ========================================================================    
    /// is formula OK ? 
    bool ok () const  ; // is formula OK ? 
    /// get the formula 
    Analysis::Formula* formula () const { return m_formula ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the selection formula 
    std::string        m_cuts    ; 
    Analysis::Formula* m_formula ;
  };
  // ==========================================================================
} //                                                  end of namespace Analysis 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_PYSELECTORWITHCUTS_H
// ============================================================================
