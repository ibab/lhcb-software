/*
 * Attempt to save Decays into ROOT Tree.
 *
 * Use example
 *
 *
 * AZ SHM, PI 2008
 */

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <map>

#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>

#include <memory>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/list_of.hpp>

#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>
#include <TClassTable.h>
#include <TH1F.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TROOT.h>
#include <TStyle.h>



#include "./Zoo/Zoo.h"

typedef ROOT::Math::SVector<double, 7> Vec7;
typedef ROOT::Math::SVector<double, 3> Vec3;
typedef std::vector<std::string> StringArrayProperty;

int main(int argc, char** argv)
{


    //Define TChain that will later contain all the tuple files. By adding more
    //tuple files it is possible to read in more than one file, e.g. from
    //splitted sub-jobs
    boost::shared_ptr<TChain> chain(new TChain("Options"));
    chain->Add("$ZOOWRITERROOT/options/MC.root");

    StringArrayProperty      *m_L0Name = 0;
    StringArrayProperty      *m_Hlt1Name = 0;
    StringArrayProperty      *m_Hlt2Name = 0;
    StringArrayProperty      *m_sel_names = 0;
    StringArrayProperty      *m_sel_collections = 0; 
    StringArrayProperty      *m_treefit_names = 0;
    std::vector<int>         *m_treefit_constraints = 0;
    std::vector<int>         *m_taggingList = 0;
    std::vector<int>         *m_triggerList = 0;
    std::vector<int>         *m_backgroundList = 0;
    std::string              *m_filename = 0;
    std::string              *m_treename = 0;
    bool                     m_writeMC;
    std::vector<int>         *m_MCList = 0;
    bool                     m_writeDLL;
    bool                     m_writeStates;
    StringArrayProperty      *m_linkToList = 0;
    StringArrayProperty      *m_linkFromList = 0;
    std::string              *m_PVReFitterName = 0;	

    bool			 m_writeTrackInfo;
    std::vector<short>       *m_extraInfoList = 0;
    bool                     m_writeHitPattern;
    bool                     m_writeExpectedHitPattern;
    bool                     m_writeCollectedHitPattern;
    bool			 m_writeOccupancies;
    bool			 m_intelligentPV;
    bool			 m_secondIpSig;
    bool			 m_onlyTreefitter;
    int		         m_minTracksPV;

    /// pids for which to write a ZooPackedParticle info block, negative values mean all
    std::vector<int>         *m_covarianceList = 0;
    /// pids for which to write a ZooPackedStates info block, negative values mean all
    std::vector<int>         *m_packedStatesList = 0;
    /// extrapolate the states to the given z (if true) or take closest
    bool                     m_packedStatesExtrapolate;
    /// write packed state at poca to z axis
    bool			 m_packedStateAtPocaToZAxis;
    /// write packed states closest to the given Z positions
    std::vector<double>      *m_packedStatesZList = 0;
    /// should ZooWriter attempt to make emergency PV if there is no PV
    bool			 m_makeEmergencyPV;
    /// write LHCbIDs
    bool m_writeLHCbIDs;
    /// write Generator level event info
    bool m_writeMCGenEventInfo;
    ///Write MC track multiplicity in acceptance
    std::vector<double> *m_mcAcceptanceEtaRegion = 0;
    double m_mcAcceptanceMinP;
    bool m_mcAcceptanceCharged;
    bool m_mcAcceptanceNeutral;
    double m_mcAcceptanceMinCtau;
    //Assigning branch names in the tuple to the pointers
   
    std::cout << 1 ; 
    chain->SetBranchAddress( "InputCollections",     &m_sel_collections);
    chain->SetBranchAddress( "DecayName" ,           &m_sel_names);
    chain->SetBranchAddress( "WriteMC",              &m_writeMC);
    chain->SetBranchAddress( "MCList",               &m_MCList);
    chain->SetBranchAddress( "WriteDLL",             &m_writeDLL);
    chain->SetBranchAddress( "IntelligentPV",        &m_intelligentPV);
    chain->SetBranchAddress( "SecondIpSig",          &m_secondIpSig);
    chain->SetBranchAddress( "OnlyTreefitter",       &m_onlyTreefitter);
    chain->SetBranchAddress( "MinTracksPV",          &m_minTracksPV);

    chain->SetBranchAddress( "TaggingList",          &m_taggingList);
    chain->SetBranchAddress( "BackgroundList",       &m_backgroundList);
    chain->SetBranchAddress( "TriggerList",          &m_triggerList);

    chain->SetBranchAddress( "L0SelectionName" ,     &m_L0Name);
    chain->SetBranchAddress( "HLT1SelectionName" ,   &m_Hlt1Name);
    chain->SetBranchAddress( "HLT2SelectionName" ,   &m_Hlt2Name);

    chain->SetBranchAddress( "Filename"   ,          &m_filename);
    chain->SetBranchAddress( "Tree"       ,          &m_treename);

    chain->SetBranchAddress( "CovarianceList",       &m_covarianceList);

    chain->SetBranchAddress( "TreefitName",     &m_treefit_names);
    chain->SetBranchAddress( "TreefitMassConstraints", &m_treefit_constraints);

    chain->SetBranchAddress( "PVReFitterName",       &m_PVReFitterName);
    chain->SetBranchAddress( "LinkToList",           &m_linkToList);
    chain->SetBranchAddress( "LinkFromList",         &m_linkFromList);
    chain->SetBranchAddress( "WriteHitPattern",      &m_writeHitPattern);
    chain->SetBranchAddress( "WriteExpectedHitPattern",  &m_writeExpectedHitPattern);
    chain->SetBranchAddress( "WriteCollectedHitPattern", &m_writeCollectedHitPattern);
    chain->SetBranchAddress( "ExtraInfoList",	     &m_extraInfoList);
    chain->SetBranchAddress( "WriteTrackInfo",       &m_writeTrackInfo);
    chain->SetBranchAddress( "PackedStatesList",     &m_packedStatesList);
    chain->SetBranchAddress( "PackedStatesExtrapolate", &m_packedStatesExtrapolate);
    chain->SetBranchAddress( "PackedStatesZList",     &m_packedStatesZList);
    chain->SetBranchAddress( "PackedStateAtPocaToZAxis", &m_packedStateAtPocaToZAxis);
    chain->SetBranchAddress( "MakeEmergencyPV",      &m_makeEmergencyPV);
    chain->SetBranchAddress( "WriteOccupancies",     &m_writeOccupancies);
    chain->SetBranchAddress( "WriteLHCbIDs",         &m_writeLHCbIDs);
    chain->SetBranchAddress( "WriteMCGenEventInfo",  &m_writeMCGenEventInfo);
    chain->SetBranchAddress( "MCAcceptanceEtaRegion", &m_mcAcceptanceEtaRegion);
    chain->SetBranchAddress( "MCAcceptanceMinP",     &m_mcAcceptanceMinP);
    chain->SetBranchAddress( "MCAcceptanceCharged",  &m_mcAcceptanceCharged);
    chain->SetBranchAddress( "MCAcceptanceNeutral",  &m_mcAcceptanceNeutral);
    chain->SetBranchAddress( "MCAcceptanceMinCtau",  &m_mcAcceptanceMinCtau);

    TH1::AddDirectory(kFALSE);
    std::cout << 2 << std::endl;
    std::cout << chain->GetEntries() << std::endl; 

    //loop over all entries of our chain, i.e. over all events
    for(int i=0;i<chain->GetEntries();i++){
	//get event number i
	std::cout << 3 << std::endl;

	chain->GetEntry(i);

	std::cout << "WriteMC " << m_writeMC << std::endl;
	std::cout << "MCList " ;
	for(int e=0;e<(*m_MCList).size();++e)
	    std::cout << (*m_MCList)[e] << " ";
	std::cout << std::endl;
	std::cout << "WriteDLL " << m_writeDLL << std::endl;
	std::cout << "IntelligentPV " << m_intelligentPV << std::endl;
	std::cout << "SecondIpSig " << m_secondIpSig << std::endl;
	std::cout << "OnlyTreefitter " << m_onlyTreefitter << std::endl;
	std::cout << "MinTracksPV " << m_minTracksPV << std::endl;
	std::cout << "TaggingList " ;
	for(int e=0;e<(*m_taggingList).size();++e)
	    std::cout << (*m_taggingList)[e] << " ";
	std::cout << std::endl;
	std::cout << "BackgroundList " ;
	for(int e=0;e<(*m_backgroundList).size();++e)
	    std::cout << (*m_backgroundList)[e] << " ";
	std::cout << std::endl;
	std::cout << "TriggerList " ;
	for(int e=0;e<(*m_triggerList).size();++e)
	    std::cout << (*m_triggerList)[e] << " ";
	std::cout << std::endl;
	std::cout << "Filename " << (*m_filename) << std::endl;
	std::cout << "Tree " << (*m_treename) << std::endl;
	std::cout << "CovarianceList " ;
	for(int e=0;e<(*m_covarianceList).size();++e)
	    std::cout << (*m_covarianceList)[e] << " ";
	std::cout << std::endl;
	std::cout << "TreeFitMassConstraints " ;
	for(int e=0;e<(*m_treefit_constraints).size();++e)
	    std::cout << (*m_treefit_constraints)[e] << " ";
	std::cout << std::endl;
	std::cout << "WriteHitPattern " << m_writeHitPattern << std::endl;
	std::cout << "WriteExpectedHitPattern " << m_writeExpectedHitPattern << std::endl;
	std::cout << "WriteCollectedHitPattern " << m_writeCollectedHitPattern << std::endl;
	std::cout << "ExtraInfoList " ;
	for(int e=0;e<(*m_extraInfoList).size();++e)
	    std::cout << (*m_extraInfoList)[e] << " ";
	std::cout << std::endl;
	std::cout << "WriteTrackInfo " << m_writeTrackInfo << std::endl;
	std::cout << "PackedStatesList " ;
	for(int e=0;e<(*m_packedStatesList).size();++e)
	    std::cout << (*m_packedStatesList)[e] << " ";
	std::cout << std::endl;
	std::cout << "PackedStatesExtrapolate " << m_packedStatesExtrapolate << std::endl;
	std::cout << "PackedStatesZList " ;
	for(int e=0;e<(*m_packedStatesZList).size();++e)
	    std::cout << (*m_packedStatesZList)[e] << " ";
	std::cout << std::endl;
	std::cout << "PackedStateAtPocaToZAxis " << m_packedStateAtPocaToZAxis << std::endl;
	std::cout << "MakeEmergencyPV " << m_makeEmergencyPV << std::endl;
	std::cout << "WriteOccupancies " << m_writeOccupancies << std::endl;
	std::cout << "WriteLHCbIDs " << m_writeLHCbIDs << std::endl;
	std::cout << "InputLocations " ;
	for(int e=0;e<(*m_sel_collections).size();++e)
	    std::cout << (*m_sel_collections)[e] << " ";
	std::cout << std::endl;
	std::cout << "DecayNames " ;
	for(int e=0;e<(*m_sel_names).size();++e)
	    std::cout << (*m_sel_names)[e] << " ";
	std::cout << std::endl;
	std::cout << "L0SelectionName " ;
	for(int e=0;e<(*m_L0Name).size();++e)
	    std::cout << (*m_L0Name)[e] << " ";
	std::cout << std::endl;
	std::cout << "Hlt1SelectionName " ;
	for(int e=0;e<(*m_Hlt1Name).size();++e)
	    std::cout << (*m_Hlt1Name)[e] << " ";
	std::cout << std::endl;
	std::cout << "Hlt2SelectionName " ;
	for(int e=0;e<(*m_Hlt2Name).size();++e)
	    std::cout << (*m_Hlt2Name)[e] << " ";
	std::cout << std::endl;
	std::cout << "TreefitName " ;
	for(int e=0;e<(*m_treefit_names).size();++e)
	    std::cout << (*m_treefit_names)[e] << " ";
	std::cout << std::endl;
	std::cout << "LinkFromList " ;
	for(int e=0;e<(*m_linkFromList).size();++e)
	    std::cout << (*m_linkFromList)[e] << " ";
	std::cout << std::endl;
	std::cout << "LinkToList " ;
	for(int e=0;e<(*m_linkToList).size();++e)
	    std::cout << (*m_linkToList)[e] << " ";
	std::cout << std::endl;
    }

    return 0;
}

// vim:sw=4:tw=78:ft=cpp
