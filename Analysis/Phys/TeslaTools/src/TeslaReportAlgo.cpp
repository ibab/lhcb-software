// Include files 
// from Gaudi
#include "Event/ODIN.h" // event & run number
#include "GaudiKernel/AlgFactory.h" 
#include "Event/HltDecReports.h"

#include "Event/HltSelReports.h"
#include "Event/HltVertexReports.h"
#include "Event/HltDecReports.h"
// local
#include "TeslaReportAlgo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TeslaReportAlgo
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TeslaReportAlgo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TeslaReportAlgo::TeslaReportAlgo( const std::string& name,
                                    ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ), m_dist(NULL), m_check(NULL), m_conv(NULL)
{
  declareProperty( "TriggerLine" ,          m_inputName    = "Hlt2CharmHadD02HH_D02KK" );
  declareProperty( "OutputPrefix" ,         m_OutputPref   = "Tesla" );
  declareProperty( "ReportVersion" ,        m_ReportVersion= 2 );
  declareProperty( "PreSplit" ,             m_PreSplit= false );
  declareProperty( "PV" ,                   m_PV = "Offline" );
  declareProperty( "PVLoc" ,                m_PVLoc = "Turbo/Primary" );
}

//=============================================================================
// Destructor
//=============================================================================
TeslaReportAlgo::~TeslaReportAlgo() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TeslaReportAlgo::initialize()
{

  StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() << "Requested processing of line: " << m_inputName << endmsg;
  m_dist = tool<IDistanceCalculator>("LoKi::DistanceCalculator", this );
  if ( !m_dist ) 
  {
    return Error("Unable to retrieve the IDistanceCalculator tool");
  }
  m_check = tool<IReportCheck>("ReportCheckTool", this );
  if ( !m_check ) 
  {
    return Error("Unable to retrieve the ReportCheckTool");
  }
  m_conv = tool<IReportConvert>("ReportConvertTool", this );
  m_conv->setReportVersionLatest();
  if ( !m_conv ) 
  {
    return Error("Unable to retrieve the ReportConvertTool");
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TeslaReportAlgo::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  // First thing's first, let's check the reports against the
  // requested version
  int init_ReportVersion = m_ReportVersion;
  bool toolow=false;
  std::stringstream HltLoc;
  HltLoc << m_inputName << "Decision";
  //
  std::string RepLoc = "Hlt2/SelReports";
  if(m_PreSplit == true) RepLoc = LHCb::HltSelReportsLocation::Default;
  std::string VertRepLoc = "Hlt1/VertexReports";
  if(m_PreSplit == true) VertRepLoc = LHCb::HltVertexReportsLocation::Default;
  //
  int versionNum = m_check->VersionTopLevel( HltLoc.str(), RepLoc );
  
  debug() << "VersionNum = " << versionNum << endmsg;

  if( versionNum != 99 ){
    if( versionNum != m_ReportVersion ) {
      warning() << "Requested version number does not equal checker response" << endmsg;
      debug() << "versionNum = " << versionNum << ", m_ReportVersion = " << m_ReportVersion << endmsg;
      if( versionNum < m_ReportVersion ) {
        m_ReportVersion = versionNum;
        warning() << "For your own safety, I will give you less information than requested, please check report generation" << endmsg;
      }
      else toolow = true;
    }
  }
	
  // Set our output locations
  std::stringstream ss_PartLoc;
  ss_PartLoc << m_OutputPref << "/Particles";
  //
  std::stringstream ss_ProtoLoc;
  ss_ProtoLoc << m_OutputPref << "/Protos"; // NOT STORED BY ORIGINAL SELREPORTS
  //
  std::stringstream ss_VertLoc;
  ss_VertLoc << m_OutputPref << "/Vertices"; // NOT STORED BY ORIGINAL SELREPORTS
  //
  std::stringstream ss_TrackLoc;
  ss_TrackLoc << m_OutputPref << "/Tracks";
  //
  std::stringstream ss_RPIDLoc;
  ss_RPIDLoc << m_OutputPref << "/RichPIDs"; // NOT STORED BY ORIGINAL SELREPORTS
  //
  std::stringstream ss_MPIDLoc;
  ss_MPIDLoc << m_OutputPref << "/MuonPIDs"; // NOT STORED BY ORIGINAL SELREPORTS
  //
  //std::stringstream ss_PVLoc;
  //ss_PVLoc << m_OutputPref << "/PV3D";
  //ss_PVLoc << "Rec/Vertex/Primary";
  //ss_PVLoc << m_OutputPref << "/Primary";
  //
  std::stringstream ss_P2PVLoc;
  ss_P2PVLoc << m_OutputPref << "/Particle2VertexRelations";
  //
  LHCb::Particle::Container* cont_Part = new LHCb::Particle::Container() ;
  put( cont_Part , ss_PartLoc.str().c_str() );
  //
  LHCb::ProtoParticle::Container* cont_Proto = new LHCb::ProtoParticle::Container() ;
  put( cont_Proto , ss_ProtoLoc.str().c_str() );
  //
  LHCb::Vertex::Container* cont_Vert = new LHCb::Vertex::Container() ;
  put( cont_Vert , ss_VertLoc.str().c_str() );
  //
  LHCb::Track::Container* cont_Track = new LHCb::Track::Container() ;
  put( cont_Track , ss_TrackLoc.str().c_str() );
  //
  LHCb::RichPID::Container* cont_RPID = new LHCb::RichPID::Container() ;
  put( cont_RPID , ss_RPIDLoc.str().c_str() );
  //
  LHCb::MuonPID::Container* cont_MPID = new LHCb::MuonPID::Container() ;
  put( cont_MPID , ss_MPIDLoc.str().c_str() );
  //
  LHCb::RecVertex::Container* cont_PV = getIfExists<LHCb::RecVertex::Container>( m_PVLoc.c_str() );
  bool reusePV = true;
  if( !cont_PV ) {
    debug() << "Making PVs" << endmsg;
    cont_PV = new LHCb::RecVertex::Container() ;
    put( cont_PV , m_PVLoc.c_str() );
    reusePV = false;
  }
  //
  Particle2Vertex::Table* cont_P2PV = new Particle2Vertex::Table() ;
  put( cont_P2PV , ss_P2PVLoc.str().c_str() );

  const LHCb::HltSelReports* selReports;
  const LHCb::HltVertexReports* vtxReports;
  const LHCb::HltVertexReports::HltVertexReport* vtxRep;

  // Vertex reports
  if ( exist<LHCb::HltVertexReports>( VertRepLoc.c_str() ) ) { // exist --> get content
    vtxReports = get<LHCb::HltVertexReports>( VertRepLoc.c_str() );
    vtxRep = vtxReports->vertexReport("PV3D");
    if ( msgLevel(MSG::DEBUG) ){
      std::vector<std::string> vnames = vtxReports->selectionNames();
      debug() << "Available vertex reports:" << endmsg;
      for(std::vector<std::string>::iterator vname_it=vnames.begin();vname_it!=vnames.end();vname_it++){
        debug() << *vname_it << endmsg;
      }
    }
    if( vtxRep && !reusePV ){
      for( LHCb::HltVertexReports::HltVertexReport::const_iterator iv=vtxRep->begin();iv!=vtxRep->end(); ++iv){
        const LHCb::VertexBase* v = *iv;
        LHCb::RecVertex* vnew = new LHCb::RecVertex();
        debug() <<"Vertex chi2= " << v->chi2()<< ", ndf =" << v->nDoF()<< endmsg;
        debug() <<"Vertex key= " << v->key() << endmsg;
        // Turn the VertexBase from the vertex reports in to a RecVertex for packing
        vnew->setChi2( v->chi2() );
        vnew->setNDoF( v->nDoF() );
        vnew->setPosition( v->position() );
        vnew->setCovMatrix( v->covMatrix() );
        vnew->setExtraInfo( v->extraInfo() );

        // Fill PV container
        cont_PV->insert( vnew );
      } 
    }
  } else{
    warning() << "Vertex reports do not exist!!!" << endmsg;
    //
    // Needed to overcome situation of missing reports for some events
    m_ReportVersion = init_ReportVersion;
    return StatusCode::SUCCESS;
    //return StatusCode::RECOVERABLE;
  }

  // Selection reports
  if ( exist<LHCb::HltSelReports>( RepLoc.c_str() ) ) { // exist --> get content
    selReports = get<LHCb::HltSelReports>( RepLoc.c_str() );
    if ( msgLevel(MSG::DEBUG) ){
      std::vector<std::string> selnames = selReports->selectionNames();
      debug() << "Available reports:" << endmsg;
      for(std::vector<std::string>::iterator selname_it=selnames.begin();selname_it!=selnames.end();selname_it++){
        debug() << *selname_it << endmsg;
      }
    }
  } else{
    warning() << "Sel. reports do not exist!!!" << endmsg;
    //
    // Needed to overcome situation of missing reports for some events
    m_ReportVersion = init_ReportVersion;
    return StatusCode::SUCCESS;
    //return StatusCode::RECOVERABLE;
  }
	
  
  
  // ODIN
  //const LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  //
  // Go and get the information from the Hlt
  const LHCb::HltObjectSummary * MotherRep = selReports->selReport(HltLoc.str().c_str());

  if ( msgLevel(MSG::DEBUG) ){ 
    if(MotherRep!=0) debug() << "Required line has been fired" << endmsg;
  }
  else return StatusCode::SUCCESS;
  if(MotherRep){
    if ( msgLevel(MSG::DEBUG) ){
      debug() << "Reference: " << endmsg;
      debug() << "Particle class ID = " << LHCb::Particle::classID() << endmsg;
      debug() << "Track class ID = " << LHCb::Track::classID() << endmsg;
      debug() << "RecVertex class ID = " << LHCb::RecVertex::classID() << endmsg;
      debug() << "Vertex class ID = " << LHCb::Vertex::classID() << endmsg;
      debug() << "ProtoParticle class ID = " << LHCb::ProtoParticle::classID() << endmsg;
    }
    //
    debug() << *MotherRep << endmsg;
    std::vector<LHCb::LHCbID> LHCbIDs = MotherRep->lhcbIDs();
    debug() << "MOTHER CLASS ID = " << MotherRep->summarizedObjectCLID() << endmsg;
    debug() << "LHCbIDs.size() = " << LHCbIDs.size() << endmsg;
    SmartRefVector <LHCb::HltObjectSummary> substructure = MotherRep->substructure();
    debug() << "Number of triggered candidates = " << substructure.size() << endmsg;
    //
    
    
    int nCandidate = -1;

    // loop over the trigger candidates
    for(SmartRefVector <LHCb::HltObjectSummary>::const_iterator child = substructure.begin();child!=substructure.end();++child){      
      nCandidate++;
      debug() << "Starting loop over triggered candidates" << endmsg;
      debug() << "Mother object CLASS ID = " << (*child)->summarizedObjectCLID() << endmsg;
      const LHCb::HltObjectSummary * Obj = child->target();
      
      // do we have a basic particle?
      // Assess basic by ID
      double motherID = (int)Obj->numericalInfo()["0#Particle.particleID.pid"];
      debug() << "Found mother with ID: " << motherID << endmsg;
      bool motherBasic = false;
      if( TMath::Abs(motherID)==11 || TMath::Abs(motherID)==211 || TMath::Abs(motherID)==321 || TMath::Abs(motherID)==22 || TMath::Abs(motherID)==13 ) motherBasic = true;
      debug() << "Mother has substructure consisting of:" << endmsg;
      for(SmartRefVector <LHCb::HltObjectSummary>::const_iterator it = Obj->substructure().begin();it!=Obj->substructure().end();++it){
        debug() << (*it)->summarizedObjectCLID() << endmsg;
      }
      
      // make the objects to be stored
      std::vector<ContainedObject*> newObjects_mother;
      LHCb::Particle* Part = new LHCb::Particle();
      newObjects_mother.push_back(Part);
      //
      if( motherBasic == true) {
        //
        LHCb::Track* track = new LHCb::Track();
        newObjects_mother.push_back(track);
        cont_Track->insert( track );
        //
        LHCb::ProtoParticle* proto = new LHCb::ProtoParticle();
        newObjects_mother.push_back(proto);
        cont_Proto->insert( proto );
        //
        LHCb::RichPID* rich = new LHCb::RichPID();
        newObjects_mother.push_back(rich);
        cont_RPID->insert( rich );
        //
        LHCb::MuonPID* muon = new LHCb::MuonPID();
        newObjects_mother.push_back(muon);
        cont_MPID->insert( muon );
        //  
      } //else {
        //
        //LHCb::Vertex* vert = new LHCb::Vertex();
        //newObjects_mother.push_back(vert);
        //cont_Vert->insert( vert );
        //  
      //}
      
      fillParticleInfo( newObjects_mother , Obj , motherBasic );
      cont_Part->insert( Part );
     
      // Which PV is best for the mother???
      double min_var = -1.0;
      int key = 0;
      for ( LHCb::RecVertex::Container::const_iterator pv = cont_PV->begin() ; pv!=cont_PV->end() ; ++pv){
        double ip, chi2;
        StatusCode test = m_dist->distance ( Part, *pv, ip, chi2 );
        if(m_PV=="Offline"){
          if( (chi2<min_var) || (min_var<0.)){
            min_var = chi2 ;
            key = (*pv)->key();
          }
        }
        else if(m_PV=="Online"){
          if( (ip<min_var) || (min_var<0.)){
            min_var = ip;
            key = (*pv)->key();
          }
        }
        else{
          warning() << "Choose a valid PV requirement or use the default (online)" << endmsg;
          return StatusCode::FAILURE;
        }
      }
      cont_P2PV->relate( Part , (LHCb::RecVertex*)cont_PV->containedObject( key ) );

      if( motherBasic == false ){
        LHCb::Vertex* vert_mother = new LHCb::Vertex();
        cont_Vert->insert( vert_mother );

        // loop over the first generation daughters
        for(SmartRefVector <LHCb::HltObjectSummary>::const_iterator child2 = Obj->substructure().begin();child2!=Obj->substructure().end();++child2){
          debug() << "Daughter level 1 CLASS ID = " << (*child2)->summarizedObjectCLID() << endmsg;
          
          const LHCb::HltObjectSummary * Obj_d1 = child2->target();

          // is this the last element?
          bool mothersubloop_last = is_last(child2,Obj->substructure());
          if ((mothersubloop_last==true) && (m_ReportVersion==2) ){
            // for a non-basic particle, the end vertex is the last
            // element of the substructure
            fillVertexInfo( vert_mother , Obj_d1 );
            Part->setEndVertex( vert_mother );
            debug() << "Setting mother decay vertex" << endmsg;
            continue;
          }
          if( (mothersubloop_last==true) && (toolow == true) ) continue;

          // do we have a basic particle?
          double d1_ID = (int)Obj_d1->numericalInfo()["0#Particle.particleID.pid"];
          debug() << "1st Gen. Daughter ID: " << Obj_d1->numericalInfo()["0#Particle.particleID.pid"] << endmsg;
          bool d1_Basic = false;
          if( TMath::Abs(d1_ID)==11 || TMath::Abs(d1_ID)==211 || TMath::Abs(d1_ID)==321 || TMath::Abs(d1_ID)==22 || TMath::Abs(d1_ID)==13 ) d1_Basic = true;
          
          // make the objects to be stored
          std::vector<ContainedObject*> newObjects_d1;
          
          LHCb::Particle* Part_d1 = new LHCb::Particle();
          newObjects_d1.push_back(Part_d1);
          Part->addToDaughters(Part_d1);
          cont_Part->insert( Part_d1 );
          cont_P2PV->relate( Part_d1 , (LHCb::RecVertex*)cont_PV->containedObject( key ) );

          //
          if( d1_Basic == true ){
            debug() << "Basic daughter level 1 has substructure consisting of:" << endmsg;
            for(SmartRefVector <LHCb::HltObjectSummary>::const_iterator basic_it = Obj_d1->substructure().begin();basic_it!=Obj_d1->substructure().end();++basic_it){
              debug() << (*basic_it)->summarizedObjectCLID() << endmsg;
            }
            //
            LHCb::Track* track_d1 = new LHCb::Track();
            newObjects_d1.push_back(track_d1);
            cont_Track->insert( track_d1 );
            //
            LHCb::ProtoParticle* proto_d1 = new LHCb::ProtoParticle();
            newObjects_d1.push_back(proto_d1);
            cont_Proto->insert( proto_d1 );
            //
            LHCb::RichPID* rich_d1 = new LHCb::RichPID();
            newObjects_d1.push_back(rich_d1);
            cont_RPID->insert( rich_d1 );
            //
            LHCb::MuonPID* muon_d1 = new LHCb::MuonPID();
            newObjects_d1.push_back(muon_d1);
            cont_MPID->insert( muon_d1 );
            //  
          } 
          
          vert_mother->addToOutgoingParticles( Part_d1 );

          fillParticleInfo( newObjects_d1 , Obj_d1 , d1_Basic );
          
          if( d1_Basic == false ){
            LHCb::Vertex* vert_d1 = new LHCb::Vertex();
            cont_Vert->insert( vert_d1 );
            // loop over the second generation daughters
            for(SmartRefVector <LHCb::HltObjectSummary>::const_iterator child3 = Obj_d1->substructure().begin();child3!=Obj_d1->substructure().end();++child3){
              debug() << "Daughter level 2 CLASS ID = " << (*child3)->summarizedObjectCLID() << endmsg;
              
              const LHCb::HltObjectSummary * Obj_d2 = child3->target();
              
              // is this the last element?
              bool d1_subloop_last = is_last(child3,Obj_d1->substructure());
              if((d1_subloop_last == true) && (m_ReportVersion==2) ){
                // for a non-basic particle, the end vertex is the last
                // element of the substructure
                fillVertexInfo( vert_d1, Obj_d2 );
                Part_d1->setEndVertex( vert_d1 );
                debug() << "Setting first generation daughter decay vertex" << d1_ID << endmsg;
                continue;
              }
              if( (d1_subloop_last==true) && (toolow == true) ) continue;
              
              debug() << "2nd Gen. Daughter ID: " << Obj_d2->numericalInfo()["0#Particle.particleID.pid"] << endmsg;
              // We should have a basic particle now
              double d2_ID = (int)Obj_d2->numericalInfo()["0#Particle.particleID.pid"];
              bool d2_Basic = false;
              if( TMath::Abs(d2_ID)==11 || TMath::Abs(d2_ID)==211 || TMath::Abs(d2_ID)==321 || TMath::Abs(d2_ID)==22 || TMath::Abs(d2_ID)==13 ) d2_Basic = true;
              if(d2_Basic == false){
                warning() << "I am not made to cascade further!!!" << endmsg;    
                //return StatusCode::RECOVERABLE;
              }
              
              // make the objects to be stored
              LHCb::Particle* Part_d2 = new LHCb::Particle();
              cont_Part->insert( Part_d2 );
              cont_P2PV->relate( Part_d2 , (LHCb::RecVertex*)cont_PV->containedObject( key ) );
              //
              std::vector<ContainedObject*> newObjects_d2;
              newObjects_d2.push_back(Part_d2);
              if( (d2_Basic==true) ){
                LHCb::Track* track_d2 = new LHCb::Track();
                newObjects_d2.push_back(track_d2);
                cont_Track->insert( track_d2 );
                //
                LHCb::ProtoParticle* proto_d2 = new LHCb::ProtoParticle();
                newObjects_d2.push_back(proto_d2);
                cont_Proto->insert( proto_d2 );
                //
                LHCb::RichPID* rich_d2 = new LHCb::RichPID();
                newObjects_d2.push_back(rich_d2);
                cont_RPID->insert( rich_d2 );
                //
                LHCb::MuonPID* muon_d2 = new LHCb::MuonPID();
                newObjects_d2.push_back(muon_d2);
                cont_MPID->insert( muon_d2 );
        
                //  
                Part_d1->addToDaughters(Part_d2);
              }

              vert_d1->addToOutgoingParticles(Part_d2);

              fillParticleInfo( newObjects_d2 , Obj_d2 , d2_Basic );
            } // second gen (loop)
          } // d1 basic (if)
        } // first gen (loop)
      } // motherBasic (if)
    } // candidate (loop)
  } // report exists (if)
	
  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  debug() << "End of algorithm execution" << endmsg;
  return StatusCode::SUCCESS;
}

void TeslaReportAlgo::fillParticleInfo(std::vector<ContainedObject*> vec_obj, const LHCb::HltObjectSummary* obj, bool isBasic){
/*
 * vector Key:
 * For basic particle:
 * 0: particle
 * 1: track
 * 2: protoparticle
 * 3: RICH PID
 * 4: Muon PID
 *
 * For non-basic:
 * 0: particle
 */

  // Set particle information
  const LHCb::HltObjectSummary::Info HLT_info = obj->numericalInfo();
  
  // What keys are present:
  std::vector<std::string> vec_keys = obj->numericalInfoKeys();
  debug() << "Available information" << endmsg;
  for(std::vector<std::string>::iterator it = vec_keys.begin(); it!=vec_keys.end(); it++) debug() << *it << " = " << HLT_info[(*it)] << endmsg;
  // 
  
  // PARTICLE *******************************************************
  // ID/Mass/Kinematics
  LHCb::Particle* part = (LHCb::Particle*)vec_obj[0];
  bool turbo=true;
  if(m_ReportVersion!=2) turbo=false;
  m_conv->ParticleObjectFromSummary(&HLT_info,part,turbo);
  debug() << "p = " << part->p() << endmsg;
  debug() << "p (par_mom) = " << part->p() << endmsg; // duplicate to make test match ref
  debug() << "pT (par_mom) = " << part->pt() << endmsg;
  debug() << "Particle measured mass = " << part->measuredMass() << endmsg;
  if(turbo==true) debug() << "Particle CL = " << part->confLevel() << endmsg;


  // What follows depends on whether or not the particle is basic or not
  if( isBasic == true ) {
    LHCb::Track* track = (LHCb::Track*)vec_obj[1];
    LHCb::ProtoParticle* proto = (LHCb::ProtoParticle*)vec_obj[2];
    LHCb::RichPID* rich = (LHCb::RichPID*)vec_obj[3];
    LHCb::MuonPID* muon = (LHCb::MuonPID*)vec_obj[4];

    for(SmartRefVector <LHCb::HltObjectSummary>::const_iterator it_basic = obj->substructure().begin();it_basic!=obj->substructure().end();++it_basic){

      const LHCb::HltObjectSummary* ObjBasic = it_basic->target();
      switch( ObjBasic->summarizedObjectCLID() )
      {
        case LHCb::CLID_Track:
          {
            const LHCb::HltObjectSummary::Info Track_info = ObjBasic->numericalInfo();

            // TODO: Stop the stub from being created in the first place.
            // this is here to stop the track being overridden.
            LHCb::Track* testTrack = new LHCb::Track();
            testTrack->setFlags( (unsigned int)Track_info["10#Track.flags"] );
            debug() << "***** TRACK TYPE TEST = " << testTrack->type() << endmsg;
            if( testTrack->type() == LHCb::Track::Types::Muon ) {
              delete testTrack;
              break;
            }
            else{
              delete testTrack;
            }
            //

            // What keys are present:
            std::vector<std::string> vec_keys_tr = ObjBasic->numericalInfoKeys();
            debug() << "Available information (track)" << endmsg;
            for(std::vector<std::string>::iterator it = vec_keys_tr.begin(); it!=vec_keys_tr.end(); it++) debug() << *it << " = " << Track_info[(*it)] << endmsg;
            // 
            // LHCb track members:
            // double   m_chi2PerDoF ##
            // int  m_nDoF ##
            // double   m_likelihood ##
            // double   m_ghostProbability ##
            // unsigned int   m_flags
            // std::vector< LHCb::LHCbID >  m_lhcbIDs
            // std::vector< LHCb::State * >   m_states
            // LHCb::TrackFitResult *   m_fitResult
            // ExtraInfo  m_extraInfo
            // SmartRefVector< LHCb::Track >  m_ancestors
            m_conv->TrackObjectFromSummary(&Track_info,track,turbo);
            debug() << "Track #chi^{2}/DoF = " << track->chi2PerDoF() << endmsg;
            track->setLhcbIDs( ObjBasic->lhcbIDs() );
            proto->setTrack( track );
            
            debug() << "Track details added" << endmsg; 
            break;
          } 

        case LHCb::CLID_ProtoParticle:
          {
            const LHCb::HltObjectSummary::Info Proto_info = ObjBasic->numericalInfo();
            // What keys are present:
            std::vector<std::string> vec_keys_pp = ObjBasic->numericalInfoKeys();
            for(std::vector<std::string>::iterator it = vec_keys_pp.begin(); it!=vec_keys_pp.end(); it++) debug() << *it << " = " << Proto_info[(*it)] << endmsg;
            debug() << "Available information (proto)" << endmsg;
            m_conv->ProtoParticleObjectFromSummary(&Proto_info,proto,turbo);
            // LHCb protoparticle members:
            // ExtraInfo   m_extraInfo (other members are a SmartRef to objects we create)
            // THIS IS A PLACE WHERE NEED TO KNOW WHAT PEOPLE NEED
            debug() << "Proto isPhoton = " << proto->info( 381,-1000) << endmsg;
            debug() << "ProtoParticle details added" << endmsg; 
            break;
          }

        case LHCb::CLID_RichPID:
          {
            const LHCb::HltObjectSummary::Info Rich_info = ObjBasic->numericalInfo();
            // What keys are present:
            std::vector<std::string> vec_keys_rich = ObjBasic->numericalInfoKeys();
            debug() << "Available information (rich)" << endmsg;
            for(std::vector<std::string>::iterator it = vec_keys_rich.begin(); it!=vec_keys_rich.end(); it++) debug() << *it << " = " << Rich_info[(*it)] << endmsg;
            m_conv->RichPIDObjectFromSummary(&Rich_info,rich,turbo);
            // 
            // LHCb RichPID  members:
            // unsigned int   m_pidResultCode
            // std::vector< float >   m_particleLLValues
            // SmartRef< LHCb::Track >  m_track (NOT INCLUDED)
            //
            proto->setRichPID( rich );
            rich->setTrack( track );
            proto->addInfo(LHCb::ProtoParticle::CombDLLe,rich->particleDeltaLL(Rich::ParticleIDType::Electron));
            proto->addInfo(LHCb::ProtoParticle::CombDLLmu,rich->particleDeltaLL(Rich::ParticleIDType::Muon));
            proto->addInfo(LHCb::ProtoParticle::CombDLLpi,rich->particleDeltaLL(Rich::ParticleIDType::Pion));
            proto->addInfo(LHCb::ProtoParticle::CombDLLk,rich->particleDeltaLL(Rich::ParticleIDType::Kaon));
            proto->addInfo(LHCb::ProtoParticle::CombDLLp,rich->particleDeltaLL(Rich::ParticleIDType::Proton));
            //
            debug() << "RichPID details added" << endmsg; 
            break;
          }

        case LHCb::CLID_MuonPID:
          {
            const LHCb::HltObjectSummary::Info Muon_info = ObjBasic->numericalInfo();
            // What keys are present:
            std::vector<std::string> vec_keys_muon = ObjBasic->numericalInfoKeys();
            debug() << "Available information (muon)" << endmsg;
            for(std::vector<std::string>::iterator it = vec_keys_muon.begin(); it!=vec_keys_muon.end(); it++) debug() << *it << " = " << Muon_info[(*it)] << endmsg;
            // 
            // LHCb MuonPID  members:
            // double   m_MuonLLMu
            // double   m_MuonLLBg
            // int  m_NShared
            // unsigned int   m_Status
            // SmartRef< LHCb::Track >  m_IDTrack (NOT INCLUDED)
            // SmartRef< LHCb::Track >  m_muonTrack (NOT INCLUDED)
            m_conv->MuonPIDObjectFromSummary(&Muon_info,muon,turbo);
            debug() << "Muon LLMu = " << muon->MuonLLMu() << endmsg;
            proto->setMuonPID( muon );
            debug() << "MuonPID details added" << endmsg; 
            break;
          }
      }
      // tie our pieces together
      debug() << "Starting to tie pieces together" << endmsg;
      part->setProto( proto );
      debug() << "Pieces tied together" << endmsg;
    }
  }
}

void TeslaReportAlgo::fillVertexInfo(LHCb::Vertex* vert, const LHCb::HltObjectSummary* obj){

  // What keys are present:
  const LHCb::HltObjectSummary::Info Vert_info = obj->numericalInfo();
  std::vector<std::string> vec_keys_v = obj->numericalInfoKeys();
  debug() << "Available information (vertex)" << endmsg;
  for(std::vector<std::string>::iterator it = vec_keys_v.begin(); it!=vec_keys_v.end(); it++) debug() << *it << " = " << Vert_info[(*it)] << endmsg;
  //
  m_conv->VertexObjectFromSummary(&Vert_info,vert,true);
  debug() << "Decay vertex chi2 = " << vert->chi2() << endmsg;

  return;
}
