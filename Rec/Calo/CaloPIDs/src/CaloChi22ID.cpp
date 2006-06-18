// $Id: CaloChi22ID.cpp,v 1.1 2006-06-18 18:35:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloChi22ID.h"
// ============================================================================
/** @file 
 *  Implementation file for class CaloChi22ID
 *  @date 2006-06-18 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
//// declare algorithm factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY ( CaloChi22ID ) ;
// ============================================================================
/// Standard protected constructor
// ============================================================================
CaloChi22ID::CaloChi22ID( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : CaloTrackAlg ( name , pSvcLocator )
  //
  , m_tracks ( 1 , LHCb::TrackLocation::Default )
  //
  , m_input  () 
  , m_output () 
  , m_large  ( 10000 )
{
  declareProperty ( "Tracks" , m_tracks ) ;
  declareProperty ( "Input"  , m_input  ) ;
  declareProperty ( "Output" , m_output ) ;
  declareProperty ( "CutOff" , m_large  ) ;
} ;
// ============================================================================
/// algorithm execution 
// ============================================================================
StatusCode CaloChi22ID::execute() 
{
  typedef LHCb::Calo2Track::ITrHypoTable      TABLEI ;
  typedef LHCb::Relation1D<LHCb::Track,float> Table  ;
  // get the input table 
  const TABLEI* input = get<TABLEI>( m_input ) ;
  // create and register the output table 
  Table* output = new Table ( input->relations().size() + 10 ) ;
  put ( output , m_output ) ;
  // perform the actual jobs 
  return doTheJob( input , output ) ;
} ;
// =============================================================================

// ============================================================================
// The END 
// ============================================================================
