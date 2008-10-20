// $Id: CaloID2DLL.cpp,v 1.7 2008-10-20 09:22:36 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , verison $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2008/06/30 15:37:34  odescham
// prepare for HLT processing
//
// Revision 1.5  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
// Revision 1.4  2006/11/27 15:48:09  vegorych
// warning VERY PRELIMINARY was removed
//
// Revision 1.3  2006/11/06 11:05:24  vegorych
//  New PID tuning for DC06
//
// Revision 1.2  2006/06/22 15:40:39  ibelyaev
//  fix incinsistencied for 'Brem'-algorithms
//
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
// 
// ============================================================================
// Include files 
// ============================================================================
#include "GaudiKernel/IHistogramSvc.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/Relation1D.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloID2DLL.h"
// ============================================================================
/** @file 
 *  Implementation file for class CaloID2DLL
 *  @date 2006-06-18 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( CaloID2DLL );
// ============================================================================
/// standard proected constructor 
// ============================================================================
CaloID2DLL::CaloID2DLL
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : CaloTrackAlg ( name , pSvcLocator )
  //
  , m_input  () 
  , m_output () 
  //
  , m_title_lt  () 
  , m_title_dt  () 
  , m_title_tt  () 
  , m_title_ut  () 
  , m_title_vt  () 
  , m_pScale_lt ( -1 ) 
  , m_pScale_dt ( -1 ) 
  , m_pScale_tt ( -1 ) 
  , m_pScale_ut ( -1 ) 
  , m_pScale_vt ( -1 ) 
  , m_vScale_lt ( -1 ) 
  , m_vScale_dt ( -1 ) 
  , m_vScale_tt ( -1 )
  , m_vScale_ut ( -1 ) 
  , m_vScale_vt ( -1 ) 
  , m_histo_lt ( 0 )
  , m_histo_dt ( 0 )
  , m_histo_tt ( 0 )
  , m_histo_ut ( 0 )
  , m_histo_vt ( 0 )
{
  declareProperty ( "Input"     , m_input   ) ;
  declareProperty ( "Output"    , m_output  ) ;
  declareProperty ( "HistogramL" , m_title_lt   ) ;
  declareProperty ( "HistogramD" , m_title_dt   ) ;
  declareProperty ( "HistogramT" , m_title_tt   ) ;
  declareProperty ( "HistogramU" , m_title_ut   ) ;
  declareProperty ( "HistogramV" , m_title_vt   ) ;
  declareProperty ( "nMlong"    , m_pScale_lt) ;
  declareProperty ( "nMdown"    , m_pScale_dt) ;
  declareProperty ( "nMTtrack"  , m_pScale_tt) ;
  declareProperty ( "nMupstr"   , m_pScale_ut) ;
  declareProperty ( "nMvelo"    , m_pScale_vt) ;
  declareProperty ( "nVlong"    , m_vScale_lt ) ;  
  declareProperty ( "nVdown"    , m_vScale_dt ) ;  
  declareProperty ( "nVTtrack"  , m_vScale_tt ) ;  
  declareProperty ( "nVupstr"   , m_vScale_ut ) ;  
  declareProperty ( "nVvelo"    , m_vScale_vt ) ;  
  // track types 
  _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                 ( LHCb::Track::Long       ,
                   LHCb::Track::Ttrack     ,
                   LHCb::Track::Downstream ) ) ;
} ;

// ============================================================================
/// Algorithm initialization
// ============================================================================
StatusCode CaloID2DLL::initialize() 
{
  StatusCode sc = CaloTrackAlg::initialize(); 
  if ( sc.isFailure() ) { return sc ; } 
  // locate the histogram 
  //

  if ( !m_title_lt.empty() ) 
  { m_histo_lt = get<AIDA::IHistogram2D>( histoSvc() , m_title_lt ); }
  
  if ( !m_title_dt.empty() ) {
    m_histo_dt = get<AIDA::IHistogram2D>( histoSvc() , m_title_dt );
  }

  if ( !m_title_tt.empty() ) {
    m_histo_tt = get<AIDA::IHistogram2D>( histoSvc() , m_title_tt );
  }

  if ( !m_title_ut.empty() ) {
    m_histo_ut = get<AIDA::IHistogram2D>( histoSvc() , m_title_ut );
  }

  if ( !m_title_vt.empty() ) {
    m_histo_vt = get<AIDA::IHistogram2D>( histoSvc() , m_title_vt );
  }

  return StatusCode::SUCCESS;
} ;
// ============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
// ============================================================================
/// Algorithm execution
// ============================================================================
StatusCode CaloID2DLL::execute() 
{
  typedef LHCb::Relation1D<LHCb::Track,float>  Table  ;
  typedef LHCb::Calo2Track::ITrEvalTable       ITable ;
  
  BOOST_STATIC_ASSERT(INHERITS(Table,ITable));
  
  // get the input data 
  const ITable* input = get<ITable>( m_input ) ;
  ITable::Range links = input->relations() ;
  
  // create and register the output ocntainer 
  Table* table = new Table( links.size() ) ;
  put ( table , m_output ) ;
  
  // loop over all links 
  for ( ITable::Range::iterator link = links.begin() ; 
        links.end() != link ; ++link ) 
  {
    const LHCb::Track* track = link->from () ;
    const double       value = link->to   () ;
    if ( !use ( track ) ) { continue ; }                         // CONTINUE ;
    // convert value to DLL 

    const double DLL = dLL ( track->p() , value , track->type() ) ;

    // fill the relation table 
    table->i_push ( track , DLL ) ;                        // NB!: i_push
  } ;
  /// MANDATORY: i_sort after i_push 
  table->i_sort() ;

  return StatusCode::SUCCESS;
} ;
// ============================================================================



