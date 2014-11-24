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
: GaudiAlgorithm ( name , pSvcLocator ), m_dist(NULL), m_check(NULL)
{
  declareProperty( "TriggerLine" ,          m_inputName    = "Hlt2CharmHadD02HH_D02KK" );
  declareProperty( "OutputPrefix" ,         m_OutputPref   = "Tesla" );
  declareProperty( "ReportVersion" ,        m_ReportVersion= 2 );
  declareProperty( "PreSplit" ,             m_PreSplit= false );
  declareProperty( "PV" ,                   m_PV = "Online" );
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
  std::stringstream ss_PVLoc;
  //ss_PVLoc << m_OutputPref << "/PV3D";
  ss_PVLoc << "Rec/Vertex/Primary";
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
  LHCb::VertexBase::Container* cont_PV = new LHCb::VertexBase::Container() ;
  put( cont_PV , ss_PVLoc.str().c_str() );
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
    if( vtxRep ){
      for( LHCb::HltVertexReports::HltVertexReport::const_iterator iv=vtxRep->begin();iv!=vtxRep->end(); ++iv){
        const LHCb::VertexBase* v = *iv;
        LHCb::VertexBase* vnew = v->clone();
        debug() <<"Vertex chi2= " << v->chi2()<< ", ndf =" << v->nDoF()<< endmsg;
        debug() <<"Vertex key= " << v->key() << endmsg;
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
    debug() << substructure << endmsg;
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
      for ( LHCb::VertexBase::Container::const_iterator pv = cont_PV->begin() ; pv!=cont_PV->end() ; ++pv){
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
      cont_P2PV->relate( Part , (LHCb::VertexBase*)cont_PV->containedObject( key ) );

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
          cont_P2PV->relate( Part_d1 , (LHCb::VertexBase*)cont_PV->containedObject( key ) );

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
              cont_P2PV->relate( Part_d2 , (LHCb::VertexBase*)cont_PV->containedObject( key ) );
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
  LHCb::HltObjectSummary::Info HLT_info = obj->numericalInfo();
  
  // What keys are present:
  std::vector<std::string> vec_keys = obj->numericalInfoKeys();
  debug() << "Available information" << endmsg;
  for(std::vector<std::string>::iterator it = vec_keys.begin(); it!=vec_keys.end(); it++) debug() << *it << " = " << HLT_info[(*it)] << endmsg;
  // 
  
  // PARTICLE *******************************************************
  // ID/Mass/Kinematics
  LHCb::Particle* part = (LHCb::Particle*)vec_obj[0];
  const int pid = HLT_info["0#Particle.particleID.pid"];
  const double mm = HLT_info["1#Particle.measuredMass"];
  const double slopex = HLT_info["5#Particle.slopes.x"];
  const double slopey = HLT_info["6#Particle.slopes.y"];
  const double p = 1.0/HLT_info["7#Particle.1/p"]; // end of usual SelReports info
  double cl;
  double merr=0.0;
  double momCov00=0.0; double momCov11=0.0; double momCov22=0.0; double momCov33=0.0; double momCov10=0.0; double momCov20=0.0; double momCov21=0.0; double momCov30=0.0; double momCov31=0.0; double momCov32=0.0;
  double posmomCov00=0.0; double posmomCov11=0.0; double posmomCov22=0.0; double posmomCov10=0.0; double posmomCov01=0.0; double posmomCov20=0.0; double posmomCov02=0.0; double posmomCov21=0.0; double posmomCov12=0.0; double posmomCov30=0.0; double posmomCov31=0.0; double posmomCov32=0.0;
  double posCov00=0.0; double posCov11=0.0; double posCov22=0.0; double posCov10=0.0; double posCov20=0.0; double posCov21=0.0;
  if( m_ReportVersion == 2) {
    cl = HLT_info["8#Particle.conflevel"];
    merr = HLT_info["9#Particle.massErr"];
    momCov00 = HLT_info["10#Particle.momCov00"];
    momCov11 = HLT_info["11#Particle.momCov11"];
    momCov22 = HLT_info["12#Particle.momCov22"];
    momCov33 = HLT_info["13#Particle.momCov33"];
    momCov10 = HLT_info["14#Particle.momCov10"];
    momCov20 = HLT_info["15#Particle.momCov20"];
    momCov21 = HLT_info["16#Particle.momCov21"];
    momCov30 = HLT_info["17#Particle.momCov30"];
    momCov31 = HLT_info["18#Particle.momCov31"];
    momCov32 = HLT_info["19#Particle.momCov32"];
    posmomCov00 = HLT_info["20#Particle.posmomCov00"];
    posmomCov11 = HLT_info["21#Particle.posmomCov11"];
    posmomCov22 = HLT_info["22#Particle.posmomCov22"];
    posmomCov10 = HLT_info["23#Particle.posmomCov10"];
    posmomCov10 = HLT_info["24#Particle.posmomCov01"];
    posmomCov20 = HLT_info["25#Particle.posmomCov20"];
    posmomCov02 = HLT_info["26#Particle.posmomCov02"];
    posmomCov21 = HLT_info["27#Particle.posmomCov21"];
    posmomCov12 = HLT_info["28#Particle.posmomCov12"];
    posmomCov30 = HLT_info["29#Particle.posmomCov30"];
    posmomCov31 = HLT_info["30#Particle.posmomCov31"];
    posmomCov32 = HLT_info["31#Particle.posmomCov32"];
    posCov00 = HLT_info["32#Particle.posmomCov00"];
    posCov11 = HLT_info["33#Particle.posmomCov11"];
    posCov22 = HLT_info["34#Particle.posmomCov22"];
    posCov10 = HLT_info["35#Particle.posmomCov10"];
    posCov20 = HLT_info["36#Particle.posmomCov20"];
    posCov21 = HLT_info["37#Particle.posmomCov21"];
  }
  //
  const double slopez = 1/sqrt(slopex*slopex + slopey*slopey + 1.0);
  const double pz = slopez*p;
  const double px = slopex*pz;
  const double py = slopey*pz;
  const double pe = sqrt(p*p+mm*mm);
  debug() << "p = " << p << endmsg;
  //
  LHCb::ParticleID id(pid);
  //
  Gaudi::LorentzVector part_mom(px,py,pz,pe);
  //
  debug() << "p (par_mom) = " << part_mom.P() << endmsg;
  debug() << "pT (par_mom) = " << part_mom.Pt() << endmsg;
  //
  if( m_ReportVersion != 3){
    part->setParticleID(id);
    part->setMeasuredMass(mm);
    part->setMeasuredMassErr(merr);
    part->setMomentum(part_mom);
    Gaudi::SymMatrix4x4 & momCov = *(const_cast<Gaudi::SymMatrix4x4*>(&part->momCovMatrix()));
    momCov(0,0)=momCov00;
    momCov(1,1)=momCov11;
    momCov(2,2)=momCov22;
    momCov(3,3)=momCov33;
    momCov(1,0)=momCov10;
    momCov(2,0)=momCov20;
    momCov(2,1)=momCov21;
    momCov(3,0)=momCov30;
    momCov(3,1)=momCov31;
    momCov(3,2)=momCov32;
    Gaudi::Matrix4x3 & posMomCov = *(const_cast<Gaudi::Matrix4x3*>(&part->posMomCovMatrix()));
    posMomCov(0,0)=posmomCov00;
    posMomCov(1,1)=posmomCov11;
    posMomCov(2,2)=posmomCov22;
    posMomCov(1,0)=posmomCov10;
    posMomCov(0,1)=posmomCov01;
    posMomCov(2,0)=posmomCov20;
    posMomCov(0,2)=posmomCov02;
    posMomCov(2,1)=posmomCov21;
    posMomCov(1,2)=posmomCov12;
    posMomCov(3,0)=posmomCov30;
    posMomCov(3,1)=posmomCov31;
    posMomCov(3,2)=posmomCov32;
    Gaudi::SymMatrix3x3 & posCov = *(const_cast<Gaudi::SymMatrix3x3*>(&part->posCovMatrix()));
    posCov(0,0)=posCov00;
    posCov(1,1)=posCov11;
    posCov(2,2)=posCov22;
    posCov(1,0)=posCov10;
    posCov(2,0)=posCov20;
    posCov(2,1)=posCov21;
  }
  debug() << "Particle measured mass = " << part->measuredMass() << endmsg;
  if( m_ReportVersion == 2) debug() << "Particle CL = " << cl << endmsg;
  // members todo list:
  // m_measuredMassErr
  // m_referencePoint
  // m_momCovMatrix
  // m_posCovMatrix
  // m_posMomCovMatrix
  
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
          LHCb::HltObjectSummary::Info Track_info = ObjBasic->numericalInfo();
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
          const double TrackZ = Track_info["0#Track.firstState.z"];
          const double TrackX = Track_info["1#Track.firstState.x"];
          const double TrackY = Track_info["2#Track.firstState.y"];
          const double TrackTX = Track_info["3#Track.firstState.tx"];
          const double TrackTY = Track_info["4#Track.firstState.ty"];
          const double TrackQOP = Track_info["5#Track.firstState.qOverP"];
          const double TrackChi2PerDoF = Track_info["6#Track.chi2PerDoF"]; debug() << "Track #chi^{2}/DoF = " << TrackChi2PerDoF << endmsg; // end of basic SelReports info
          //
          double TrackDoF;
          double TrackLikelihood, TrackGhostProb;
          double flags;
          double lTrackZ, lTrackX, lTrackY, lTrackTX, lTrackTY, lTrackQOP;
          if( m_ReportVersion == 2 ){
            TrackDoF = Track_info["7#Track.nDoF"];
            TrackLikelihood = Track_info["8#Track.Likelihood"];
            TrackGhostProb = Track_info["9#Track.GhostProb"];
            flags = Track_info["10#Track.flags"];
            lTrackZ = Track_info["11#Track.lastState.z"];
            lTrackX = Track_info["12#Track.lastState.x"];
            lTrackY = Track_info["13#Track.lastState.y"];
            lTrackTX = Track_info["14#Track.lastState.tx"];
            lTrackTY = Track_info["15#Track.lastState.ty"];
            lTrackQOP = Track_info["16#Track.lastState.qOverP"];
          }
          //
          if( m_ReportVersion == 2 ){
            debug() << "Track extra for Rep v2, adding last state" << endmsg;
            track->setNDoF( (int)TrackDoF );
            track->setGhostProbability( TrackGhostProb );
            track->setLikelihood( TrackLikelihood );
            LHCb::State* last = new LHCb::State();
            last->setState(lTrackX,lTrackY,lTrackZ,lTrackTX,lTrackTY,lTrackQOP);
            track->addToStates(*last);
            track->setFlags( (unsigned int)flags );
            LHCb::State* first = new LHCb::State();
            first->setState(TrackX,TrackY,TrackZ,TrackTX,TrackTY,TrackQOP);
            track->addToStates(*first);
            track->setLhcbIDs( ObjBasic->lhcbIDs() );
          }
          track->setChi2PerDoF( TrackChi2PerDoF );
          const LHCb::Track* test1 = proto->track();
          if( !test1 ) proto->setTrack( track );
          debug() << "Track details added" << endmsg; 
          break;
        } 
      
        case LHCb::CLID_ProtoParticle:
        {
          if( m_ReportVersion == 2 ){
            LHCb::HltObjectSummary::Info Proto_info = ObjBasic->numericalInfo();
            // What keys are present:
            std::vector<std::string> vec_keys_pp = ObjBasic->numericalInfoKeys();
            debug() << "Available information (proto)" << endmsg;
            for(std::vector<std::string>::iterator it = vec_keys_pp.begin(); it!=vec_keys_pp.end(); it++) debug() << *it << " = " << Proto_info[(*it)] << endmsg;
            // 
            // LHCb protoparticle members:
            // ExtraInfo   m_extraInfo (other members are a SmartRef to objects we create)
            // THIS IS A PLACE WHERE NEED TO KNOW WHAT PEOPLE NEED
            const double isPhoton = Proto_info["0#Proto.extraInfo.IsPhoton"];
            //
            proto->addInfo( 381, (int)isPhoton ); debug() << "Proto isPhoton = " << isPhoton << endmsg;
          }
          debug() << "ProtoParticle details added" << endmsg; 
          break;
        }

        case LHCb::CLID_RichPID:
        {
          if( m_ReportVersion == 2 ){
            LHCb::HltObjectSummary::Info Rich_info = ObjBasic->numericalInfo();
            // What keys are present:
            std::vector<std::string> vec_keys_rich = ObjBasic->numericalInfoKeys();
            debug() << "Available information (rich)" << endmsg;
            for(std::vector<std::string>::iterator it = vec_keys_rich.begin(); it!=vec_keys_rich.end(); it++) debug() << *it << " = " << Rich_info[(*it)] << endmsg;
            // 
            // LHCb RichPID  members:
            // unsigned int   m_pidResultCode
            // std::vector< float >   m_particleLLValues
            // SmartRef< LHCb::Track >  m_track (NOT INCLUDED)
            const double pidResultCode = Rich_info["0#Rich.pidResultCode"];
            const double DLLe = Rich_info["1#Rich.DLLe"];
            const double DLLmu = Rich_info["2#Rich.DLLmu"];
            const double DLLpi = Rich_info["3#Rich.DLLpi"];
            const double DLLK = Rich_info["4#Rich.DLLK"];
            const double DLLp = Rich_info["5#Rich.DLLp"];
            //
            rich->setPidResultCode( (int)pidResultCode );
            rich->setParticleDeltaLL( Rich::ParticleIDType::Electron, DLLe );
            rich->setParticleDeltaLL( Rich::ParticleIDType::Muon, DLLmu );
            rich->setParticleDeltaLL( Rich::ParticleIDType::Pion, DLLpi );
            rich->setParticleDeltaLL( Rich::ParticleIDType::Kaon, DLLK );
            rich->setParticleDeltaLL( Rich::ParticleIDType::Proton, DLLp );
            //
            proto->setRichPID( rich );
            proto->addInfo(LHCb::ProtoParticle::CombDLLe,DLLe);
            proto->addInfo(LHCb::ProtoParticle::CombDLLmu,DLLmu);
            proto->addInfo(LHCb::ProtoParticle::CombDLLpi,DLLpi);
            proto->addInfo(LHCb::ProtoParticle::CombDLLk,DLLK);
            proto->addInfo(LHCb::ProtoParticle::CombDLLp,DLLp);
            //
            debug() << "RICH DLLK = " << DLLK << endmsg;
          }
          debug() << "RichPID details added" << endmsg; 
          break;
        }

        case LHCb::CLID_MuonPID:
        {
          if( m_ReportVersion == 2 ){
            LHCb::HltObjectSummary::Info Muon_info = ObjBasic->numericalInfo();
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
            const double MuonLLMu = Muon_info["0#Muon.MuonLLMu"];
            const double MuonLLBg = Muon_info["1#Muon.MuonLLBg"];
            const double MuonNShared = Muon_info["2#Muon.NShared"];
            const double MuonStatus = Muon_info["3#Muon.Status"];
            const double IsMuon = Muon_info["4#Muon.IsMuon"];
            const double IsMuonLoose = Muon_info["5#Muon.IsMuonLoose"];
            const double IsMuonTight = Muon_info["6#Muon.IsMuonTight"];
            //
            muon->setMuonLLMu( MuonLLMu );
            muon->setMuonLLBg( MuonLLBg );
            muon->setNShared( (int)MuonNShared );
            muon->setStatus( (int)MuonStatus );
            muon->setIsMuon( (int)IsMuon );
            muon->setIsMuonLoose( (int)IsMuonLoose );
            muon->setIsMuonTight( (int)IsMuonTight );
            debug() << "Muon LLMu = " << MuonLLMu << endmsg;
          }
          debug() << "MuonPID details added" << endmsg; 
          break;
          proto->setMuonPID( muon );
        }
      }
    }
    debug() << "Starting to tie pieces together" << endmsg;
    // tie our pieces together
    part->setProto( proto );
    debug() << "Pieces tied together" << endmsg;
  }
}

