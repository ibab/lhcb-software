
//-----------------------------------------------------------------------------
/** @file RichDetParams.h
 *
 * A collection of general parameters for histogramming limits etc.
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-06-20
 *
 * @todo Remove this file and replace with IRichDetParameters tool
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHDETPARAMS_H
#define RICHKERNEL_RICHDETPARAMS_H 1

#define RADIATOR_NAMES       constexpr std::string radiator[] = { "aerogel", "Rich1Gas", "Rich2Gas" }
#define RICH_NAMES           constexpr std::string rich[] =     { "Rich1", "Rich2" }
#define HYPOTHESIS_NAMES     constexpr std::string hypothesis[] = { "el", "mu", "pi", "Ka", "Pr", "X" }
#define RICH_HISTO_OFFSET    constexpr int richOffset = 100000
#define RAD_HISTO_OFFSET     constexpr int radOffset  = 10000
#define HYPO_HISTO_OFFSET    constexpr int hypoOffset = 1000
//            Radiator                                  Aerogel  Rich1Gas  Rich2Gas
#define MAX_CKTHETA_RAD      constexpr double maxCkTheta[] = { 0.31,   0.065,    0.035  }
#define MIN_CKTHETA_RAD      constexpr double minCkTheta[] = { 0.0,    0.001,    0.005  }
#define CKTHETADIFF_RANGE    constexpr double ckRange[]    = { 0.015,  0.01,     0.005  }
#define RADIATOR_MAX_CKTHETA constexpr double ckThetaMax[] = { 0.3,    0.06,     0.06   }

//                               Aerogel  Rich1Gas    Rich2Gas
#define RADIATOR_GLOBAL_POSITIONS_X_ENTRY \
constexpr double xRadEntGlo[]  = {  400.0,  400.0,  3500.0 }
#define RADIATOR_GLOBAL_POSITIONS_X_EXIT \
constexpr double xRadExitGlo[] = {  400.0,  400.0,  4000.0 }
#define RADIATOR_GLOBAL_POSITIONS_X \
 RADIATOR_GLOBAL_POSITIONS_X_ENTRY;\
 RADIATOR_GLOBAL_POSITIONS_X_EXIT
#define RADIATOR_GLOBAL_POSITIONS_Y_ENTRY \
constexpr double yRadEntGlo[]  = {  400.0,  400.0,  3500.0 }
#define RADIATOR_GLOBAL_POSITIONS_Y_EXIT \
constexpr double yRadExitGlo[] = {  400.0,  400.0,  4000.0 }
#define RADIATOR_GLOBAL_POSITIONS_Y \
 RADIATOR_GLOBAL_POSITIONS_Y_ENTRY;\
 RADIATOR_GLOBAL_POSITIONS_Y_EXIT
#define RADIATOR_GLOBAL_POSITIONS_Z \
constexpr double zRadEntGlo[]  = { 1100.0,   900.0,  9200.0 };\
constexpr double zRadExitGlo[] = { 1180.0,  2100.0, 12000.0 }
#define RADIATOR_GLOBAL_LENGTHS \
constexpr double zRadLenMin[]  = {    0.0,     0.0,     0.0 };\
constexpr double zRadLenMax[]  = {   60.0,  2000.0,  2500.0 }
#define RADIATOR_GLOBAL_POSITIONS \
 RADIATOR_GLOBAL_POSITIONS_X; \
 RADIATOR_GLOBAL_POSITIONS_Y; \
 RADIATOR_GLOBAL_POSITIONS_Z; \
 RADIATOR_GLOBAL_LENGTHS;

//                                 Rich1     Rich2
#define PD_GLOBAL_POSITIONS_X \
constexpr double xMinPDGlo[]  = {    -800.0,   -4500.0   };\
constexpr double xMaxPDGlo[]  = {     800.0,    4500.0   }
#define PD_GLOBAL_POSITIONS_Y \
constexpr double yMinPDGlo[]  = {   -1200.0,    -750.0   };\
constexpr double yMaxPDGlo[]  = {    1200.0,     750.0   }
#define PD_GLOBAL_POSITIONS_Z \
constexpr double zMinPDGlo[]  = {    1150.0,   10440.0   };\
constexpr double zMaxPDGlo[]  = {    1750.0,   11100.0   }
#define PD_GLOBAL_POSITIONS \
 PD_GLOBAL_POSITIONS_X; \
 PD_GLOBAL_POSITIONS_Y; \
 PD_GLOBAL_POSITIONS_Z;

//                                 Rich1     Rich2
#define PD_LOCAL_POSITIONS_X \
constexpr double xMinPDLoc[] = {    -650.0,    -800.0   };\
constexpr double xMaxPDLoc[] = {     650.0,     800.0   }
#define PD_LOCAL_POSITIONS_Y \
constexpr double yMinPDLoc[] = {    -650.0,    -800.0   };\
constexpr double yMaxPDLoc[] = {     650.0,     800.0   }
#define PD_LOCAL_POSITIONS_Z \
constexpr double zMinPDLoc[] = {      -5.0,      -5.0   };\
constexpr double zMaxPDLoc[] = {       5.0,       5.0   }
#define PD_LOCAL_POSITIONS \
 PD_LOCAL_POSITIONS_X; \
 PD_LOCAL_POSITIONS_Y; \
 PD_LOCAL_POSITIONS_Z;

//                                 Rich1     Rich2
#define SPHERICAL_MIRROR_GLOBAL_POSITIONS_X \
constexpr double xMinSpheGlo[] = {   -1000.0,   -1100.0   };\
constexpr double xMaxSpheGlo[] = {    1000.0,    1100.0   }
#define SPHERICAL_MIRROR_GLOBAL_POSITIONS_Y \
constexpr double yMinSpheGlo[] = {   -1000.0,   -1100.0   };\
constexpr double yMaxSpheGlo[] = {    1000.0,    1100.0   }
#define SPHERICAL_MIRROR_GLOBAL_POSITIONS_Z \
constexpr double zMinSpheGlo[] = {    1800.0,   11100.0   };\
constexpr double zMaxSpheGlo[] = {    2080.0,   11800.0   }
#define SPHERICAL_MIRROR_GLOBAL_POSITIONS \
 SPHERICAL_MIRROR_GLOBAL_POSITIONS_X; \
 SPHERICAL_MIRROR_GLOBAL_POSITIONS_Y; \
 SPHERICAL_MIRROR_GLOBAL_POSITIONS_Z;

//                                 Rich1     Rich2
#define FLAT_MIRROR_GLOBAL_POSITIONS_X \
constexpr double xMinFlatGlo[] = {    -575.0,   -3000.0   };\
constexpr double xMaxFlatGlo[] = {     575.0,    3000.0   }
#define FLAT_MIRROR_GLOBAL_POSITIONS_Y \
constexpr double yMinFlatGlo[] = {   -1100.0,   -1000.0   };\
constexpr double yMaxFlatGlo[] = {    1100.0,    1000.0   }
#define FLAT_MIRROR_GLOBAL_POSITIONS_Z \
constexpr double zMinFlatGlo[] = {    1175.0,    9600.0   };\
constexpr double zMaxFlatGlo[] = {    1420.0,    9900.0   }
#define FLAT_MIRROR_GLOBAL_POSITIONS \
 FLAT_MIRROR_GLOBAL_POSITIONS_X; \
 FLAT_MIRROR_GLOBAL_POSITIONS_Y; \
 FLAT_MIRROR_GLOBAL_POSITIONS_Z;

#endif // RICHKERNEL_RICHDETPARAMS_H
