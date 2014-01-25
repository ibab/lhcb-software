// $Id$
// ============================================================================
// $URL$
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
// HltBase 
// ============================================================================
#include "Event/HltCandidate.h"
#include "Event/HltStage.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
// ============================================================================
/** @file LoKi/HltBase.h
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
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
    /// the actual type of TES-location 
    typedef Hlt::IData::Key Location ;
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
    /** get TES-selection 
     *  @param location TES location 
     *  @return data 
     */
    template <class TYPE>
    const TYPE* tesData 
    ( const Hlt::IData::Key& location ) const ;
    // ========================================================================
  protected:
    // ========================================================================
    /// get or create the TES-container of Hlt-Candidates 
    Hlt::Candidate::Container*         hltCandidates 
    ( const std::string& location = Hlt::CandidateLocation::Default ) const ;
    /// get or create the TES-container of Hlt-Stages 
    Hlt::Stage::Container*             hltStages  
    ( const std::string& location = Hlt::StageLocation::Default     ) const ;
    /// get or create the TES-container of Hlt-dimuons  
    Hlt::L0DiMuonCandidate::Container* hltDiMuons
    ( const std::string& location = Hlt::L0DiMuonCandidateLocation::Default ) const ;
    /// get or create the TES-container of Hlt-multitracks  
    Hlt::MultiTrack::Container*        hltMultiTracks
    ( const std::string& location = Hlt::MultiTrackLocation::Default        ) const ;
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
} //                                                       end of namespace Hlt
// ============================================================================
/*  get TES-selection 
 *  @param location TES location 
 *  @return data 
 */
// ============================================================================
template <class TYPE>
const TYPE* Hlt::Base::tesData ( const Hlt::IData::Key& location ) const 
{
  const DataObject* data_ = this->hltSvc()->tes ( this , location ) ;
  Assert ( 0 != data_ , "tesData: Unable to retrieve DataObject " ) ;
  const TYPE* data        = dynamic_cast<const TYPE*> ( data_     ) ;
  Assert ( 0 != data  , "tesData: Invalid data "                  ) ;
  return data ;
}
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_HLTBASE_H
// ============================================================================
