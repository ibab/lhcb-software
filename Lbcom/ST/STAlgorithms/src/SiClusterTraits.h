// $Id: SiClusterTraits.h,v 1.2 2009-03-02 08:10:50 mneedham Exp $
#ifndef SICLUSTERTRAITS_H
#define SICLUSTERTRAITS_H 1


/** @class STClusterTraits
 *
 *  traits for light clusters
 *  ST specialization
 *
 *  @author M.Needham
 *  @date   02/10/2008
 */


// ST includes
#include "Event/STCluster.h"
#include "Event/STLiteCluster.h"
#include "Event/STDigit.h"

// Velo includes
#include "Event/VeloCluster.h"
#include "Event/VeloLiteCluster.h"
#include "Event/VeloDigit.h"


#include "Kernel/STAlgBase.h"
#include "GaudiAlg/GaudiAlgorithm.h"
 
template<typename TYPE>
class SiClusterTraits{
 public:
    typedef TYPE CLUSCONT; 
    typedef TYPE CLUSTYPE;
    typedef TYPE LITECONT; 
    typedef TYPE DIGIT;
    typedef GaudiAlgorithm BASEALG;
};

// specialization for ST 
template<>
class SiClusterTraits<LHCb::STCluster> {
 public:
    typedef LHCb::STClusters CLUSCONT; 
    typedef CLUSCONT::value_type CLUSTYPE;
    typedef LHCb::STLiteCluster::STLiteClusters LITECONT; 
    typedef LHCb::STDigit DIGIT;
    typedef ST::AlgBase BASEALG;
};

// specialization for Velo
template<>
class SiClusterTraits<LHCb::VeloCluster> {
 public:
    typedef LHCb::VeloClusters CLUSCONT; 
    typedef CLUSCONT::value_type CLUSTYPE;
    typedef LHCb::VeloLiteCluster::VeloLiteClusters LITECONT;
    typedef LHCb::VeloDigit DIGIT; 
    typedef GaudiAlgorithm BASEALG;
};



#endif
