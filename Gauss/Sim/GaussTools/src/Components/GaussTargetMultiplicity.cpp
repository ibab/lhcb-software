// ============================================================================
#define GAUSS_Target_Multiplicity_CPP 1 
// ============================================================================
/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
/// Geant4 
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PrimaryParticle.hh"
#include "G4VPhysicalVolume.hh"
/// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/INTupleSvc.h"
#include "AIDA/IHistogram1D.h"
/// GiGa 
#include "GiGaCnv/GiGaPrimaryParticleInformation.h"
// GaussTools
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
/// local
#include "GaussTargetMultiplicity.h"
///

/** @file 
 *  
 *  Implementation of class GaussTargetMultiplicity
 *
 *  @author Silvia Miglioranzi
 * 	Modif: Luca Pescatore, Sajan Easo
 */

// ============================================================================
/// factory business 
// ============================================================================
DECLARE_TOOL_FACTORY( GaussTargetMultiplicity )


	// ============================================================================
	/** standard constructor 
	 *  @see GiGaTrackActionBase 
	 *  @see GiGaBase 
	 *  @see AlgTool 
	 *  @param type type of the object (?)
	 *  @param name name of the object
	 *  @param parent  pointer to parent object
	 */
	// ============================================================================
	GaussTargetMultiplicity::GaussTargetMultiplicity
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
	: GiGaTrackActionBase( type , name , parent )
, m_intVolumeName(), m_intVolumeString(), mp_targMat(), mp_physList(), mp_projPdgID(), mp_projEng(), mp_targThick()
{
	// declare own properties
	declareProperty( "InteractionVolumeName" , m_intVolumeName ) ;
	declareProperty( "InteractionVolumeString" , m_intVolumeString ) ;
	declareProperty( "ProjectilePdgID" , mp_projPdgID ) ;
	declareProperty( "ProjectileEnergy" , mp_projEng ) ;
	declareProperty( "TargetThickness" , mp_targThick ) ;
	declareProperty( "TargetMaterial" , mp_targMat ) ;
	declareProperty( "PhysicsList" , mp_physList ) ;
}


// ============================================================================
/// destructor 
// ============================================================================
GaussTargetMultiplicity::~GaussTargetMultiplicity(){}


