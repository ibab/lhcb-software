#ifndef L0NAMES_L0NAMES_H
#define L0NAMES_L0NAMES_H 1
namespace L0Names {
	 inline int l0ChannelID(const std::string& e){
		 if (e == "electron") return 1;
     if (e == "photon") return 3;
		 if (e == "localPi0") return 5;
		 if (e == "globalPi0") return 7;
		 if (e == "hadron") return 9;
		 if (e == "muon") return 11;
		 if (e == "dimuon") return 13;
		 return -1; 
	}
  inline std::string l0ChannelStr(int e){
    if (e == 1) return "electron";
    if (e == 3) return "photon";
    if (e == 5) return "localPi0";
    if (e == 7) return "globalPi0";
    if (e == 9) return "hadron";
    if (e == 11) return "muon";
    if (e == 13) return "dimuon";  
    return "l0ChannelUnknown";
  }
};
#endif
