

#include "VeloCaloBuilder.h"

#include "math.h"
#include <stdlib.h>
#include <stdio.h>
#include "Event/TrackFitResult.h"
#include "Event/RecVertex.h"
#include "Kernel/LHCbID.h"
#include "Kernel/CaloCellID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCaloBuilder
//
// 2010-09-16 : Paul Seyfert
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( VeloCaloBuilder )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloCaloBuilder::VeloCaloBuilder( const std::string& name, ISvcLocator* pSvcLocator )
  : GaudiTupleAlg( name , pSvcLocator )
  , m_caloTracks(NULL)
  , m_magFieldSvc(NULL)
  , m_getter(NULL)
{
  declareProperty( "zcut"      , m_zcut = 100); // whare the first Velo State should be
  declareProperty( "IPcut"     , m_ipcut = 0 ); // min pseudo IP cut
  declareProperty( "quali"     , m_qualimodi = 10); // min matching chi^2
  declareProperty( "VeloLoc"   , m_veloTracksName = LHCb::TrackLocation::Velo);
  declareProperty( "ClusterLoc", m_clusterlocation =  LHCb::CaloClusterLocation::Ecal);
  declareProperty( "OutputTracks", m_outputTracksName = "Rec/Track/VeloCalo");

  // -- those now taken from N.Zwahlen's code. m_zKick also used for momentum
  declareProperty("PtkickConstant", m_ptkickConstant = 1.263f*(float)Gaudi::Units::GeV);
  declareProperty("zKick", m_zKick   = 525.0);
  declareProperty("eRes0", m_eres[0] = 0.60f );
  declareProperty("eRes1", m_eres[1] = 0.70f );
  declareProperty("eCorrect", m_eCorrect = 1.2f );
  
}

//=============================================================================
// Destructor
//=============================================================================
VeloCaloBuilder::~VeloCaloBuilder() {}


//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloCaloBuilder::initialize()
{
  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure() ) { return sc; }

  m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
  m_getter = tool<ICaloGetterTool>("CaloGetterTool");

  return StatusCode::SUCCESS;
}


