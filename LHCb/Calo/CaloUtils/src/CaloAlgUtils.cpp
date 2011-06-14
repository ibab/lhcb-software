#include "Event/ProtoParticle.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "GaudiKernel/IRegistry.h"


// Default location for CaloObject as function of detector and possibly context

// DetectorElement location from name
std::string LHCb::CaloAlgUtils::DeCaloLocation( std::string name ){
  std::string det = LHCb::CaloAlgUtils::CaloNameFromAlg( name );
  if ( det == "Ecal" ) return  DeCalorimeterLocation::Ecal;
  else if ( det == "Hcal")return  DeCalorimeterLocation::Hcal;
  else if ( det == "Prs")return  DeCalorimeterLocation::Prs;
  else if ( det == "Spd")return  DeCalorimeterLocation::Spd;
  return "";
}

// Digit location from name (CaloDigits are context independent so far)
std::string  LHCb::CaloAlgUtils::CaloDigitLocation( std::string name  , std::string context ){
  std::string det = LHCb::CaloAlgUtils::CaloNameFromAlg( name );

  using namespace LHCb::CaloDigitLocation;

  std::string cont = context;
  if(cont != "")cont = ""; // just to avoid a compilation warning


  if ( det == "Ecal" ) return Ecal;
  else if ( det == "Hcal")return Hcal;
  else if ( det == "Prs")return  Prs;
  else if ( det == "Spd")return  Spd;
  // default is Ecal in offline mode
  return Ecal;
}


// Cluster location from context
std::string  LHCb::CaloAlgUtils::CaloSplitClusterLocation( std::string context ){
  using namespace LHCb::CaloClusterLocation;
  return LHCb::CaloAlgUtils::PathFromContext( context , EcalSplit ) ;
}


std::string  LHCb::CaloAlgUtils::CaloClusterLocation( std::string name , std::string context){

  //##  splitClusters
  std::string uName = toUpper( name );
  if( std::string::npos != uName.find( "SPLITCLUSTER" ) )
    return CaloSplitClusterLocation( context );

  // ## standard clusters
  std::string det = LHCb::CaloAlgUtils::CaloNameFromAlg( name );

  // caloClusters are almost context-independent : either Offline type or HLT-type!
  // MUST be synchronous with CaloRecoConf 
  std::string uCont = toUpper( context );
  if( std::string::npos != uCont.find( "HLT" ) )
    context = "Hlt";
  else
    context = "";
  
  using namespace LHCb::CaloClusterLocation;
  if ( det == "Hcal" )return LHCb::CaloAlgUtils::PathFromContext( context ,Hcal );
  return LHCb::CaloAlgUtils::PathFromContext( context , Ecal  ); // default is Ecal 
}

// Hypo location from type/context
std::string  LHCb::CaloAlgUtils::CaloHypoLocation( std::string type , std::string context){
  using namespace LHCb::CaloHypoLocation;
  std::string uType = toUpper(type);
  if ( std::string::npos != uType.find("ELECTRON") ) return LHCb::CaloAlgUtils::PathFromContext( context ,Electrons );
  else if ( std::string::npos != uType.find("SPLITPHOTON") )return LHCb::CaloAlgUtils::PathFromContext( context , SplitPhotons );
  else if ( std::string::npos != uType.find("BREM") )return LHCb::CaloAlgUtils::PathFromContext( context ,Photons );//Brem=photon
  else if ( std::string::npos != uType.find("PHOTON"))return LHCb::CaloAlgUtils::PathFromContext( context ,Photons);//after split 
  else if ( std::string::npos != uType.find("MERGED") )return LHCb::CaloAlgUtils::PathFromContext( context ,MergedPi0s );
  return "";
}

