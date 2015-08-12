// $Id: L0SelReportsMaker.cpp,v 1.2 2010-06-25 13:13:24 tskwarni Exp $
// #define DEBUGCODE
// Include files
//#include "boost/lexical_cast.hpp"
#include <boost/functional/hash.hpp>

// from Gaudi
#include "GaudiKernel/StatusCode.h"

#include "Event/HltSelReports.h"
#include "Event/HltObjectSummary.h"

#include "Event/L0DUBase.h"
#include "Event/L0DUReport.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0CaloCandidate.h"


// local
#include "L0SelReportsMaker.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : L0SelReportsMaker
//
// 2010-06-23 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0SelReportsMaker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  L0SelReportsMaker::L0SelReportsMaker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputL0DUReportLocation",
                  m_inputL0DUReportLocation= LHCb::L0DUReportLocation::Default );

  declareProperty("OutputHltSelReportsLocation",
                  m_outputHltSelReportsLocation= "HltLikeL0/SelReports" );

  // recognizable selection conditions
  m_selectionConditions.push_back( "Muon1(Pt)" );
  m_selectionConditions.push_back( "Muon2(Pt)" );
  m_selectionConditions.push_back( "Hadron(Et)" );
  m_selectionConditions.push_back( "Electron(Et)" );
  m_selectionConditions.push_back( "Photon(Et)" );
  m_selectionConditions.push_back( "LocalPi0(Et)" );
  m_selectionConditions.push_back( "GlobalPi0(Et)" );
  m_selectionConditions.push_back( "DiMuon(Pt)" );
  m_selectionConditions.push_back( "Muon12(Pt)" );

}

//=============================================================================
// Destructor
//=============================================================================
L0SelReportsMaker::~L0SelReportsMaker() {}


