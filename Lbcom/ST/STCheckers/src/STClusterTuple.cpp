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
  //Input clusters
  declareSTConfigProperty("inputData", m_clusterLocation , STClusterLocation::TTClusters);

  //Input MC hits
  declareSTConfigProperty("inputMCHits", m_hitLocation , MCHitLocation::TT);

  //Associator table
  declareSTConfigProperty("asctLocation", m_asctLocation , m_clusterLocation + "2MCHits");

  //Maximum size of the tuple arrays
  declareProperty("MaxEntries", m_nMax=20000);

  //Dump MC quantities info in the tuple
  declareProperty("MonteCarlo", m_MonteCarlo=false);

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

  //Retrieve event and run numbers
  const ODIN* odin = get<ODIN> ( ODINLocation::Default );

  //Get clusters
  const STClusters* clusterCont = getIfExists<STClusters>(m_clusterLocation);
  
  //Create MCHits-Clusters associator table
  AsctTool* associator;
  const Table* aTable = NULL;
  if(m_MonteCarlo)
  {
    associator = new AsctTool(evtSvc(), m_asctLocation);
    aTable = associator->direct();
    if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);
  }
  
  //Output tree
  Tuple tree = nTuple("ClusterTuple", "Info about clusters and MC hits");

  vector<double> TOF;                   if(m_MonteCarlo) TOF.reserve(m_nMax);
  vector<double> Energy;                if(m_MonteCarlo) Energy.reserve(m_nMax);
  vector<double> Px;                    if(m_MonteCarlo) Px.reserve(m_nMax);
  vector<double> Py;                    if(m_MonteCarlo) Py.reserve(m_nMax);
  vector<double> Pz;                    if(m_MonteCarlo) Pz.reserve(m_nMax);
  vector<double> X;                     if(m_MonteCarlo) X.reserve(m_nMax);
  vector<double> Y;                     if(m_MonteCarlo) Y.reserve(m_nMax);
  vector<double> Z;                     if(m_MonteCarlo) Z.reserve(m_nMax);
  vector<double> entry_X;               if(m_MonteCarlo) entry_X.reserve(m_nMax);
  vector<double> entry_Y;               if(m_MonteCarlo) entry_Y.reserve(m_nMax);
  vector<double> entry_Z;               if(m_MonteCarlo) entry_Z.reserve(m_nMax);
  vector<double> exit_X;                if(m_MonteCarlo) exit_X.reserve(m_nMax);
  vector<double> exit_Y;                if(m_MonteCarlo) exit_Y.reserve(m_nMax);
  vector<double> exit_Z;                if(m_MonteCarlo) exit_Z.reserve(m_nMax);
  vector<double> pathlength;            if(m_MonteCarlo) pathlength.reserve(m_nMax);
  vector<double> dxdz;                  if(m_MonteCarlo) dxdz.reserve(m_nMax);
  vector<double> dydz;                  if(m_MonteCarlo) dydz.reserve(m_nMax);
  vector<double> ID;                    if(m_MonteCarlo) ID.reserve(m_nMax);
  vector<double> mothID;                if(m_MonteCarlo) mothID.reserve(m_nMax);
  vector<int> vertexType;               if(m_MonteCarlo) vertexType.reserve(m_nMax);
  vector<bool> isLinked;                if(m_MonteCarlo) isLinked.reserve(m_nMax);

  vector<double> clusterCharge;         clusterCharge.reserve(m_nMax);
  vector<unsigned int> clusterSize;     clusterSize.reserve(m_nMax);
  vector<bool> spilloverFlag;           spilloverFlag.reserve(m_nMax);
  vector<double> interStripFraction;    interStripFraction.reserve(m_nMax);
  vector<unsigned int> channelID;       channelID.reserve(m_nMax);
  vector<unsigned int> strip;           strip.reserve(m_nMax);
  vector<unsigned int> station;         station.reserve(m_nMax);
  vector<unsigned int> layer;           layer.reserve(m_nMax);
  vector<unsigned int> detRegion;       detRegion.reserve(m_nMax);
  vector<unsigned int> sector;          sector.reserve(m_nMax);
  vector<double> neighbourSum;          neighbourSum.reserve(m_nMax);
  vector<int> spill;                    spill.reserve(m_nMax);
  
  //Loop over clusters
  if(!clusterCont) debug() << "Clusters missing in the event: " << m_nEvent << endmsg;
  else
  {
    if(clusterCont->size() > m_nMax-1) warning() << "Number of clusters greater than the limit of " << m_nMax << endmsg;

    unsigned int iClus = 0;
    STClusters::const_iterator iterClus = clusterCont->begin() ;
    for( ; iterClus != clusterCont->end(); ++iterClus, ++iClus )
    {
      const STCluster* aCluster = *iterClus;
      
      //Retrieve MCHits from cluster
      if(m_MonteCarlo)
      {
        Range range = aTable->relations(*iterClus);
        if (range.empty())
        {
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
            vertexType.push_back(-9999.0);
            isLinked.push_back(0);
          } //if (iClus)
          
        } //if (range.empty())
        else
        {
          debug() << "STCluster/MCHit linking successful" << endmsg;

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
            vertexType.push_back((int)aHit->mcParticle()->originVertex()->type());
            isLinked.push_back(1);

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
            else debug() << "Mother ID: no mother" << endmsg;
          } //if (iClus)
        } //else
      } //if (m_MonteCarlo)
      
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
        spill.push_back((int)aCluster->spill());
        
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
        debug() << "Spill: " << (int)aCluster->spill() << endmsg;

      } // endif
    } //iterClus
  } //end of getIfExist condition

  //Fill the tuple
  if( ! tree->column( "RunNumber", odin->runNumber()) ) return StatusCode::FAILURE;
  if( ! tree->column( "EventNumber", odin->eventNumber()) ) return StatusCode::FAILURE;
  if( ! tree->column( "BunchID", odin->bunchId()) ) return StatusCode::FAILURE;
  if(m_MonteCarlo)
  {
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
    tree->farray( "vertexType", vertexType, "nClusters", m_nMax);
    tree->farray( "isLinked", isLinked, "nClusters", m_nMax);
  }
  tree->farray( "clusterCharge", clusterCharge, "nClusters", m_nMax);
  tree->farray( "clusterSize", clusterSize, "nClusters", m_nMax);
  tree->farray( "spilloverFlag", spilloverFlag, "nClusters", m_nMax);
  tree->farray( "interStripFraction", interStripFraction, "nClusters", m_nMax);
  tree->farray( "channelID", channelID, "nClusters", m_nMax);
  tree->farray( "strip", strip, "nClusters", m_nMax);
  tree->farray( "clusterStation", station, "nClusters", m_nMax);
  tree->farray( "clusterLayer", layer, "nClusters", m_nMax);
  tree->farray( "clusterDetRegion", detRegion, "nClusters", m_nMax);
  tree->farray( "clusterSector", sector, "nClusters", m_nMax);
  tree->farray( "neighbourSum", neighbourSum, "nClusters", m_nMax);
  tree->farray( "spill", spill, "nClusters", m_nMax);

  if( !(tree->write()) ) err() << "Cannot fill the tuple!" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STClusterTuple::finalize() {

  debug() << "==> Finalize" << endmsg;

  return ST::TupleAlgBase::finalize();  // must be called after all other actions
}