//CaloId location from type/context
std::string LHCb::CaloAlgUtils::CaloIdLocation( std::string type, std::string context){
  using namespace LHCb::CaloIdLocation;
  std::string uType = toUpper(type);
  if( std::string::npos != uType.find("BREMMATCH" ))return PathFromContext( context , BremMatch ) ;
  else if( std::string::npos != uType.find("ELECTRONMATCH" ))return PathFromContext( context , ElectronMatch   ) ;
  else if( std::string::npos != uType.find("PHOTONMATCH"  ))return PathFromContext( context , ClusterMatch     ) ;//=ClusterMatch
  else if( std::string::npos != uType.find("CLUSTERMATCH"  ))return PathFromContext( context , ClusterMatch    ) ;
  else if( std::string::npos != uType.find("BREMCHI2"  ))return PathFromContext( context , BremChi2   ) ;
  else if( std::string::npos != uType.find("ECALCHI2"  ))return PathFromContext( context , EcalChi2   ) ;
  else if( std::string::npos != uType.find("CLUSCHI2"  ))return PathFromContext( context , ClusChi2   ) ; 
  else if( std::string::npos != uType.find("ECALE"     ))return PathFromContext( context , EcalE      ) ;
  else if( std::string::npos != uType.find("HCALE"     ))return PathFromContext( context , HcalE      ) ;
  else if( std::string::npos != uType.find("PRSE"      ))return PathFromContext( context , PrsE       ) ;
  else if( std::string::npos != uType.find("SPDE"      ))return PathFromContext( context , SpdE       ) ;
  else if( std::string::npos != uType.find("PRSPIDE"   ))return PathFromContext( context , PrsPIDe    ) ; 
  else if( std::string::npos != uType.find("BREMPIDE"  ))return PathFromContext( context , BremPIDe   ) ; 
  else if( std::string::npos != uType.find("ECALPIDE"  ))return PathFromContext( context , EcalPIDe   ) ;
  else if( std::string::npos != uType.find("HCALPIDE"  ))return PathFromContext( context , HcalPIDe   ) ;
  else if( std::string::npos != uType.find("ECALPIDMU" ))return PathFromContext( context , EcalPIDmu  ) ;
  else if( std::string::npos != uType.find("HCALPIDMU" ))return PathFromContext( context , HcalPIDmu  ) ;
  else if( std::string::npos != uType.find("INBREM" ))return PathFromContext( context , InBrem        ) ;
  else if( std::string::npos != uType.find("INECAL" ))return PathFromContext( context , InEcal        ) ;
  else if( std::string::npos != uType.find("INHCAL" ))return PathFromContext( context , InHcal        ) ;
  else if( std::string::npos != uType.find("INPRS"  ))return PathFromContext( context , InPrs         ) ;
  else if( std::string::npos != uType.find("INSPD"  ))return PathFromContext( context , InSpd         ) ;
  else if( std::string::npos != uType.find("PHOTONFROMMERGEDID"  ))return PathFromContext( context , PhotonFromMergedID   );
  else if( std::string::npos != uType.find("SPLITPHOTONID"  ))return PathFromContext( context , PhotonFromMergedID   ); //same
  else if( std::string::npos != uType.find("PHOTONID"  ))return PathFromContext( context , PhotonID   ) ;// after SplitPhotonID
  else if( std::string::npos != uType.find("MERGEDID"  ))return PathFromContext( context , MergedID   ) ;// after PhotonFromMerged
  return "";
}

    


// Track location from context - specific path
std::vector<std::string>  LHCb::CaloAlgUtils::TrackLocations( std::string context){
  std::vector<std::string> locs;
  using namespace LHCb::TrackLocation;

  
  // ----  push first the predefined contexts
  // if( toUpper(context) == "HLT1UNFITTED" || toUpper(context) == "HLT1")locs.push_back("/HLT1/Track/Unfitted/Default"); 
  // else if( toUpper(context) == "HLT1FITTED")locs.push_back( "/HLT1/Track/Fitted/Default"); 
  // else if( toUpper(context) == "HLT2UNFITTED" || toUpper(context) == "HLT2")locs.push_back("/HLT2/Track/Unfitted/Default"); 
  // else if( toUpper(context) == "HLT2FITTED")locs.push_back("/HLT2/Track/Fitted/Default"); 
  if( hltContext( context ) )locs.push_back("Hlt2/Track/Forward");
  else if ( toUpper(context) == "OFFLINE" || context == "" )locs.push_back(Default);  // Rec/Track/Best
  else locs.push_back(Default); // default is offline

  //locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context , Default )); // no path from context for Tracks

  return locs;
}

