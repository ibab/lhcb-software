// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Hlt
#include "Event/HltSelReports.h"
#include "Event/HltDecReports.h"
#include "Event/HltObjectSummary.h"
#include "Kernel/ANNSvc.h"

// local
#include "HltTrackConverter.h"

#include "Event/Track.h"
//#include "Kernel/LHCbID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackConverter
//
// 2010-05-03 : Albert Frithjof Bursche
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackConverter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackConverter::HltTrackConverter( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "HltObjectSummary", 
                   m_HltObjectSummaryLocation = LHCb::HltObjectSummaryLocation::Default
		   // LHCb::HltSelReportsLocation::Default
                   , "Location of the HltObjectSummary in TES" );
  declareProperty( "TrackDestignation", m_ConvertedTracksDestignation = "Rec/Track/ConvertedHltTracks" 
                   , "Location in TES the convertet tracks are supposed to end up." );
  //  double CloneOverlapTreshold = 0.01;
  declareProperty( "CloneOverlapTreshold",m_CloneOverlapTreshold = 0.7 
                   , "Remove every track if there is another track with 'CloneOverlapTreshold' overlap. The other track is then kept." );
  declareProperty( "MinimalHits",m_MinimalHits = 3 
                   , "Remove every track having less than this number of hits (in the selected tracking stations)" );
  
 {
    std::vector<std::string> UseHitsFromDefault;
    UseHitsFromDefault.push_back("AllTrackingStations");
    declareProperty( "UseHitsFrom", m_UseHitsFrom = UseHitsFromDefault
		     , "Names of stations of which hits should be used" );
    
    for (int i=0;i<9;i++)
      m_UseHitsFromLookupTable[i] = false;
    for (std::vector<std::string>::iterator it = m_UseHitsFrom.begin();it!=m_UseHitsFrom.end();++it)
      { 
	if (*it == "Velo")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::Velo] = true;
	    continue;
	  }
	if (*it == "TT")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::TT] = true;
	    continue;
	  }
	if (*it == "IT")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::IT] = true;
	    continue;
	  }
	if (*it == "OT")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::OT] = true;
	    continue;
	  }
	if (*it == "Muon")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::Muon] = true;
	    continue;
	  }
	if (*it == "ST")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::TT] = true;
	    m_UseHitsFromLookupTable[LHCb::LHCbID::IT] = true;
	    continue;
	  }
	if (*it == "AllTrackingStations")
	  {
	    m_UseHitsFromLookupTable[LHCb::LHCbID::Velo] = true;
	    m_UseHitsFromLookupTable[LHCb::LHCbID::TT] = true;
	    m_UseHitsFromLookupTable[LHCb::LHCbID::IT] = true;
	    m_UseHitsFromLookupTable[LHCb::LHCbID::OT] = true;
	    continue;
	  }
      }
  }
  {
    m_HltLines.push_back("Hlt1Global");// see Thomasz mail for better solution
    declareProperty( "HltLinesToUse", m_HltLines
		     , "Names of Trigger lines of which hits should be used" );
    
  }
  declareProperty( "ReadHltLinesFrom1stEvent", m_HltLinesFrom1stEvent = false
		   , "Names of Trigger lines of which hits should be used" );
  declareProperty( "RequireTrackClassID", m_requireTrackClassID= true
		   , "Require that the HLTObjectSummaryClID is equal to LHCb::Track::classID()" );
  declareProperty( "TryToAddFirstState", m_addFirstState = true
		   , "Names of Trigger lines of which hits should be used" );
  declareProperty( "RequireFirstState", m_requireFirstState = m_addFirstState 
		   , "Add only tracks were the first state could be decoded." );
  if(m_requireFirstState&&!m_addFirstState)
    warning() << "Algorithm is configured to require decoded track first state but will not add it to the track!"<<endmsg;
  
  { //configure the decoding of the numericInfo 
    const AssignNumInfos z("0#Track.firstState.z",2);
    const AssignNumInfos x("1#Track.firstState.x",3);
    const AssignNumInfos y("2#Track.firstState.y",5);
    const AssignNumInfos tx("3#Track.firstState.tx",7);
    const AssignNumInfos ty("4#Track.firstState.ty",11);
    const AssignNumInfos qOverP("5#Track.firstState.qOverP",13);
    
    requiredNumericalInformation["z"] = z;
    requiredNumericalInformation["x"] = x;
    requiredNumericalInformation["y"] = y;
    requiredNumericalInformation["tx"] = tx;
    requiredNumericalInformation["ty"] = ty;
    requiredNumericalInformation["qOverP"] = qOverP;
  }
}
StatusCode HltTrackConverter::initializeTriggerLists()
{
  std::vector<std::string> m_hlt1_init = svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt1SelectionID");
  std::vector<std::string> m_hlt2_init = svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt2SelectionID");
  debug()<< "Available Hlt1 Triggers\n";
  for (std::vector<std::string>::iterator it = m_hlt1_init.begin();it!=m_hlt1_init.end();++it)
    {
      m_HltLines.push_back(*it);
      debug()<< *it << "\n";
    }
  debug() << endmsg;
  debug()<< "Available Hlt2 Triggers\n";
  for (std::vector<std::string>::iterator it = m_hlt2_init.begin();it!=m_hlt2_init.end();++it)
    {
      m_HltLines.push_back(*it);
      debug()<< *it << "\n";
    }
  debug () << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Destructor
//=============================================================================
HltTrackConverter::~HltTrackConverter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackConverter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
}
 
