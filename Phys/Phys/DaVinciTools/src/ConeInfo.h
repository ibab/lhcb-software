#ifndef CONE_INFO_H
#define CONE_INFO_H 1

#include "Kernel/IRelatedInfoTool.h"

class ConeInfo : public RelatedInfoMap {

  public: 

    /// Index values
    enum index { 
      Unknown = -1,        // Unknown/illegal value of the index
      Angle = 0,           // Cone angle
      Mult = Angle+1,      // Cone multiplicity
      PX = Angle+2,        // Cone Px
      PY = Angle+3,        // Cone Py
      PZ = Angle+4,        // Cone Pz
      P =  Angle+5,        // Cone P
      PT = Angle+6,        // Cone Pt
      PXAsym = Angle+7,    // Cone Px asymmetry
      PYAsym = Angle+8,    // Cone Py asymmetry
      PZAsym = Angle+9,    // Cone Pz asymmetry
      PAsym =  Angle+10,   // Cone P asymmetry
      PTAsym = Angle+11,   // Cone Pt asymmetry
      DeltaEta = Angle+12, // Cone DeltaEta
      DeltaPhi = Angle+13  // Cone DeltaPhi
    };
    
    static std::string nameByIndex(int key) {
      switch (key) {
        case ConeInfo::Angle : return "Angle"; 
        case ConeInfo::Mult  : return "Mult"; 
        case ConeInfo::PX    : return "PX"; 
        case ConeInfo::PY    : return "PY"; 
        case ConeInfo::PZ    : return "PZ"; 
        case ConeInfo::P     : return "P"; 
        case ConeInfo::PT    : return "PT"; 
        case ConeInfo::PXAsym    : return "PXAsym"; 
        case ConeInfo::PYAsym    : return "PYAsym"; 
        case ConeInfo::PZAsym    : return "PZAsym"; 
        case ConeInfo::PAsym     : return "PAsym"; 
        case ConeInfo::PTAsym    : return "PTAsym"; 
        case ConeInfo::DeltaEta  : return "DeltaEta"; 
        case ConeInfo::DeltaPhi  : return "DeltaPhi"; 
        default : return "Unknown"; 
      }
    }

    static short int indexByName(std::string name) {
      if (name.compare("Angle") == 0) return ConeInfo::Angle; 
      if (name.compare("Mult") == 0)  return ConeInfo::Mult; 
      if (name.compare("PX") == 0)    return ConeInfo::PX; 
      if (name.compare("PY") == 0)    return ConeInfo::PY; 
      if (name.compare("PZ") == 0)    return ConeInfo::PZ; 
      if (name.compare("P") == 0)     return ConeInfo::P; 
      if (name.compare("PT") == 0)    return ConeInfo::PT; 
      if (name.compare("PXAsym") == 0) return ConeInfo::PXAsym; 
      if (name.compare("PYAsym") == 0) return ConeInfo::PYAsym; 
      if (name.compare("PZAsym") == 0) return ConeInfo::PZAsym; 
      if (name.compare("PAsym") == 0)  return ConeInfo::PAsym; 
      if (name.compare("PTAsym") == 0) return ConeInfo::PTAsym; 
      if (name.compare("DeltaEta") == 0) return ConeInfo::DeltaEta; 
      if (name.compare("DeltaPhi") == 0) return ConeInfo::DeltaPhi; 
      return ConeInfo::Unknown; 
    }

}; 

#endif