//=============================================================================
// Main execution
//=============================================================================
StatusCode L0SelReportsMaker::execute() {

  if ( msgLevel(MSG::VERBOSE) ) verbose() << "==> Execute" << endmsg;

  //  L0 Decision Unit Report
  L0DUReport* pL0DUReport = 0;
  //  L0 Decision Unit Report
  //       try without RootInTES first
  if( exist<L0DUReport>( m_inputL0DUReportLocation.value(), false ) ){
    pL0DUReport = get<L0DUReport>( m_inputL0DUReportLocation.value(), false );
    //       now try at RootInTES location
  } else if( exist<L0DUReport>( m_inputL0DUReportLocation ) ){
    pL0DUReport = get<L0DUReport>( m_inputL0DUReportLocation.value());
  }
  if( !pL0DUReport ){return Error( " No L0DUReport at " + m_inputL0DUReportLocation.value(), StatusCode::SUCCESS, 10 );}
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " L0 global decision= " << pL0DUReport->decision() << endmsg;

  // create output container and put it on TES
  HltSelReports* outputSummary = new HltSelReports();
  put( outputSummary, m_outputHltSelReportsLocation );

  // create output container for Object Summaries and put it on TES
  m_objectSummaries = new HltObjectSummary::Container();
  put( m_objectSummaries, m_outputHltSelReportsLocation.value() + "/Candidates" );

  // translate all L0MuonCandidates into Object Summaries


  // BCSU is the full list of candidates; while Default is 6 best; try BCSU first
  L0MuonCandidates* pL0MuonCandidates = 0;
  if( exist<L0MuonCandidates>( L0MuonCandidateLocation::BCSU, false ) ){
    pL0MuonCandidates = get<L0MuonCandidates>( L0MuonCandidateLocation::BCSU, false );
  } else if( exist<L0MuonCandidates>( L0MuonCandidateLocation::BCSU ) ){
    pL0MuonCandidates = get<L0MuonCandidates>( L0MuonCandidateLocation::BCSU );
  } else if( exist<L0MuonCandidates>( L0MuonCandidateLocation::Default, false ) )  {
    Warning(" No L0MuonCandidates at " + L0MuonCandidateLocation::BCSU + " using truncated list from "
            +  L0MuonCandidateLocation::Default, StatusCode::SUCCESS, 5 );
    pL0MuonCandidates = get<L0MuonCandidates>( L0MuonCandidateLocation::Default, false );
  } else if( exist<L0MuonCandidates>( L0MuonCandidateLocation::Default ) )  {
    Warning(" No L0MuonCandidates at " + L0MuonCandidateLocation::BCSU + " using truncated list from "
            +  L0MuonCandidateLocation::Default, StatusCode::SUCCESS, 5 );
    pL0MuonCandidates = get<L0MuonCandidates>( L0MuonCandidateLocation::Default );
  }
  if( pL0MuonCandidates )
  {
    HltObjectSummary selSumOut;
    selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)
    const std::string selName="L0MuonCandidates";

    // integer selection id
    static boost::hash<std::string> hasher;
    selSumOut.addToInfo("0#SelectionID", float( hasher(selName) )  ); // fake: cannot make them persistent

    for(L0MuonCandidates::const_iterator ic=pL0MuonCandidates->begin();ic!=pL0MuonCandidates->end();++ic){
      const HltObjectSummary* hos = store<LHCb::L0MuonCandidate>(*ic);
      if( !hos ){
        Error( " Could not store HltObjectSummary of L0MuonCandidate" , StatusCode::SUCCESS, 10 );
      } else {
        selSumOut.addToSubstructure(hos);
      }
    }

    // insert selection into the container
    if( outputSummary->insert(selName,selSumOut) == StatusCode::FAILURE ){
      Warning(" Failed to add Hlt selection name "+selName
              +" to its container ",StatusCode::SUCCESS, 10 );
    }

  } else {
    Warning( " No L0MuonCandidates at " + L0MuonCandidateLocation::BCSU + " nor at " + L0MuonCandidateLocation::Default
             , StatusCode::SUCCESS, 10 );
  }

  // translate all L0CaloCandidates into Object Summaries
  L0CaloCandidates* pL0CaloCandidates = 0;
  if( exist<L0CaloCandidates>( L0CaloCandidateLocation::Full, false ) ){
    pL0CaloCandidates=get<L0CaloCandidates>( L0CaloCandidateLocation::Full, false );
  } else if( exist<L0CaloCandidates>( L0CaloCandidateLocation::Full ) ){
    pL0CaloCandidates=get<L0CaloCandidates>( L0CaloCandidateLocation::Full );
  }
  if( pL0CaloCandidates )
  {
    HltObjectSummary selSumOut;
    selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)
    const std::string selName="L0CaloCandidates";

    // integer selection id
    static boost::hash<std::string> hasher;
    selSumOut.addToInfo("0#SelectionID", float( hasher(selName) )  ); // fake: cannot make them persistent

    for(L0CaloCandidates::const_iterator ic=pL0CaloCandidates->begin();ic!=pL0CaloCandidates->end();++ic){
      const HltObjectSummary* hos = store<LHCb::L0CaloCandidate>(*ic);
      if( !hos ){
        Error( " Could not store HltObjectSummary of L0CaloCandidate" , StatusCode::SUCCESS, 10 );
      } else {
        selSumOut.addToSubstructure(hos);
      }

    }

    // insert selection into the container
    if( outputSummary->insert(selName,selSumOut) == StatusCode::FAILURE ){
      Warning(" Failed to add Hlt selection name "+selName
              +" to its container ",StatusCode::SUCCESS, 10 );
    }

  } else {
    Warning( " No L0CaloCandidates at " + L0CaloCandidateLocation::Default , StatusCode::SUCCESS, 10 );
  }

  if ( pL0DUReport->decision() ){

    // loop over l0 channels; translate conditions into candidates ( create substructure for muon1 && muon2 etc. )
    //       individual L0 trigger lines
    L0DUChannel::Map channels = pL0DUReport->configuration()->channels();
    for( L0DUChannel::Map::const_iterator it=channels.begin();it!=channels.end();++it){
      if ( msgLevel(MSG::VERBOSE) ){
        verbose() << " L0 trigger name= " << it->first << " decision= " << pL0DUReport->channelDecision(it->second->id()) << endmsg;
        for( LHCb::L0DUElementaryCondition::Map::const_iterator ic = it->second->elementaryConditions().begin();
             ic != it->second->elementaryConditions().end(); ++ic){
          if ( msgLevel(MSG::VERBOSE) ) verbose() << "        " << ic->first << endmsg;
        }
      }
      if( pL0DUReport->channelDecision(it->second->id()) ){
        std::string  selName = "L0" + it->first;
        if( it->second->decisionType() & L0DUDecision::Physics ){
          selName = selName + "Decision";
        }
        typedef SmartRefVector<HltObjectSummary> SubSelection;
        const int maxSubSel(3);
        SubSelection subSelections[maxSubSel];
        int nSubSel(0);

        for( LHCb::L0DUElementaryCondition::Map::const_iterator ic = it->second->elementaryConditions().begin();
             ic != it->second->elementaryConditions().end(); ++ic){
          //->                if ( msgLevel(MSG::VERBOSE) ) verbose() << "        " << ic->first << endmsg;
          const std::string & conditionName = ic->first;
          for( std::vector< std::string >::const_iterator
                 icond=m_selectionConditions.begin();icond!=m_selectionConditions.end();++icond){
            const std::string & cond = *icond;
            if( conditionName.find(cond) == 0 ){
              //->             if ( msgLevel(MSG::VERBOSE) )
              //->             verbose() << " cut string " << conditionName.substr( cond.length()+1,conditionName.length()-cond.length()-1 ) << endmsg;
              int cut = atoi( conditionName.substr( cond.length()+1,conditionName.length()-cond.length()-1 ).c_str() );
              //->             if ( msgLevel(MSG::VERBOSE) ) verbose() << " cut value " << cut << endmsg;
              bool disum(  cond.substr(0,1)=="D" );
              if ( disum || ( cond.find("Muon12")==0 ) ){
                std::vector<HltObjectSummary*> tmpHosBuffer;
                for( HltObjectSummary::Container::const_iterator ppHos1=m_objectSummaries->begin();
                     ppHos1!=m_objectSummaries->end();++ppHos1){
                  const HltObjectSummary* pHos1=*ppHos1;
                  if( pHos1->summarizedObjectCLID() != L0MuonCandidate::classID() )continue;
                  int pt1=dynamic_cast<const L0MuonCandidate*>( pHos1->summarizedObject() )->encodedPt()&0x7F;
                  for( HltObjectSummary::Container::const_iterator ppHos2=ppHos1;
                       ppHos2!=m_objectSummaries->end();++ppHos2){
                    if( ppHos1 == ppHos2 )continue;
                    const HltObjectSummary* pHos2=*ppHos2;
                    if( pHos2->summarizedObjectCLID() != L0MuonCandidate::classID() )continue;
                    int pt2=dynamic_cast<const L0MuonCandidate*>( pHos2->summarizedObject() )->encodedPt()&0x7F;
                    if( disum ){
                      // old DC06 dimuon based on a sum of PTs
                      if( pt1+pt2 <= cut )continue;
                    } else {
                      // new 2011 dimuon based on a product of PTs
                      if( pt1*pt2 <= cut )continue;
                      // ps: 2010 dimuon is based on Muon1 x Muon2 (nSub=2) and is handled below
                    }
                    HltObjectSummary* hos = new HltObjectSummary();
                    hos->setSummarizedObjectCLID( 0 ); // we are not summarizing any existing object
                    //              hos->setSummarizedObject(object);
                    hos->addToSubstructure( pHos1 );
                    hos->addToSubstructure( pHos2 );
                    hos->addToInfo("0#L0Type",-2.0);
                    tmpHosBuffer.push_back(hos);
                  }
                }
                for( std::vector<HltObjectSummary*>::iterator ppHos = tmpHosBuffer.begin();
                     ppHos != tmpHosBuffer.end(); ++ppHos ){
                  m_objectSummaries->push_back(*ppHos);
                  subSelections[nSubSel].push_back( SmartRef<HltObjectSummary>(*ppHos) );
                }
              } else {
                for( HltObjectSummary::Container::const_iterator ppHos=m_objectSummaries->begin();
                     ppHos!=m_objectSummaries->end();++ppHos){
                  const HltObjectSummary* pHos=*ppHos;
                  if( cond.substr(0,1)=="M" ){
                    // Muon1 or Muon2
                    if( pHos->summarizedObjectCLID() != L0MuonCandidate::classID() )continue;
                    if( (dynamic_cast<const L0MuonCandidate*>( pHos->summarizedObject() )->encodedPt()&0x7F) <= cut )continue;
                  } else {
                    // various calo conditions
                    if( pHos->summarizedObjectCLID() != L0CaloCandidate::classID() )continue;
                    const L0CaloCandidate* l0Calo = dynamic_cast<const L0CaloCandidate*>( pHos->summarizedObject() );
                    if( l0Calo->etCode() <= cut )continue;
                    if(  cond.substr(0,1)=="H" ){
                      if( l0Calo->type() != L0DUBase::CaloType::Hadron )continue;
                    } else if( cond.substr(0,1)=="E" ){
                      if( l0Calo->type() != L0DUBase::CaloType::Electron )continue;
                    } else if( cond.substr(0,1)=="P" ){
                      if( l0Calo->type() != L0DUBase::CaloType::Photon )continue;
                    } else if( cond.substr(0,1)=="L" ){
                      if( l0Calo->type() != L0DUBase::CaloType::Pi0Local )continue;
                    } else if( cond.substr(0,1)=="G" ){
                      if( l0Calo->type() != L0DUBase::CaloType::Pi0Global )continue;
                    } else {
                      Error(" Unrecognized Calo selection condition type " + cond,  StatusCode::SUCCESS, 10 );
                      continue;
                    }
                  }
                  subSelections[nSubSel].push_back( SmartRef<HltObjectSummary>(pHos) );
                }
              }
              if ( msgLevel(MSG::VERBOSE) )
                verbose() << "        " << " Number of selected objects=" << subSelections[nSubSel].size() << endmsg;
              if( subSelections[nSubSel].size() ){
                nSubSel++;
              } else {
                Error(" Condition " + conditionName + " resulted in empty selection ",  StatusCode::SUCCESS, 10 );
              }
              if( nSubSel == maxSubSel ){
                Warning(" Maximal number of subselections reached ", StatusCode::SUCCESS, 10 );
                break;
              }
            }
          }
        }
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "        "  << " Number of subselections= " << nSubSel << endmsg;
        if( !nSubSel )continue; // do not save empty selections

        if( nSubSel>2 ){
          Warning(" L0 trigger " + selName + " makes more than 2 selections. Only first 2 will be used ", StatusCode::SUCCESS, 10 );
          nSubSel=2;
        }


        HltObjectSummary selSumOut;
        selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)

        // integer selection id
        static boost::hash<std::string> hasher;
        selSumOut.addToInfo("0#SelectionID", float( hasher(selName) )  ); // fake: cannot make them persistent

        if( nSubSel == 1 ){
          // this is easy; just save candidates in substracture
          selSumOut.setSubstructure( subSelections[0] );

        } else {
          // candidates are pairs or more; create HltObjectSummary for every combination

          for( SubSelection::const_iterator io1= subSelections[0].begin();io1!=subSelections[0].end();++io1){
            const HltObjectSummary* pHos1=*io1;
            for( SubSelection::const_iterator io2= subSelections[1].begin();io2!=subSelections[1].end();++io2){
              const HltObjectSummary* pHos2=*io2;
              if( pHos1==pHos2 )continue;
              // if same type; must make sure not stored already in reverse order
              if( pHos1->numericalInfo()["0#L0Type"] == pHos2->numericalInfo()["0#L0Type"] ){
                bool found(false);
                const SmartRefVector<HltObjectSummary> & candidates = selSumOut.substructure();
                for( SubSelection::const_iterator ic=candidates.begin();ic!=candidates.end();++ic){
                  if( (const HltObjectSummary*)((*ic)->substructure()[0]) == pHos2
                      && (const HltObjectSummary*)((*ic)->substructure()[1]) == pHos1 ){
                    found=true;
                    break;
                  }
                }
                if( found )continue;
              }
              HltObjectSummary* hos = new HltObjectSummary();
              hos->setSummarizedObjectCLID( 0 ); // we are not summarizing any existing object
              //              hos->setSummarizedObject(object);
              hos->addToSubstructure( pHos1 );
              hos->addToSubstructure( pHos2 );
              hos->addToInfo("0#L0Type",-2.0);
              m_objectSummaries->push_back(hos);
              selSumOut.addToSubstructure(hos);
            }
          }
        }

        // insert selection into the container
        if( outputSummary->insert(selName,selSumOut) == StatusCode::FAILURE ){
          Warning(" Failed to add Hlt selection name "+selName
                  +" to its container ",StatusCode::SUCCESS, 10 );
        }
      }
    }

    // -------------------------------------------------------------------------------------
    // clone HltObjectSummary-s of selection and insert them into object store (needed for raw data storage
    // and global selections)
    // -------------------------------------------------------------------------------------
    for( HltSelReports::Container::const_iterator i=outputSummary->begin();i!=outputSummary->end();++i){
      const HltObjectSummary& sHos= i->second;
      HltObjectSummary* hos= new HltObjectSummary( sHos );
      m_objectSummaries->push_back(hos);
    }

    // -------------------------------------------------------------------------------------
    // create global selections ------------------------------------------------------------
    // -------------------------------------------------------------------------------------
    if( !outputSummary->hasSelectionName("L0Global") ){

      HltObjectSummary selSumOut;
      selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)

      // integer selection id
      selSumOut.addToInfo("0#SelectionID",float(kL0GlobalID));

      // see which decisions contributed to it
      for( HltSelReports::Container::const_iterator it=outputSummary->begin();it!=outputSummary->end();++it){
        const std::string & selName = it->first;
        if( selName.find("Decision") == std::string::npos )continue;
        const LHCb::HltObjectSummary& selRep = it->second;
        // must find corresponding HltObjectSummary in the object store
        HltObjectSummary::Info::const_iterator i = selRep.numericalInfo().find("0#SelectionID");
        if( i!=selRep.numericalInfo().end() ){
          float id = i->second;
          for( HltObjectSummary::Container::const_iterator pObj=m_objectSummaries->begin();
               pObj!=m_objectSummaries->end();++pObj){
            if( (*pObj)->summarizedObjectCLID()!=1 )continue;
            HltObjectSummary::Info::const_iterator j=(*pObj)->numericalInfo().find("0#SelectionID");
            if( j!=(*pObj)->numericalInfo().end() ){
              if( id == j->second ){
                selSumOut.addToSubstructure( (const SmartRef<HltObjectSummary>)(*pObj) );
                break;
              }
            } else {
              Error(" (2) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
            }
          }
        } else {
          Error(" (1) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
        }
      }
      if( selSumOut.substructure().size() ){

        // insert selection into the container
        if( outputSummary->insert("L0Global",selSumOut) == StatusCode::FAILURE ){
          Error( "  Failed to add Hlt selection name L0Global to its container ", StatusCode::SUCCESS, 10 );
        }
        HltObjectSummary* hos= new HltObjectSummary(  selSumOut );
        m_objectSummaries->push_back(hos);


      }
    }

    // L0DUReport->decision()
  }


  // ----------------- printout ------------------------
  if ( msgLevel(MSG::VERBOSE) ){

    verbose() << " ======= HltSelReports size= " << outputSummary->size() << endmsg;
    verbose() << *outputSummary << endmsg;

    verbose() << " ======= HltObjectSummary container size= " << m_objectSummaries->size() << endmsg;
    for( HltObjectSummary::Container::const_iterator ppHos=m_objectSummaries->begin();
         ppHos!=m_objectSummaries->end();++ppHos){
      const HltObjectSummary* pHos=*ppHos;
      verbose() << " key " << pHos->index();
      std::vector<std::string> selby = outputSummary->selectedAsCandidateBy(pHos);
      if( selby.size() ){
        verbose() << " selectedAsCandidateBy=";
        for( std::vector<std::string>::const_iterator i=selby.begin();i!=selby.end();++i){
          verbose()  << *i << " ";
        }
      }
      verbose() << *pHos << endmsg;
    }

  }


  return StatusCode::SUCCESS;
}





