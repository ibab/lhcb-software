// $Id: NeutralPP2MC.cpp,v 1.23 2009-07-25 00:42:17 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.23 $
// ============================================================================
#include "Relations/IRelationWeighted.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/StreamBuffer.h"
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"
#include "Relations/RelationWeighted1D.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "CaloUtils/Calo2MC.h"
#include "Kernel/PP2MCLocation.h"
#include "Kernel/Particle2MCLinker.h"
#include "NeutralPP2MC.h"

/** @class NeutralPP2MC NeutralPP2MC.cpp
 *  
 *  The algorithm for building the relation table for 
 *  MC truth information for neutrals protoparticles.
 *
 *  The major properties of the algorithm :
 *  
 *   - "InputData"
 *      The defualt value is ProtoParticleLocation::Neutrals
 *      The list of (input) location of ProtoParticles 
 *
 *   - "OutputTable" 
 *      The defualt valeu is  NeutralPP2MCAsctLocation
 *      The locatioin of (output) relation table for
 *      ProtoParticle -> MCRelation 
 *   
 *   - "MCCaloTable"
 *      The default value is "Rec/Relations/EcalClusters2MCParticles"
 *      The locatiin in TES the major source of MC information : 
 *      the relation table for CaloCluster -> MCParticle relations 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/04/2002
 */
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( NeutralPP2MC )

NeutralPP2MC::NeutralPP2MC( const std::string& name , ISvcLocator*       svc  )
  : AsctAlgorithm ( name , svc ),
    m_mcTable ( )  {
  declareProperty ( "MCCaloTable" , m_mcTable ) ;
  std::string out( context() );
  std::transform( context().begin() , context().end() , out.begin () , ::toupper ) ;
  if( out == "HLT" ){
  }else{
    m_outputTable = LHCb::ProtoParticle2MCLocation::Neutrals ;
  }
  m_test = context();
}


// ============================================================================

StatusCode NeutralPP2MC::initialize(){
  StatusCode sc = GaudiAlgorithm::initialize();
  if( !sc.isSuccess() ) return sc;

  // property setting for Neutral is based on context() 
  // default setup in initialize() to please the linker design
  std::string out( context() );
  std::transform( context().begin() , context().end() , out.begin () , ::toupper ) ;
  if( out == "HLT" ){
    if(m_inputData.empty()   ) m_inputData.push_back( LHCb::ProtoParticleLocation::HltNeutrals );
    if(m_outputTable.empty() ) m_outputTable =  "Relations/" +  LHCb::ProtoParticleLocation::HltNeutrals ;;
    if(m_mcTable.empty()     ) m_mcTable     =  "Relations/" + LHCb::CaloClusterLocation::DefaultHlt;
  }else{
    if(m_inputData.empty()   ) m_inputData.push_back( LHCb::ProtoParticleLocation::Neutrals );
    if(m_outputTable.empty() ) m_outputTable =  "Relations/" +  LHCb::ProtoParticleLocation::Neutrals ;;
    if(m_mcTable.empty()     ) m_mcTable     =  "Relations/" + LHCb::CaloClusterLocation::Default;
  }
  
  
  
  return StatusCode::SUCCESS;
}



