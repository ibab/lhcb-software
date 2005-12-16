// $Id: MCTrackInfoBits.h,v 1.1 2005-12-16 10:11:58 cattanem Exp $
#ifndef MCEVENT_MCTRACKINFOBITS_H 
#define MCEVENT_MCTRACKINFOBITS_H 1

// Include files

// Defines bits for MCTrackInfo

namespace MCTrackInfoBits {

  enum flagMasks { 
    maskVeloR      = 0x00000001,
    maskVeloPhi    = 0x00000002,
    maskTT1        = 0x00000004,
    maskTT2        = 0x00000008,
    maskT1X        = 0x00000010,
    maskT1S        = 0x00000020,
    maskT2X        = 0x00000040,
    maskT2S        = 0x00000080,
    maskT3X        = 0x00000100,
    maskT3S        = 0x00000200,

    maskHasVelo    = 0x00000003,   // R + Phi
    maskHasTT      = 0x0000000C,   // TT1 and TT2
    maskHasT       = 0x000003F0,   // all T stations

    maskAccVeloR   = 0x00000400,
    maskAccVeloPhi = 0x00000800,
    maskAccTT1     = 0x00001000,
    maskAccTT2     = 0x00002000,
    maskAccT1X     = 0x00004000,
    maskAccT1S     = 0x00008000,
    maskAccT2X     = 0x00010000,
    maskAccT2S     = 0x00020000,
    maskAccT3X     = 0x00040000,
    maskAccT3S     = 0x00080000,

    maskAccVelo    = 0x00000C00,  // R and Phi hits
    maskAccTT      = 0x00003000,  // TT1 and TT2
    maskAccT       = 0x000FC000,  // T stations

    multVeloR      = 20,
    multVeloPhi    = 25,

    maskMultVeloR  = 0x01F00000,  // Velo R CLuster multiplicity
    maskMultVeloPhi= 0x3E000000   // VeloPhi cluster multipliity

  };
  
}

#endif // MCEVENT_MCTRACKINFOBITS_H