// ============================================================================
/** initialize the track action  
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GaussTargetMultiplicity::initialize () 
{
	// initialize the base 
	StatusCode status = GiGaTrackActionBase::initialize() ;
	if( status.isFailure() ) 
	{ return Error("Could not intialize base class GiGaTrackActionBase!",
			status ) ; }
	///
	info() << "********** Initialize GaussTargetMultiplicity tool **********"
		<< endmsg;

	std::string filename = "/NTUPLES/FILE1";
	NTupleFilePtr ntfile(ntupleSvc(), filename);

	debug() << ntupleSvc() << "   " << filename << endmsg;

	if( ntfile ) {
		StatusCode status = StatusCode::SUCCESS;
		std::string ntname = filename + "/"+(this->name())+"/tree";
		NTuplePtr nt1(ntupleSvc(), ntname);

		if( !nt1 ) {
			nt1 = ntupleSvc()->book(ntname,CLID_ColumnWiseTuple,"Multiplicities");
			if( nt1 ) {
				if( status.isSuccess() ) 
				{
					nt1->addItem("ProjectilePdgID",                m_projPdgID);
					nt1->addItem("ProjectileEnergy",                 m_projEng);
					nt1->addItem("TargetThickness",                 m_targThick);
					nt1->addItem("TargetMaterial",                    m_targMat);
					nt1->addItem("PhysicsList",                      m_physList);
					nt1->addItem("TrackID",                           m_trackid);
					nt1->addItem("TrackPDG",                         m_trackpdg);
					nt1->addItem("TrackParent",                      m_trackpar);
					nt1->addItem("Produced_at_x",                       m_prodx);
					nt1->addItem("Produced_at_y",                       m_prody);
					nt1->addItem("Produced_at_z",                       m_prodz);
					nt1->addItem("CreatorProcessType",                 m_cptype);
					nt1->addItem("CreatorProcessSubType",             m_cpstype);
					nt1->addItem("Ended_at_x",                           m_endx);
					nt1->addItem("Ended_at_y",                           m_endy);
					nt1->addItem("Ended_at_z",                           m_endz);
					nt1->addItem("Momx",                                 m_momx);
					nt1->addItem("Momy",                                 m_momy);
					nt1->addItem("Momz",                                 m_momz);
					nt1->addItem("KinE",                                 m_kine);
					nt1->addItem("charge",                             m_charge);
					nt1->addItem("NDaugthers",             m_ndaugh,     0, 100);
					nt1->addItem("NInelChDaugthers", m_ninelchdaugh,     0, 100);
					nt1->addItem("IsInside",           m_ndaugh,     m_isinside);
					nt1->addItem("directParent",      m_ndaugh,     m_dirparent);
					nt1->addItem("ParentID",         m_ndaugh,       m_parentid);
					nt1->addItem("DaughterPDG",         m_ndaugh,      m_daupdg);
					nt1->addItem("DaughterMomx",       m_ndaugh,      m_daumomx);
					nt1->addItem("DaughterMomy",       m_ndaugh,      m_daumomy);
					nt1->addItem("DaughterMomz",       m_ndaugh,      m_daumomz);
					nt1->addItem("DaughterKinE",       m_ndaugh,      m_daukine);
					nt1->addItem("DaughterCharge",       m_ndaugh,  m_daucharge);
					nt1->addItem("EndingProcessType",   m_ndaugh,      m_eptype);
					nt1->addItem("EndingProcessSubType",   m_ndaugh , m_epstype);	
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


		AIDA::IHistogram1D* h1 = histoSvc()->book( "Z_end_point", 1, "Z end point", 2000, 0., 20000.);
		h_end_z = h1;
		AIDA::IHistogram1D* h2 = histoSvc()->book( "Z_end_point_inside", 1, "Z end point inside targets", 2000, 0., 20000.);
		h_end_z_inside = h2;
		AIDA::IHistogram1D* h3 = histoSvc()->book( "in_dau_mult", 1, "Inelastic multiplicity", 50, 0., 100.);
		h_in_dau_mult = h3;
		AIDA::IHistogram1D* h4 = histoSvc()->book( "in_chdau_mult", 1, "Charged inelastic multiplicity", 50, 0., 100.);
		h_in_chdau_mult = h4;
		AIDA::IHistogram1D* h5 = histoSvc()->book( "el_dau_mult", 1, "Elastic multiplicity",  5, 0, 5);
		h_el_dau_mult = h5;
		AIDA::IHistogram1D* h6 = histoSvc()->book( "pdg_dau_high", 1, "PDGID ions", 150000, -150000, 150000);
		h_pdg_dau_high = h6;
		AIDA::IHistogram1D* h7 = histoSvc()->book( "pdg_dau", 1, "PDGID", 8000, -4000, 4000);
		h_pdg_dau = h7;
		AIDA::IHistogram1D* h8 = histoSvc()->book( "end_proc", 1, "Ending process", 100, 0, 200);
		h_end_proc = h8;
		AIDA::IHistogram1D* h9 = histoSvc()->book( "dau_gammas_kine", 1, "Kinetic energy of gammas",100, 0., 15);
		h_dau_gammas_kine = h9;
		AIDA::IHistogram1D* h10 = histoSvc()->book( "dau_high_kine", 1, "Kinetic energy of inelastic ion daughters", 200, 0., 20000);
		h_dau_high_kine = h10;
		AIDA::IHistogram1D* h11 = histoSvc()->book( "dau_kine", 1, "Kinetic energy of daugthers", 200, 0., 20000);
		h_dau_kine = h11;
		AIDA::IHistogram1D* h12 = histoSvc()->book( "in_nch_perc", 1, "% of charged inelastic daughters", 50, 0, 1);
		h_in_nch_perc = h12;
		AIDA::IHistogram1D* h13 = histoSvc()->book( "in_chplus_perc", 1, "% of positively charged inelastic daughters", 50, 0, 1);
		h_in_chplus_perc = h13;
		AIDA::IHistogram1D* h14 = histoSvc()->book( "in_chminus_perc", 1, "% of negatively charged inelastic daughters", 50, 0, 1);
		h_in_chminus_perc = h14;
		AIDA::IHistogram1D* h15 = histoSvc()->book( "end_subproc", 1, "Ending subprocess", 100, 0, 200);
		h_end_subproc = h15;
	}
	else info() << "no ntfile" << endmsg;

	///
	return StatusCode::SUCCESS;
}


// ============================================================================
/** finalize the action object 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code
 */ 
