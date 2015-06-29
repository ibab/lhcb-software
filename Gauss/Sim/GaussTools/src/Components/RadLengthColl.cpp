// $Id: $
// Include files

#include "CLHEP/Geometry/Point3D.h"
///
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
// to enable neutrinos
#include "G4QNeutrinoPhysics.hh"
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
/// stream
#include <fstream>
#include <cstdlib>
#include <vector>
// local
#include "RadLengthColl.h"

#include "TH1F.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RadLengthColl
//
// 2005-07-19 : Yasmine Sarah Amhis
// 2009-05-29 : Silvia Migliranzi
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RadLengthColl )

	//=============================================================================
	// Standard constructor, initializes variables
	//=============================================================================
	RadLengthColl::RadLengthColl( const std::string& name, const std::string& type, const IInterface*  parent )
: GiGaStepActionBase( name , type, parent ),
	mp_origx(0.), mp_origy(0.), mp_origz(0.)
{
	declareProperty( "orig_x" , mp_origx ) ;	
	declareProperty( "orig_y" , mp_origy ) ;
	declareProperty( "orig_z" , mp_origz ) ;
}


//=============================================================================
// Destructor
//=============================================================================
RadLengthColl::~RadLengthColl() {}


//=============================================================================
// Initialization
//=============================================================================
StatusCode RadLengthColl::initialize()  {

	info() << "********** Initializing RadLengthColl tool **********"<<endmsg;

	StatusCode scb = GiGaStepActionBase::initialize();
	if( scb.isFailure() ) 
	{
		err() << "Error initializing base class" << endmsg;
		return scb;
	}

	INTupleSvc* ntupleSvc = 0;
	StatusCode sc= svcLoc()->service( "NTupleSvc" , ntupleSvc , true );
	if( sc.isFailure()   ) 
	{ return Error("Unable to locate Ntuple Service ", sc ); }

	ntname = "/NTUPLES/FILE2";
	debug() << "ntupleSvc = " << ntupleSvc << endmsg;
	NTupleFilePtr ntfile(ntupleSvc, ntname); 
	if( !ntfile )
	{
		err() << "Could not access" << ntname << endmsg;
		return StatusCode::FAILURE;
	}

	StatusCode status = StatusCode::SUCCESS;
	ntname += "/RadLengthColl/tree";
	NTuplePtr m_matScan_PlaneDatas(ntupleSvc, ntname);
	if ( !m_matScan_PlaneDatas ) 
	{
		m_matScan_PlaneDatas = ntupleSvc->book(ntname, CLID_ColumnWiseTuple, "RadL");
		if ( 0!=m_matScan_PlaneDatas ) 
		{
			status = m_matScan_PlaneDatas->addItem ("Item",m_ntrk,0,100);
			status = m_matScan_PlaneDatas->addIndexedItem ("ID",m_ntrk,m_planeID);
			status = m_matScan_PlaneDatas->addIndexedItem ("Xpos",m_ntrk,m_Xpos);
			status = m_matScan_PlaneDatas->addIndexedItem ("Ypos",m_ntrk,m_Ypos);
			status = m_matScan_PlaneDatas->addIndexedItem ("Zpos",m_ntrk,m_Zpos);
			status = m_matScan_PlaneDatas->addIndexedItem ("eta",m_ntrk,m_eta);
			status = m_matScan_PlaneDatas->addIndexedItem ("phi",m_ntrk,m_phi);
			status = m_matScan_PlaneDatas->addIndexedItem ("cumradlgh",m_ntrk,m_cumradlgh);
			status = m_matScan_PlaneDatas->addIndexedItem ("p2pradlgh",m_ntrk,m_p2pradlgh);
			status = m_matScan_PlaneDatas->addIndexedItem ("cuminterlgh",m_ntrk,m_cuminterlgh);
			status = m_matScan_PlaneDatas->addIndexedItem ("p2pinterlgh",m_ntrk,m_p2pinterlgh);
			status = m_matScan_PlaneDatas->addItem ("origx",m_origx);
			status = m_matScan_PlaneDatas->addItem ("origy",m_origy);
			status = m_matScan_PlaneDatas->addItem ("origz",m_origz);
			if( !status.isSuccess() ) 
			{
				err() << "Failure booking ntuples" << endmsg;
				return StatusCode::FAILURE;
			}
		}
		else 
		{
			err() << "Ntuple already exist" << endmsg;
			return StatusCode::FAILURE;
		}
	}


	// Initialize ntuple variables:
	m_ntrk      = 0;
	for (m_ntrk = 0; m_ntrk < 100; ++m_ntrk) 
	{
		m_planeID[m_ntrk]= 0;
		m_Xpos[m_ntrk] = -9999.0;
		m_Ypos[m_ntrk] = -9999.0;
		m_Zpos[m_ntrk] = -9999.0;
		m_eta[m_ntrk] = -9999.0;
		m_phi[m_ntrk] = -9999.0;
		m_cumradlgh[m_ntrk] = -9999.0;
		m_p2pradlgh[m_ntrk] = -9999.0;
	}


	info() << "==> Initialize successful" << endmsg;
	return StatusCode::SUCCESS;
}


