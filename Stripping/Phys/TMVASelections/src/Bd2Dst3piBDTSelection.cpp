// $Id: $
// Include files
#include "LoKi/IHybridFactory.h"

#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

// local
#include "Bd2Dst3piBDTSelection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Bd2Dst3piBDTSelection
//
// 2013-03-15 Conor Fitzpatrick
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Bd2Dst3piBDTSelection )

	//=============================================================================
	// Standard constructor, initializes variables
	//=============================================================================
Bd2Dst3piBDTSelection::Bd2Dst3piBDTSelection( const std::string& type,
		const std::string& name,
		const IInterface* parent )
: base_class(type,name,parent),
	m_dist(NULL),
	m_dva(NULL),
	m_BDTReader(NULL),
	m_nVars(35),
	m_cut(-1.0)
{
	m_values = new float[m_nVars];

	declareProperty("BDTCut", m_cut, "BDT cut value");
	declareProperty("BDTWeightsFile", m_weightsFile, "Weights file");

}
//=============================================================================
// Destructor
//=============================================================================
Bd2Dst3piBDTSelection::~Bd2Dst3piBDTSelection()
{
	delete []m_values;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode Bd2Dst3piBDTSelection::initialize()
{
	// initialize the base class  (the first action)
	StatusCode sc = GaudiTool::initialize();
	if(sc.isFailure()) return sc;

	//====================================================================
	// Initialize DVAlg, etc
	//====================================================================
	m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
	if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
			StatusCode::FAILURE);

	m_dist = m_dva->distanceCalculator();
	if( !m_dist )
	{
		return Error("Unable to retrieve the IDistanceCalculator tool");
	}

	const char* m_variables[] = {"tau_LOKI_DOCAMAX", "B0_LOKI_BPVDIRA", "tau_LOKI_VFASPF_VCHI2VDOF", "tau_LOKI_VFASPF_VPCHI2", "D0_LOKI_MIPDV_PRIMARY", "tau_LOKI_BPVDIRA", "tau_LOKI_BPVVDR", "D0_LOKI_BPVVDR", "min(min((tau_pion0_LOKI_PT),(tau_pion1_LOKI_PT)),(tau_pion2_LOKI_PT))", "dz_kaon_LOKI_MIPDV_PRIMARY", "dz_pion_LOKI_MIPDV_PRIMARY", "min(min((tau_pion0_LOKI_TRPCHI2),(tau_pion1_LOKI_TRPCHI2)),(tau_pion2_LOKI_TRPCHI2))", "Dst_LOKI_BPVDIRA", "min(min((tau_pion0_LOKI_TRLH),(tau_pion1_LOKI_TRLH)),(tau_pion2_LOKI_TRLH))", "max(max((tau_pion0_LOKI_MIPDV_PRIMARY),(tau_pion1_LOKI_MIPDV_PRIMARY)),(tau_pion2_LOKI_MIPDV_PRIMARY))", "max(max((tau_pion0_LOKI_TRCHI2),(tau_pion1_LOKI_TRCHI2)),(tau_pion2_LOKI_TRCHI2))", "min(min((tau_pion0_LOKI_P),(tau_pion1_LOKI_P)),(tau_pion2_LOKI_P))", "B0_LOKI_VFASPF_VPCHI2", "D0_LOKI_BPVDIRA", "dz_pion_LOKI_BPVIPCHI2", "dst_pion_LOKI_TRPCHI2", "max(max((tau_pion0_LOKI_PT),(tau_pion1_LOKI_PT)),(tau_pion2_LOKI_PT))", "dz_pion_LOKI_PT", "dst_pion_LOKI_TRCHI2", "min(min((tau_pion0_LOKI_TRCHI2DOF),(tau_pion1_LOKI_TRCHI2DOF)),(tau_pion2_LOKI_TRCHI2DOF))", "dz_kaon_LOKI_PT", "min(min((tau_pion0_LOKI_TRCHI2),(tau_pion1_LOKI_TRCHI2)),(tau_pion2_LOKI_TRCHI2))", "tau_LOKI_DOCACHI2MAX", "max(max((tau_pion0_LOKI_P),(tau_pion1_LOKI_P)),(tau_pion2_LOKI_P))", "max(max((tau_pion0_LOKI_TRLH),(tau_pion1_LOKI_TRLH)),(tau_pion2_LOKI_TRLH))", "min(min((tau_pion0_LOKI_MIPDV_PRIMARY),(tau_pion1_LOKI_MIPDV_PRIMARY)),(tau_pion2_LOKI_MIPDV_PRIMARY))", "max(max((tau_pion0_LOKI_BPVIPCHI2),(tau_pion1_LOKI_BPVIPCHI2)),(tau_pion2_LOKI_BPVIPCHI2))", "tau_LOKI_BPVVDCHI2", "tau_LOKI_BPVIPCHI2", "min(min((tau_pion0_LOKI_MIPCHI2DV_PRIMARY),(tau_pion1_LOKI_MIPCHI2DV_PRIMARY)),(tau_pion2_LOKI_MIPCHI2DV_PRIMARY))" };

	//====================================================================
	// Initialize TMVA reader
	//====================================================================

	//m_BDTReader = new TMVA::Reader( "!Color:!Silent" );
	m_BDTReader = new TMVA::Reader( "Silent" );

	TString varName;
	for(unsigned int index = 0; index < m_nVars ; index++){
		varName = m_variables[index] ;
		m_BDTReader->AddVariable( varName, &m_values[index] );
	}

	const std::string methodName = "method";
	if(msgLevel(MSG::INFO)){info() << "Weights in: " << m_weightsFile << endmsg;}


	m_BDTReader->BookMVA( methodName, m_weightsFile );

	return sc;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode Bd2Dst3piBDTSelection::finalize()
{
	delete m_BDTReader;
	m_BDTReader = NULL;
	return GaudiTool::finalize();
}

//====================================================================
// BDT cut
//====================================================================
bool Bd2Dst3piBDTSelection::operator()(const LHCb::Particle* p) const
{
	if(0 == p) {
		Error("LHCb::Particle* points to NULL, return false").ignore();
		return false ;
	}

	if(!set(p)) return false;

	std::string methodName = "method";

	double BDTResValue = m_BDTReader->EvaluateMVA(methodName);

	if(msgLevel(MSG::DEBUG))
		debug() << " BDT value of this candidate is: " << BDTResValue << " "
			<< m_values[0] << " " << m_values[1] << " "
			<< m_values[2] << " " << m_values[3] << " "
			<< m_values[4] << " " << m_values[5] << " "
			<< m_values[6] << " " << m_values[7] << " "
			<< m_values[8] << " " << m_values[9] << " "
			<< m_values[10] << " " << m_values[11] << " "
			<< m_values[12] << " " << m_values[13] << " "
			<< m_values[14] << " " << m_values[15] << " "
			<< m_values[16] << " " << m_values[17] << " "
			<< m_values[18] << " " << m_values[19] << " "
			<< m_values[20] << " " << m_values[21] << " "
			<< m_values[22] << " " << m_values[23] << " "
			<< m_values[24] << " " << m_values[25] << " "
			<< m_values[26] << " " << m_values[27] << " "
			<< m_values[28] << " " << m_values[29] << " "
			<< m_values[30] << " " << m_values[31] << " "
			<< m_values[32] << " " << m_values[33] << " "
			<< m_values[34] << " " << endmsg;

	if(BDTResValue > m_cut) return true;

	return false;
}

//====================================================================
// All variables for BDT
//====================================================================
bool Bd2Dst3piBDTSelection::set (const LHCb::Particle* p) const{

	if(0 == p) return false;


	const LHCb::Particle* dst; 
	const LHCb::Particle* tau; 
	const LHCb::Particle* 	dz; 
	const LHCb::Particle* 	dstpi; 
	const LHCb::Particle* 	dzpi; 
	const LHCb::Particle* 	dzk; 
	const LHCb::Particle* 	taupi1; 
	const LHCb::Particle* 	taupi2; 
	const LHCb::Particle* 	taupi3;

	//Bd->Dst
	dst = LoKi::Child::child( p, 1 ); 
	//    tau
	tau = LoKi::Child::child(p, 2);
	
	//    Dst->pi D0
	dstpi = LoKi::Child::child( dst, 1);
	dz = LoKi::Child::child( dst, 2);

	//            D0->K pi
	dzk = LoKi::Child::child( dz, 1 );
	dzpi = LoKi::Child::child(dz, 2);
	//    tau->pipipi
	taupi1 = LoKi::Child::child(tau, 1);
	taupi2 = LoKi::Child::child(tau, 2);
	taupi3 = LoKi::Child::child(tau, 3);



	if( !dst||!tau||!dz||!dstpi||!dzpi||!dzk||!taupi1||!taupi2||!taupi3)
	{

		if(!dst){
			Error("Can't get the dst, check the inputs!").ignore();
		}else{
			info()<< " dst PID: " << dst->particleID() << " (413)"<< endmsg;
		}
		if(!tau){
			Error("Can't get the tau, check the inputs!").ignore();
		}else{
			info()<< ", tau PID: " << tau->particleID() << " (15)"<<endmsg;
		}
		if(!dz){
			Error("Can't get the dz, check the inputs!").ignore();
		}else{
			info()<<", dz PID: " << dz->particleID() << " (421)"<<endmsg;
		}
		if(!dstpi){
			Error("Can't get the dstpi, check the inputs!").ignore();
		}else{
			info()<< ", dstpi PID: " << dstpi->particleID() << " (211)"<<endmsg;
		}
		if(!dzpi){
			Error("Can't get the dzpi, check the inputs!").ignore();
		}else{
			info()<< ", dzpi PID: " << dzpi->particleID() << " (211)"<<endmsg;
		}
		if(!dzk){
			Error("Can't get the dzk, check the inputs!").ignore();
		}else{
			info()<< ", dzk PID: " << dzk->particleID() << " (321)"<<endmsg;
		}
		if(!taupi1){
			Error("Can't get the taupi1, check the inputs!").ignore();
		}else{

			info()<< ", taupi1 PID: " << taupi1->particleID() << " (211)"<<endmsg;
		}
		if(!taupi2){
			Error("Can't get the taupi2, check the inputs!").ignore();
		}else{
			info()<< ", taupi2 PID: " << taupi2->particleID() << " (211)"<<endmsg;
		}
		if(!taupi3){
			Error("Can't get the taupi3, check the inputs!").ignore();
		}else{
			info()<< ", taupi3 PID: " << taupi3->particleID() << " (211)"<<endmsg;
		}
		Error("Can't get all the particles, check the inputs!").ignore();
		return false;
	}

	if(msgLevel(MSG::DEBUG)) debug() 
		<< " B PID: " << p->particleID() << " (511)"
			<< " dst PID: " << dst->particleID() << " (413)"
			<< ", tau PID: " << tau->particleID() << " (15)"
			<< ", dz PID: " << dz->particleID() << " (421)"
			<< ", dstpi PID: " << dstpi->particleID() << " (211)"
			<< ", dzpi PID: " << dzpi->particleID() << " (211)"
			<< ", dzk PID: " << dzk->particleID() << " (321)"
			<< ", taupi1 PID: " << taupi1->particleID() << " (211)"
			<< ", taupi2 PID: " << taupi2->particleID() << " (211)"
			<< ", taupi3 PID: " << taupi3->particleID() << " (211)"
			<< endmsg;

	//	const LHCb::VertexBase* m_bpv = m_dva->bestVertex(p);

	//tau docamax
	const LoKi::ATypes::AFun m_AMAXDOCA = LoKi::Cuts::AMAXDOCA(m_dist);
	m_values[0] = m_AMAXDOCA(tau->daughtersVector());

	//B BPVDIRA
	const LoKi::Types::Fun m_BPVDIRA = LoKi::Cuts::BPVDIRA;
	m_values[1]=m_BPVDIRA(p);

	//tau_LOKI_VFASPF_VCHI2VDOF

	//	const LoKi::Types::Fun m_VCHI2VDOF = LoKi::Cuts::VFASPF(LoKi::Cuts::VCHI2/LoKi::Cuts::VDOF);
	const LoKi::Types::Fun m_VCHI2VDOF = LoKi::Cuts::VFASPF(LoKi::Cuts::VCHI2PDOF);
	m_values[2]=m_VCHI2VDOF(tau);

	//tau_LOKI_VFASPF_VPCHI2
	const LoKi::Types::Fun m_VPCHI2 =  LoKi::Cuts::VFASPF ( LoKi::Vertices::Chi2Prob() );
	m_values[3]=m_VPCHI2(tau);

	//"D0_LOKI_MIPDV_PRIMARY", 
	const LoKi::Types::Fun m_MIPDV  = LoKi::Cuts::MIPDV ( "" ) ;
	m_values[4]=m_MIPDV(dz);

	//"tau_LOKI_BPVDIRA"
	m_values[5]=m_BPVDIRA(tau);

	//"tau_LOKI_BPVVDR"
	const LoKi::Types::Fun m_BPVVDR = LoKi::Cuts::BPVVDR;
	m_values[6]=m_BPVVDR(tau);

	//"D0_LOKI_BPVVDR"
	m_values[7]=m_BPVVDR(dz);

	//"min(min((tau_pion0_LOKI_PT),(tau_pion1_LOKI_PT)),(tau_pion2_LOKI_PT))"
	const LoKi::Types::Fun m_PT = LoKi::Cuts::PT;
	m_values[8]= std::min(std::min(m_PT(taupi1),m_PT(taupi2)),m_PT(taupi3));

	//"dz_kaon_LOKI_MIPDV_PRIMARY"
	m_values[9]=m_MIPDV(dzk);

	//"dz_pion_LOKI_MIPDV_PRIMARY"
	m_values[10]=m_MIPDV(dzpi);

	//"min(min((tau_pion0_LOKI_TRPCHI2),(tau_pion1_LOKI_TRPCHI2)),(tau_pion2_LOKI_TRPCHI2))"
	const  LoKi::Types::Fun m_TRPCHI2 = LoKi::Cuts::TRPCHI2;
	m_values[11]= std::min(std::min(m_TRPCHI2(taupi1),m_TRPCHI2(taupi2)),m_TRPCHI2(taupi3));

	//"Dst_LOKI_BPVDIRA"
	m_values[12]=m_BPVDIRA(dst);

	//"min(min((tau_pion0_LOKI_TRLH),(tau_pion1_LOKI_TRLH)),(tau_pion2_LOKI_TRLH))"
	const LoKi::Types::Fun m_TRLH = LoKi::Cuts::TRLH;
	m_values[13]= std::min(std::min(m_TRLH(taupi1),m_TRLH(taupi2)),m_TRLH(taupi3));

	//"max(max((tau_pion0_LOKI_MIPDV_PRIMARY),(tau_pion1_LOKI_MIPDV_PRIMARY)),(tau_pion2_LOKI_MIPDV_PRIMARY))"
	m_values[14]=std::max(std::max(m_MIPDV(taupi1),m_MIPDV(taupi2)),m_MIPDV(taupi3));

	//"max(max((tau_pion0_LOKI_TRCHI2),(tau_pion1_LOKI_TRCHI2)),(tau_pion2_LOKI_TRCHI2))"
	const LoKi::Types::Fun m_TRCHI2 = LoKi::Cuts::TRCHI2;
	m_values[15]=std::max(std::max(m_TRCHI2(taupi1),m_TRCHI2(taupi2)),m_TRCHI2(taupi3));

	//"min(min((tau_pion0_LOKI_P),(tau_pion1_LOKI_P)),(tau_pion2_LOKI_P))"
	const LoKi::Types::Fun m_P = LoKi::Cuts::P;
	m_values[16]= std::min(std::min(m_P(taupi1),m_P(taupi2)),m_P(taupi3));

	//"B0_LOKI_VFASPF_VPCHI2"
	m_values[17]=m_VPCHI2(p);

	//"D0_LOKI_BPVDIRA"
	m_values[18]=m_BPVDIRA(dz);

	//"dz_pion_LOKI_BPVIPCHI2"
	const LoKi::Cuts::BPVIPCHI2 m_BPVIPCHI2;
	m_values[19]=m_BPVIPCHI2(dz);

	//"dst_pion_LOKI_TRPCHI2"
	m_values[20]=m_TRPCHI2(dstpi);

	//"max(max((tau_pion0_LOKI_PT),(tau_pion1_LOKI_PT)),(tau_pion2_LOKI_PT))"
	m_values[21]=std::max(std::max(m_PT(taupi1),m_PT(taupi2)),m_PT(taupi3));	

	//"dz_pion_LOKI_PT"
	m_values[22]=m_PT(dzpi);

	//"dst_pion_LOKI_TRCHI2"
	m_values[23]=m_TRCHI2(dstpi);

	//"min(min((tau_pion0_LOKI_TRCHI2DOF),(tau_pion1_LOKI_TRCHI2DOF)),(tau_pion2_LOKI_TRCHI2DOF))"
	const LoKi::Types::Fun m_TRCHI2DOF = LoKi::Cuts::TRCHI2DOF;
	m_values[24]=std::min(std::min(m_TRCHI2DOF(taupi1),m_TRCHI2DOF(taupi2)),m_TRCHI2DOF(taupi3));

	//"dz_kaon_LOKI_PT"
	m_values[25]=m_PT(dzk);

	//"min(min((tau_pion0_LOKI_TRCHI2),(tau_pion1_LOKI_TRCHI2)),(tau_pion2_LOKI_TRCHI2))"
	m_values[26]=std::min(std::min(m_TRCHI2(taupi1),m_TRCHI2(taupi2)),m_TRCHI2(taupi3));

	//"tau_LOKI_DOCACHI2MAX"
	const LoKi::Types::Fun m_DOCACHI2MAX = LoKi::Cuts::DOCACHI2MAX;
	m_values[27]=m_DOCACHI2MAX(tau);

	//"max(max((tau_pion0_LOKI_P),(tau_pion1_LOKI_P)),(tau_pion2_LOKI_P))"
	m_values[28]=std::max(std::max(m_P(taupi1),m_P(taupi2)),m_P(taupi3));

	//"max(max((tau_pion0_LOKI_TRLH),(tau_pion1_LOKI_TRLH)),(tau_pion2_LOKI_TRLH))"
	m_values[29]= std::max(std::max(m_TRLH(taupi1),m_TRLH(taupi2)),m_TRLH(taupi3));

	//"min(min((tau_pion0_LOKI_MIPDV_PRIMARY),(tau_pion1_LOKI_MIPDV_PRIMARY)),(tau_pion2_LOKI_MIPDV_PRIMARY))"
	m_values[30]=std::min(std::min(m_MIPDV(taupi1),m_MIPDV(taupi2)),m_MIPDV(taupi3));

	//"max(max((tau_pion0_LOKI_BPVIPCHI2),(tau_pion1_LOKI_BPVIPCHI2)),(tau_pion2_LOKI_BPVIPCHI2))"
	m_values[31]=std::max(std::max(m_BPVIPCHI2(taupi1),m_BPVIPCHI2(taupi2)),m_BPVIPCHI2(taupi3));

	//"tau_LOKI_BPVVDCHI2"
	const LoKi::Types::Fun m_BPVVDCHI2 = LoKi::Cuts::BPVVDCHI2;
	m_values[32]=m_BPVVDCHI2(tau);

	//"tau_LOKI_BPVIPCHI2"
	m_values[33]=m_BPVIPCHI2(tau);

	//"min(min((tau_pion0_LOKI_MIPCHI2DV_PRIMARY),(tau_pion1_LOKI_MIPCHI2DV_PRIMARY)),(tau_pion2_LOKI_MIPCHI2DV_PRIMARY))"
	const LoKi::Types::Fun  m_MIPCHI2DV  = LoKi::Cuts::MIPCHI2DV ( "" ) ;
	m_values[34]=std::min(std::min(m_MIPCHI2DV(taupi1),m_MIPCHI2DV(taupi2)),m_MIPCHI2DV(taupi3));


	return true;
}
