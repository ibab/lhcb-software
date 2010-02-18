// $Id: RichDetParams.h,v 1.1 2005-06-18 11:36:05 jonrob Exp $
#ifndef RICHKERNEL_RICHDETPARAMS_H
#define RICHKERNEL_RICHDETPARAMS_H 1

// WARNING : This file is temporary and will be removed later on
//           It should only be used for monitoring purposes

#define RADIATOR_NAMES      const std::string radiator[] = { "aerogel", "C4F10", "CF4" }
#define RICH_NAMES          const std::string rich[] =     { "Rich1", "Rich2" }
#define HYPOTHESIS_NAMES    const std::string hypothesis[] = { "el", "mu", "pi", "Ka", "Pr" }
#define RICH_HISTO_OFFSET   const int richOffset = 10000
#define RAD_HISTO_OFFSET    const int radOffset  = 1000
#define HYPO_HISTO_OFFSET   const int hypoOffset = 100
#define MAX_CKTHETA_RAD     const double maxCkTheta[] = { 0.255, 0.055, 0.032 }
#define MIN_CKTHETA_RAD     const double minCkTheta[] = { 0.05,  0.0,   0.0 }

#define RADIATOR_MAX_CKTHETA \
const double ckThetaMax[] = { 0.3, 0.06, 0.06 }

//                               Aerogel  C4F10    CF4
#define RADIATOR_GLOBAL_POSITIONS_X \
const double xRadEntGlo[]  = {  420.0,  1500.0,  3500.0 };\
const double xRadExitGlo[] = {  420.0,  1500.0,  4000.0 }
#define RADIATOR_GLOBAL_POSITIONS_Y \
const double yRadEntGlo[]  = {  320.0,  1500.0,  3500.0 };\
const double yRadExitGlo[] = {  320.0,  1500.0,  4000.0 }
#define RADIATOR_GLOBAL_POSITIONS_Z \
const double zRadEntGlo[]  = { 1100.0,   900.0,  9500.0 };\
const double zRadExitGlo[] = { 1200.0,  2100.0, 12000.0 }
#define RADIATOR_GLOBAL_LENGTHS \
const double zRadLenMin[]  = {    0.0,     0.0,     0.0 };\
const double zRadLenMax[]  = {   60.0,  2000.0,  2500.0 }
#define RADIATOR_GLOBAL_POSITIONS \
 RADIATOR_GLOBAL_POSITIONS_X; \
 RADIATOR_GLOBAL_POSITIONS_Y; \
 RADIATOR_GLOBAL_POSITIONS_Z; \
 RADIATOR_GLOBAL_LENGTHS;

//                                 Rich1     Rich2
#define PD_GLOBAL_POSITIONS_X \
const double xMinPDGlo[]  = {    -800.0,   -4500.0   };\
const double xMaxPDGlo[]  = {     800.0,    4500.0   }
#define PD_GLOBAL_POSITIONS_Y \
const double yMinPDGlo[]  = {   -1200.0,    -750.0   };\
const double yMaxPDGlo[]  = {    1200.0,     750.0   }
#define PD_GLOBAL_POSITIONS_Z \
const double zMinPDGlo[]  = {    1150.0,   10440.0   };\
const double zMaxPDGlo[]  = {    1750.0,   11100.0   }
#define PD_GLOBAL_POSITIONS \
 PD_GLOBAL_POSITIONS_X; \
 PD_GLOBAL_POSITIONS_Y; \
 PD_GLOBAL_POSITIONS_Z;

//                                 Rich1     Rich2
#define PD_LOCAL_POSITIONS_X \
const double xMinPDLoc[] = {    -700.0,    -700.0   };\
const double xMaxPDLoc[] = {     700.0,     700.0   }
#define PD_LOCAL_POSITIONS_Y \
const double yMinPDLoc[] = {    -700.0,    -700.0   };\
const double yMaxPDLoc[] = {     700.0,     700.0   }
#define PD_LOCAL_POSITIONS_Z \
const double zMinPDLoc[] = {      -5.0,      -5.0   };\
const double zMaxPDLoc[] = {       5.0,       5.0   }
#define PD_LOCAL_POSITIONS \
 PD_LOCAL_POSITIONS_X; \
 PD_LOCAL_POSITIONS_Y; \
 PD_LOCAL_POSITIONS_Z;

//                                 Rich1     Rich2
#define SPHERICAL_MIRROR_GLOBAL_POSITIONS_X \
const double xMinSpheGlo[] = {   -1000.0,   -1100.0   };\
const double xMaxSpheGlo[] = {    1000.0,    1100.0   }
#define SPHERICAL_MIRROR_GLOBAL_POSITIONS_Y \
const double yMinSpheGlo[] = {   -1000.0,   -1100.0   };\
const double yMaxSpheGlo[] = {    1000.0,    1100.0   }
#define SPHERICAL_MIRROR_GLOBAL_POSITIONS_Z \
const double zMinSpheGlo[] = {    1800.0,   11100.0   };\
const double zMaxSpheGlo[] = {    2080.0,   11800.0   }
#define SPHERICAL_MIRROR_GLOBAL_POSITIONS \
 SPHERICAL_MIRROR_GLOBAL_POSITIONS_X; \
 SPHERICAL_MIRROR_GLOBAL_POSITIONS_Y; \
 SPHERICAL_MIRROR_GLOBAL_POSITIONS_Z;

//                                 Rich1     Rich2
#define FLAT_MIRROR_GLOBAL_POSITIONS_X \
const double xMinFlatGlo[] = {    -575.0,   -3000.0   };\
const double xMaxFlatGlo[] = {     575.0,    3000.0   }
#define FLAT_MIRROR_GLOBAL_POSITIONS_Y \
const double yMinFlatGlo[] = {   -1100.0,   -1000.0   };\
const double yMaxFlatGlo[] = {    1100.0,    1000.0   }
#define FLAT_MIRROR_GLOBAL_POSITIONS_Z \
const double zMinFlatGlo[] = {    1175.0,    9600.0   };\
const double zMaxFlatGlo[] = {    1420.0,    9900.0   }
#define FLAT_MIRROR_GLOBAL_POSITIONS \
 FLAT_MIRROR_GLOBAL_POSITIONS_X; \
 FLAT_MIRROR_GLOBAL_POSITIONS_Y; \
 FLAT_MIRROR_GLOBAL_POSITIONS_Z;

#endif // RICHKERNEL_RICHDETPARAMS_H