//=============================================================================
// Scan Stepping Action
//=============================================================================
void RadLengthColl::UserSteppingAction ( const G4Step* theStep )
{
	track = theStep->GetTrack();
	partdef = track->GetDefinition();
	ParticleName = partdef->GetParticleName();

	G4int PartPdgId = partdef->GetPDGEncoding();
	int m_trackpar = track->GetParentID();
	const G4VProcess* proc = track->GetCreatorProcess();
	if(proc!=0) return;

	debug() << "Projectile ID: " << PartPdgId << ", ID particle: " << m_trackpar << endmsg;


	thePreStepPoint = theStep->GetPreStepPoint();
	Vol = thePreStepPoint->GetPhysicalVolume();
	VOL = Vol->GetLogicalVolume();
	initial_position = thePreStepPoint->GetPosition();

	// initialize the counters at the origin of the track
	debug() << "Origin vertex position: " << mp_origx << "  " << mp_origy << "  " << mp_origz << endmsg;
	if(initial_position[0] - mp_origx < 1e-6 && initial_position[1] - mp_origy < 1e-6  && initial_position[2] - mp_origz < 1e-6 )
	{
		m_origx  = mp_origx;
		m_origy  = mp_origy;
		m_origz  = mp_origz;

		// Radiation Length
		theRadLength = 0;
		theCumulatedRadLength = 0;
		thePlane2PlaneRadLength = 0;

		// Interaction Length
		theInterLength = 0;
		theCumulatedInterLength = 0;
		thePlane2PlaneInterLength = 0;

		// Other stuff
		index = 0;
		m_ntrk = 0;
	}

	// Getting Step Length
	StepLength = theStep->GetStepLength();

	// Adding radiation length
	MaterialRadiationLength = VOL->GetMaterial()->GetRadlen();
	theRadLength = StepLength/MaterialRadiationLength;
	theCumulatedRadLength += theRadLength;
	thePlane2PlaneRadLength += theRadLength;

	// Adding interaction length
	MaterialInterLength = VOL->GetMaterial()->GetNuclearInterLength();
	theInterLength = StepLength/MaterialInterLength;
	theCumulatedInterLength += theInterLength;
	thePlane2PlaneInterLength += theInterLength;


	VolName = (std::string) Vol->GetName();

	// This commented block of code does not include interaction lengths

	debug() << "*** - VolName - *** = " << VolName << endmsg;

	debug() << "*** - Zpos - *** = " << thePreStepPoint->GetPosition().z() << endmsg;
	debug() << "*** - stepsize  - *** = " << StepLength << endmsg;
	debug() << "*** - Material RadLength - *** = " << MaterialRadiationLength << endmsg;
	debug() << "*** - theRadLength  - *** = " << theRadLength << endmsg;
	debug() << "*** - CumulativeRadLength  - *** = " << theCumulatedRadLength << endmsg;



	// get the Volume Name
	// returns the position of the found string or string::npos if not found
	if(VolName.find("Structure")!= std::string::npos)  // "/dd/Geometry"
	{
		std::string::size_type pos = VolName.find("Scoring_Plane");
		if(pos!=std::string::npos)
		{
			debug() << VolName << endmsg;

			VolumeName="Scoring_Plane";
			IDPlane = VolName.substr(pos+13,100);

			debug() << IDPlane << endmsg;
			IDP = atoi(IDPlane.c_str());

			debug() << "IDPlane = " << IDPlane << " IDP " << IDP << endmsg;
		}
		else
		{
			debug() << "Particle was not recorded in a Scoring_Plane" << endmsg;
		}
	}
	else if(VolName=="Universe")
	{
		VolumeName="Universe";
		float z = thePreStepPoint->GetPosition().z();
		debug() << "Hit Universe at Z = " << z << " (eta = " << thePreStepPoint->GetPosition().eta() << ")" << endmsg;		
	}




	if(VolumeName == "Scoring_Plane" && m_trackpar == 0 ) // == if the volume is a dummy scorer plane
	{
		debug() << "VolumeName = " << VolumeName << endmsg;

		index++;
		m_planeID[m_ntrk] = IDP;
		m_Xpos[m_ntrk] = thePreStepPoint->GetPosition().x();
		m_Ypos[m_ntrk] = thePreStepPoint->GetPosition().y();
		m_Zpos[m_ntrk] = thePreStepPoint->GetPosition().z();
		m_eta[m_ntrk] = thePreStepPoint->GetPosition().eta();
		m_phi[m_ntrk] = thePreStepPoint->GetPosition().phi();
		m_cumradlgh[m_ntrk] = theCumulatedRadLength;
		m_p2pradlgh[m_ntrk] = thePlane2PlaneRadLength;
		debug() << "theCumulatedInterLength = " << theCumulatedInterLength << "and thePlane2PlaneInterLength = "
			<< thePlane2PlaneInterLength << endmsg;
		m_cuminterlgh[m_ntrk] = theCumulatedInterLength;
		m_p2pinterlgh[m_ntrk] = thePlane2PlaneInterLength;

		// reinitialize the value of the plane 2 plane radlength and interlength to 0 
		thePlane2PlaneRadLength = 0;
		thePlane2PlaneInterLength = 0;

		debug() << "idplane = "<< m_planeID[m_ntrk] << endmsg;
		debug() << "m_Zpos[m_ntrk], cum and p2p: " << m_Zpos[m_ntrk] << ", " << m_cumradlgh[m_ntrk] << ", " << m_p2pradlgh[m_ntrk]
			<< endmsg;

		m_ntrk++;

		if(m_ntrk >= m_ntrk->range().distance())
		{
			return;
			debug() << "Array overflow!----- !" << endmsg;
			getchar();
			m_ntrk=0;
		}

		writestatus = ntupleSvc()->writeRecord(ntname);
		if(! writestatus.isSuccess() )
		{
			debug() << "CANNOT Write m_matScan_Plane" << endmsg;
			return;
		}
	}

	return;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode RadLengthColl::finalize() {

	//m_matScan_PlaneDatas->Draw("cumradlgh:eta:phi>>hh","ID==11","profs");
	//TH1 * hh = (TH1 *)gPad->GetPrimitive("hh");

	debug() << "==> Finalize" << endmsg;
	return GiGaStepActionBase::finalize();
}



//=============================================================================
