// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/L0DUReport.h"
#include "GaudiKernel/SystemOfUnits.h"
// local
#include "B2DHPreselMvaUtilityTool.h"

using namespace LHCb ;
using namespace Gaudi ;


//-----------------------------------------------------------------------------
// Implementation file for class : B2DHPreselMvaUtilityTool
//
// 2010-07-16 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( B2DHPreselMvaUtilityTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
B2DHPreselMvaUtilityTool::B2DHPreselMvaUtilityTool( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
: GaudiTool ( type, name , parent ),
  m_basicPhysStringPath(std::string("Phys/")),
  m_particleLocation(std::string("Particles")),
  m_P2RefitPVSortedRel(std::string("P2ReFitPVSortedRelations")),
  m_P2RefitPVRel(std::string("Particle2VertexRelations")),
  m_bs2DsK_Recon_Dir(std::string("PreSelCombBs2DsK/")),
  m_bs2DsPi_Recon_Dir(std::string("PreSelCombBs2DsPi/")),
  m_bs2DsK_PvRefitAlgName(std::string("PVReFitterAlgBs2DsK/")),
  m_bs2DsPi_PvRefitAlgName(std::string("PVReFitterAlgBs2DsPi/")),
  m_bsParticleDir(std::vector<std::string>(0)),
  m_particle2RefittedSortedPVRelationsLoc(std::vector<std::string>(0)),
  m_particle2RefittedPVRelationsLoc(std::vector<std::string>(0)),
  m_currentBsLocation(std::vector<std::string>(0)),
  m_currentStdPVLocation(std::string("Rec/Vertex/Primary")),
  m_pathtoWeightsFile_Bs2DsK(std::string("myDs2DsKWeightsDirectory/")),
  m_pathtoWeightsFile_Bs2DsPi(std::string("myDs2DsPiWeightsDirectory/")),
  m_fisherWeightsLocalFileName_Bs2DsK(std::string("myBs2DsKWeightsFileLocalName")),
  m_fisherWeightsLocalFileName_Bs2DsPi(std::string("myBs2DsPiWeightsFileLocalName")),
  m_fisherWeightsFullFileName(std::vector<std::string>(0))
{
  declareInterface<IB2DHPreselMvaUtilityTool>(this);
  declareProperty("PhysicsPathString",m_basicPhysStringPath);
  declareProperty("ParticleLocation",m_particleLocation);
  declareProperty("RefittedPVSortedRelationLoc",m_P2RefitPVSortedRel);
  declareProperty("RefittedPVRelationLoc", m_P2RefitPVRel);

  declareProperty("StandardPVLocation",m_currentStdPVLocation);

  declareProperty("Bs2DsKReconDir",   m_bs2DsK_Recon_Dir);
  declareProperty("Bs2DsPiReconDir",  m_bs2DsPi_Recon_Dir);

  declareProperty("Bs2DsKPVRefitAlgName", m_bs2DsK_PvRefitAlgName);
  declareProperty("Bs2DsPiPVRefitAlgName", m_bs2DsPi_PvRefitAlgName);

  declareProperty("PathToWeightsFileForBs2DsK", m_pathtoWeightsFile_Bs2DsK);
  declareProperty("FisherWeightsLocalFileNameForBs2DsK",
                  m_fisherWeightsLocalFileName_Bs2DsK);
  declareProperty("PathToWeightsFileForBs2DsPi", m_pathtoWeightsFile_Bs2DsPi);
  declareProperty("FisherWeightsLocalFileNameForBs2DsPi",
                  m_fisherWeightsLocalFileName_Bs2DsPi);
}
//=============================================================================
// Destructor
//=============================================================================
B2DHPreselMvaUtilityTool::~B2DHPreselMvaUtilityTool() {}

//=============================================================================
StatusCode B2DHPreselMvaUtilityTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  initializeDataSelectionPathStrings();
  initializeStdParamForMvaSel();

  return sc;
}

