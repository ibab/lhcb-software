// $Id: NeutralPP2MC.cpp,v 1.1 2002-09-12 12:16:12 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/09/07 11:01:49  ibelyaev
//  fix the bugs, kindly found by Gloria and Galina
//
// ============================================================================
// Include files
// LHCbKernel 
#include "Relations/IAssociatorWeighted.h"
#include "Relations/RelationWeighted2D.h"
#include "Relations/RelationWeighted1D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/StreamBuffer.h"
// Event 
#include "Event/MCParticle.h"
#include "Event/KeyedObject.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CasloEvent/Event
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloMCTools.h"
// PhysEvent/Event 
#include "Event/ProtoParticle.h"
// local
//#include "CaloMCTools.h"
#include "NeutralPP2MC.h"

// ============================================================================
/** @file
 * 
 *  Implementation file for class : NeutralPP2MC
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 08/04/2002 
 */
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<NeutralPP2MC>         s_factory ;
const        IAlgFactory&NeutralPP2MCFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svc  service locator
 */
// ============================================================================
NeutralPP2MC::NeutralPP2MC
( const std::string& name ,
  ISvcLocator*       svc  )
  : CaloAlgorithm ( name , svc                  )
  , m_asctType ( "AssociatorWeighted<CaloCluster,MCParticle,float>" )
  , m_asctName ( "CCs2MCPs" )
  , m_asct     ( 0 ) 
{ 
  declareProperty( "MCAssociatorType" , m_asctType  ) ;
  declareProperty( "MCAssociatorName" , m_asctName  ) ;
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
NeutralPP2MC::~NeutralPP2MC() {}; 
// ============================================================================

// ============================================================================
/** standard initialization of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode NeutralPP2MC::initialize()
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  /// initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  /// locate the associator 
  m_asct = tool( m_asctType , m_asctName , m_asct );
  if( 0 == m_asct ) { return StatusCode::FAILURE ; }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard finalization of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode NeutralPP2MC::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  /// finalize thebase class
  return CaloAlgorithm::finalize () ;
};

// ============================================================================
/** standard execution of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode NeutralPP2MC::execute() 
{
  /// avoid the long name and always use "const" qualifier  
  
  typedef const ProtoParticles                               PPs      ;
  typedef const SmartRefVector<CaloHypo>                     Hypos    ;
  typedef const SmartRefVector<CaloCluster>                  Clusters ;
  
  typedef RelationWeighted1D<ProtoParticle,MCParticle,float> Table    ;
  
  typedef const MCAsct::DirectType                           MCTable  ;
  typedef const MCTable::Range                               Range    ;
  typedef       MCTable::iterator                            relation ;
  
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // get input protoparticles 
  const PPs*   pps  = get ( eventSvc () , inputData () , pps ) ;
  if( 0 == pps ) { return StatusCode::FAILURE ; }
  
  // create relation table and register it in the event transient store 
  Table* table = new Table();
  StatusCode sc = put( table , outputData () );
  if( sc.isFailure() ) { return sc ; }
  
  // get MC association table 
  const MCTable* mctable = m_asct->direct();  
  if( 0 == mctable ) { return Warning("MCrelation table points to NULL!" , 
                                      StatusCode::SUCCESS                ) ; }
  
  // loop over all protoparticles 
  for( PPs::const_iterator pp = pps->begin() ; pps->end() != pp ; ++pp )
    {
      // skip nulls 
      if( 0 == *pp ) { continue ; }
      // get all calo hypos 
      const Hypos& hypos = (*pp)->calo() ;
      // loop over all hypotheses 
      for( Hypos::const_iterator hypo = hypos.begin() ; 
           hypos.end() != hypo ; ++hypo)
        {
          // skip nulls
          if( *hypo == 0 ) { continue ; }                         // CONTINUE 
          // get all clusters
          const Clusters& clusters = (*hypo)->clusters();
          // loop over all clusters
          for( Clusters::const_iterator cluster = clusters.begin() ; 
               clusters.end() != cluster ; ++cluster ) 
            {
              // skip nulls 
              if( *cluster == 0 ) { continue ; }                  // CONTINUE 
              const Range range = mctable->relations( *cluster );
              for( relation rel = range.begin() ; range.end() != rel ; ++rel ) 
                {
                  const MCParticle* particle = rel->to();
                  // skip nulls 
                  if( 0 == particle ) { continue ; }              // CONTINUE 
                  // propagate the relation to protoparticle 
                  table->relate( *pp                             , 
                                 particle                        , 
                                 rel->weight() / (*cluster)->e() );  
                }
            }
        }
    }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
