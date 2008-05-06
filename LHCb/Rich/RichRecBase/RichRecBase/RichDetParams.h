
//-----------------------------------------------------------------------------
/** @file RichDetParams.h
 *
 * A collection of general parameters for histogramming limits etc.
 *
 * CVS Log :-
 * $Id: RichDetParams.h,v 1.10 2008-05-06 15:31:12 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-06-20
 *
 * @todo Remove this file anad replace with IRichDetParameters tool
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHDETPARAMS_H
#define RICHKERNEL_RICHDETPARAMS_H 1

#define RADIATOR_NAMES      const std::string radiator[] = { "aerogel", "Rich1Gas", "Rich2Gas" }
#define RICH_NAMES          const std::string rich[] =     { "Rich1", "Rich2" }
#define HYPOTHESIS_NAMES    const std::string hypothesis[] = { "el", "mu", "pi", "Ka", "Pr", "X" }
#define RICH_HISTO_OFFSET   const int richOffset = 100000
#define RAD_HISTO_OFFSET    const int radOffset  = 10000
#define HYPO_HISTO_OFFSET   const int hypoOffset = 1000
#define MAX_CKTHETA_RAD     const double maxCkTheta[] = { 0.31, 0.07, 0.04 }
#define MIN_CKTHETA_RAD     const double minCkTheta[] = { 0.0,  0.0,   0.0 }

#define RADIATOR_MAX_CKTHETA \
const double ckThetaMax[] = { 0.3, 0.06, 0.06 }

//                               Aerogel  Rich1Gas    Rich2Gas
#define RADIATOR_GLOBAL_POSITIONS_X \
const double xRadEntGlo[]  = {  420.0,  1500.0,  3500.0 };\
const double xRadExitGlo[] = {  420.0,  1500.0,  4000.0 }
#define RADIATOR_GLOBAL_POSITIONS_Y \
const double yRadEntGlo[]  = {  320.0,  1500.0,  3500.0 };\
const double yRadExitGlo[] = {  320.0,  1500.0,  4000.0 }
#define RADIATOR_GLOBAL_POSITIONS_Z \
const double zRadEntGlo[]  = { 1100.0,   900.0,  9200.0 };\
const double zRadExitGlo[] = { 1180.0,  2100.0, 12000.0 }
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
const double xMinPDLoc[] = {    -650.0,    -800.0   };\
const double xMaxPDLoc[] = {     650.0,     800.0   }
#define PD_LOCAL_POSITIONS_Y \
const double yMinPDLoc[] = {    -650.0,    -800.0   };\
const double yMaxPDLoc[] = {     650.0,     800.0   }
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
