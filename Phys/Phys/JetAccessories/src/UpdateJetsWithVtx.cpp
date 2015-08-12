// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"

#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/CoreCuts.h"
#include "Event/Particle.h"
#include "Kernel/IJets2Jets.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/Particle2Vertex.h"
#include "Kernel/DefaultDVToolTypes.h"

class IRelatedPVFinder;

// ============================================================================


class UpdateJetsWithVtx : public GaudiAlgorithm
{
  // ========================================================================
  /// the friend factory for instantiation
  friend class AlgFactory<UpdateJetsWithVtx> ;
  // ========================================================================
protected:
  // ========================================================================
  /** Standard constructor
   *  @param name instance name
   *  @param pSvc pointer to Service Locator
   */
  UpdateJetsWithVtx
  ( const std::string& name ,
    ISvcLocator*       pSvc )
    : GaudiAlgorithm ( name , pSvc ),
      m_pvRelator(0)
  {
    declareProperty ( "InputJetsLocation"   ,  m_inputJetsLocation   , "Location of jets to be updated") ;
    declareProperty ( "InputVertexLocation" ,  m_inputVertexLocation , "Location of vertices to be updated") ;
    // declareProperty ( "UpdatedJetsLocation" ,  m_updatedJetsLocation , "Location of the updated jets") ;
    declareProperty("PVRelatorName",
                    m_pvRelatorName = DaVinci::DefaultTools::PVRelator );
  }

  /// destructor
  virtual ~UpdateJetsWithVtx( ){}
  // ========================================================================
public:
  // ========================================================================
  /** standard execution of the algorithm
   *  @see LoKi::Algo
   *  @return status code
   */
  virtual StatusCode initialize ()
  {
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

    if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

    m_pvRelator = tool<IRelatedPVFinder>(m_pvRelatorName,this);

    return sc;
  }

  virtual StatusCode execute() ;

  // ========================================================================
private:
  // ========================================================================
  // the default constructor is disabled
  //UpdateJetsWithVtx () ;
  // the copy constructor is disabled
  //UpdateJetsWithVtx ( const UpdateJetsWithVtx& )  ;
  // the assignement operator is disabled
  //UpdateJetsWithVtx& operator=( const  UpdateJetsWithVtx& )  ;
  // ========================================================================
private:
  std::string m_inputJetsLocation ;
  std::string m_inputVertexLocation ;
  //std::string m_updatedJetsLocation ;
  IRelatedPVFinder* m_pvRelator ;
  std::string m_pvRelatorName;   ///< The name of the PV relator to use
};

DECLARE_ALGORITHM_FACTORY( UpdateJetsWithVtx )
// ===========================================================================

