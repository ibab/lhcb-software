#ifndef VELOCLUSTERWORD_H
#define VELOCLUSTERWORD_H 1

#include "SiDAQ/SiClusterWord.h"
#include "VeloBitsPolicy.h"

/** Decodes/encodes velo clusterword
 *
 *  This class is a simple type wrapper for an
 *  SiClusterWord instantiated with a VeloBitsPoliciy.
 *  
 * @see SiClusterWord
 * @see VeloBitsPolicy
 *
 * @author Kurt Rinnert
 * @date   2006-02-02
 */
typedef SiClusterWord<VeloBitsPolicy> VeloClusterWord;

#endif

