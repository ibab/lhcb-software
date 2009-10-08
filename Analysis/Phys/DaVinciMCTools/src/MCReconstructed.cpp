//=============================================================================
// $Id: MCReconstructed.cpp,v 1.1 2009-10-08 16:14:20 pkoppenb Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// the data
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"

// from
//#include "Relations/RelationWeighted.h"

// from CaloUtils
#include "CaloUtils/ClusterFunctors.h"

// local
#include "MCReconstructed.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCReconstructed
//
// 2004-03-08 : Christian Jacoby
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCReconstructed );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCReconstructed::MCReconstructed( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  //, m_zECAL(12696.0*mm)
  //, m_xECALInn(363.3*mm)
  //, m_yECALInn(363.3*mm)
  //, m_xECALOut(3757.2*mm)
  //, m_yECALOut(3030.0*mm)
  //, m_lowEt(200*MeV)
  , m_pCPPAsct(0)
  , m_pNPPAsct(0)
{
  declareInterface<IMCReconstructed>(this);

}

//=============================================================================
// Destructor
//=============================================================================
MCReconstructed::~MCReconstructed() {}; 

//=============================================================================
// Initialize
//=============================================================================
StatusCode MCReconstructed::initialize() {
 
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) return sc;
  
  debug() << "==> MCReconstructed initialize" << endmsg;
 
  m_pCPPAsct = new ProtoParticle2MCLinker( this, Particle2MCMethod::ChargedPP, LHCb::ProtoParticleLocation::Charged);
  m_pNPPAsct = new ProtoParticle2MCLinker( this, Particle2MCMethod::NeutralPP, LHCb::ProtoParticleLocation::Neutrals);

  //std::string asctType = "RelationWeighted<LHCb::CaloCluster,LHCb::MCParticle,float>";
  //std::string asctName = "CCs2MCPs";
  //sc = toolSvc()->retrieveTool( asctType , asctName , m_asctCaloClus );
  //if(sc.isFailure()){
  //  fatal() << " Unable to retrieve CaloClusterAssociator/CCs2MCPs"
  //      << endmsg;
  //  return sc;
  //}

  return sc;
}


StatusCode MCReconstructed::finalize()
{
  if( 0 != m_pCPPAsct ) {
    delete m_pCPPAsct;
    m_pCPPAsct = 0;
  }
  
  if( 0 != m_pNPPAsct ) {
    delete m_pNPPAsct;
    m_pNPPAsct = 0;
  }
  
  return GaudiTool::finalize();
}