StatusCode UpdateJetsWithVtx::execute ()
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts    ;

  setFilterPassed ( false ) ;


  if(!exist<LHCb::Particles>(  m_inputJetsLocation )){
    //Failure
    return StatusCode::SUCCESS ;
  }

  LHCb::Particles* myJets = get<LHCb::Particles>(  m_inputJetsLocation );

  int type = 0;
  if(exist<LHCb::Particles>( m_inputVertexLocation ))     type = 1;
  if(exist<LHCb::RecVertex::Container>( m_inputVertexLocation ))     type = 2;


  if(!type){
    //Failure
    return StatusCode::SUCCESS ;
  }


  IJets2Jets::Jets Vert(0);
  IJets2Jets::Jets Jets(0);

  typedef std::vector<const LHCb::Particle*> Parts;

  // select

  // create a map containing the weight info to vertex
  std::vector< std::vector<float> > jetList;


  if(type == 1){
    //// Create relation table between vertex and jets
    IJets2Jets::Table* table = new IJets2Jets::Table() ;

    const LHCb::Particles* myInput = get<LHCb::Particles>( m_inputVertexLocation );


    for( LHCb::Particles::const_iterator secjet =myJets->begin() ; myJets->end()!= secjet ; secjet++ )
    {
      std::vector<float> tmp_jet;
      tmp_jet.push_back(KEY(*secjet));
      tmp_jet.push_back(-1.);
      tmp_jet.push_back(-1.);
      double nonDown(0.);
      Parts daug_secjet ;
      int jetPID = PID(*secjet);
      LoKi::Extract::getParticles (*secjet, std::back_inserter (daug_secjet), PID!=jetPID);
      for(Parts::iterator idaug_secjet = daug_secjet.begin() ; daug_secjet.end() != idaug_secjet   ; idaug_secjet++ )
      {
        if((*idaug_secjet)->proto()==NULL)continue;
        const LHCb::Track* track_second = (*idaug_secjet)->proto()->track();
        if ( track_second== NULL )continue;
        if (track_second->type() != LHCb::Track::Downstream){
          nonDown+=E(*idaug_secjet);
        }
      }
      tmp_jet.push_back(nonDown);
      jetList.push_back(tmp_jet);
      for(LHCb::Particles::const_iterator primjet =myInput->begin() ; myInput->end()!= primjet ; primjet++)
      {
        double weight_jetsec_jetprim = 0.;
        Parts daug_secjet ;
        int jetPID = PID(*secjet);
        int vertPID = PID(*primjet);
        LoKi::Extract::getParticles (*secjet, std::back_inserter (daug_secjet), PID!=jetPID);
        Parts daug_primjet ;
        LoKi::Extract::getParticles (*primjet, std::back_inserter (daug_primjet), PID!=vertPID);
        for(Parts::iterator idaug_secjet = daug_secjet.begin() ; daug_secjet.end() != idaug_secjet   ; idaug_secjet++ )
        {
          if((*idaug_secjet)->proto()==NULL)continue;
          const LHCb::Track* track_second = (*idaug_secjet)->proto()->track();
          if ( track_second== NULL )continue;
          for(Parts::iterator idaug_primjet = daug_primjet.begin() ; daug_primjet.end()!= idaug_primjet  ; idaug_primjet++ )
          {
            // both are charged and have track... compare tracking LHCbIDs
            double sharedID = 0.;
            if ((*idaug_primjet)->proto()==NULL)continue;

            const LHCb::Track* track_first = (*idaug_primjet)->proto()->track();
            if (track_first== NULL )continue;

            const LHCb::Track::LHCbIDContainer idsFirst = track_first->lhcbIDs () ;

            const LHCb::Track::LHCbIDContainer idsSec = track_second->lhcbIDs () ;
            for (LHCb::Track::LHCbIDContainer::const_iterator i_idsFirst = idsFirst.begin()
                   ; idsFirst.end()!=i_idsFirst;i_idsFirst++){
              for (LHCb::Track::LHCbIDContainer::const_iterator i_idsSec = idsSec.begin()
                     ; idsSec.end()!=i_idsSec;i_idsSec++){
                if ((*i_idsFirst)==(*i_idsSec)) sharedID += 1.;
              }
            }
            if(idsFirst.size()<idsSec.size()) sharedID = sharedID/idsFirst.size();
            else sharedID = sharedID/idsSec.size();

            // Check how many hits are shared for the tracks and how many calodigit are shared for the ...
            if(sharedID>0.9)
            {
              weight_jetsec_jetprim += E(*idaug_secjet)/E(*primjet);
              continue;
            }
          }
        }
        if(weight_jetsec_jetprim>0) {
          table->relate((*primjet),(*secjet),weight_jetsec_jetprim);
        }

      }
    }



    // Loop over vertices
    int vertex_index = 0;
    for (LHCb::Particles::const_iterator i_vert = myInput->begin() ; myInput->end() != i_vert ; ++i_vert ){
      IJets2Jets::Table::Range links = table->relations(*i_vert);
      int jet_index = 0;
      // find the links to jet
      for (IJets2Jets::Table::Range::const_iterator link = links.begin() ; links.end() != link ; ++link ){
        const LHCb::Particle* theLinkedJet = (*link).to();
        // For the correspondig jet if the weight is better than previous association, tag it as comming from
        // the vertex
        for (int i = 0 ; i < (int)jetList.size(); i++){
          if (jetList[i][0] == KEY(theLinkedJet)){
            if (jetList[i][1]<(*link).weight()){
              jetList[i][1]=(*link).weight();
              jetList[i][2]=(*i_vert)->key();
            }
          }
        }
        jet_index++;
      }
      vertex_index++;
    }




    //LHCb::Particles* newJets = new LHCb::Particles();
    //put (newJets , m_updatedJetsLocation );

    for (int i = 0 ; i < (int)jetList.size(); i++){
      if (jetList[i][2]>-0.5 ){
        myJets->object(jetList[i][0])->setReferencePoint( myInput->object(jetList[i][2])->referencePoint());
        myJets->object(jetList[i][0])->addInfo(53, jetList[i][1] );
        myJets->object(jetList[i][0])->addInfo(54, jetList[i][1] * E(myInput->object(jetList[i][2])) / jetList[i][3] );

        /*
          LHCb::Particle* theNewJet = new LHCb::Particle();
          theNewJet =  myJets->object(jetList[i][0])->clone();
          theNewJet->setReferencePoint( myInput->object(jetList[i][2])->referencePoint());
          theNewJet->addInfo(53, jetList[i][1] );
          theNewJet->addInfo(54, jetList[i][1] * E(myInput->object(jetList[i][2])) / jetList[i][3] );*/
        // newJets->insert(theNewJet);
        //this->markNewTree(theNewJet);
        setFilterPassed ( true ) ;
      }
      else{
        myJets->object(jetList[i][0])->setReferencePoint( Gaudi::XYZPoint(0.,0.,0.)  );
        //       myJets->addInfo(53, jetList[i][1] );
        //       myJets->addInfo(54, jetList[i][1] * E(myInput->object(jetList[i][2])) / jetList[i][3] );


        //       LHCb::Particle* theNewJet = new LHCb::Particle();
        //       theNewJet =  myJets->object(jetList[i][0])->clone();
        //       theNewJet->setReferencePoint(Gaudi::XYZPoint(0.,0.,0.));
        //newJets->insert(theNewJet);
        //this->markNewTree(theNewJet);
        setFilterPassed ( true ) ;
      }
    }

    delete table;


  }
  else if (type ==2){
    const LHCb::RecVertex::Container*  PVS = get<LHCb::RecVertex::Container>( m_inputVertexLocation );

    Gaudi::XYZPoint   point  =    Gaudi::XYZPoint(0,0,0);
    std::map< double , Gaudi::XYZPoint > PtPV;


    for ( LHCb::RecVertex::Container::const_iterator ipv = PVS->begin();  ipv != PVS->end(); ipv++){
      LHCb::RecVertex* pv = *ipv;
      const SmartRefVector< LHCb::Track > trks = pv->tracks();
      double pt = 0;
      for ( SmartRefVector< LHCb::Track >::const_iterator itk = trks.begin();  itk != trks.end(); itk++)
        pt += (*itk)->pt();

      PtPV[pt] = pv->position();

    }


    for( LHCb::Particles::const_iterator secjet =myJets->begin() ; myJets->end()!= secjet ; secjet++ )
    {

      std::map< double ,  double> tmpPair;
      std::map<double, Gaudi::XYZPoint> tmpPair2;

      //  for( LHCb::RecVertex::Container::const_iterator ipv = PVS->begin() ; PVS->end()!= ipv ; ipv++ )
      //    {
      //      const LHCb::RecVertex* PV = *ipv;
      //      int endkey = PV->tracks().back()->key();


      //      SmartRefVector< LHCb::Track >::const_iterator iPV = PV->tracks().begin();

      //      double PV2JetMatchW = 0.;
      Parts daug_secjet ;
      int jetPID = PID(*secjet);
      LoKi::Extract::getParticles (*secjet, std::back_inserter (daug_secjet), PID!=jetPID);

      if(PVS->size() == 0){
        (*secjet)->setReferencePoint(point);
      }else if(PVS->size() == 1){
        LHCb::RecVertex::Container::const_iterator ipv = PVS->begin();
        (*secjet)->setReferencePoint((*ipv)->position());

      }else{

        for(Parts::iterator idaug_secjet = daug_secjet.begin() ; daug_secjet.end() != idaug_secjet   ; idaug_secjet++ )
        {



          if((*idaug_secjet)->proto()==NULL)continue;
          if((*idaug_secjet)->charge()==0)continue;

          const LHCb::Track* track = (*idaug_secjet)->proto()->track();
          if ( track== NULL )continue;
          if (track->type() != LHCb::Track::Downstream){


            const LHCb::VertexBase* vtx =
              m_pvRelator->relatedPV(*idaug_secjet,
                                     LHCb::VertexBase::ConstVector(PVS->begin(),
                                                                   PVS->end()));


            //   std::cout << track->p() << " => " << vtx->position().Z() << std::endl;
            if(tmpPair.count(vtx->position().Z()) < 0.5 ){
              tmpPair[vtx->position().Z()] = track->p();
              tmpPair2[(tmpPair.find(vtx->position().Z())->second)] = vtx->position();
            } else{
              double tmpval = tmpPair.find(vtx->position().Z())->second;
              tmpval += track->p();
              tmpPair2.erase(tmpPair.find(vtx->position().Z())->second);
              tmpPair[vtx->position().Z()] = tmpval;
              tmpPair2[(tmpPair.find(vtx->position().Z())->second)] = vtx->position();
            }
          }


        }


        std::map<double, Gaudi::XYZPoint>::reverse_iterator it3;

        if (tmpPair2.size() > 0){
          it3=tmpPair2.rbegin();
          (*secjet)->setReferencePoint(((*it3).second));
        } else {
          it3=PtPV.rbegin();
          (*secjet)->setReferencePoint(((*it3).second)); //no charge track into the jet.....
        }
      }


    }


    setFilterPassed ( true ) ;
  }


  return StatusCode::SUCCESS ;




}
// ============================================================================
// The END
// ============================================================================
