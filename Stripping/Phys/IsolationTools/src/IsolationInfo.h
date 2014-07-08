#ifndef ISOLATION_INFO_H
#define ISOLATION_INFO_H 1

#include "Kernel/IRelatedInfoTool.h"
#include "Event/RelatedInfoMap.h"

class IsolationInfo : public LHCb::RelatedInfoMap {

  public: 

    /// Index values
    enum index { 
      Unknown = -1,        // Unknown/illegal value of the index
      Default = 0,
      VertexIsoFirstBDTValue = 1,  // Top BDT value from VertexIsolation BDT
      VertexIsoSecondBDTValue = 2,  // Second BDT value form VertexIsolationBDT
      VertexIsoThirdBDTValue = 3,  // Third BDT value from VertexIsolatinBDT
      TrackIsoFirstBDTValue = 4,  // Top BDT value from TrackIsolation BDT
      TrackIsoSecondBDTValue = 5,  // Second BDT value form TrackIsolationBDT
      TrackIsoThirdBDTValue = 6,  // Third BDT value from TrackIsolatinBDT
   };
    
    static std::string nameByIndex(int key) {
      switch (key) {
        case IsolationInfo::VertexIsoFirstBDTValue : 
            return "VertexIsoFirstBDTValue"; 
        case IsolationInfo::VertexIsoSecondBDTValue : 
            return "VertexIsoSecondBDTValue"; 
        case IsolationInfo::VertexIsoThirdBDTValue : 
            return "VertexIsoThirdBDTValue"; 
        case IsolationInfo::TrackIsoFirstBDTValue : 
            return "TrackIsoFirstBDTValue"; 
        case IsolationInfo::TrackIsoSecondBDTValue : 
            return "TrackIsoSecondBDTValue"; 
        case IsolationInfo::TrackIsoThirdBDTValue : 
            return "TrackIsoThirdBDTValue"; 
       default : return "Unknown"; 
      }
    }

    static short int indexByName(std::string name) {
      if (name.compare("VertexIsoFirstBDTValue") == 0) 
          return IsolationInfo::VertexIsoFirstBDTValue ; 
      if (name.compare("VertexIsoSecondBDTValue") == 0) 
          return IsolationInfo::VertexIsoSecondBDTValue ; 
      if (name.compare("VertexIsoThirdBDTValue") == 0) 
          return IsolationInfo::VertexIsoThirdBDTValue ; 
      if (name.compare("TrackIsoFirstBDTValue") == 0) 
          return IsolationInfo::TrackIsoFirstBDTValue ; 
      if (name.compare("TrackIsoSecondBDTValue") == 0) 
          return IsolationInfo::TrackIsoSecondBDTValue ; 
      if (name.compare("TrackIsoThirdBDTValue") == 0) 
          return IsolationInfo::TrackIsoThirdBDTValue ; 
     return IsolationInfo::Unknown; 
    }

}; 

#endif