//===========================================================================
// Reconstruted as merged pi0
//===========================================================================
/*
bool MCReconstructed::recoAsMergedPi0( const LHCb::MCParticle *aMCPart ) {
  bool recoMerged = false;
  const LHCb::MCParticle* theMCPi0 = aMCPart->mother();
  if( theMCPi0->particleID().pid() != 111 ) {
    return false;
  }
  if( !m_asctCaloClus->tableExists() ) {
    return false;
  }
  MCCaloRange rangeinv = (m_asctCaloClus->inverse())->relations(theMCPi0);

  // Require only 1 associated cluster for merged pi0
  if( 1 != rangeinv.size() ) return false;
  
  CaloCluster* cluster = (rangeinv.begin())->to();
  
  // Cell size for this Ecal region
  const CaloCluster::Entries& entries = cluster->entries();
  CaloCluster::Entries::const_iterator iseed = 
    ClusterFunctors::locateDigit ( entries.begin () , 
                                   entries.end   () , 
                                   CaloDigitStatus::SeedCell );
  const CaloDigit*  seed    = iseed->digit();
  CaloCellID     cellID = seed->cellID() ;
  int zone = cellID.area() + 1;
  double CellSize = 121.7 / zone;
  
  double x1 = 0;
  double y1 = 0;
  double x2 = 0;
  double y2 = 0;
  double dgg = 999999;
  
  const SmartRefVector<MCVertex> evertices = theMCPi0->endVertices();
  if( 0 != evertices.size() ){
    const SmartRefVector<MCVertex>::const_iterator 
      ievertex = evertices.begin();
    const MCVertex* evertex=*ievertex;
    const SmartRefVector<LHCb::MCParticle>& products = evertex->products();
    if( 2 == products.size() ){
      SmartRefVector<LHCb::MCParticle>::const_iterator  iprod1=products.begin();
      const LHCb::MCParticle* prod1 = *iprod1;
      SmartRefVector<LHCb::MCParticle>::const_iterator  iprod2=products.begin()+1;
      const LHCb::MCParticle* prod2 = *iprod2;
    
      if( 22 == prod1->particleID().pid() && 22 == prod2->particleID().pid() ){
        double dec1_px = prod1->momentum().px();
        double dec1_py = prod1->momentum().py();
        double dec1_pz = prod1->momentum().pz();
        double dec2_px = prod2->momentum().px();
        double dec2_py = prod2->momentum().py();
        double dec2_pz = prod2->momentum().pz();
        double xv1 = prod1->originVertex()->position().x();
        double yv1 = prod1->originVertex()->position().y();
        double zv1 = prod1->originVertex()->position().z();
        double xv2 = prod2->originVertex()->position().x();
        double yv2 = prod2->originVertex()->position().y();
        double zv2 = prod2->originVertex()->position().z();
        x1 = dec1_px / dec1_pz * (m_zECAL-zv1) + xv1;
        y1 = dec1_py / dec1_pz * (m_zECAL-zv1) + yv1;
        x2 = dec2_px / dec2_pz * (m_zECAL-zv2) + xv2;
        y2 = dec2_py / dec2_pz * (m_zECAL-zv2) + yv2;
        dgg = sqrt ( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
      }
    }
    dgg = dgg/CellSize;
  }
  bool InCal =true;
  if( m_xECALOut < fabs(x1) || m_yECALOut < fabs(y1) ){InCal=false;}
  if( m_xECALOut < fabs(x2) || m_yECALOut < fabs(y2) ){InCal=false;}
  if( m_xECALInn > fabs(x1) &&  m_yECALInn > fabs(y1) ){InCal=false;}
  if( m_xECALInn > fabs(x2) &&  m_yECALInn > fabs(y2) ){InCal=false;}
  if( InCal && 2.5 > dgg){
    recoMerged = true;
  }
  return recoMerged;
}
*/
 


//=============================================================================
//  Method to check if a particle is reconstructed
//  input:  MCParticle
//  output: 0 = not rec'ted (default), 1 = long, 2 = TTT, 3 = VTT, 4=NEUTRAL
//=============================================================================
IMCReconstructed::RecCategory MCReconstructed::reconstructed( const LHCb::MCParticle *iMCstable )
{

  IMCReconstructed::RecCategory rected = IMCReconstructed::NotReconstructed;

  debug() << "Checking MCParticle: " << endmsg;
  debug() << *iMCstable << endmsg;

  if (iMCstable->particleID().threeCharge() != 0) ///< for charged
  {
      LHCb::ProtoParticle* pp = m_pCPPAsct->firstP( iMCstable );
      while( pp )
      {
        debug() << "    has an associated charged ProtoParticle." << endmsg;
        const LHCb::Track *ptrack = pp->track();
        if( ptrack && !(ptrack->flags() & LHCb::Track::Clone) )
	{
	  switch( ptrack->type() )
	  {
	    case LHCb::Track::Long        : rected = IMCReconstructed::ChargedLong; break;
	    case LHCb::Track::Downstream  : rected = IMCReconstructed::ChargedDownstream; break;
	    case LHCb::Track::Upstream    : rected = IMCReconstructed::ChargedUpstream; break;

	    // Other enum values not covered by the original logic
	    case LHCb::Track::TypeUnknown : 
	    case LHCb::Track::Velo        : 
	    case LHCb::Track::VeloR       : 
	    case LHCb::Track::Ttrack      : 
	    case LHCb::Track::Muon        :
	    default                       : break;
	  }

	  debug() << "      rected = " << rected << endmsg;

          break;
        }

        pp = m_pCPPAsct->nextP();
      }
  }
  else ///< for neutrals
  {
      LHCb::ProtoParticle* pp = m_pNPPAsct->firstP( iMCstable );
      if( pp )
      {
        debug() << "    has an associated neutral ProtoParticle." << endmsg;

        rected = IMCReconstructed::Neutral;

        // pp = m_pNPPAsct->nextP();
      }

      debug() << "      rected = " << rected << endmsg;
  }
 

  debug() << "  returning " << IMCReconstructed::text(rected) << endmsg;

  return rected;
  
}

//=============================================================================
