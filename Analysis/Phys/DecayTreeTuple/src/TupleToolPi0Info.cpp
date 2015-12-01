// $Id: TupleToolPi0Info.cpp,v 1.4 2010-01-26 15:39:26 rlambert Exp $
// Include files

// local
#include "TupleToolPi0Info.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/Particle.h"
#include "CaloUtils/CaloMomentum.h"
#include "GaudiKernel/IRegistry.h" //

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolPi0Info
//
// 2008-10-31 :(Happy Halloween)
// Yasmine Amhis
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolPi0Info )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolPi0Info::TupleToolPi0Info( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
    : TupleToolBase ( type, name , parent ),
      m_Pi0ID(111)
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("RequireMCTruth",m_RequireMCTruth = false);
}

//=============================================================================
StatusCode TupleToolPi0Info::fill(const Particle* , const Particle* P
                                  ,const std::string& head
                                  ,Tuples::Tuple& tuple )
{
  const std::string prefix=fullName(head);

  bool filltuple = true;
  if( P ){
    if (P->particleID().pid() == m_Pi0ID &&  isPureNeutralCalo(P)) {
      debug() << " Make the special case for the pi0  " <<  P->particleID().pid() << endmsg;
      debug() << " The pi0s has : " <<  P->daughters().size()   << " daughters  "<< endmsg;
      int Type = 0;
      if (P->daughters().size() >0 ){
        Type= 2; //for the  resolved pi0s
      }
      else Type  = 1; //for the merged pi0s

      filltuple &= tuple->column( prefix+"_Type", Type );
      filltuple &= tuple->column( prefix+"_CL"  , P->confLevel() );

      // daughter info in case of merged pi0
      if( Type == 1){        
        const LHCb::CaloHypo*   hypo  = *( (P->proto()->calo()).begin() );
        if(LHCb::CaloHypo::Pi0Merged != hypo->hypothesis() ){
          // == extract SplitPhotons hypos
          const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
          const LHCb::CaloHypo* g1 = *(hypos.begin() );
          const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
          LHCb::CaloMomentum g1Momentum( g1 ); 
          LHCb::CaloMomentum g2Momentum( g2 ); 
          filltuple &= tuple->column( prefix+"_gamma0_PT"  , g1Momentum.pt() );
          filltuple &= tuple->column( prefix+"_gamma1_PT"  , g2Momentum.pt() );
        } 
      }
      



      if(m_RequireMCTruth == true){
        Warning("MC association with TupleToolPi0Info is deprecated - please use the standard TupleToolMCTruth"
                , StatusCode::SUCCESS).ignore();
        debug() << "Get the association done for a pi0 " << endmsg;
        const MCParticle* MCPi0 = NULL;
        const MCParticle* MCPi0Mother = NULL;
        const MCParticle* MCPi0GrandMother = NULL;
        const MCParticle* MCPi0GrandGrandMother = NULL;

        int  MCPi0_id =0;
        int  MCPi0Mother_id =0;
        int  MCPi0GrandMother_id =0;
        int  MCPi0GrandGrandMother_id =0;
        int  MCPi0_key =0;
        int  MCPi0Mother_key =0;
        int  MCPi0GrandMother_key =0;
        int  MCPi0GrandGrandMother_key =0;
        double weight = 1;

        MCPi0 = getMCPi0(P,weight);
        if (MCPi0 != NULL) {
          MCPi0_id  = MCPi0->particleID().pid();
          MCPi0_key = MCPi0->key();
          MCPi0Mother = MCPi0->mother() ;
        }//particleMC info
        if (MCPi0Mother != NULL) {
          MCPi0Mother_id  = MCPi0Mother->particleID().pid();
          MCPi0Mother_key = MCPi0Mother->key();
          MCPi0GrandMother = MCPi0Mother->mother() ;
        }//particle mother MC info

        if (MCPi0GrandMother != NULL)  {
          MCPi0GrandMother_id  = MCPi0GrandMother->particleID().pid();
          MCPi0GrandMother_key = MCPi0GrandMother->key();
          MCPi0GrandGrandMother = MCPi0GrandMother->mother()  ;
        }
        if (MCPi0GrandGrandMother != NULL)  {
          MCPi0GrandGrandMother_key = MCPi0GrandGrandMother->key();
          MCPi0GrandGrandMother_id  = MCPi0GrandGrandMother->particleID().pid();
        }

        filltuple &= tuple->column( prefix+"_MCPi0_id", MCPi0_id  );
        filltuple &= tuple->column( prefix+"_MCPi0_key", MCPi0_key  );

        filltuple &= tuple->column( prefix+"_MCPi0Mother_id", MCPi0Mother_id );
        filltuple &= tuple->column( prefix+"_MCPi0Mother_key", MCPi0Mother_key );

        filltuple &= tuple->column( prefix+"_MCPi0GrandMother_id", MCPi0GrandMother_id  );
        filltuple &= tuple->column( prefix+"_MCPi0GrandMother_key", MCPi0GrandMother_key );

        filltuple &= tuple->column( prefix+"_MCPi0GrandGrandMother_id", MCPi0GrandGrandMother_id  );
        filltuple &= tuple->column( prefix+"_MCPi0GrandGrandMother_key", MCPi0GrandGrandMother_key  );
        debug() << "This is the MC part associated to your  pi0 -->   "
                <<  MCPi0_id  << "   and its  mother   -->"
                <<  MCPi0Mother_id   << endmsg;
      }//require MC truth
    }//this is a pi0
  }//get the particle

  return StatusCode(filltuple);

}