// -------------------------------------
// store L0MuonCandidate in HltObjectSummary store
// -------------------------------------
const LHCb::HltObjectSummary* L0SelReportsMaker::store_(const LHCb::L0MuonCandidate* object)
{
  if( !object )return (HltObjectSummary*)(0);
  for( HltObjectSummarys::const_iterator ppHos=m_objectSummaries->begin();
       ppHos!=m_objectSummaries->end();++ppHos){
    const HltObjectSummary* pHos(*ppHos);
    if( pHos->summarizedObjectCLID() == object->clID() ){
      if( pHos->summarizedObject() == object ){
        return pHos;
      }
    }
  }
  // not yet in object store; create new one
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);

  HltObjectSummary::Info theseInfo = infoToSave( hos );
  std::vector<LHCbID> theseHits;
  for(int s=0;s<3;++s){
    std::vector< LHCb::MuonTileID > muHits = object->muonTileIDs(s);
    for(std::vector<LHCb::MuonTileID>::const_iterator muHit=muHits.begin();muHit!=muHits.end();++muHit){
      theseHits.push_back( LHCbID(*muHit) );
    }
  }
  hos->setLhcbIDs( theseHits );
  hos->setNumericalInfo( theseInfo );

  m_objectSummaries->push_back(hos);
  return hos;
}