// ============================================================================
StatusCode GaussTargetMultiplicity::finalize   () 
{ 
	// finilize the base 
	return GiGaTrackActionBase::finalize() ;
}


// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track opbject 
 */
// ============================================================================
void GaussTargetMultiplicity::PreUserTrackingAction ( const G4Track* track )
{
	// Is the track valid? Is tracking manager valid? 
	if( 0 == track || 0 == trackMgr() ) return;
}


// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track object 
 */
// ============================================================================
void GaussTargetMultiplicity::PostUserTrackingAction ( const G4Track* track ) 
{
	// Is the track valid? Is tracking manager valid?
	if( 0 == track || 0 == trackMgr())  return ;

	if(mp_projPdgID.empty() || mp_projEng.empty() || mp_targThick.empty() || mp_targMat.empty() || mp_physList.empty())
		info() << "=========>>> ATTENTION!! You need to set all properties!!" << endmsg;
	else
	{
		m_projPdgID  = mp_projPdgID[0];
		m_projEng    = mp_projEng[0];
		m_targThick  = mp_targThick[0];
		m_targMat    = ((char *)mp_targMat[0].c_str())[0];
		m_physList   = ((char *)mp_physList[0].c_str())[0];
	}

	int trid = track->GetTrackID();
	G4ParticleDefinition* partdefin = track->GetDefinition();
	std::string PartName = partdefin->GetParticleName();
	G4int PartPdgId = partdefin->GetPDGEncoding();

	const G4VProcess* proc = track->GetCreatorProcess();
	m_trackpar = track->GetParentID();

	debug() << "Particle Name " << PartName << endmsg;
	debug() << "Particle PDG ID " << PartPdgId << endmsg;
	debug() << "Parent ID " << m_trackpar << endmsg;
	debug() << "Track Volume " << track->GetVolume()->GetName() << endmsg;

	if (proc != 0)
	{
		std::string procName = proc->GetProcessName();
		G4int proctype = proc->GetProcessType();
		G4int procsubtype = proc->GetProcessSubType();
		debug() << "Creator process name " << procName << endmsg;
		debug() << "Creator process type " << proctype << endmsg;
		debug() << "Creator process sub-type " << procsubtype << endmsg;
		m_cptype    = proctype;
		m_cpstype   = procsubtype;
	}
	else
	{
		//it is a generator particle (proc == 0)
		m_cptype   = -2;
		m_cpstype   = -2;
		debug() << "It's generator particle!!!!!!" << endmsg;
	}


	m_momx = track->GetMomentum().x();
	m_momy =  track->GetMomentum().y();
	m_momz =  track->GetMomentum().z();
	m_kine =  track->GetKineticEnergy();

	const G4DynamicParticle* tdynpart = track->GetDynamicParticle();
	G4double charge = tdynpart->GetCharge();
	m_charge = charge;

	double prod_at_x =  track->GetVertexPosition().x();
	double prod_at_y =  track->GetVertexPosition().y();
	double prod_at_z =  track->GetVertexPosition().z();

	double ended_at_x = track->GetPosition().x();
	double ended_at_y = track->GetPosition().y();
	double ended_at_z = track->GetPosition().z();


	debug() << "Track momentum (x,y,z) "<<m_momx <<" "<<m_momy<<" "<<m_momz<< endmsg;
	debug() << "Track Ekine "<<m_kine<<" and charge "<<m_charge<< endmsg;
	debug() << "Produced at (x,y,z) "<<prod_at_x <<" "<<prod_at_y<<" "<<prod_at_z<< endmsg;
	debug() << "Ended at (x,y,z) "<<ended_at_x<<" "<<ended_at_y<<" "<<ended_at_z<< endmsg;


	// The following three lines commented to avoid compilation warnings. SE.
	//const G4Step* tstep = track->GetStep();
	//G4StepPoint* post = tstep->GetPostStepPoint();
	//G4StepPoint* pre  = tstep->GetPreStepPoint();

	if ( 0 != trackMgr()->GimmeSecondaries() ) 
	{
		G4TrackVector* childrens = trackMgr()->GimmeSecondaries();
		debug() << "--- daughters container size ---" << childrens->size() << endmsg;
		G4int numDaugh = 0;
		G4int numInelChDaugh = 0;

		int in_daughters = 0;
		int el_daughters = 0;
		int in_chDaughters = 0;
		int in_chPlusDaughters = 0;
		int in_chMinusDaughters = 0;

		//loop on daughters
		for (unsigned int index = 0; index < childrens->size(); ++index) 
		{
			G4Track* dtr = (*childrens)[index] ;
			if( 0 == dtr ) continue;

			if(dtr == track) debug() << " ATTENTION!!!! mother counted in daughter list"<< endmsg;


			G4ParticleDefinition* drpartdefin = dtr->GetDefinition();
			std::string drPartName = drpartdefin->GetParticleName();
			G4int DauPdgId = drpartdefin->GetPDGEncoding();

			m_daupdg[index] = DauPdgId;
			m_daumomx[index] = dtr->GetMomentum().x();
			m_daumomy[index] = dtr->GetMomentum().y();
			m_daumomz[index] = dtr->GetMomentum().z();
			double ekine = dtr->GetKineticEnergy();
			m_daukine[index] = ekine;

			const G4DynamicParticle* dynpart = dtr->GetDynamicParticle();
			G4double daucharge = dynpart->GetCharge();
			m_daucharge[index] = daucharge;

			//store che parentID of the daughter and flag if it is a direct connection
			if( dtr->GetParentID() != track->GetTrackID() ) 
			{
				debug() << "Could not reconstruct properly the parent! " << endmsg;
				debug() << " --- track id "<< track->GetTrackID() << endmsg;
				debug() << " --- daughter getparent id " << dtr->GetParentID() << endmsg;
				m_dirparent[index] = 0;
			}
			else m_dirparent[index] = 1;

			m_parentid[index] = dtr->GetParentID();


			// store the creator process of the daughter 
			const G4VProcess* drproc = dtr->GetCreatorProcess();
			G4int drproctype = drproc->GetProcessType();
			G4int drprocsubtype = drproc->GetProcessSubType();
			m_eptype[index]    = drproctype;
			m_epstype[index]   = drprocsubtype;

			//check if the daughter is produced inside the selected region
			m_isinside[index] = 0;
			int inside = checkIntRegion(dtr);
			m_isinside[index] = inside;


			debug() << "--- daughter  "<< numDaugh <<"-th" << endmsg;
			debug() << "daughter Particle Name " << drPartName << endmsg;
			debug() << "daughter Particle PDG ID " << DauPdgId << endmsg;
			debug() << " --- daughter isinside  " << m_isinside[index] << endmsg;
			debug() << "daughter Particle Momentum - x "<< m_daumomx[index]/MeV<<" MeV"<< endmsg;
			debug() << "daughter Particle Momentum - y "<< m_daumomy[index]/MeV<<" MeV"<< endmsg;
			debug() << "daughter Particle Momentum - z "<< m_daumomz[index]/MeV<<" MeV"<< endmsg;
			debug() << "daughter Kinetic Energy "<< m_daukine[index]/MeV<<" MeV"<< endmsg;
			debug() << "daughter Charge "<< daucharge << endmsg;
			debug() << "daughter track pointer "<< dtr << endmsg;
			debug() << "daughter creator process  "<< drproc->GetProcessName()<< endmsg;
			debug() << "daughter creator type  "<< drproctype<< endmsg;
			debug() << "daughter parent id "<< dtr->GetParentID()<< endmsg;
			debug() << " --- daughter creator subtype  "<< drprocsubtype<< endmsg;
			debug() << " --- daughter diparent  "<< m_dirparent[index]<< endmsg;


			if(daucharge != 0 && drprocsubtype == 121) numInelChDaugh++;
			numDaugh++;


			//Make plots instead of only ntuples allows to use the oracle database, the ntuples still contain the full information

			if( m_cptype==-2 )   //Takes only primary particles and analyses their daugthers
			{
				h_end_z->fill(m_endz);

				if (inside == 0) continue;

				h_end_z_inside->fill(m_endz);

				h_end_proc->fill(drprocsubtype);

				if (drprocsubtype == 111) 
				{
					el_daughters++;
				}
				else if (drprocsubtype == 121)
				{
					in_daughters++;
					if(daucharge == 0) in_chDaughters++;
					if(daucharge > 0) in_chPlusDaughters++;
					if(daucharge < 0) in_chMinusDaughters++;

					if(DauPdgId>1000000000)
					{
						h_pdg_dau_high->fill(DauPdgId-1000000000);
						h_dau_kine->fill(ekine);
						h_dau_high_kine->fill(ekine);
					}
					else
					{
						h_pdg_dau->fill(DauPdgId);
						h_dau_kine->fill(ekine);
						if( DauPdgId == 22 ){h_dau_gammas_kine->fill(ekine);}
					}
				}

				h_end_subproc->fill(drprocsubtype);	
			}
		}

		m_ninelchdaugh   = numInelChDaugh;
		m_ndaugh   = numDaugh;

		if(el_daughters > 0) h_el_dau_mult->fill(el_daughters);
		if(in_daughters > 0) h_in_dau_mult->fill(in_daughters);
		if(in_chDaughters > 0) h_in_chdau_mult->fill(in_chDaughters);

		if(in_daughters > 0) 
		{
			h_in_nch_perc->fill((double) in_chDaughters / in_daughters);
			h_in_chplus_perc->fill((double) in_chPlusDaughters / in_daughters);
			h_in_chminus_perc->fill((double) in_chMinusDaughters / in_daughters);
		}
	}

	m_trackid  = trid;
	m_trackpdg = PartPdgId;
	m_prodx    = prod_at_x;
	m_prody    = prod_at_y;
	m_prodz    = prod_at_z;
	m_endx    = ended_at_x;
	m_endy    = ended_at_y;
	m_endz    = ended_at_z;

	m_ntuple->write();

	return;

}


