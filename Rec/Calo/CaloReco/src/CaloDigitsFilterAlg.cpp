// $Id: CaloDigitsFilterAlg.cpp,v 1.2 2004-02-17 12:08:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.5  2002/07/19 08:45:05  ibelyaev
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/SmartRef.h" 
// Event 
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// local
#include "CaloDigitsFilterAlg.h"

// ============================================================================
/** @file CaloDigitsFilterAlg.cpp
 * 
 *  Template implementation file for class : CaloDigitsFilterAlg
 *  @see CaloDigitsFilterAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloDigitsFilterAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloDigitsFilterAlg>         s_Factory ;
const        IAlgFactory&CaloDigitsFilterAlgFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloDigitsFilterAlg::CaloDigitsFilterAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm ( name , svcloc ) 
  , m_hypos    ()
  , m_clusters ()
  , m_statuses () 
{
  //
  m_statuses .push_back ( CaloDigitStatus::SeedCell       ) ;
  m_statuses .push_back ( CaloDigitStatus::LocalMaximum   ) ;
  m_statuses .push_back ( CaloDigitStatus::CentralCell    ) ;

  declareProperty       ( "Hypotheses"  ,  m_hypos        ) ;
  declareProperty       ( "Clusters"    ,  m_clusters     ) ;
  declareProperty       ( "Statuses"    ,  m_statuses     ) ;  
};
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloDigitsFilterAlg::~CaloDigitsFilterAlg() {};


// ============================================================================
/** standard algorithm finalization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloDigitsFilterAlg::finalize() 
{  
  /// clear container 
  m_hypos.clear();
  /// finalize the base class 
  return CaloAlgorithm::finalize();
};
// ============================================================================

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloDigitsFilterAlg::execute() 
{  
  /// avoid long names and types 
  typedef  CaloHypos                     Hypos    ;
  typedef  CaloHypo::Digits              Digs     ;
  typedef  CaloDigits                    Digits   ;
  typedef  CaloClusters                  Clusters ;
  typedef  CaloCluster::Entries          Entries  ;
  typedef  const CaloDigitStatus::Status Status   ;
  
  // get digits
  Digits*  digits = get<Digits>( inputData() );
  if( 0 == digits ) { return StatusCode::FAILURE ; }
  
  // create new digits 
  Digits  filter;
  
  // auxillary container of "used" IDs 
  typedef std::vector<CaloDigit*> Tmp;
  Tmp used;
  
  // loop over containers of hypos 
  for( Addresses::const_iterator address = m_hypos.begin() ; 
       m_hypos.end() != address ; ++address )
  {
    Hypos* hypos = get<Hypos> ( *address ) ;
    if( 0 == hypos ) { return StatusCode::FAILURE ; }
    // loop over all hypos 
    for( Hypos::iterator hypo = hypos->begin() ; 
         hypos->end() != hypo ; ++hypo) 
    {
      if( 0 == *hypo ) { continue ; }
      // loop over all "extra" digits 
      Digs& digs = (*hypo)->digits();
      for( Digs::iterator digit = digs.begin() ; 
           digs.end() != digit ; ++digit )
      {
        CaloDigit* dig = *digit ;
        if( 0 == dig ) { continue ; }
        if( dig->parent() == digits ) { used.push_back( dig ); }
      } // end of loop over all "extra" digits
    } // end loop over all hypos 
  } // end of loop over all containers of hypos
  
  
  if( !m_statuses.empty() ) 
  {
    // loop over containers clusters 
    for( Addresses::const_iterator address = m_clusters.begin() ;
         m_clusters.end() != address ; ++address )
    {
      Clusters* clusters = get<Clusters>( *address );
      if( 0 == clusters ) { return StatusCode::FAILURE ; }
      // loop over  clusters
      for( Clusters::iterator cluster = clusters->begin() ;
           clusters->end() != cluster ; ++cluster ) 
      {
        CaloCluster* cl = *cluster ;
        if( 0 == cl ) { continue ; }
        Entries& entries = cl->entries() ;
        for( Entries::iterator entry = entries.begin() ;
             entries.end() != entry ; ++entry ) 
        {
          CaloDigit* digit = entry->digit();
          if( 0 == digit                ) { continue ; }
          // correct parent ?
          if( digits != digit->parent() ) { continue ; }
          bool keep = false ;
          for( Statuses::const_iterator st = m_statuses.begin() ; 
               m_statuses.end() != st ; ++st )
          {
            if( entry->status() & (Status) *st ) 
            { keep = true ; break; }
          }
          if( keep ) { used.push_back  ( digit          ) ; }
          else       { entry->setDigit ( (CaloDigit*) 0 ) ; }
        }
      }
    }
  }
  
  { // eliminate duplicates 
    std::stable_sort( used.begin() , used.end() );  
    Tmp::iterator it = std::unique( used.begin() , used.end() );
    used.erase( it , used.end() );
    while( !used.empty() ) { filter.insert( used.back() ) ; used.pop_back();} 
  }
  
  const unsigned int all  = digits -> size() ;
  
  {  // clear digits 
    digits -> clear();
    // copy filter to digits 
    for( Digits::iterator dig = filter.begin() ; filter.end() != dig ; ++dig ) 
    { digits->insert( *dig ); }
    // clear filter 
    filter.clear();
  }
  
  const unsigned int kept = digits->size() ;
  
  debug () << "'"        << inputData() << "' : "
           << " Kept  "  << kept        << " digits from " 
           << all        << endreq ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