void B2DHPreselMvaUtilityTool::initializeDataSelectionPathStrings()
{

  m_num_Bs_Phys_Channels_Used=2;  // for now setup for only 2 channels.



  m_bsParticleDir.clear();
  m_particle2RefittedSortedPVRelationsLoc.clear();
  m_currentBsLocation.clear();

  m_bsParticleDir.resize(m_num_Bs_Phys_Channels_Used);
  m_particle2RefittedSortedPVRelationsLoc.resize(m_num_Bs_Phys_Channels_Used);
  m_particle2RefittedPVRelationsLoc.resize(m_num_Bs_Phys_Channels_Used);

  m_currentBsLocation.resize(m_num_Bs_Phys_Channels_Used);

  m_fisherWeightsFullFileName.clear();
  m_fisherWeightsFullFileName.resize(m_num_Bs_Phys_Channels_Used);

  for(int i=0; i< m_num_Bs_Phys_Channels_Used; ++i ){

    if( i == 0 ) { // set Bs2DsK as physics channel number 0
      // now for Bs2DsK
      m_bsParticleDir[i]=m_bs2DsK_Recon_Dir;
      m_fisherWeightsFullFileName[i]= m_pathtoWeightsFile_Bs2DsK +
        m_fisherWeightsLocalFileName_Bs2DsK;
      //      m_particle2RefittedPVRelationsLoc[i]=
      //   m_basicPhysStringPath+ m_bs2DsK_PvRefitAlgName +m_P2RefitPVRel; // changed from dv25r6

      m_particle2RefittedPVRelationsLoc[i]=
        m_basicPhysStringPath+  m_bsParticleDir[i] +m_P2RefitPVRel;



    }else if (i == 1 ) { // set Bs2DsPi as physics channel number 1
      // now for Bs2DsPi

      m_bsParticleDir[i]= m_bs2DsPi_Recon_Dir;
      m_fisherWeightsFullFileName[i]=m_pathtoWeightsFile_Bs2DsPi +
        m_fisherWeightsLocalFileName_Bs2DsPi;

      //      m_particle2RefittedPVRelationsLoc[i]=
      //   m_basicPhysStringPath+ m_bs2DsPi_PvRefitAlgName +m_P2RefitPVRel; //changed from dv25r6

      m_particle2RefittedPVRelationsLoc[i]=
        m_basicPhysStringPath+ m_bsParticleDir[i] +m_P2RefitPVRel;

    }
    m_particle2RefittedSortedPVRelationsLoc[i]=
      m_basicPhysStringPath+ m_bsParticleDir[i]+m_P2RefitPVSortedRel;

    m_currentBsLocation[i]=m_basicPhysStringPath+m_bsParticleDir[i]+m_particleLocation;



  }



}
void B2DHPreselMvaUtilityTool::initializeStdParamForMvaSel() {

  /*
    The following does not work yet. Hence a temporary workaround is implemented for now.
    IParticlePropertySvc* kppSvc= svc<IParticlePropertySvc>("ParticlePropertySvc", true);
    const ParticleProperty* aDsProperty = kppSvc->find("Ds");
    m_pdgDsValue=  aDsProperty->pdgID();
    m_massStdDsValue = aDsProperty->mass();
    const ParticleProperty* aKaonProperty = kppSvc->find("kaon");
    m_pdgKaonValue = aKaonProperty->pdgID() ;
    const ParticleProperty* aBsProperty = kppSvc->find("Bs");
    m_massStdBsValue = aBsProperty->mass();
    const ParticleProperty* aPionProperty = kppSvc->find("pion");
    m_pdgKaonValue = aPionProperty->pdgID() ;
  */
  m_pdgDsValue=431;
  m_massStdDsValue=1968.5*Gaudi::Units::MeV;
  m_pdgKaonValue= 321;
  m_massStdBsValue=5369.6*Gaudi::Units::MeV;
  m_pdgPionValue=211;
  m_pdg_MinDValue=400;
  m_pdg_MaxDValue=450;

}
bool B2DHPreselMvaUtilityTool::getL0Decision ()
{

  const LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);

  bool curL0 = (l0) ? l0->decision() : true;  // check if l0 report exists.

  return curL0;
}

int B2DHPreselMvaUtilityTool::FindBestCandidateInEv(std::vector<int> aCandIndex,
                                                    std::vector<double> aIps_Bs, std::vector<double>aprob_Bs, int SelOption) {

  // For now the highest fisher prob value is used to get the best candidate.
  // This can be improved by looking at smallest BsIps  if needed.
  // here SelOption=0 returns the best prob value as the best candidate.
  //      SelOption=1 returns the smallest ips as the best candidate.

  double aProbSave=0.0;

  double aIpsSave=10000.0;
  int abestIndexFromIps=-100;
  int abestIndexFromProb=-100;
  int aNumCand = (int) aCandIndex.size();


  if(aNumCand ==1 ) {
    abestIndexFromIps=aCandIndex[0];
    abestIndexFromProb=aCandIndex[0];

  }else if ( aNumCand > 1 )  {

    for (int ic=0;ic <aNumCand;  ++ic) {
      int aIndex= aCandIndex[ic];
      if(aprob_Bs[aIndex] > aProbSave ) {
        aProbSave=aprob_Bs[aIndex];
        abestIndexFromProb=aIndex;
      }

    }


    // for check on Bs IPs.

    for (int id=0; id < aNumCand;  ++id) {
      int bIndex = aCandIndex[id];
      if(aIps_Bs[id] < aIpsSave ) {
        aIpsSave=   aIps_Bs[id];
        abestIndexFromIps=bIndex;
      }

    }

  } // end of check on numcand.

  int abestIndex=abestIndexFromProb;
  if(SelOption == 1) abestIndex=abestIndexFromIps;


  verbose()<< "best cand check numcand prob ips "<<aNumCand<<"  "
           << abestIndex <<"   "<<abestIndexFromIps<<endmsg;



  return abestIndex;


}
std::string B2DHPreselMvaUtilityTool::fisherWeightsFullFileName(int channel ){
  std::string aName = m_fisherWeightsFullFileName[0];
  if(( channel >0) && (channel <  m_num_Bs_Phys_Channels_Used) ) {
    aName =  m_fisherWeightsFullFileName[channel];

  }else if (channel >= m_num_Bs_Phys_Channels_Used ) {
    warning() <<" This Channel not available for selection "
              <<channel<<endmsg;


  }
  info()<<" test input file name "<<channel <<"  "<<aName<<endmsg;

  return aName;


}


