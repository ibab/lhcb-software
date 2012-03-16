// ============================================================================
// Include files 
// ============================================================================
#include "GaudiKernel/ITHistSvc.h"
// ============================================================================
/////// #include "GaudiKernel/IHistogramSvc.h"
#include "DetDesc/HistoParam.h"
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
//// #include "TFile.h"
// ============================================================================
#include <GaudiUtils/Aida2ROOT.h>
// ============================================================================
/** @file 
 *  Implementation file for class CaloID2DLL
 *  @date 2006-06-18 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( CaloID2DLL )
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
  //
  , m_title_lt_ths () 
  , m_title_dt_ths () 
  , m_title_tt_ths () 
  , m_title_ut_ths () 
  , m_title_vt_ths () 
  //
  , m_pScale_lt ( -1 ) 
  , m_pScale_dt ( -1 ) 
  , m_pScale_tt ( -1 ) 
  , m_pScale_ut ( -1 ) 
  , m_pScale_vt ( -1 ) 
  //
  , m_vScale_lt ( -1 ) 
  , m_vScale_dt ( -1 ) 
  , m_vScale_tt ( -1 )
  , m_vScale_ut ( -1 ) 
  , m_vScale_vt ( -1 ) 
  //
  , m_histo_lt ( 0 )
  , m_histo_dt ( 0 )
  , m_histo_tt ( 0 )
  , m_histo_ut ( 0 )
  , m_histo_vt ( 0 )
  //
  , m_useCondDB( true )
  , m_conditionName()
  , m_cond    ( NULL )
{
  declareProperty ( "Input"     , m_input   ) ;
  declareProperty ( "Output"    , m_output  ) ;

  declareProperty ( "HistogramL" , m_title_lt   ) ;
  declareProperty ( "HistogramD" , m_title_dt   ) ;
  declareProperty ( "HistogramT" , m_title_tt   ) ;
  declareProperty ( "HistogramU" , m_title_ut   ) ;
  declareProperty ( "HistogramV" , m_title_vt   ) ;

  declareProperty ( "HistogramL_THS" , m_title_lt_ths   ) ;
  declareProperty ( "HistogramD_THS" , m_title_dt_ths   ) ;
  declareProperty ( "HistogramT_THS" , m_title_tt_ths   ) ;
  declareProperty ( "HistogramU_THS" , m_title_ut_ths   ) ;
  declareProperty ( "HistogramV_THS" , m_title_vt_ths   ) ;

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

  declareProperty ( "UseCondDB",    m_useCondDB   = true,
                    "get DLLs from CondDB or from a root file via THS (the old way)" );
  declareProperty ( "ConditionName", m_conditionName = "" ) ;

  // track types 
  _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                 ( LHCb::Track::Long       ,
                   LHCb::Track::Ttrack     ,
                   LHCb::Track::Downstream ) ) ;
}

// ============================================================================
/// Algorithm initialization
// ============================================================================
StatusCode CaloID2DLL::initialize() 
{
  StatusCode sc = CaloTrackAlg::initialize(); 
  if ( sc.isFailure() ) return sc ;

  if (! existDet<DataObject>(detSvc(), m_conditionName) ){
    warning() << "Initialise: Condition '" << m_conditionName
              << "' not found -- switch to reading the DLLs from THS!" << endmsg; 
    m_useCondDB = false;
  }

  sc = m_useCondDB ? initializeWithCondDB() : initializeWithoutCondDB();

  return sc;
}
// ============================================================================
StatusCode CaloID2DLL::initializeWithCondDB()  
{
  info() << "init with CondDB, m_conditionName = " << m_conditionName << endmsg;
  try {
    registerCondition(m_conditionName, m_cond, &CaloID2DLL::i_updateDLL);
  }
  catch (GaudiException &e) {
    fatal() << e << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode sc = runUpdate();  // load the conditions

  return sc;
}
// ============================================================================
StatusCode CaloID2DLL::initializeWithoutCondDB()
{
  info() << "init w/o CondDB (read the 2D DLL histograms from a root file through THS)" << endmsg;

  // locate the histogram 
  if ( !m_title_lt_ths.empty() ) {
    AIDA::IHistogram2D *aida = get<AIDA::IHistogram2D>( histoSvc() , m_title_lt_ths );
    m_histo_lt = Gaudi::Utils::Aida2ROOT::aida2root( aida );
  }

  if ( !m_title_dt_ths.empty() ) {
    AIDA::IHistogram2D *aida = get<AIDA::IHistogram2D>( histoSvc() , m_title_dt_ths );
    m_histo_dt = Gaudi::Utils::Aida2ROOT::aida2root( aida );
  }

  if ( !m_title_tt_ths.empty() ) {
    AIDA::IHistogram2D *aida = get<AIDA::IHistogram2D>( histoSvc() , m_title_tt_ths );
    m_histo_tt = Gaudi::Utils::Aida2ROOT::aida2root( aida );
  }

  if ( !m_title_vt_ths.empty() ) {
    AIDA::IHistogram2D *aida = get<AIDA::IHistogram2D>( histoSvc() , m_title_vt_ths );
    m_histo_vt = Gaudi::Utils::Aida2ROOT::aida2root( aida );
  }

  if ( !m_title_ut_ths.empty() ) {
    AIDA::IHistogram2D *aida = get<AIDA::IHistogram2D>( histoSvc() , m_title_ut_ths );
    m_histo_ut = Gaudi::Utils::Aida2ROOT::aida2root( aida );
  }

  if( msgLevel(MSG::DEBUG) )
    debug() <<"\nLong:        '"<<m_title_lt_ths<<"' -> "<<(void *)m_histo_lt
            <<"\nDownstream:  '"<<m_title_dt_ths<<"' -> "<<(void *)m_histo_dt
            <<"\nTTuricensis: '"<<m_title_tt_ths<<"' -> "<<(void *)m_histo_tt
            <<"\nUpstream:    '"<<m_title_ut_ths<<"' -> "<<(void *)m_histo_ut
            <<"\nVelo:        '"<<m_title_vt_ths<<"' -> "<<(void *)m_histo_vt << endmsg;

  return StatusCode::SUCCESS;
}
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
  if( !exist<ITable>(m_input))return Warning(" Input missing '" + m_input + "'",StatusCode::SUCCESS);

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
    table->i_push ( track , (float) DLL ) ;                        // NB!: i_push
  } ;
  /// MANDATORY: i_sort after i_push 
  table->i_sort() ;

  counter (m_input + "=>" + m_output ) += table->i_relations().size() ;

  return StatusCode::SUCCESS;
}
// ============================================================================
StatusCode CaloID2DLL::i_updateDLL()
{
  if( msgLevel(MSG::DEBUG) ) debug() << "i_updateDLL() called" << endmsg;
  if ( !m_cond ) return StatusCode::FAILURE;

  try {
    if ( !m_title_lt.empty() ){
      m_histo_lt = reinterpret_cast<TH2D *>(
        & m_cond->param<DetDesc::Params::Histo2D>(m_title_lt) );
    }

    if ( !m_title_dt.empty() ){
      m_histo_dt = reinterpret_cast<TH2D *>(
        & m_cond->param<DetDesc::Params::Histo2D>(m_title_dt) );
    }

    if ( !m_title_tt.empty() ){
      m_histo_tt = reinterpret_cast<TH2D *>(
        & m_cond->param<DetDesc::Params::Histo2D>(m_title_tt) );
    }

    if ( !m_title_ut.empty() ){
      m_histo_ut = reinterpret_cast<TH2D *>(
        & m_cond->param<DetDesc::Params::Histo2D>(m_title_ut) );
    }

    if ( !m_title_vt.empty() ){
      m_histo_vt = reinterpret_cast<TH2D *>(
        & m_cond->param<DetDesc::Params::Histo2D>(m_title_vt) );
    }
  }
  catch ( GaudiException &exc ){
    fatal() << "DLL update failed! msg ='" << exc << "'" << endmsg;
    return StatusCode::FAILURE;
  }

  if( msgLevel(MSG::DEBUG) )
    debug() <<"\nLong:        '"<<m_title_lt<<"' -> "<<(void *)m_histo_lt
            <<"\nDownstream:  '"<<m_title_dt<<"' -> "<<(void *)m_histo_dt
            <<"\nTTuricensis: '"<<m_title_tt<<"' -> "<<(void *)m_histo_tt
            <<"\nUpstream:    '"<<m_title_ut<<"' -> "<<(void *)m_histo_ut
            <<"\nVelo:        '"<<m_title_vt<<"' -> "<<(void *)m_histo_vt << endmsg;

  return StatusCode::SUCCESS;
}
// ============================================================================
