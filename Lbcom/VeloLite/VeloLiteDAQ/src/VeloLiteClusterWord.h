#ifndef VELOLITECLUSTERWORD_H
#define VELOLITECLUSTERWORD_H 1

#include "SiDAQ/SiClusterWord.h"
#include "VeloLiteBitsPolicy.h"

/** Decodes/encodes VeloLite ClusterWord
 *
 *  This class is a simple type wrapper for an
 *  SiClusterWord instantiated with a VeloBitsPoliciy.
 *  
 *
 */
typedef SiClusterWord<VeloLiteBitsPolicy> VeloLiteClusterWord;

#endif