using namespace LHCb;
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloCaloBuilder::execute()
{
  m_field = - (float)(m_magFieldSvc->signedRelativeCurrent()); // -- get field polarity
  if (true)  { // debuging purpose
    LHCb::Tracks* velotracks = get<LHCb::Tracks>(m_veloTracksName);

    m_caloTracks = new LHCb::Tracks();
    m_caloTracks->reserve(100);
  
    put(m_caloTracks,m_outputTracksName);

    if(true) { // debuging purpose
      double mass = 139.57;//pi or k mass
      m_getter->update();
    
      LHCb::CaloClusters* cluster = m_getter->clusters(m_clusterlocation);
      // -- if there are no clusters, there's nothing to do
      if (!cluster) return StatusCode::SUCCESS;
      LHCb::CaloClusters::const_iterator clusterIter;
      int matches = 0;
      int nomatches = 0;
      // -- loop over clusters
      for (clusterIter = cluster->begin(); clusterIter!=cluster->end(); ++clusterIter ) {
      
	LHCb::CaloCluster* thecluster = *clusterIter;
      
	if (thecluster->e()<mass) continue;
	LHCb::Tracks::const_iterator veloIt;
	int locmatches = 0;
	// -- loop over velo tracks
	for (veloIt = velotracks->begin(); velotracks->end() != veloIt; ++veloIt) {
	  // -- apply IP, z, type cuts
	  {
	    float x,y,tx,ty,z,norm,IP;
	    x = (float)(*veloIt)->firstState().x();
	    y = (float)(*veloIt)->firstState().y();
	    tx = (float)(*veloIt)->firstState().tx();
	    ty = (float)(*veloIt)->firstState().ty();
	    z = (float)(*veloIt)->firstState().z();
	    norm = sqrt(tx*tx+ty*ty);
	    IP = (fabs(x*ty-y*tx)/norm);
	    if (IP<m_ipcut) continue;
	    if (z<m_zcut) continue;
	    if ((*veloIt)->type() == LHCb::Track::VeloR) continue;
	  }

	  LHCb::Track* mytrack = (*veloIt);

	  // -- now make the calo stub
	  LHCb::Track* calotrack = new LHCb::Track(thecluster->key());
	  calotrack->setType(LHCb::Track::Calo);
	  const std::vector<LHCb::CaloClusterEntry> caloentries = thecluster->entries();
          // unsigned int caloarea; // not needed in the current version.
	  for (std::vector<LHCb::CaloClusterEntry>::const_iterator itter = caloentries.begin(); itter!=caloentries.end() ;++itter) {
	    LHCb::CaloClusterEntry theentry = *itter;
	    SmartRef<LHCb::CaloDigit> reffer = theentry.digit();
	    LHCb::CaloDigit diggy = *reffer;
	    LHCbID iddy = LHCbID(diggy.cellID());
	    // caloarea = diggy.cellID().area();
	    calotrack->addToLhcbIDs(iddy);
	  }

	  Gaudi::TrackVector vector;
	  vector[0] = thecluster->position().x();
	  vector[1] = thecluster->position().y();
	  vector[2] = thecluster->position().z();
	  vector[3] = vector[0]/thecluster->position().z();
	  vector[4] = vector[1]/thecluster->position().z();
	  Gaudi::TrackSymMatrix matrix;
	  matrix(0,0) =  (thecluster->position().spread())(0,0);
	  matrix(1,1) =  (thecluster->position().spread())(1,1);
	  matrix(2,2) = vector[0]/thecluster->position().z()*vector[0]/thecluster->position().z() ;
	  matrix(3,3) = vector[3]*vector[3]*0.05*0.05 ;
	  matrix(4,4) = vector[4]*vector[4]*0.08*0.08 ;
	  LHCb::State calostate = LHCb::State(vector, matrix, thecluster->position().z(), LHCb::State::BegECal);
	  calotrack->addToStates((calostate));
	  
	  float quality = matchchi(mytrack,calotrack);
	  // -- at this point the HltVeloEcalMatch tool was used
	  //m_matchtool->function(*mytrack,*calotrack);

	  // -- if the chi^2 cut is passed ...
	  if (quality < m_qualimodi ) {
	    // ... the momentum is calculated
	    float kick = float(mytrack->firstState().x() + mytrack->firstState().tx() * (calotrack->firstState().z() - mytrack->firstState().z()) - calotrack->firstState().x());
	    float qp = float(-kick/9950000. * m_field); // m_field corrects up/down field configuration
	    float corrtx = float(calotrack->firstState().x()-mytrack->firstState().x()-mytrack->firstState().tx()*(10*m_zKick - mytrack->firstState().z()));
	    corrtx = corrtx / (float(calotrack->firstState().z()) - m_zKick*10);
	   
	    // -- using upstream instead of velo tracks, a momentum cut can purify the VeloCalo tracks here
	    if (true) {
	      matches++;
	      locmatches++;
	      // -- the final track
	      LHCb::Track* aCopy = new LHCb::Track();
	      aCopy->addToAncestors(*mytrack);
	      aCopy->addToAncestors(*calotrack);
	      {
		SmartRefVector<LHCb::Track>::iterator anc2;
		for (anc2=mytrack->ancestors().begin(); anc2!=mytrack->ancestors().end(); ++anc2) {
		  aCopy->addToAncestors(*(*anc2));
		}
	      }
	      std::vector< LHCb::State* > velostates = mytrack->states();
	      std::vector< LHCb::State* > copiedstates;
	      for (std::vector<LHCb::State*>::iterator stateiter = velostates.begin(); stateiter != velostates.end(); ++stateiter) {
		copiedstates.push_back((*stateiter)->clone());
		//calculate momentum
		Gaudi::TrackSymMatrix cov  = copiedstates.back()->covariance();
		copiedstates.back()->setQOverP(qp);
		cov(4,4) = qp*qp*0.07*0.07;
		// this is only a fake covariance matrix, but the offdiagonal entries should at least reflekt the pt-kick calculation
		copiedstates.back()->setCovariance(cov);
	      }
	      aCopy->addToStates(copiedstates);
	      aCopy->addToLhcbIDs(mytrack->lhcbIDs());

	      velostates.clear(); copiedstates.clear();
	      velostates = calotrack->states();

	      for (std::vector<LHCb::State*>::iterator stateiter = velostates.begin(); stateiter != velostates.end(); ++stateiter) {
		copiedstates.push_back((*stateiter)->clone());
		// -- calculated momentum
		copiedstates.back()->setQOverP(qp);
		copiedstates.back()->setTx(corrtx);
	      }
	      aCopy->addToStates(copiedstates);
	      aCopy->addToLhcbIDs(calotrack->lhcbIDs());

	      LHCb::TrackFitResult * result = new LHCb::TrackFitResult();
	      result->setPScatter(1/qp);
	      
	      aCopy->setFitResult(result);

	      aCopy->setType(LHCb::Track::Long);
	      aCopy->setChi2AndDoF(quality*60,60);
	      TESpush(aCopy, mytrack,calotrack);
	    } else {
	      nomatches++;
	    }
	  } else {
	    nomatches++;
	  }
	  delete calotrack;
	} // -- looped over velo tracks
      } // -- looped over clusters
    } // if true
  } // if true
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Add tracks to output container and write an additional info
//=============================================================================
StatusCode VeloCaloBuilder::TESpush(LHCb::Track* track,  LHCb::Track* ancestor, LHCb::Track* calotrack) {
  track->addInfo(2001,ancestor->key());
  track->addInfo(2002,calotrack->key());
  m_caloTracks->add(track);    
  return StatusCode::SUCCESS;
}

//=============================================================================
//  calculate matching chi^2. mainly taken from N. Zwahlen's HltVeloEcalMatch
//=============================================================================
float VeloCaloBuilder::matchchi(LHCb::Track* velo, LHCb::Track* calo) {
  static const double cellSizeECal[3] = { 121.7, 60.85, 40.56 };
  int ids_ecal = 0;
  double siz_ecal =0.;
  for (std::vector< LHCb::LHCbID >::const_iterator itid = calo->lhcbIDs().begin(); itid != calo->lhcbIDs().end(); ++itid) {
    if( ! itid->isCalo() ) continue;
    LHCb::CaloCellID cid = itid->caloID();
    if ( cid.calo() != 2 ) continue;
    int area = cid.area();
    if (area < 0 || area > 2 ) continue;
    ids_ecal++;
    siz_ecal += cellSizeECal[area];
  }

  double cell_size = cellSizeECal[0];
  if ( ids_ecal > 0 ) {
    cell_size = siz_ecal/ids_ecal;
  }
  
  const LHCb::State& state = calo->firstState();
  float x      = (float)(state.x()/Gaudi::Units::cm);
  float y      = (float) (state.y()/Gaudi::Units::cm);
  float z      = (float) (state.z()/Gaudi::Units::cm);   // Shower Max
  float ex     = (float) (2.*cell_size/Gaudi::Units::cm);
  float ey     = ex;
  float e      = (float) (calo->pt()/Gaudi::Units::GeV);

  e *= m_eCorrect;
 
  // -- get track slopes
  double trackDxDz = velo->firstState().tx();
  double trackDyDz = velo->firstState().ty();
  
  // -- Absolute energy uncertainty:
  double de = e*(sqrt( m_eres[0]*m_eres[0] + m_eres[1]*m_eres[1]/e ));

  double deltaZ = z - m_zKick;
  double xkick  = deltaZ * (m_ptkickConstant/Gaudi::Units::GeV)/e;
  double exkick = fabs( xkick/e)*de;
  
  // -- Calculate the slopes and their uncertainties:
  double edxdz  = sqrt(ex*ex + exkick*exkick)/z;
  double dydz   = y/z;
  double edydz  = ey/z;
  
  double uncorrdxdz = (x/z);
  double dxdz;
  // -- apply the kick in the "right" direction
  if (uncorrdxdz-trackDxDz<0)
    dxdz = (x+xkick)/z;
  else
    dxdz = (x-xkick)/z;
  
  // -- calculate chi2 
  double deltaX = (dxdz - trackDxDz)/edxdz;
  double deltaY = (dydz/fabs(dydz))*(dydz - trackDyDz)/edydz;
  float chi2 = (float)(deltaX*deltaX + deltaY*deltaY);
  
  return chi2;
}
