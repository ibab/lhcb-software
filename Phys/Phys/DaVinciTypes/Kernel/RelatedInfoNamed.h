#ifndef CONE_INFO_H
#define CONE_INFO_H 1

#include "Event/RelatedInfoMap.h"

class RelatedInfoNamed : public LHCb::RelatedInfoMap {

  public: 

    /// Index values
    enum index { 
      UNKNOWN      = -1,       // Unknown/illegal value of the index

      // Cone variables 
      CONEANGLE    = 0,        // Cone angle
      CONEMULT     = CONEANGLE+1,  // Cone multiplicity
      CONEPX       = CONEANGLE+2,  // Cone Px
      CONEPY       = CONEANGLE+3,  // Cone Py
      CONEPZ       = CONEANGLE+4,  // Cone Pz
      CONEP        = CONEANGLE+5,  // Cone P
      CONEPT       = CONEANGLE+6,  // Cone Pt
      CONEPXASYM   = CONEANGLE+7,  // Cone Px asymmetry
      CONEPYASYM   = CONEANGLE+8,  // Cone Py asymmetry
      CONEPZASYM   = CONEANGLE+9,  // Cone Pz asymmetry
      CONEPASYM    = CONEANGLE+10, // Cone P asymmetry
      CONEPTASYM   = CONEANGLE+11, // Cone Pt asymmetry
      CONEDELTAETA = CONEANGLE+12, // Cone DeltaEta
      CONEDELTAPHI = CONEANGLE+13, // Cone DeltaPhi

      // Cone variables for EW      
      EWCONEANGLE   = CONEDELTAPHI+1,  // Cone angle
      EWCONEMULT    = EWCONEANGLE+1,   // Cone multiplicity
      EWCONEPX      = EWCONEANGLE+2,   // Cone Px
      EWCONEPY      = EWCONEANGLE+3,   // Cone Py
      EWCONEPZ      = EWCONEANGLE+4,   // Cone Pz
      EWCONEVP      = EWCONEANGLE+5,   // Cone vP
      EWCONEVPT     = EWCONEANGLE+6,   // Cone vPt
      EWCONESP      = EWCONEANGLE+7,   // Cone sP
      EWCONESPT     = EWCONEANGLE+8,   // Cone sPt
      EWCONETP      = EWCONEANGLE+9,   // Cone tP
      EWCONETPT     = EWCONEANGLE+10,  // Cone tPt
      EWCONEMINPTE  = EWCONEANGLE+11,  // Cone min(Pt_e)
      EWCONEMAXPTE  = EWCONEANGLE+12,  // Cone max(Pt_e)
      EWCONEMINPTMU = EWCONEANGLE+13,  // Cone min(Pt_mu)
      EWCONEMAXPTMU = EWCONEANGLE+14,  // Cone max(Pt_mu)
      EWCONENMULT   = EWCONEANGLE+15,  // Neutral cone multiplicity
      EWCONENPX     = EWCONEANGLE+16,  // Neutral Cone Px
      EWCONENPY     = EWCONEANGLE+17,  // Neutral Cone Py
      EWCONENPZ     = EWCONEANGLE+18,  // Neutral Cone Pz
      EWCONENVP     = EWCONEANGLE+19,  // Neutral Cone vP
      EWCONENVPT    = EWCONEANGLE+20,  // Neutral Cone vPt
      EWCONENSP     = EWCONEANGLE+21,  // Neutral Cone sP
      EWCONENSPT    = EWCONEANGLE+22,  // Neutral Cone sPt

      // Vertex isolation variables
      VTXISONUMVTX            = EWCONENSPT+1,   // NumVtxWithinChi2WindowOneTrack
      VTXISODCHI2ONETRACK     = VTXISONUMVTX+1, // SmallestDeltaChi2OneTrack
      VTXISODCHI2MASSONETRACK = VTXISONUMVTX+2, // SmallestDeltaChi2MassOneTrack
      VTXISODCHI2TWOTRACK     = VTXISONUMVTX+3, // SmallestDeltaChi2OneTrack
      VTXISODCHI2MASSTWOTRACK = VTXISONUMVTX+4  // SmallestDeltaChi2MassOneTrack

    };

