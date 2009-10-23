// $Id: HistoCondition.h,v 1.1 2009-10-23 09:43:13 ibelyaev Exp $
// ============================================================================
#ifndef DETDESC_HISTOCONDITION_H 
#define DETDESC_HISTOCONDITION_H 1
// ============================================================================
// Include files
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/HistoParam.h"
#include "DetDesc/Condition.h"
// ============================================================================
namespace DetDesc 
{ 
  // ===========================================================================
  /** @class Histo1DCond DetDesc/HistoCondition.h
   *  Simple condition which "keeps" 1D histogram 
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date   2009-10-22
   */
  class Histo1DCond : public Condition 
  {
  public:
    // ========================================================================
    /// the constructor 
    Histo1DCond () ;                                      //    the constructor 
    /// virtual destructor 
    virtual ~Histo1DCond () ;                             // virtual destructor 
    // ========================================================================
  public:
    // ========================================================================
    /// perform the specific initialization (==creation of the histogram)
    virtual StatusCode initialize () ;
    /// update from other data object: 
    virtual void update ( ValidDataObject&  right ) ;
    /// update from other data object: 
    virtual void update ( Histo1DCond&      right ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// The unique Class ID 
    virtual const CLID& clID    () const { return classID(); } 
    /// The unique Class ID 
    static  const CLID& classID () ;                     // The unique Class ID 
    // ======================================================================== 
  public:
    // ========================================================================
    /// acces to the histogram ( only const version)
    inline const TH1D&  histo   () const { return m_histo ; }
    // ========================================================================
  private: 
    // ========================================================================
    /// the histogram itself 
    TH1D m_histo ;                                      // the histogram itself 
    // ========================================================================
  };
  // ===========================================================================
  /** @class Histo2DCond DetDesc/HistoCondition.h
   *  Simple condition which "keeps" 2D histogram 
   *  @author Vanya BELYAEV Ivan.Bwlyaev@nikhef.nl
   *  @date   2009-10-22
   */
  class Histo2DCond : public Condition 
  {
  public:
    // ========================================================================
    /// the constructor 
    Histo2DCond () ;                                      //    the constructor 
    /// virtual destructor 
    virtual ~Histo2DCond () ;                             // virtual destructor 
    // ========================================================================
  public:
    // ========================================================================
    /// perform the specific initialization (==creation of the histogram)
    virtual StatusCode initialize () ;
    /// update from other data object: 
    virtual void update ( ValidDataObject&  right ) ;
    /// update from other data object: 
    virtual void update ( Histo2DCond&      right ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// The unique Class ID 
    virtual const CLID& clID    () const { return classID(); } 
    /// The unique Class ID 
    static  const CLID& classID () ;                     // The unique Class ID 
    // ======================================================================== 
  public:
    // ========================================================================
    /// acces to the histogram ( only const version)
    inline const TH2D&  histo   () const { return m_histo ; }
    // ========================================================================
  private: 
    // ========================================================================
    /// the histogram itself 
    TH2D m_histo ;                                      // the histogram itself 
    // ========================================================================
  };
  // ==========================================================================
} //                                                   end of namespace DetDesc 
// ============================================================================
// The END 
// ============================================================================
#endif // DETDESC_HISTOCONDITION_H
// ============================================================================