// -------------------------------------
// store L0CaloCandidate in HltObjectSummary store
// -------------------------------------
const LHCb::HltObjectSummary* L0SelReportsMaker::store_(const LHCb::L0CaloCandidate* object)
{
  if( !object )return (HltObjectSummary*)(0);
  for( HltObjectSummarys::const_iterator ppHos=m_objectSummaries->begin();
       ppHos!=m_objectSummaries->end();++ppHos){
    const HltObjectSummary* pHos(*ppHos);
    if( pHos->summarizedObjectCLID() == object->clID() ){
      if( pHos->summarizedObject() == object ){
        return pHos;
      }
    }
  }
  // not yet in object store; create new one
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);

  HltObjectSummary::Info theseInfo = infoToSave( hos );
  std::vector<LHCbID> theseHits;
  CaloCellID id = object->id();
  CaloCellID id1(id.calo(), id.area(), id.row()+1, id.col()   );
  CaloCellID id2(id.calo(), id.area(), id.row()+1, id.col()+1 );
  CaloCellID id3(id.calo(), id.area(), id.row()  , id.col()+1 );
  theseHits.push_back( LHCbID(id) );
  theseHits.push_back( LHCbID(id1) );
  theseHits.push_back( LHCbID(id2) );
  theseHits.push_back( LHCbID(id3) );

  hos->setLhcbIDs( theseHits );
  hos->setNumericalInfo( theseInfo );

  m_objectSummaries->push_back(hos);
  return hos;
}




