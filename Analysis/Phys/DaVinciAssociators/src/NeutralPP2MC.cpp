// $Id: NeutralPP2MC.cpp,v 1.2 2002-10-02 07:06:28 phicharp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/12 12:16:12  gcorti
// Neutral PP2MC algorithm
//
// Revision 1.3  2002/09/07 11:01:49  ibelyaev
//  fix the bugs, kindly found by Gloria and Galina
//
// ============================================================================
// Include files
// LHCbKernel 
#include "Relations/IAssociatorWeighted.h"
//#include "Relations/RelationWeighted2D.h"
//#include "Relations/RelationWeighted1D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/StreamBuffer.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CaloEvent/Event
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloMCTools.h"
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

#define ifLog(sev) log << sev; if( log.level() <= (sev) ) log

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
  : Algorithm ( name , svc )
  , m_asctType ( "AssociatorWeighted<CaloCluster,MCParticle,float>" )
  , m_asctName ( "CCs2MCPs" )
  , m_asct     ( 0 ) 
  , m_outputTable( NeutralPP2MCAsctLocation )
{ 
  m_inputData.push_back( ProtoParticleLocation::Neutrals );

  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable );
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
  
  /// locate the associator 
  StatusCode sc = toolSvc()->retrieveTool( m_asctType , m_asctName , m_asct );
  if( !sc.isSuccess() ) return sc;
  
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

  return StatusCode::SUCCESS ;
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
  
  typedef const SmartRefVector<CaloHypo>                     Hypos    ;
  typedef const SmartRefVector<CaloCluster>                  Clusters ;
  
  typedef const MCAsct::DirectType                           MCTable  ;
  typedef const MCTable::Range                               Range    ;
  typedef       MCTable::iterator                            relation ;
  
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // create relation table  
  ProtoParticle2MCAsct::Table* table = new ProtoParticle2MCAsct::Table();

  // get input protoparticles 
  for( std::vector<std::string>::const_iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // get MC association table for CaloHypos
    const MCTable* mctable = m_asct->direct();  
    if( 0 == mctable ) { 
      log << MSG::INFO << "Unable to retrieve the CaloHypo2MC association table"
          <<endreq;
      break;
    }
    // Get ProtoParticles
    SmartDataPtr<ProtoParticles> protos ( eventSvc(), *inp ) ;
    if( 0 != protos ) {
      log << MSG::VERBOSE << "    " << protos->size()
          << " ProtoParticles retrieved from " 
          << *inp << endreq;
    }
    else {
      log << MSG::INFO << "    *** Could not retrieve ProtoParticles from "
          << *inp << endreq;
      continue;
    }
  
    // loop over all protoparticles 
    int nrel = 0;
    for( ProtoParticles::const_iterator pp = protos->begin() ; 
         protos->end() != pp ; ++pp ) {
      // skip nulls 
      if( 0 == *pp ) { continue ; }
      // get all calo hypos 
      const Hypos& hypos = (*pp)->calo() ;
      // loop over all hypotheses 
      for( Hypos::const_iterator hypo = hypos.begin() ; 
           hypos.end() != hypo ; ++hypo) {
        // skip nulls
        if( *hypo == 0 ) { continue ; }                         // CONTINUE 
        // get all clusters
        const Clusters& clusters = (*hypo)->clusters();
        // loop over all clusters
        for( Clusters::const_iterator cluster = clusters.begin() ; 
             clusters.end() != cluster ; ++cluster ) {
          // skip nulls 
          if( *cluster == 0 ) { continue ; }                  // CONTINUE 
          const Range range = mctable->relations( *cluster );
          for( relation rel = range.begin() ; range.end() != rel ; ++rel ) {
            const MCParticle* particle = rel->to();
            // skip nulls 
            if( 0 == particle ) { continue ; }              // CONTINUE 
            // propagate the relation to protoparticle 
            nrel++;
            table->relate( *pp                             , 
                           particle                        , 
                           rel->weight() / (*cluster)->e() );  
          }
        }
      }
    }
    ifLog( MSG::VERBOSE )
        << protos->end() - protos->begin() << " ProtoParts associated: "
        << nrel << " relations found" << endreq;
  }
  
  // Register the table on the TES
  StatusCode sc = eventSvc()->registerObject( outputTable(), table);
  if( sc.isFailure() ) {
    ifLog( MSG::FATAL )
      << "     *** Could not register table " << outputTable() << endreq;
    delete table;
    return sc;
  } else {
    ifLog( MSG::VERBOSE )
      << "     Registered table " << outputTable() << endreq;
  }
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