//=============================================================================
// Main execution
//=============================================================================
LHCb::Track::Types HltTrackConverter::SetTrackType(LHCb::Track * t)
{
  //  enum  	Types  {
  //    TypeUnknown = 0, Velo, VeloR, Long,
  //    Upstream, Downstream, Ttrack, Muon,
  //    Calo, TT, VeloPix
  //  }
  std::vector<LHCb::LHCbID> lhcbIDs = t->lhcbIDs(); 
  bool hasIT = false;
  bool hasOT = false;
  bool hasVelo = false;
  bool hasVeloPhi = false;
  bool hasTT =  false;
  bool hasMuon = false;

  for(std::vector<LHCb::LHCbID>::iterator it = lhcbIDs.begin();it!=lhcbIDs.end();++it)
    switch(it->detectorType())
      {
      case LHCb::LHCbID::Velo:
	hasVelo = true;
	if (it->isVeloPhi())
	  hasVeloPhi = true;
	continue;
      case LHCb::LHCbID::TT:
	hasTT = true;
	continue;
      case LHCb::LHCbID::IT:
	hasIT = true;
	continue;
      case LHCb::LHCbID::OT:
	hasOT = true;
	continue;
      case LHCb::LHCbID::Muon:
	hasMuon = true;
	continue;
	
      }
  bool hasT = hasIT || hasOT;
  
  if(hasVelo && hasT)
    return LHCb::Track::Long;
  if(hasVelo && hasTT)
    return LHCb::Track::Upstream;
  if(hasTT && hasT)
    return LHCb::Track::Downstream;
  if(hasVelo)
    {
      if(hasVeloPhi)
	return LHCb::Track::Velo;
      else
	return  LHCb::Track::VeloR;
    }
  return LHCb::Track::TypeUnknown;
}

