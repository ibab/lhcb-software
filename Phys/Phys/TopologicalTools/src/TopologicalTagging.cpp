#include "TopologicalTagging.h"

//=======================================
//
// addapted from FilterDesktop
//
//=======================================

DECLARE_ALGORITHM_FACTORY( TopologicalTagging )

    //=============================================================================
    // Constructor
TopologicalTagging::TopologicalTagging(const std::string& name,
        ISvcLocator* pSvcLocator)
    : DaVinciAlgorithm(name,pSvcLocator)
      , m_TriggerTisTosTool(0)
      , m_tinputs ("Phys/Test/Particles")
      , m_abspid  (98)

{
    // Algorithm related properties
    declareProperty("TriggerLine",
            m_TLine = "Hlt2Topo.*Decision");
    declareProperty("SVLocation", m_tinputs);
    declareProperty("ParticleAbsPID", m_abspid);

}

//=============================================================================
// Initalise
StatusCode  TopologicalTagging::initialize ()
{
    const StatusCode sc = DaVinciAlgorithm::initialize();
    if ( sc.isFailure() ) return sc;


    m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos","TriggerTisTos",this );
    if(m_TriggerTisTosTool == 0)
        return Error("Couldn't get requested jet tag tool", StatusCode::SUCCESS);


    return sc;
}