// ==============================================================================
// Check if daughters is created inside the specified Region
// ==============================================================================
int GaussTargetMultiplicity::checkIntRegion( const G4Track * dtr ){

	//Physics and Logical Volume names
	std::string volname = dtr->GetVolume()->GetName();
	G4LogicalVolume* logvol = dtr->GetVolume()->GetLogicalVolume();
	std::string lvolname = logvol->GetName(); 
	// Material
	G4Material* mater = logvol->GetMaterial();
	std::string matname = mater->GetName();

	debug() << "daughter track Phys Volume " << volname << endmsg;
	debug() << "daughter track Logical Volume " << lvolname << endmsg;
	debug() << "daughter track Vol Material " << matname << endmsg;

	int insider = 0;

	//check if the daugther Physical and Logical volume names and material are those specified by the user via properties
	if(std::find(m_intVolumeName.begin(), m_intVolumeName.end(),volname) != m_intVolumeName.end()) 
	{
		insider = 1;
	}
	else if(std::find(m_intVolumeName.begin(), m_intVolumeName.end(), lvolname) != m_intVolumeName.end()) 
	{
		insider = 1;
	}
	else if(std::find(mp_targMat.begin(), mp_targMat.end(), matname) != mp_targMat.end()) 
	{
		insider = 1;
	}

	//check if the daughter volume name contains the substring specified by the user via properties
	for( std::vector<std::string>::iterator it=m_intVolumeString.begin(); it<m_intVolumeString.end(); ++it ) 
	{
		std::string::size_type pos;
		//check the Physical Volume name
		pos = volname.find(*it);
		if (pos != std::string::npos)
		{
			debug() << "string found in phys volume name"<< endmsg;
			insider = 1;
		}
		//check the Logical Volume name
		pos = lvolname.find(*it);
		if (pos != std::string::npos)
		{
			debug() << "string found in logical volume name"<< endmsg;
			insider = 1;
		}
	}

	return insider;
}

