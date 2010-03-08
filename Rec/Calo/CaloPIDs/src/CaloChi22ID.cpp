// $Id: CaloChi22ID.cpp,v 1.6 2010-03-08 01:31:33 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2009/09/10 10:47:05  odescham
// add protection + reduce verbosity
//
// Revision 1.4  2009/08/21 16:49:45  odescham
// implement generic context-dependent TES I/O
//
// Revision 1.3  2008/06/30 15:37:34  odescham
// prepare for HLT processing
//
// Revision 1.2  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
// 
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
  , m_tracks ()
  //
  , m_input  () 
  , m_output () 
  , m_large  ( 10000 )
{
  declareProperty ( "Tracks" , m_tracks ) ;
  declareProperty ( "Input"  , m_input  ) ;
  declareProperty ( "Output" , m_output ) ;
  declareProperty ( "CutOff" , m_large  ) ;
  // context-dependent default track container 
  m_tracks.clear();
  m_tracks =  LHCb::CaloAlgUtils::TrackLocations( context() ) ;
} ;
// ============================================================================
/// algorithm execution 
// ============================================================================
StatusCode CaloChi22ID::execute() 
{
  typedef LHCb::Calo2Track::ITrHypoTable      TABLEI ;
  typedef LHCb::Relation1D<LHCb::Track,float> Table  ;
  // get the input table 
  // get the input data 
  if( !exist<TABLEI>(m_input))return Warning(" Input missing '" + m_input + "'",StatusCode::SUCCESS);
  const TABLEI* input = get<TABLEI>( m_input ) ;
  // create and register the output table 
  Table* output = new Table ( input->relations().size() + 10 ) ;
  put ( output , m_output ) ;
  // perform the actual jobs 
  StatusCode sc = doTheJob( input , output ) ;
  counter (Gaudi::Utils::toString( m_tracks) +  "->" 
            + m_input + "=>" + m_output) += output->i_relations().size() ;
  return sc;
} ;
// =============================================================================
