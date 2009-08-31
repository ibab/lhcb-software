// ============================================================================
#ifndef CALOUTILS_CALOALGUTILS_H 
#define CALOUTILS_CALOALGUTILS_H 1
#include <string>

namespace LHCb{
  namespace CaloAlgUtils{
    inline std::string CaloNameFromAlg( std::string algName ){
      std::string name = "";
      // find tool name separator if any
      int index = algName.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
      std::string inst = algName.substr(index,std::string::npos);
      std::string uInst( inst );
      std::transform( inst.begin() , inst.end() , uInst.begin () , ::toupper ) ;
      if( uInst.rfind( "ECAL" )      != std::string::npos ) name = "Ecal";
      else if( uInst.rfind( "HCAL" ) != std::string::npos ) name = "Hcal";
      else if ( uInst.rfind( "PRS" ) != std::string::npos ) name = "Prs";
      else if ( uInst.rfind( "SPD" ) != std::string::npos ) name = "Spd";
      return name ;
    }

    inline std::string PathFromContext( std::string context , std::string def , std::string hltDef =""){
      std::string loc = "";
      std::string uCont( context );
      std::transform( context.begin() , context.end() , uCont.begin () , ::toupper ) ;

      if( uCont == "" || uCont == "OFFLINE" )return def;          // default standard Offline
      else if( uCont == "HLT" && hltDef != "" )return hltDef;     // default standard Hlt
      else{                                                       // non-standard locations
        int index = def.find_first_of("/") +1 ;
        loc =  context + "/" + def.substr(index, std::string::npos  );
      }      
      return loc;
    }
    // Default location for CaloObject as function of detector and possibly context
    std::string DeCaloLocation( std::string name );
    std::string CaloDigitLocation( std::string name   , std::string context="");
    std::string CaloClusterLocation( std::string name , std::string context="");
    std::string CaloSplitClusterLocation( std::string context="");
    std::string CaloHypoLocation( std::string type , std::string context="");
    std::string CaloIdLocation( std::string type, std::string context = "");
    std::string TrackLocation( std::string context="" );

  } // end of namespace CaloUtils
}// end of namespace LHCb
#endif // CALOUTILS_CALOALGUTILS_H