void  HltTrackConverter::RemoveClones(LHCb::Tracks * tracks)
{
 
  std::sort(tracks->begin(),tracks->end(), tc);
  bool CloneFound = false;
  for(LHCb::Tracks::iterator it = tracks->begin();it!=tracks->end();++it)
    for(LHCb::Tracks::iterator jt = tracks->begin();jt!=it;++jt)
      {
	if ((*jt)-> flags()==LHCb::Track::Clone)
	  continue; //don't compare to track which are gonna be removed anyway
	double nOverlap = (*jt)->containsLhcbIDs((**it));
	double minN = (*it)->nLHCbIDs();
	if (nOverlap/minN>=m_CloneOverlapTreshold)
	  {
	    (*it)->setFlags(LHCb::Track::Clone);
	    CloneFound = true;//cant call erase here because it invalidates all iterators.
	  }
	debug() << "In clone remover. Comparing " << *jt << " and " << *it
		<<" Overlap: " << nOverlap/minN 
		<< " = "  << nOverlap << " / "  << minN << endmsg;
      }
  bool ClonesRemaining = CloneFound;
  while (ClonesRemaining)
    { 
      ClonesRemaining = false;
      for(LHCb::Tracks::iterator it = tracks->begin();it!=tracks->end();++it)
	if((*it)->flags() == LHCb::Track::Clone)
	  {
	    ClonesRemaining= true;
	    debug() << " removed clone " << (*it) << endmsg;
	    delete(*it);
	    (*it)=0; 
 	    tracks->erase(it);
	    break; // iterators have been invalidated because of the erase call. Start again to catch remaining clones
	  }
    } 
}
StatusCode HltTrackConverter::execute() 
{
  if(m_HltLinesFrom1stEvent)
    if(initializeTriggerLists()) // side effects 
      m_HltLinesFrom1stEvent = false;
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "==> Execute" << endmsg;
  tracks = 0;
  if(exist<LHCb::HltSelReports>  (LHCb::HltSelReportsLocation::Default))
    {
      LHCb::HltSelReports* selReports = get<LHCb::HltSelReports>(LHCb::HltSelReportsLocation::Default);
      for (std::vector<std::string>::iterator s = m_HltLines.begin();s!= m_HltLines.end();++s)
	{
	  const LHCb::HltObjectSummary*  selReport =  selReports->selReport(*s);
	  if (selReport)
	    executeRecursive(selReport);
	}
    }
  if (!tracks)
    {
      tracks = new LHCb::Tracks(); 
      debug() << "Inserting empty track collection to" << m_ConvertedTracksDestignation << endmsg;
    }
  else
    {
      RemoveClones(tracks);
      debug() << "Inserting track collection with "  << tracks->size() <<" tracks to " << m_ConvertedTracksDestignation << endmsg;
    }
  put(tracks,m_ConvertedTracksDestignation );
  return StatusCode::SUCCESS;
}

  void HltTrackConverter::executeRecursive(const LHCb::HltObjectSummary* SelRep)
  {
    if(!SelRep)
      return;
    debug() << "called executeRecursive(" <<  SelRep << ");" << endmsg;
    SmartRefVector <LHCb::HltObjectSummary> substructure = SelRep->substructure();
    for(SmartRefVector <LHCb::HltObjectSummary>::const_iterator child = substructure.begin();child!=substructure.end();++child)
      executeRecursive(*child);
    if (SelRep->summarizedObjectCLID() ==  LHCb::Track::classID() )  
      debug() << "found track" << endmsg; //never prints anything
    else
      if (m_requireTrackClassID)
	return; 
    {
      std::vector<LHCb::LHCbID> LhcbIDs = SelRep->lhcbIDs();
      for (std::vector<LHCb::LHCbID>::iterator it = LhcbIDs.begin();it!=LhcbIDs.end();++it)
	if (m_UseHitsFromLookupTable[it->detectorType()])
	  {
	    //creation of the Track object
	    LHCb::Track * t = new LHCb::Track();
	    for (;it != LhcbIDs.end();++it)
	      if (m_UseHitsFromLookupTable[it->detectorType()])
		{
		  t -> addToLhcbIDs(*it);
		  debug() << "added" << *it << " to track " << t << endmsg; 
		}
	    if (m_addFirstState|| m_requireFirstState)
	      //decode first state from numericalInfo() and add it to track
	      {
		std::vector<std::string> numericalInfoKeys = SelRep->numericalInfoKeys();
		LHCb::HltObjectSummary::Info numericalInfo = SelRep->numericalInfo();
		if ( msgLevel(MSG::DEBUG) )
		  { 
		    for (std::vector<std::string>::iterator it = numericalInfoKeys.begin();it!=numericalInfoKeys.end();++it)
		      debug() << *it << "\t";
		    debug() << endmsg;
		    debug() <<  numericalInfo[requiredNumericalInformation["x"].m_name]	<< "\t"
			    <<  numericalInfo[requiredNumericalInformation["y"].m_name]	<< "\t"
			    <<  numericalInfo[requiredNumericalInformation["y"].m_name]	<< "\t"
			    <<  numericalInfo[requiredNumericalInformation["tx"].m_name]<< "\t"
			    <<  numericalInfo[requiredNumericalInformation["ty"].m_name]<< "\t"
			    <<  numericalInfo[requiredNumericalInformation["qOverP"].m_name]	
			    << endmsg;
		  }
		
		//Are all required variables in the std::map recievet from SelRep?
		unsigned int test = 1;
		for (std::vector<std::string>::iterator it = numericalInfoKeys.begin();it!=numericalInfoKeys.end();++it)
		  for(std::map<std::string,AssignNumInfos>::iterator jt =requiredNumericalInformation.begin();jt!=requiredNumericalInformation.end();++jt)
		    if(*it==jt->second.m_name)
		      //for (unsigned int i=0;i<requiredNumericalInformation.size();i++)
		      //if (*it==requiredNumericalInformation[i])
		      test*=jt->second.m_prime; //this condition must ecactly once be true for each element of requiredNumericalInformation
		if (test == 30030)              //so the product should be the product of the first n elemets of primes. Since these numbers are prime this is the unique factorisation.  
		  {
		    if ( msgLevel(MSG::DEBUG) )
		      debug() << "Trach state recieved from HltObjectSummary" << endmsg;
		    LHCb::State s;
		    s.setX( numericalInfo[requiredNumericalInformation["x"].m_name] );
		    s.setY( numericalInfo[requiredNumericalInformation["y"].m_name] );
		    s.setZ ( numericalInfo[requiredNumericalInformation["z"].m_name] );
		    s.setTx ( numericalInfo[requiredNumericalInformation["tx"].m_name] );
		    s.setTy ( numericalInfo[requiredNumericalInformation["ty"].m_name] );
		    s.setQOverP ( numericalInfo[requiredNumericalInformation["qOverP"].m_name] );
		    if (m_addFirstState )
		      t->addToStates(s);
		  }
		
		else
		  {
		    if (m_requireFirstState)
		      {
			delete t;
			return;
		      }
		    if ( msgLevel(MSG::DEBUG) )
		      { 
			debug() << "test evaluates to :" << test << " which is not what it supposed to be!" << endmsg;
			for (std::vector<std::string>::iterator it = numericalInfoKeys.begin();it!=numericalInfoKeys.end();++it)
			  debug() << *it << "\t";
			debug() << endmsg;
			for (std::map<std::string,AssignNumInfos>::iterator it = requiredNumericalInformation.begin();it!=requiredNumericalInformation.end();++it)
			  if (count(numericalInfoKeys.begin(),numericalInfoKeys.end(),it->second.m_name) !=1 )
			    debug() << it->second.m_name << " occurs " << count(numericalInfoKeys.begin(),numericalInfoKeys.end(),it->second.m_name)  << "(supposed to be 1!)" <<endmsg ;
		      }
		  }
	      }
	    t->setType(SetTrackType(t));
	    t->setHistory(LHCb::Track::HLTImportedTrack);
	    debug() << t;
	      
		
	    
	    if (!tracks)
	      tracks = new LHCb::Tracks();
	    if (t->nLHCbIDs()>= m_MinimalHits )
	      tracks->add(t);
	    else
	      delete t;
	    t=NULL;
	    break; //outer loop over LHCbIds
	  } 
    }

  }
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackConverter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
