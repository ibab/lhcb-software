#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/ProtoParticle.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloAlgUtils.h"


// Default location for CaloObject as function of detector and possibly context
std::string LHCb::CaloAlgUtils::DeCaloLocation( std::string name ){
  std::string det = LHCb::CaloAlgUtils::CaloNameFromAlg( name );
  if ( det == "Ecal" ) return  DeCalorimeterLocation::Ecal;
  else if ( det == "Hcal")return  DeCalorimeterLocation::Hcal;
  else if ( det == "Prs")return  DeCalorimeterLocation::Prs;
  else if ( det == "Spd")return  DeCalorimeterLocation::Spd;
      return "";
}
std::string  LHCb::CaloAlgUtils::CaloDigitLocation( std::string name  , std::string context ){
  std::string det = LHCb::CaloAlgUtils::CaloNameFromAlg( name );
  using namespace LHCb::CaloDigitLocation;
  if ( det == "Ecal" ) return LHCb::CaloAlgUtils::PathFromContext(context, Ecal, Ecal);//same digit container for Hlt and offline
  else if ( det == "Hcal")return  LHCb::CaloAlgUtils::PathFromContext(context, Hcal, Hcal);
  else if ( det == "Prs")return   LHCb::CaloAlgUtils::PathFromContext(context, Prs, Prs );
  else if ( det == "Spd")return   LHCb::CaloAlgUtils::PathFromContext(context, Spd,Spd);
  // default is Ecal in offline mode
  return Ecal;
}


std::string  LHCb::CaloAlgUtils::CaloSplitClusterLocation( std::string context ){
  using namespace LHCb::CaloClusterLocation;
  return LHCb::CaloAlgUtils::PathFromContext( context , EcalSplit , EcalSplitHlt ) ;
}


std::string  LHCb::CaloAlgUtils::CaloClusterLocation( std::string name , std::string context ){
  std::string det = LHCb::CaloAlgUtils::CaloNameFromAlg( name );
  using namespace LHCb::CaloClusterLocation;
  if ( det == "Ecal" )return LHCb::CaloAlgUtils::PathFromContext( context , Ecal , EcalHlt );
  else if ( det == "Hcal" )return LHCb::CaloAlgUtils::PathFromContext( context ,Hcal , HcalHlt );
  // default is Ecal in offline mode
  return Ecal;
}

std::string  LHCb::CaloAlgUtils::CaloHypoLocation( std::string type , std::string context){
  using namespace LHCb::CaloHypoLocation;
  if ( type == "Electrons" ) return LHCb::CaloAlgUtils::PathFromContext( context ,Electrons , ElectronsHlt );
  else if ( type == "Photons" )return LHCb::CaloAlgUtils::PathFromContext( context ,Photons , PhotonsHlt );
  else if ( type == "MergedPi0s" )return LHCb::CaloAlgUtils::PathFromContext( context ,MergedPi0s , MergedPi0sHlt );
  else if ( type == "SplitPhotons" )return LHCb::CaloAlgUtils::PathFromContext( context , SplitPhotons , SplitPhotonsHlt );
  // default is Ecal in offline mode
  return "";
}

std::string LHCb::CaloAlgUtils::CaloIdLocation( std::string type, std::string context){
  using namespace LHCb::CaloIdLocation;
  if( type == "BremMatch" )return PathFromContext( context , BremMatch, BremMatchHlt  ) ;
  else if( type == "ElectronMatch" )return PathFromContext( context , ElectronMatch, ElectronMatchHlt  ) ;
  else if( type == "ClusterMatch"  )return PathFromContext( context , ClusterMatch , ClusterMatchHlt   ) ;
  else if( type == "BremChi2"  )return PathFromContext( context , BremChi2 , BremChi2Hlt   ) ;
  else if( type == "EcalChi2"  )return PathFromContext( context , EcalChi2 , EcalChi2Hlt   ) ;
  else if( type == "ClusChi2"  )return PathFromContext( context , ClusChi2 , ClusChi2Hlt   ) ; 
  else if( type == "EcalE"     )return PathFromContext( context , EcalE    , EcalEHlt      ) ;
  else if( type == "HcalE"     )return PathFromContext( context , HcalE    , HcalEHlt      ) ;
  else if( type == "PrsE"      )return PathFromContext( context , PrsE     , PrsEHlt       ) ;
  else if( type == "SpdE"      )return PathFromContext( context , SpdE     , SpdEHlt       ) ;
  else if( type == "PrsPIDe"   )return PathFromContext( context , PrsPIDe  , PrsPIDeHlt    ) ; 
  else if( type == "BremPIDe"  )return PathFromContext( context , BremPIDe , BremPIDeHlt   ) ; 
  else if( type == "EcalPIDe"  )return PathFromContext( context , EcalPIDe , EcalPIDeHlt   ) ;
  else if( type == "HcalPIDe"  )return PathFromContext( context , HcalPIDe , HcalPIDeHlt   ) ;
  else if( type == "EcalPIDmu" )return PathFromContext( context , EcalPIDmu, EcalPIDmuHlt  ) ;
  else if( type == "HcalPIDmu" )return PathFromContext( context , HcalPIDmu, HcalPIDmuHlt  ) ;
  else if( type == "InBrem" )return PathFromContext( context , InBrem   , InBremHlt  ) ;
  else if( type == "InEcal" )return PathFromContext( context , InEcal   , InEcalHlt  ) ;
  else if( type == "InHcal" )return PathFromContext( context , InHcal   , InHcalHlt  ) ;
  else if( type == "InPrs"  )return PathFromContext( context , InPrs    , InPrsHlt   ) ;
  else if( type == "InSpd"  )return PathFromContext( context , InSpd    , InSpdHlt   ) ;
  return "";
}

    



std::string  LHCb::CaloAlgUtils::TrackLocation( std::string context){
  using namespace LHCb::TrackLocation;
  return LHCb::CaloAlgUtils::PathFromContext( context , Default , HltForward );
  return "";
}