StatusCode TopologicalTagging::execute()
{
    setFilterPassed(false);
    i_markedParticles.clear();

    const LHCb::Particle::ConstVector& jets = i_particles () ;

    LHCb::Particle::Range topos;
    bool locseed = exist<LHCb::Particle::Range>( m_tinputs );
    if(!locseed){
        debug()<< "Unable to get location at '"
            << m_tinputs  << "'" << endmsg;
    }else if(locseed){
        topos = get<LHCb::Particle::Range>(  m_tinputs );
        if( !topos ){
            debug()<< "Unable to get particles at '"
                << m_tinputs  << "'" << endmsg;
        }
    }
    std::vector<const LHCb::HltObjectSummary*> hltObjs
        = m_TriggerTisTosTool->hltObjectSummaries("Hlt2Topo.*BBDTDecision",2,2);
    m_TriggerTisTosTool->setOfflineInput();
    unsigned int num = hltObjs.size();


    for ( LHCb::Particle::ConstVector::const_iterator ijet = jets.begin();
            jets.end() != ijet ;++ijet )
    {
        const LHCb::Particle* jet = *ijet;
        //std::cout<<"dsize: "<<(jet->daughtersVector()).size()<<std::endl;
        //check if jet is TOS Hlt2Topo*BBDT, if yes add 10 to the tagging HLT value
        if((int) jet->particleID().abspid() != m_abspid) continue;
        int tagTOPO = 0;
        int tagHLT = 0;
        bool closeTopo = false;
        bool closeTopoOneTrk = false;
        bool topoAllTracks = false;
        ITriggerTisTos::TisTosDecision classifiedDec;
        m_TriggerTisTosTool->setOfflineInput(*jet);
        m_TriggerTisTosTool->setTriggerInput(m_TLine);
        classifiedDec = m_TriggerTisTosTool->triggerTisTos();

        if(classifiedDec.tos() == true) tagHLT+=10;



        //get jet tracks.
        LHCb::Track::ConstVector jetTracks ;
        getTracks( jet , jetTracks ) ;


        LHCb::Particle::Range::iterator itopo;

        std::vector< LHCb::LHCbID > AllIDs;
        AllIDs.clear();
        getJetLHCbIDs(jet, AllIDs);
        Gaudi::XYZVector A = jet->momentum().Vect();
        if(locseed){

            //   std::cout<<"topo size" << topos.size() << std::endl;

            //    const LHCb::VertexBase* JetOriVtx = m_dva->bestVertex ( jet );

            for(itopo = topos.begin(); itopo!=topos.end(); itopo++){


                const LHCb::Particle* myTopo = *itopo;
                const LHCb::VertexBase* evtx = myTopo->endVertex();
                const LHCb::VertexBase* oriVtx = bestVertex ( myTopo );

                //jet endVertex position is define as it's PV in the jet reco
                if((jet->endVertex()->position() - oriVtx->position()).Mag2() > 1e-4) continue;


                Gaudi::XYZVector B = evtx->position() - oriVtx->position();

                LHCb::Track::ConstVector topoTracks ;
                getTracks( myTopo , topoTracks  ) ;


                LHCb::Track::ConstVector::iterator iTopoTracks ;
                LHCb::Track::ConstVector::iterator iJetTracks ;

                int NTracksInJet = 0;
                for(iTopoTracks=topoTracks.begin();iTopoTracks != topoTracks.end(); iTopoTracks++){
                    for(iJetTracks=jetTracks.begin();iJetTracks != jetTracks.end(); iJetTracks++){
                        if((*iTopoTracks)->key() == (*iJetTracks)->key())
                            NTracksInJet++;
                    }
                }
                if(NTracksInJet == (int)topoTracks.size() && !topoAllTracks){
                    //check if jet contain all track of at least one particle of the Secondary verticies container, if yes add 100 to the tagging TOPO value
                    topoAllTracks = true;
                    tagTOPO+=100;

                }
                // LoKi::Cuts::DR2 deltaRFunction = Cuts::DR2(jet->momentum());

                const double PI=3.14159265;
                double Dphi,phi1,phi2,e1,e2;
                phi1 = A.phi();
                phi2 = B.phi();
                e1 = A.eta();
                e2 = B.eta();
                Dphi = std::fabs(phi1-phi2);
                if (Dphi>PI) { Dphi = 2*PI - Dphi; };
                double distance = std::sqrt( Dphi*Dphi + (e1-e2)*(e1-e2) );
                if(distance < 0.5) {
                    if(!closeTopo){
                        //check if any particle from the Secondary verticies container has it vertex inside the jet, if yes add 1 to the tagging TOPO value
                        closeTopo=true;
                        tagTOPO+=1;
                    }
                    if(!closeTopoOneTrk && NTracksInJet > 0){
                        //check if any particle from the Secondary verticies container has it vertex inside the jet + one track matching a jet track, if yes add 10 to the tagging TOPO value
                        closeTopoOneTrk=true;
                        tagTOPO+=10;
                    }
                }

                if(closeTopo && closeTopoOneTrk && topoAllTracks) break;

            }
        }
        //check if any Hlt2Topo*BBDT object is within the jet cone (DeltaR 0.5), if yes add 1 to the tagging HLT value

        float ratio = 0.;
        for(unsigned int i = 0; i < num; i++){
            std::vector< LHCb::LHCbID > hltLHCbIDs;
            hltLHCbIDs.clear();
            getHltObjLHCbIDs((hltObjs[i]),hltLHCbIDs);
            float TotN = (float) hltLHCbIDs.size();
            float TotMatching = 0.0;
            ratio = 0.;
            for(std::vector<LHCb::LHCbID>::iterator iID1 = hltLHCbIDs.begin(); iID1!= hltLHCbIDs.end(); iID1++){
                for(std::vector<LHCb::LHCbID>::iterator iAllIDs = AllIDs.begin(); iAllIDs != AllIDs.end(); iAllIDs++){
                    if((*iID1).lhcbID() == (*iAllIDs).lhcbID()){
                        TotMatching+=1.0;
                        ratio = TotMatching/TotN;
                        if (ratio > 0.6)break;
                    }
                    if (ratio > 0.6)break;
                }
                if (ratio > 0.6)break;
            }

            //     std::cout<<"recVtx: "<<B1<<std::endl;
            //   std::cout<<"PVcVtx: "<<B2<<std::endl;
            if(ratio > 0.6) {
                tagHLT+=1;
                break;
            }
        }
        if (tagHLT > 0 || tagTOPO > 0)
        {

            LHCb::Particle* tagJet = jet->clone();

            if ( !m_SubPID.empty() )
            {
                LHCb::IParticlePropertySvc* ppSvc =
                    svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);
                const LHCb::ParticleProperty* pp = ppSvc->find ( m_SubPID ) ;
                tagJet->setParticleID(pp->particleID());
            }

            const LHCb::VertexBase* bestPV = getStoredBestPV(jet);
            if ( 0!= bestPV ) { this->relate ( tagJet , bestPV ) ; }
            if(!tagJet->hasInfo(9990)){
                tagJet->addInfo(9990, tagHLT);
                tagJet->addInfo(9991, tagTOPO);
            }

            i_markedParticles.push_back( tagJet ) ;

            setFilterPassed(true);
        }

    }


    return StatusCode::SUCCESS ;
}


// ============================================================================
// save (clone if needed) selected particles in TES
// ============================================================================
template <class Particles>
StatusCode TopologicalTagging::_save ( ) const
{
    //
    Particles* p_tes = new Particles () ;
    //
    put ( p_tes ,    particleOutputLocation () ) ;


    for ( std::vector<LHCb::Particle*>::const_iterator ip = i_markedParticles.begin();
            ip != i_markedParticles.end(); ++ip )
    {
        //
        LHCb::Particle* p = *ip ;
        if ( NULL == p ) { continue ; } // CONTINUE

        p_tes -> insert ( p ) ;
        //

    }

    return ( i_markedParticles.size() != p_tes->size() ?
            StatusCode::FAILURE : StatusCode::SUCCESS   );
}