void TeslaReportAlgo::fillVertexInfo(LHCb::Vertex* vert, const LHCb::HltObjectSummary* obj){

  // What keys are present:
  LHCb::HltObjectSummary::Info Vert_info = obj->numericalInfo();
  std::vector<std::string> vec_keys_v = obj->numericalInfoKeys();
  debug() << "Available information (vertex)" << endmsg;
  for(std::vector<std::string>::iterator it = vec_keys_v.begin(); it!=vec_keys_v.end(); it++) debug() << *it << " = " << Vert_info[(*it)] << endmsg;
  // 
  if( m_ReportVersion == 2 ){
    LHCb::HltObjectSummary::Info Vert_info = obj->numericalInfo();
    const double VertChi2 = Vert_info["0#Vertex.chi2"];
    const double VertNDoF = Vert_info["1#Vertex.ndf"];
    const double Vert_x   = Vert_info["2#Vertex.position.x"];
    const double Vert_y   = Vert_info["3#Vertex.position.y"];
    const double Vert_z   = Vert_info["4#Vertex.position.z"];
    //const double Vert_tech= Vert_info["5#Vertex.technique"];
    const double VertCov00= Vert_info["6#Vertex.cov00"];
    const double VertCov11= Vert_info["7#Vertex.cov11"];
    const double VertCov22= Vert_info["8#Vertex.cov22"];
    const double VertCov10= Vert_info["9#Vertex.cov10"];
    const double VertCov20= Vert_info["10#Vertex.cov20"];
    const double VertCov21= Vert_info["11#Vertex.cov21"];
    //
    debug() << "Decay vertex chi2 = " << VertChi2 << endmsg;
    vert->setChi2AndDoF( VertChi2, (int)VertNDoF );
    vert->setPosition( Gaudi::XYZPoint( Vert_x, Vert_y, Vert_z ) );
    Gaudi::SymMatrix3x3 & cov = *(const_cast<Gaudi::SymMatrix3x3*>(&vert->covMatrix()));
    cov(0,0) = VertCov00;
    cov(1,1) = VertCov11;
    cov(2,2) = VertCov22;
    cov(1,0) = VertCov10;
    cov(2,0) = VertCov20;
    cov(2,1) = VertCov21;
    //LHCb::Vertex::CreationMethod tech = static_cast<LHCb::Vertex::CreationMethod>( Vert_tech );
    //vert->setTechnique( tech );
  }

  return;
}
