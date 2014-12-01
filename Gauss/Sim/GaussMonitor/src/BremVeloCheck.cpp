
// $Id: $
// Include files
#include "CLHEP/Geometry/Point3D.h"
///
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4Timer.hh"
///
#include "GaudiKernel/MsgStream.h"
//
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/NTuple.h"

#include "GaudiKernel/INTupleSvc.h"

#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAuditorSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Chrono.h" 
#include "GaudiKernel/Stat.h" 

#include "GaudiKernel/GaudiException.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
/// stream
#include <fstream>
#include <cstdlib>
#include <vector>


// local
#include "BremVeloCheck.h"

#include "TH1F.h"


//-----------------------------------------------------------------------------
// Implementation file for class : BremVeloCheck
// 2014-06-12 Peter Noel Griffith  
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BremVeloCheck )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

BremVeloCheck::BremVeloCheck
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
: GiGaTrackActionBase( type , name , parent ){}


//=============================================================================
// Destructor
//=============================================================================
BremVeloCheck::~BremVeloCheck() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode BremVeloCheck::initialize () 
{

  std::cout <<"::::::::::::::::::::::::::::::::::::::::::::::::::::::::"<<std::endl;
	std::cout << "INITIALIZING BREMVELOCHECK"<<std::endl;
	std::cout <<"::::::::::::::::::::::::::::::::::::::::::::::::::::::::"<<std::endl;
	// initialize the base 
	StatusCode status = GiGaTrackActionBase::initialize() ;
	if( status.isFailure() ) 
	{ return Error("Could not intialize base class GiGaTrackActionBase!",
			status ) ; }
	///
	info() << "********** Initialize BremVeloCheck tool **********" << endmsg;

	std::string filename = "/NTUPLES/FILE1";
	NTupleFilePtr ntfile(ntupleSvc(), filename);

	info() << ntupleSvc() << "   " << filename << endmsg;

	if( ntfile ) {
		StatusCode status = StatusCode::SUCCESS;
		std::string ntname = filename + "/"+(this->name())+"/tree";
		NTuplePtr nt1(ntupleSvc(), ntname);

		if( !nt1 ) {
			nt1 = ntupleSvc()->book(ntname,CLID_ColumnWiseTuple,"PhotonInfo");
			if( nt1 ) {
				if( status.isSuccess() ) 
				{
					nt1->addItem("TrackID",                 m_trackid				);
					nt1->addItem("TrackParent",				m_trackpar				);
					nt1->addItem("NDaugthers",     			m_ndaugh,		0, 100000);
					nt1->addItem("DaughterE",      			m_ndaugh,		m_dau_e	);
					nt1->addItem("DaughterID",      		m_ndaugh,		m_daughID);
					nt1->addItem("DaughterProcessType",     m_ndaugh,      m_dptype	);
					nt1->addItem("DaughterProcessSubType",  m_ndaugh ,     m_dpstype);
				}
				else
				{
					error() << "Failure booking ntuples" << endmsg;
					return StatusCode::FAILURE;
				}
				m_ntuple = nt1;
			}
			else 
			{
				error() << "Ntuple already exist" << endmsg;
				return StatusCode::FAILURE;
			}
		}


		AIDA::IHistogram1D* h1 = histoSvc()->book( "photon energy", 1, "photon energy", 200, 0., 30.);
		h_photon_E = h1;
		AIDA::IHistogram1D* h2 = histoSvc()->book( "number of photons per track", 1, "number of photons per track", 200, 0., 100.);
		h_nphotons = h2;
		AIDA::IHistogram2D* h3 = histoSvc()->book( "photons production position", 1, "photon production position", 500,-1000.,1000.,500,0.,2000.);
		h_photon_posXZ = h3;
	}
	else info() << "no ntfile" << endmsg;

	std::cout <<"::::::::::::::::::::::::::::::::::::::::::::::::::::::::"<<std::endl;
	std::cout<< "BREMVELOCHECK INITIALIZED"<<std::endl;
	std::cout <<"::::::::::::::::::::::::::::::::::::::::::::::::::::::::"<<std::endl;
	///
	return StatusCode::SUCCESS;
}