// ============================================================================

// ============================================================================

StatusCode TopologicalTagging::_saveInTES ()
{
    if (msgLevel(MSG::VERBOSE))
    {
        verbose() << "TopologicalTagging::_saveInTES " << i_markedParticles.size() << " Particles" << endmsg;
    }
    return this -> _save<LHCb::Particle::Container> ();
}
// ============================================================================


// ============================================================================
void TopologicalTagging::writeEmptyTESContainers()
{
    return true ?
        writeEmptyKeyedContainers()  :
        writeEmptySharedContainers() ;

}
// ============================================================================
void TopologicalTagging::writeEmptyKeyedContainers() const
{
    LHCb::Particle::Container* container = new LHCb::Particle::Container();
    put(container, particleOutputLocation());
}
// ============================================================================
void TopologicalTagging::writeEmptySharedContainers() const
{
    LHCb::Particle::Selection* container = new LHCb::Particle::Selection();
    put(container, particleOutputLocation());
}


Gaudi::XYZVector TopologicalTagging::getHltObjVtxPos (const LHCb::HltObjectSummary *obj) const{
    LHCb::HltObjectSummary::Info infod = obj->numericalInfo();

    double p = 1/infod["7#Particle.1/p"];
    double slopex = infod["5#Particle.slopes.x"];
    double slopey = infod["6#Particle.slopes.y"];
    double slopez = 1/sqrt(slopex*slopex + slopey*slopey + 1.0);
    double pz = slopez*p;
    double mm = infod["1#Particle.measuredMass"];
    std::cout<<p<<"  "<<pz<<"   "<<mm<<std::endl;
    double X = infod["0#RecVertex.position.x"];
    double Y = infod["1#RecVertex.position.y"];
    double Z = infod["2#RecVertex.position.z"];
    return Gaudi::XYZVector(X,Y,Z);
}
//=========================================================================
StatusCode TopologicalTagging::getHltObjLHCbIDs(const LHCb::HltObjectSummary * sum, std::vector< LHCb::LHCbID > & AllIDs) const{
    if (0==sum) return StatusCode::SUCCESS ;
    if(sum->substructure().size()>0){
        for ( SmartRefVector< LHCb::HltObjectSummary >::const_iterator s = sum->substructure().begin() ;
                s != sum->substructure().end() ; ++s)
            getHltObjLHCbIDs(*s,AllIDs);


        const std::vector< LHCb::LHCbID > lIDs = sum->lhcbIDsFlattened();
        AllIDs.insert(AllIDs.end(), lIDs.begin(), lIDs.end());


    }else{

        const std::vector< LHCb::LHCbID > lIDs = sum->lhcbIDsFlattened();
        AllIDs.insert(AllIDs.end(), lIDs.begin(), lIDs.end());
    }

    return StatusCode::SUCCESS ;

}

StatusCode TopologicalTagging::getJetLHCbIDs(const LHCb::Particle* part, std::vector< LHCb::LHCbID > & AllIDs) const{

    LHCb::Particle::ConstVector daus = part->daughtersVector();
    if(daus.size() > 0){
        for(LHCb::Particle::ConstVector::iterator idaus = daus.begin(); idaus != daus.end(); idaus++)
            getJetLHCbIDs((*idaus),AllIDs);
    }else{

        const LHCb::ProtoParticle* proto = part->proto();
        if(proto != NULL){
            const LHCb::Track* track = proto->track();
            if(track != NULL){
                const std::vector< LHCb::LHCbID > lIDs = track->lhcbIDs();
                AllIDs.insert(AllIDs.end(), lIDs.begin(), lIDs.end());
            }

        }

    }

    return StatusCode::SUCCESS ;


}


void TopologicalTagging::getTracks( const LHCb::Particle* p,
                               LHCb::Track::ConstVector& tracks ) const
{
  const LHCb::ProtoParticle * proto = p->proto() ;
  if ( proto )
  {
    if ( proto->track() ) { tracks.push_back( proto->track() ); }
  }
  else
  {
    for ( SmartRefVector<LHCb::Particle>::const_iterator iP = p->daughters().begin();
          iP != p->daughters().end(); ++iP )
    {
      getTracks( *iP, tracks );
    }
  }
}