//================================================================================
//Here you get the protoparticle corresponding to your pi0 candidate
//================================================================================
TupleToolPi0Info::mcRange TupleToolPi0Info::getRange(const LHCb::Particle* part){
  mcRange range;
  if( NULL == part) return range;
  const LHCb::ProtoParticle* proto = part->proto();
  debug() << "This is  the pointer to the protoparticle "<< proto << endmsg;
  if( NULL == proto )return range;
  return getRange(proto);
}
//================================================================================
//Here you get the relation table between the protoparticles and the MC particles
//================================================================================
TupleToolPi0Info::mcRange TupleToolPi0Info::getRange(const LHCb::ProtoParticle* proto){
  mcRange range;
  if( NULL == proto)return range;
  std::string locationhard;
  if( 0 == proto->charge() ){
    if(  context() == "HLT"){
      debug ()  <<" You are running in the Hlt context   "  <<   context() << endmsg;
      locationhard = "Relations/Hlt/ProtoP/Neutrals";


    } else  {
      debug ()  <<"You are running in the Offline context   "  <<  context() << endmsg;
      locationhard = "Relations/Rec/ProtoP/Neutrals";
    }
  } //check neutral
  const mcTable* table = get<mcTable> ( locationhard ) ;
  // create protoP<->MC output relation table
  debug()  <<"ProtoP<->MC output relation table    " <<  table  << endmsg;
  if( 0 == table)return range;
  range = table->relations( proto );
  return range;
}
//=======================================================================================
const LHCb::MCParticle* TupleToolPi0Info::getMCPi0(const LHCb::Particle* part, double& weight ){
  // Return the highest-weighted matching MCPart WITH THE SAME particleID than part->ID
  // if not the highest-weighted among ALL matching MCPart then  weight = -weight
  // If not found return the highest-weighted matching MCPART (irrespective of part->ID)

  weight = 0;
  if ( NULL == part )return NULL;

  double weightMax =- 99999;
  double weightPart = -99999;
  LHCb::MCParticle* mcBest = NULL;
  LHCb::MCParticle* mcPart  = NULL;

  if( part->daughters().size() > 0 )  {
    //==============================
    // special case of ResolvedPi0s
    //==============================
    const SmartRefVector<LHCb::Particle> & daughters = part->daughters();
    SmartRefVector<LHCb::Particle>::const_iterator ig1 = daughters.begin();
    SmartRefVector<LHCb::Particle>::const_iterator ig2 = ig1+1;
    mcRange range1 = getRange( *ig1 );
    mcRange range2 = getRange( *ig2 );
    if( 0 == range1.size() || 0 == range2.size() ) return NULL;
    for(mcRange::const_iterator ir1 = range1.begin();ir1 != range1.end() ; ++ir1 ){
      for(mcRange::const_iterator ir2 = range2.begin();ir2 != range2.end() ; ++ir2 ){
        double w = (ir1)->weight();
        if ( (ir2)->weight() > (ir1)->weight() ) w=(ir2)->weight();
        if( (ir1)->to() == (ir2)->to() )w = (ir1)->weight() + (ir2)->weight();
        if( w > weightMax){
          weightMax = w;
          mcBest = (ir1)->to();
          if ( (ir2)->weight() > (ir1)->weight() ) mcBest = (ir2)->to();
        }
        if( m_Pi0ID == abs((ir1->to())->particleID().pid()) && (ir1)->to() == (ir2)->to()) {
          if(w > weightPart){
            mcPart = (ir1)->to();
            weightPart = w;
          }
        }
      }
    }
  }
  else{
    //==============
    // Pi0 merged
    //==============
    mcRange range = getRange(part);
    if( 0 == range.size() ) return NULL;
    for(mcRange::iterator ir = range.begin(); ir != range.end() ; ++ir ){
      double w = (ir)->weight();
      if( w > weightMax){
        weightMax = w;
        mcBest=(ir)->to();
      }
      if( m_Pi0ID == abs((ir->to())->particleID().pid()) ){
        if( w > weightPart){
          mcPart = (ir)->to();
          weightPart = w;
        }
      }
    }
  }
  //
  if( NULL != mcPart ){
    const LHCb::MCParticle* mc=mcPart;
    weight = weightPart;
    if( weightPart < weightMax) weight= - weightPart;
    return mc;
  }else if( NULL != mcBest){
    const LHCb::MCParticle* mc=mcBest;
    weight = weightMax;
    return mc;
  } else {
    const LHCb::MCParticle*  mc=NULL;
    weight=0;
    return mc;
  }
}
//==================================================================================================
