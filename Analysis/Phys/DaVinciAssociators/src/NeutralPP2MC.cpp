// $Id: NeutralPP2MC.cpp,v 1.5 2004-06-11 15:26:17 phicharp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2004/03/11 10:30:26  pkoppenb
// upstream -> downstream
//
// Revision 1.3  2003/04/17 09:58:26  phicharp
// Allow Links associator to use Particles not in the TES
//
// Revision 1.2  2002/10/02 07:06:28  phicharp
// New associators, include multiple associations etc..
//
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

#define ifMsg(sev) msg << sev; if( msg.level() <= (sev) ) msg

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
  : AsctAlgorithm ( name , svc )
  , m_asctType ( "AssociatorWeighted<CaloCluster,MCParticle,float>" )
  , m_asctName ( "CCs2MCPs" )
  , m_asct     ( 0 ) 
{ 
  m_inputData.push_back( ProtoParticleLocation::Neutrals );

  m_outputTable = NeutralPP2MCAsctLocation;
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
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;
  StatusCode sc = GaudiAlgorithm::initialize();
  if( !sc.isSuccess() ) return sc;
  
  /// locate the associator 
  m_asct = tool<MCAsct>( m_asctType , m_asctName );
  
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
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return GaudiAlgorithm::finalize() ;
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
  
  
  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  
  // create relation table  
  ProtoParticle2MCAsct::Table* table = 
    "" == outputTable() ? NULL : new ProtoParticle2MCAsct::Table();

  ///////////////////////////////////////
  // Loop on ProtoParticles containers //
  ///////////////////////////////////////

  for( std::vector<std::string>::const_iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // get MC association table for CaloHypos
    const MCTable* mctable = m_asct->direct();  
    if( 0 == mctable ) { 
      msg << MSG::INFO << "Unable to retrieve the CaloHypo2MC association table"
          <<endreq;
      continue;
    }
    // Get ProtoParticles
    SmartDataPtr<ProtoParticles> protos ( eventSvc(), *inp ) ;
    if( 0 == protos ) continue;
    // Create a linker table
    const std::string linkContainer = 
      *inp + Particle2MCMethod::extension[Particle2MCMethod::NeutralPP];
    // Just a fake helper class
    Object2MCLink p2MCLink(this);
    Object2MCLink::Linker*
      linkerTable = p2MCLink.linkerTable( linkContainer );
    if( NULL != linkerTable ) {
      ifMsg(MSG::VERBOSE) << "    Created Linker table for container "
                          << linkContainer << endreq;
    } else {
      ifMsg(MSG::VERBOSE) << "    Linker table for container "
                          << linkContainer << " already exists"
                          << endreq;
    }
    if( NULL == table && NULL == linkerTable ) continue;

    int npp = protos->size();
    ifMsg(MSG::VERBOSE) << "    " << npp
        << " ProtoParticles retrieved from " 
        << *inp << endreq;
  
    int nrel = 0;
    int nass = 0;
    for( ProtoParticles::const_iterator pp = protos->begin() ; 
         protos->end() != pp ; ++pp ) {
      // skip nulls 
      if( 0 == *pp ) { continue ; }
      ifMsg(MSG::VERBOSE) << "    ProtoParticle " << (*pp)->key() ;
      bool matched = false;
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
          if( !range.empty() ) {
            if( !matched) {
              ifMsg(MSG::VERBOSE) << " associated to MCParts";
              matched = true;
              nass++;
            }
            for( relation rel = range.begin() ; range.end() != rel ; ++rel ) {
              const MCParticle* mcPart = rel->to();
              // skip nulls 
              if( NULL == mcPart ) { continue ; }              // CONTINUE 
              // propagate the relation to protoparticle 
              ifMsg(MSG::VERBOSE) << " - " << mcPart->key();
              double weight = rel->weight() / (*cluster)->e() ;
              if( NULL != table )
                table->relate( *pp, mcPart, weight ); 
              if( NULL != linkerTable ) 
                linkerTable->link( *pp, mcPart, weight);
              nrel++;
            }
          }
        }
      }
      if( !matched) {
        ifMsg(MSG::VERBOSE) << " not associated to an MCPart";
      }
      ifMsg(MSG::VERBOSE) << endreq;
    }
    ifMsg( MSG::DEBUG )
      << "Out of " << npp << " Neutral ProtoParts in " << *inp << ", "
      << nass << " are associated, "
      << nrel << " relations found" << endreq;
  }
  
  // Register the table on the TES
  if( NULL != table ) {
    put( table, outputTable() );
    ifMsg( MSG::DEBUG )
      << "     Registered table " << outputTable() << endreq;
  }
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