const LHCb::CaloCluster*  LHCb::CaloAlgUtils::ClusterFromHypo(const LHCb::CaloHypo* hypo,bool split){
  if( NULL == hypo)return NULL;

  if( 1 == hypo->clusters().size() )return hypo->clusters().front(); // single hypo

  const SmartRefVector<LHCb::CaloCluster>& clusters = hypo->clusters();
  // split hypo
  if( NULL != hypo->parent() && NULL != hypo->parent()->registry() ){
    std::string hcont = hypo->parent()->registry()->identifier();
    std::string uHcont = toUpper( hcont );
    bool hsplit = ( std::string::npos != uHcont.find( "SPLIT" ) && 1 < hypo->clusters().size() ); // split hypo
    if( !hsplit )return hypo->clusters().front(); // SHOULD NEVER OCCUR
    SmartRef<LHCb::CaloCluster> cluster ;
    for(LHCb::CaloHypo::Clusters::const_iterator icl =  clusters.begin();icl!= clusters.end();++icl){
      if( 0 == *icl)continue;
      std::string ccont = (*icl)->parent()->registry()->identifier();
      std::string uCcont = toUpper( ccont );
      if( split  &&  std::string::npos != uCcont.find( "SPLIT" ) )cluster = *icl;   // return splitCluster
      else if( !split &&  std::string::npos == uCcont.find( "SPLIT" ) )cluster = *icl; // return mainCluster
    }
    return cluster;
    
  }else{ // unregistered caloHypo -> return the smallest/largest cluster according to getsplit
    SmartRef<LHCb::CaloCluster> minCl ;
    SmartRef<LHCb::CaloCluster> maxCl ;
    unsigned int min = 999999;
    unsigned int max = 0;
    for(LHCb::CaloHypo::Clusters::const_iterator icl = clusters.begin();icl!=clusters.end();++icl){
      if( 0 == *icl)continue;
      if( (*icl)->entries().size() < min){
        minCl = *icl;
        min = (*icl)->entries().size();
      }
      if( (*icl)->entries().size() > max){
        maxCl = *icl;
        max = (*icl)->entries().size();
      }
    }
    return split ? minCl : maxCl;
  }
}

  

bool LHCb::CaloAlgUtils::StringMatcher(std::vector<std::string> refs, std::string name){

  bool ok=false;
  bool ko=true;

  if( refs.empty())return true;
  for( std::vector<std::string>::iterator iref=refs.begin() ; refs.end() != iref; ++iref){
    std::string ref = *iref;
    if( toUpper(ref) == "ALL")return true;
    if( toUpper(ref) == "NONE")return false;
    bool match = StringMatcher( ref , name );
    if( ref.find("!") != std::string::npos && match)ko=false;
    else if( match )ok=true;
  }
  if( !ko )return  false; // rejection win
  if(  ok )return  true;
  return false;
}


bool LHCb::CaloAlgUtils::StringMatcher(std::string ref, std::string name){

  std::string uref=ref;
  if( toUpper(uref) == "ALL")return true;
  if( toUpper(uref) == "NONE")return false;
  std::string uname=name;
  bool rev=false;
  if( std::string::npos != uref.find("!")  ){
    rev=true;
    int ind=uref.find_first_of("!");
    uref =  uref.erase(ind,1);
  }
  bool exact=true;
  if( std::string::npos !=  uref.find("*") ){
    exact=false;
    if( uref.find_first_of("*") != std::string::npos){
    int i1=uref.find_first_of("*");
    uref.erase(i1,1);
    }
    if( uref.find_last_of("*") != std::string::npos){
      int i2=uref.find_last_of("*");
      uref.erase(i2,1);
    }
  }
  
  uname = toUpper(uname);
  uref  = toUpper(uref);
  
  bool match;
  if( exact )
    match = ( uref == uname );
  else
    match = (uname.find(uref) != std::string::npos);

  return rev ? !match : match;

}

