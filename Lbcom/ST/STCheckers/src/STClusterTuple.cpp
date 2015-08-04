// Include files 

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiAlg/GaudiTupleAlg.h"

// event
#include "Event/STCluster.h"
#include "Event/MCHit.h"
#include "Event/ODIN.h"

// local
#include "STClusterTuple.h"
#include <boost/assign/std/vector.hpp>

//other
#include <iostream>

using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : STClusterTuple
//
// 2015-06-05 : Vincenzo Battista
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STClusterTuple )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STClusterTuple::STClusterTuple( const string& name,
                                        ISvcLocator* pSvcLocator)
: ST::TupleAlgBase ( name , pSvcLocator ),
  m_nEvent(0)

{
  this->declareProperty("SpillVector", m_spillVector);

  //Input clusters
  declareSTConfigProperty("inputData", m_clusterLocation , STClusterLocation::TTClusters);

  //Associator table
  declareSTConfigProperty("asctLocation", m_asctLocation , m_clusterLocation + "2MCHits");

  //Maximum size of the tuple arrays
  declareProperty("MaxEntries", m_nMax=20000);

  //Dump MCVertex info in the tuple
  declareProperty("MCVertexInfo", m_MCVertexInfo=true);

  //Create useful arrays for counters
  using namespace boost::assign;
  m_spillVector += "/PrevPrev/", "/Prev/", "/", "/Next/", "/NextNext/";
  m_Flags += "Yes", "No";

  m_hitLocation = MCHitLocation::TT;

  //Switch between TT and IT
  addToFlipList(&m_hitLocation);
  addToFlipList(&m_clusterLocation);
  addToFlipList(&m_asctLocation);

  setForcedInit();
}
//=============================================================================
// Destructor
//=============================================================================
STClusterTuple::~STClusterTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STClusterTuple::initialize() {
  StatusCode sc = ST::TupleAlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  //Set hits, clusters and associator table location
  vector<string>::const_iterator iSpillName = m_spillVector.begin() ;
  
  for (; iSpillName != m_spillVector.end(); ++iSpillName)
  {
    string mcHitPath = "/Event"+(*iSpillName)+m_hitLocation;
    info () << "Adding " << mcHitPath << " to spill names list" << endmsg;
    m_spillNames.push_back(mcHitPath);
  }

  debug() << "Hits Location: " << m_hitLocation << endmsg;
  debug() << "Clusters Location: " << m_clusterLocation << endmsg;
  debug() << "Associator Table Location: " << m_asctLocation << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STClusterTuple::execute() {

  debug() << "==> Execute" << endmsg;
  
  //Set event counter
  ++m_nEvent;
  
  //Initialize counters
  m_countClus = 0;
  m_clusCharge = 0.0;

  unsigned int iSpill = 0;
  for( ; iSpill < m_spillVector.size(); ++iSpill)
  {
    m_countHits[m_spillVector[iSpill]] = 0;
  }
  m_countHits["WithClusters"] = 0;
  m_countHits["WithNoClusters"] = 0;

  unsigned int iFlag = 0;
  for( ; iFlag < m_Flags.size(); ++iFlag)
  {
    m_countFlags[m_Flags[iFlag]] = 0;
  }

  //Retrieve event and run numbers
  const ODIN* odin = get<ODIN> ( ODINLocation::Default );

  //Get clusters
  const STClusters* clusterCont = getIfExists<STClusters>(m_clusterLocation);
  
  //Create MCHits-Clusters associator table
  AsctTool associator(evtSvc(), m_asctLocation);
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  //Output tree
  Tuple tree = nTuple("ClusterTuple", "Info about clusters and MC hits");
  
  vector<unsigned int> nMCHits;
  vector<double> TOF;
  vector<double> Energy;
  vector<double> Px;
  vector<double> Py;
  vector<double> Pz;
  vector<double> X;
  vector<double> Y;
  vector<double> Z;
  vector<double> entry_X;
  vector<double> entry_Y;
  vector<double> entry_Z;
  vector<double> exit_X;
  vector<double> exit_Y;
  vector<double> exit_Z;
  vector<double> pathlength;
  vector<double> dxdz;
  vector<double> dydz;
  vector<double> ID;
  vector<double> mothID;
  vector<bool> DeltaRay;
  vector<bool> PairProduction;
  vector<bool> PhotoElectric;
  vector<bool> Compton;
  vector<bool> Annihilation;
  vector<bool> ppCollision;
  vector<bool> DecayVertex;
  vector<bool> OscillatedAndDecay;
  vector<bool> StringFragmentation;
  vector<bool> HadronicInteraction;
  vector<bool> Bremsstrahlung;
  vector<bool> RICHPhotoElectric;
  vector<bool> Cerenkov;
  vector<bool> RichHpdBackScat;
  vector<bool> GenericInteraction;
  vector<bool> LHCHalo;
  vector<bool> MuonBackground;
  vector<bool> MuonBackgroundFlat;
  vector<bool> MuonBackgroundSpillover;
  vector<bool> WorldLimit;
  vector<bool> KinematicLimit;
  vector<bool> Unknown;
  
  vector<double> clusterCharge;
  vector<unsigned int> clusterSize;
  vector<bool> spilloverFlag;
  vector<double> interStripFraction;
  vector<unsigned int> channelID;
  vector<unsigned int> strip;
  vector<unsigned int> station;
  vector<unsigned int> layer;
  vector<unsigned int> detRegion;
  vector<unsigned int> sector;
  vector<double> neighbourSum;

  //Get MCHits
  for (unsigned int iSpill = 0; iSpill < m_spillNames.size(); ++iSpill)
  {
    SmartDataPtr<MCHits> hitsCont( eventSvc(), m_spillNames[iSpill] );
    if (!hitsCont)
    {
      debug() << "Hits missing in the spill: " << m_spillNames[iSpill] << ", event: " << m_nEvent << endmsg;
    }
    else
    {
      MCHits::const_iterator iterHit = hitsCont->begin();
      for( ; iterHit != hitsCont->end(); ++iterHit)
      {
        ++m_countHits[m_spillVector[iSpill]];
      } //iterHit
    } //else (!hitsCont)
    nMCHits.push_back(m_countHits[m_spillVector[iSpill]]);
  } //iSpill
  
  //Loop over clusters
  if(!clusterCont) debug() << "Clusters missing in the event: " << m_nEvent << endmsg;
  else{    
    m_countClus = clusterCont->size();
    if(m_countClus > m_nMax-1) warning() << "Number of clusters greater than the limit of " << m_nMax << endmsg;

    unsigned int iClus = 0;
    STClusters::const_iterator iterClus = clusterCont->begin() ;
    for( ; iterClus != clusterCont->end(); ++iterClus, ++iClus )
    {
      const STCluster* aCluster = *iterClus;
      
      //Retrieve MCHits from cluster
      Range range = aTable->relations(*iterClus);
      if (range.empty())
      {
        ++m_countHits["WithNoClusters"];
        debug() << "No MCHits for the current cluster" << endmsg;
        
        if(iClus < m_nMax)
        {
          TOF.push_back(-9999.0);
          Energy.push_back(-9999);
          Px.push_back(-9999.0);
          Py.push_back(-9999.0);
          Pz.push_back(-9999.0);
          X.push_back(-9999.0);
          Y.push_back(-9999.0);
          Z.push_back(-9999.0);
          entry_X.push_back(-9999.0);
          entry_Y.push_back(-9999.0);
          entry_Z.push_back(-9999.0);
          exit_X.push_back(-9999.0);
          exit_Y.push_back(-9999.0);
          exit_Z.push_back(-9999.0);
          pathlength.push_back(-9999.0);
          dxdz.push_back(-9999.0);
          dydz.push_back(-9999.0);
          ID.push_back(-9999.0);
          mothID.push_back(-9999.0);
          DeltaRay.push_back(0);
          PairProduction.push_back(0);
          PhotoElectric.push_back(0);
          Compton.push_back(0);
          Annihilation.push_back(0);
          ppCollision.push_back(0);
          DecayVertex.push_back(0);
          OscillatedAndDecay.push_back(0);
          StringFragmentation.push_back(0);
          HadronicInteraction.push_back(0);
          Bremsstrahlung.push_back(0);
          RICHPhotoElectric.push_back(0);
          Cerenkov.push_back(0);
          RichHpdBackScat.push_back(0);
          GenericInteraction.push_back(0);
          LHCHalo.push_back(0);
          MuonBackground.push_back(0);
          MuonBackgroundFlat.push_back(0);
          MuonBackgroundSpillover.push_back(0);
          WorldLimit.push_back(0);
          KinematicLimit.push_back(0);
          Unknown.push_back(1);
        } //if (iClus)
        
      } //if (range.empty())
      else
      { 
        ++m_countHits["WithClusters"];
        const MCHit* aHit = ((*range.begin()).to());
        
        if(iClus < m_nMax)
        {
          TOF.push_back(aHit->time());
          Energy.push_back(aHit->mcParticle()->momentum().E());
          Px.push_back(aHit->mcParticle()->momentum().Px());
          Py.push_back(aHit->mcParticle()->momentum().Py());
          Pz.push_back(aHit->mcParticle()->momentum().Pz());
          X.push_back(aHit->mcParticle()->originVertex()->position4vector().X());
          Y.push_back(aHit->mcParticle()->originVertex()->position4vector().Y());
          Z.push_back(aHit->mcParticle()->originVertex()->position4vector().Z());
          entry_X.push_back(aHit->entry().X());
          entry_Y.push_back(aHit->entry().Y());
          entry_Z.push_back(aHit->entry().Z());
          exit_X.push_back(aHit->exit().X());
          exit_Y.push_back(aHit->exit().Y());
          exit_Z.push_back(aHit->exit().Z());
          pathlength.push_back(aHit->pathLength());
          dxdz.push_back(aHit->dxdz());
          dydz.push_back(aHit->dydz());
          ID.push_back(aHit->mcParticle()->particleID().pid());
          if(aHit->mcParticle()->mother())
          {
            mothID.push_back(aHit->mcParticle()->mother()->particleID().pid());
          }
          else mothID.push_back(-1.0);
          DeltaRay.push_back(Origin(aHit,LHCb::MCVertex::DeltaRay));
          PairProduction.push_back(Origin(aHit,LHCb::MCVertex::PairProduction));
          PhotoElectric.push_back(Origin(aHit,LHCb::MCVertex::PhotoElectric));
          Compton.push_back(Origin(aHit,LHCb::MCVertex::Compton));
          Annihilation.push_back(Origin(aHit,LHCb::MCVertex::Annihilation));
          ppCollision.push_back(Origin(aHit,LHCb::MCVertex::ppCollision));
          DecayVertex.push_back(Origin(aHit,LHCb::MCVertex::DecayVertex));
          OscillatedAndDecay.push_back(Origin(aHit,LHCb::MCVertex::OscillatedAndDecay));
          StringFragmentation.push_back(Origin(aHit,LHCb::MCVertex::StringFragmentation));
          HadronicInteraction.push_back(Origin(aHit,LHCb::MCVertex::HadronicInteraction));
          Bremsstrahlung.push_back(Origin(aHit,LHCb::MCVertex::Bremsstrahlung));
          RICHPhotoElectric.push_back(Origin(aHit,LHCb::MCVertex::RICHPhotoElectric));
          Cerenkov.push_back(Origin(aHit,LHCb::MCVertex::Cerenkov));
          RichHpdBackScat.push_back(Origin(aHit,LHCb::MCVertex::RichHpdBackScat));
          GenericInteraction.push_back(Origin(aHit,LHCb::MCVertex::GenericInteraction));
          LHCHalo.push_back(Origin(aHit,LHCb::MCVertex::LHCHalo));
          MuonBackground.push_back(Origin(aHit,LHCb::MCVertex::MuonBackground));
          MuonBackgroundFlat.push_back(Origin(aHit,LHCb::MCVertex::MuonBackgroundFlat));
          MuonBackgroundSpillover.push_back(Origin(aHit,LHCb::MCVertex::MuonBackgroundSpillover));
          WorldLimit.push_back(Origin(aHit,LHCb::MCVertex::WorldLimit));
          KinematicLimit.push_back(Origin(aHit,LHCb::MCVertex::KinematicLimit));
          Unknown.push_back(Origin(aHit,LHCb::MCVertex::Unknown));

          //MCHits debugging
          debug() << "TOF [ns]: " << aHit->time() << endmsg;
          debug() << "Energy [MeV]: " << aHit->mcParticle()->momentum().E() << endmsg;
          debug() << "Px [MeV/c]: " << aHit->mcParticle()->momentum().Px() << endmsg;
          debug() << "Py [MeV/c]: " << aHit->mcParticle()->momentum().Py() << endmsg;
          debug() << "Pz [MeV/c]: " << aHit->mcParticle()->momentum().Pz() << endmsg;
          debug() << "Vertex (X) [mm]: " << aHit->mcParticle()->originVertex()->position4vector().X() << endmsg;
          debug() << "Vertex (Y) [mm]: " << aHit->mcParticle()->originVertex()->position4vector().Y() << endmsg;
          debug() << "Vertex (Z) [mm]: " << aHit->mcParticle()->originVertex()->position4vector().Z() << endmsg;
          debug() << "Entry point (X) [mm]: " << aHit->entry().X() << endmsg;
          debug() << "Entry point (Y) [mm]: " << aHit->entry().Y() << endmsg;
          debug() << "Entry point (Z) [mm]: " << aHit->entry().Z() << endmsg;
          debug() << "Exit point (X) [mm]: " << aHit->exit().X() << endmsg;
          debug() << "Exit point (Y) [mm]: " << aHit->exit().Y() << endmsg;
          debug() << "Exit point (Z) [mm]: " << aHit->exit().Z() << endmsg;
          debug() << "Path length in the Si [mm]: " << aHit->pathLength() << endmsg;
          debug() << "Slope dx/dz: " << aHit->dxdz() << endmsg;
          debug() << "Slope dy/dz: " << aHit->dydz() << endmsg;
          debug() << "ID: " << aHit->mcParticle()->particleID().pid() << endmsg;
          if(aHit->mcParticle()->mother())
          {
            debug() << "Mother ID: " << aHit->mcParticle()->mother()->particleID().pid() << endmsg;
          }
          else info() << "Mother ID: no mother" << endmsg;

        } //if (iClus)
        
      } //else

      //Retrieve info for debugging
      m_clusCharge += aCluster->totalCharge();
      bool flag = aCluster->highThreshold();
      if (flag == true) ++m_countFlags["Yes"];
      else ++m_countFlags["No"];

      //Retrieve info for the tuple
      if(iClus < m_nMax)
      {
        clusterCharge.push_back(aCluster->totalCharge());
        clusterSize.push_back(aCluster->size());
        spilloverFlag.push_back(aCluster->highThreshold());
        interStripFraction.push_back(aCluster->interStripFraction());
        channelID.push_back(aCluster->channelID());
        strip.push_back(aCluster->strip());
        station.push_back(aCluster->station());
        layer.push_back(aCluster->layer());
        detRegion.push_back(aCluster->detRegion());
        sector.push_back(aCluster->sector());
        neighbourSum.push_back(aCluster->neighbourSum());

        //Cluster debugging
        debug() << "Cluster charge: " << aCluster->totalCharge() << endmsg;
        debug() << "Cluster size: " << aCluster->size() << endmsg;
        debug() << "Spillover threshold: " << aCluster->highThreshold() << endmsg;
        debug() << "Inter-strip fraction: " << aCluster->interStripFraction() << endmsg;
        debug() << "Channel ID: " << aCluster->channelID() << endmsg;
        debug() << "Strip: " << aCluster->strip() << endmsg;
        debug() << "Station: " << aCluster->station() << endmsg;
        debug() << "Layer: " << aCluster->layer() << endmsg;
        debug() << "Detector Region: " << aCluster->detRegion() << endmsg;
        debug() << "Sector: " << aCluster->sector() << endmsg;
        debug() << "Sum of neighbours: " << aCluster->neighbourSum() << endmsg;

      } // endif
    } //iterClus
  } //end of getIfExist condition

  //Fill the tuple
  tree->column( "run", odin->runNumber());
  tree->column( "event", static_cast<unsigned int>(odin->eventNumber()));
  tree->farray( "nMCHits", nMCHits, "spill", 5);
  tree->farray( "TOF", TOF, "nClusters", m_nMax);
  tree->farray( "Energy", Energy, "nClusters", m_nMax);
  tree->farray( "Px", Px, "nClusters", m_nMax);
  tree->farray( "Py", Py, "nClusters", m_nMax);
  tree->farray( "Pz", Pz, "nClusters", m_nMax);
  tree->farray( "X", X, "nClusters", m_nMax);
  tree->farray( "Y", Y, "nClusters", m_nMax);
  tree->farray( "Z", Z, "nClusters", m_nMax);
  tree->farray( "entry_X", entry_X, "nClusters", m_nMax);
  tree->farray( "entry_Y", entry_Y, "nClusters", m_nMax);
  tree->farray( "entry_Z", entry_Z, "nClusters", m_nMax);
  tree->farray( "exit_X", exit_X, "nClusters", m_nMax);
  tree->farray( "exit_Y", exit_Y, "nClusters", m_nMax);
  tree->farray( "exit_Z", exit_Z, "nClusters", m_nMax);
  tree->farray( "pathlength", pathlength, "nClusters", m_nMax);
  tree->farray( "dxdz", dxdz, "nClusters", m_nMax);
  tree->farray( "dydz", dydz, "nClusters", m_nMax);
  tree->farray( "ID", ID, "nClusters", m_nMax);
  tree->farray( "mothID", mothID, "nClusters", m_nMax);
  if(m_MCVertexInfo)
  {
    tree->farray( "DeltaRay", DeltaRay, "nClusters", m_nMax);
    tree->farray( "PairProduction", PairProduction, "nClusters", m_nMax);
    tree->farray( "PhotoElectric", PhotoElectric, "nClusters", m_nMax);
    tree->farray( "Compton", Compton, "nClusters", m_nMax);
    tree->farray( "Annihilation", Annihilation, "nClusters", m_nMax);
    tree->farray( "ppCollision", ppCollision, "nClusters", m_nMax);
    tree->farray( "DecayVertex", DecayVertex, "nClusters", m_nMax);
    tree->farray( "OscillatedAndDecay", OscillatedAndDecay, "nClusters", m_nMax);
    tree->farray( "StringFragmentation", StringFragmentation, "nClusters", m_nMax);
    tree->farray( "HadronicInteraction", HadronicInteraction, "nClusters", m_nMax);
    tree->farray( "Bremsstrahlung", Bremsstrahlung, "nClusters", m_nMax);
    tree->farray( "RICHPhotoElectric", RICHPhotoElectric, "nClusters", m_nMax);
    tree->farray( "Cerenkov", Cerenkov, "nClusters", m_nMax);
    tree->farray( "RichHpdBackScat", RichHpdBackScat, "nClusters", m_nMax);
    tree->farray( "GenericInteraction", GenericInteraction, "nClusters", m_nMax);
    tree->farray( "LHCHalo", LHCHalo, "nClusters", m_nMax);
    tree->farray( "MuonBackground", MuonBackground, "nClusters", m_nMax);
    tree->farray( "MuonBackgroundFlat", MuonBackgroundFlat, "nClusters", m_nMax);
    tree->farray( "MuonBackgroundSpillover", MuonBackgroundSpillover, "nClusters", m_nMax);
    tree->farray( "WorldLimit", WorldLimit, "nClusters", m_nMax);
    tree->farray( "KinematicLimit", KinematicLimit, "nClusters", m_nMax);
    tree->farray( "Unknown", Unknown, "nClusters", m_nMax);
  }
  tree->farray( "clusterCharge", clusterCharge, "nClusters", m_nMax);
  tree->farray( "clusterSize", clusterSize, "nClusters", m_nMax);
  tree->farray( "spilloverFlag", spilloverFlag, "nClusters", m_nMax);
  tree->farray( "interStripFraction", interStripFraction, "nClusters", m_nMax);
  tree->farray( "channelID", channelID, "nClusters", m_nMax);
  tree->farray( "strip", strip, "nClusters", m_nMax);
  tree->farray( "station", station, "nClusters", m_nMax);
  tree->farray( "layer", layer, "nClusters", m_nMax);
  tree->farray( "detRegion", detRegion, "nClusters", m_nMax);
  tree->farray( "sector", sector, "nClusters", m_nMax);
  tree->farray( "neighbourSum", neighbourSum, "nClusters", m_nMax);

  if( !(tree->write()) ) err() << "Cannot fill the tuple!" << endmsg;
  
  //Debugging (summary)
  debug() << "Summary:" << endmsg;
  debug() << "..MC hits:" << endmsg;
  debug() << "...-50ns " << m_countHits["/PrevPrev/"] << endmsg;
  debug() << "...-25ns " << m_countHits["/Prev/"] << endmsg;
  debug() << "...0ns " << m_countHits["/"] << endmsg;
  debug() << "...+25ns " << m_countHits["/Next/"] << endmsg;
  debug() << "...+50ns " << m_countHits["/NextNext/"] << endmsg;
  debug() << "...with clusters linked " << m_countHits["WithClusters"] << endmsg;
  debug() << "...without clusters linked " << m_countHits["WithNoClusters"] << endmsg;
  debug() << "..Clusters:" << endmsg;
  debug() << "...total number: " << m_countClus << endmsg;
  debug() << "...total charge: " << m_clusCharge << ", average per cluster " << m_clusCharge/m_countClus << endmsg;
  debug() << "...spillover flag=1: " << m_countFlags["Yes"] << endmsg;
  debug() << "...spillover flag=0: " << m_countFlags["No"] << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STClusterTuple::finalize() {

  debug() << "==> Finalize" << endmsg;

  return ST::TupleAlgBase::finalize();  // must be called after all other actions
}


//=============================================================================

bool STClusterTuple::Origin(const MCHit* aHit, LHCb::MCVertex::MCVertexType check)
{
  LHCb::MCVertex::MCVertexType type = aHit->mcParticle()->originVertex()->type();
  if (type == check) return 1;
  else               return 0;
}

//=============================================================================