//=============================================================================
// fetch info to save; to find object follow summerizedObject pointer
//=============================================================================
HltObjectSummary::Info L0SelReportsMaker::infoToSave( const HltObjectSummary* hos ) const
{
  HltObjectSummary::Info infoPersistent;

  if( !hos )return infoPersistent;
  if( !(hos->summarizedObject()) )return infoPersistent;
  switch( hos->summarizedObjectCLID() )
  {
  case LHCb::CLID_L0CaloCandidate:
    {
      const L0CaloCandidate* candi = dynamic_cast<const L0CaloCandidate*>(hos->summarizedObject());
      if( !candi )return infoPersistent;
      infoPersistent.insert( "0#L0Type", float( candi->type() ) );
      infoPersistent.insert( "1#et", float( candi->et() ) );
      infoPersistent.insert( "2#etCode", float( candi->etCode() ) );
    }
    break;
  case LHCb::CLID_L0MuonCandidate:
    {
      const L0MuonCandidate* candi = dynamic_cast<const L0MuonCandidate*>(hos->summarizedObject());
      if( !candi )return infoPersistent;
      infoPersistent.insert( "0#L0Type", -1.0 );
      infoPersistent.insert( "1#pt", float( candi->pt() ) );
      infoPersistent.insert( "2#encodedPt", float( candi->encodedPt() ) );
    }
    break;
  default:
    { }
  }
  return infoPersistent;
}