// ============================================================================
StatusCode BremVeloCheck::finalize   () 
{ 
  // finilize the base 
	return GiGaTrackActionBase::finalize() ;
}
// ============================================================================

// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track opbject 
 */
// ============================================================================
void BremVeloCheck::PreUserTrackingAction ( const G4Track* track )
{
	//Checking track is valid
	if( 0 == track || 0 == trackMgr() )
		return;

}

// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track object 
 */
// ============================================================================
void BremVeloCheck::PostUserTrackingAction ( const G4Track* track ) 
{
	if( 0 == track || 0 == trackMgr())  { return ; }

//	int trid = track->GetTrackID();
	
	G4ParticleDefinition* partdefin = track->GetDefinition();
	std::string PartName = partdefin->GetParticleName();
	int PartPdgId = partdefin->GetPDGEncoding();

	const G4VProcess* proc = track->GetCreatorProcess();
	m_trackpar = track->GetParentID();

	debug() << "Particle Name " << PartName << endmsg;
	debug() << "Particle PDG ID " << PartPdgId << endmsg;
	debug() << "Parent ID " << m_trackpar << endmsg;
	debug() << "Track Volume " << track->GetVolume()->GetName() << endmsg;

	//checking process type
	if (proc != 0)
	{
		std::string procName = proc->GetProcessName();
		G4int proctype = proc->GetProcessType();
		G4int procsubtype = proc->GetProcessSubType();
		debug() << "Creator process name " << procName << endmsg;
		debug() << "Creator process type " << proctype << endmsg;
		debug() << "Creator process sub-type " << procsubtype << endmsg;
	}


	//Access secondaries from track
	if ( 0 != trackMgr()->GimmeSecondaries() ) 
	{
		G4TrackVector* childrens = trackMgr()->GimmeSecondaries();
		debug() << "--- daughters container size ---" << childrens->size() << endmsg;
		
		G4int numDaugh = 0;
    	//loop over daughters and store information if photon
		for (unsigned int index = 0; index < childrens->size(); ++index) 
		{
			G4Track* dtr = (*childrens)[index] ;
			
			if( 0 == dtr ) continue;

			if(dtr == track) debug() << " ATTENTION!!!! mother counted in daughter list"<< endmsg;
			

			G4ParticleDefinition* drpartdefin = dtr->GetDefinition();
			std::string drPartName = drpartdefin->GetParticleName();
			G4int DauPdgId = drpartdefin->GetPDGEncoding();
			

			m_daughID[index] = DauPdgId;
			if (DauPdgId == 22)
			{
				numDaugh++;
				double ekine = dtr->GetKineticEnergy();
				m_dau_e[index] = ekine;

				G4ThreeVector dtr_pos = dtr->GetPosition();
				double dtr_x = dtr_pos.x();
				double dtr_z = dtr_pos.z();
				
				const G4VProcess* drproc = dtr->GetCreatorProcess();
				G4int drproctype = drproc->GetProcessType();

				G4int drprocsubtype = drproc->GetProcessSubType();
//				std::cout<<"================="<<std::endl;
//				std::cout<<drproctype<<"\t"<<drprocsubtype<<std::endl;
//				std::cout<<drproc->GetProcessName()<<std::endl;
//				std::cout<<"================="<<std::endl;
				
				m_dptype[index]    = drproctype;
				m_dpstype[index]   = drprocsubtype;
				m_ndaugh   = numDaugh;
				h_photon_E->fill(numDaugh);
				h_nphotons->fill(ekine);
				h_photon_posXZ->fill(dtr_x,dtr_z);

			}
			else
			{
				continue;
			}

      
			if( dtr->GetParentID() != track->GetTrackID() ) 
			{
				debug() << "Could not reconstruct properly the parent! " << endmsg;
				debug() << " --- track id "<< track->GetTrackID() << endmsg;
				debug() << " --- daughter getparent id " << dtr->GetParentID() << endmsg;
			}
		}


		m_ntuple->write();

		return;
	}

}