// ============================================================================
/** standard execution of the algorithm
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode NeutralPP2MC::execute(){

  if (msgLevel(MSG::DEBUG)) debug() << "Execute for context = "     << context() 
                                    << "::  inputData   = ("           << m_inputData << ")"
                                    << "::  output Relation table =  " << m_outputTable 
                                    << "::  input  Relation table =  " << m_mcTable
                                    << endmsg;
  // avoid the long name and always use "const" qualifier
  typedef const SmartRefVector<CaloHypo>                        Hypos    ;
  typedef const SmartRefVector<CaloCluster>                     Clusters ;
  
  // ProtoParticle -> MCParticle relations   (MC output) 
  typedef RelationWeighted1D<LHCb::ProtoParticle,LHCb::MCParticle, double>  Table;
  
  // CaloCluster -> MCParticle relations     (MC input)
  typedef const LHCb::Calo2MC::IClusterTable                    MCTable  ;
  typedef const MCTable::Range                                  Range    ;
  typedef       MCTable::iterator                               relation ;
  // input locations 
  typedef std::vector<std::string>                              Inputs   ;
  
  
  // create protoP<->MC output relation table  
  Table* table = 0 ;
  if ( !outputTable().empty() ){
    table = new Table() ;
    put( table, outputTable() );
  };
  // get cluster<->MC input relation table
  const MCTable* mcTable = getIfExists<MCTable> ( m_mcTable ) ;
  if ( 0 == mcTable ) { return StatusCode::SUCCESS ; }       // RETURN 

  
  // Loop on all input containers of ProtoParticles
  for ( Inputs::const_iterator inp = m_inputData.begin(); m_inputData.end()!= inp; inp++)  {

    // get the protoparticles from TES 
    const ProtoParticles* protos = getIfExists<ProtoParticles> ( *inp ) ;
    if ( 0 == protos ) { return StatusCode::SUCCESS ; }       // RETURN        

    
    // Create a linker table
    const std::string linkContainer = (*inp) + Particle2MCMethod::extension[Particle2MCMethod::NeutralPP];
    
    // Just a fake helper class
    Object2MCLinker<LHCb::ProtoParticle> p2MCLink(this);
    Object2MCLinker<LHCb::ProtoParticle>::Linker* linkerTable = p2MCLink.linkerTable( linkContainer );
    
    if ( NULL == table && NULL == linkerTable ) { continue; } // CONTINUE
    
    int npp = 0;
    for ( ProtoParticles::const_iterator ipp = protos->begin() ; protos->end() != ipp ; ++ipp ){
      const ProtoParticle* pp = *ipp ;
      if ( 0 == pp ) { continue ; }
      if ( msgLevel( MSG::DEBUG ))
           debug() <<"- ProtoParticle : #" << npp << " ( Context = '" << context() << "' )" << endmsg;
      npp++;
      const Hypos& hypos = pp->calo() ;
      // loop over all hypotheses for givel protoparticle
      for ( Hypos::const_iterator ihypo = hypos.begin() ; hypos.end() != ihypo ; ++ihypo){
        const CaloHypo* hypo = *ihypo ;
        if ( 0 == hypo  ) { continue ; }                         // CONTINUE 
        
        if ( msgLevel( MSG::DEBUG ))
             debug() << "  --> CaloHypo : Energy = " << hypo->e() << " : Hypothesis = " << hypo->hypothesis() << endmsg;
             
             const Clusters& clusters = hypo->clusters();
        for ( Clusters::const_iterator icluster = clusters.begin() ; clusters.end() != icluster ; ++icluster ){
          const CaloCluster* cluster = *icluster ;
          if ( 0 == cluster ) { continue ; }                  // CONTINUE 
          
          if ( msgLevel( MSG::DEBUG ))
               debug() << "   ---> CaloCluster : Energy = " << cluster->e() << endmsg;
          
          Range range = mcTable -> relations ( cluster ) ;          
          for ( relation rel = range.begin() ; range.end() != rel ; ++rel ){
            const MCParticle* mcPart = rel->to();
            if ( 0 == mcPart ) { continue ; }              // CONTINUE 
            
            const double weight = rel->weight() / cluster->e() ;

            if ( msgLevel( MSG::DEBUG ))
                 debug() << "    ----> MCParticle : Energy " << mcPart->momentum().e() << " : PID = " 
                   << mcPart->particleID().pid() << " : Weight " << weight << endmsg;

            if ( NULL != table )      { table       -> relate (  pp , mcPart , weight ) ; } 
            if ( NULL != linkerTable ){ linkerTable -> link   (  pp , mcPart , weight ) ; }
            
          } // end of loop over MC relations
        } // end of loop over clusters 
      } // end of loop over hypothesis
    } // end of loop over protoparticles  
  } // end of loop over input containters 
  
  if ( msgLevel( MSG::DEBUG ) && 0 != table ){
    debug() << "Number of MC links are " 
            << table->relations().size() << endmsg ;
  }
  return StatusCode::SUCCESS;
}
