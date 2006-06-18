// $Id: CaloID2DLL.cpp,v 1.1 2006-06-18 18:35:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , verison $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
  , m_title  () 
  , m_pScale ( -1 ) 
  , m_vScale ( -1 ) 
  , m_histo ( 0 )
{
  declareProperty ( "Input"     , m_input   ) ;
  declareProperty ( "Output"    , m_output  ) ;
  declareProperty ( "Histogram" , m_title   ) ;
  declareProperty ( "pScale"    , m_pScale  ) ;
  declareProperty ( "vScale"    , m_vScale  ) ;  
} ;
// ============================================================================
/// Algorithm initialization
// ============================================================================
StatusCode CaloID2DLL::initialize() 
{
  StatusCode sc = CaloTrackAlg::initialize(); 
  if ( sc.isFailure() ) { return sc ; } 
  // locate the histogram 
  m_histo = get<AIDA::IHistogram2D>( histoSvc() , m_title );
  // 
  Warning("It is VERY preliminary version!!") ;
  //
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



// ============================================================================
/// The END 
// ============================================================================