    static std::string nameByIndex(int key) {
      switch (key) {
        case RelatedInfoNamed::CONEANGLE : return "CONEANGLE"; 
        case RelatedInfoNamed::CONEMULT  : return "CONEMULT"; 
        case RelatedInfoNamed::CONEPX    : return "CONEPX"; 
        case RelatedInfoNamed::CONEPY    : return "CONEPY"; 
        case RelatedInfoNamed::CONEPZ    : return "CONEPZ"; 
        case RelatedInfoNamed::CONEP     : return "CONEP"; 
        case RelatedInfoNamed::CONEPT    : return "CONEPT"; 
        case RelatedInfoNamed::CONEPXASYM    : return "CONEPXASYM"; 
        case RelatedInfoNamed::CONEPYASYM    : return "CONEPYASYM"; 
        case RelatedInfoNamed::CONEPZASYM    : return "CONEPZASYM"; 
        case RelatedInfoNamed::CONEPASYM     : return "CONEPASYM"; 
        case RelatedInfoNamed::CONEPTASYM    : return "CONEPTASYM"; 
        case RelatedInfoNamed::CONEDELTAETA  : return "CONEDELTAETA"; 
        case RelatedInfoNamed::CONEDELTAPHI  : return "CONEDELTAPHI"; 

        case RelatedInfoNamed::EWCONEANGLE : return "EWCONEANGLE"; 
        case RelatedInfoNamed::EWCONEMULT  : return "EWCONEMULT"; 
        case RelatedInfoNamed::EWCONEPX    : return "EWCONEPX"; 
        case RelatedInfoNamed::EWCONEPY    : return "EWCONEPY"; 
        case RelatedInfoNamed::EWCONEPZ    : return "EWCONEPZ"; 
        case RelatedInfoNamed::EWCONEVP    : return "EWCONEVP"; 
        case RelatedInfoNamed::EWCONEVPT   : return "EWCONEVPT"; 
        case RelatedInfoNamed::EWCONESP    : return "EWCONESP"; 
        case RelatedInfoNamed::EWCONESPT   : return "EWCONESPT"; 
        case RelatedInfoNamed::EWCONETP    : return "EWCONETP"; 
        case RelatedInfoNamed::EWCONETPT   : return "EWCONETPT"; 
        case RelatedInfoNamed::EWCONEMINPTE   : return "EWCONEMINPTE"; 
        case RelatedInfoNamed::EWCONEMAXPTE   : return "EWCONEMAXPTE"; 
        case RelatedInfoNamed::EWCONEMINPTMU  : return "EWCONEMINPTMU"; 
        case RelatedInfoNamed::EWCONEMAXPTMU  : return "EWCONEMAXPTMU"; 
        case RelatedInfoNamed::EWCONENMULT  : return "EWCONENMULT"; 
        case RelatedInfoNamed::EWCONENPX    : return "EWCONENPX"; 
        case RelatedInfoNamed::EWCONENPY    : return "EWCONENPY"; 
        case RelatedInfoNamed::EWCONENPZ    : return "EWCONENPZ"; 
        case RelatedInfoNamed::EWCONENVP    : return "EWCONENVP"; 
        case RelatedInfoNamed::EWCONENVPT   : return "EWCONENVPT"; 
        case RelatedInfoNamed::EWCONENSP    : return "EWCONENSP"; 
        case RelatedInfoNamed::EWCONENSPT   : return "EWCONENSPT"; 

        case RelatedInfoNamed::VTXISONUMVTX            : return "VTXISONUMVTX"; 
        case RelatedInfoNamed::VTXISODCHI2ONETRACK     : return "VTXISODCHI2ONETRACK"; 
        case RelatedInfoNamed::VTXISODCHI2MASSONETRACK : return "VTXISODCHI2MASSONETRACK"; 
        case RelatedInfoNamed::VTXISODCHI2TWOTRACK     : return "VTXISODCHI2TWOTRACK";
        case RelatedInfoNamed::VTXISODCHI2MASSTWOTRACK : return "VTXISODCHI2MASSTWOTRACK";

        default : return "UNKNOWN"; 
      }
    }

    static short int indexByName(std::string name) {
      short i;
      for (i=0; i<=RelatedInfoNamed::VTXISODCHI2MASSTWOTRACK; i++) {
        if ( name.compare(nameByIndex(i) ) == 0 ) return i; 
      }
      return RelatedInfoNamed::UNKNOWN; 
    }

}; 

#endif
