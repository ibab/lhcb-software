// $Id: Packers.cpp,v 1.5 2010-05-18 09:03:21 jonrob Exp $

#include "PackerBaseAlg.icpp"

#include "Event/PackedMCRichHit.h"
#include "Event/PackedMCRichOpticalPhoton.h"
#include "Event/PackedMCRichSegment.h"
#include "Event/PackedMCRichTrack.h"
#include "Event/PackedMCCaloHit.h"
#include "Event/PackedMCHit.h"
#include "Event/PackedMCRichDigitSummary.h"
#include "Event/PackedMuonPID.h"
#include "Event/PackedRichPID.h"
#include "Event/PackedParticle.h"
#include "Event/PackedVertex.h"
#include "Event/PackedWeightsVector.h"
#include "Event/PackedCaloCluster.h"

template class DataPacking::Pack<LHCb::MCRichHitPacker>;
typedef DataPacking::Pack<LHCb::MCRichHitPacker> A;
DECLARE_ALGORITHM_FACTORY( A )

template class DataPacking::Pack<LHCb::MCRichOpticalPhotonPacker>;
typedef DataPacking::Pack<LHCb::MCRichOpticalPhotonPacker> B;
DECLARE_ALGORITHM_FACTORY( B )

template class DataPacking::Pack<LHCb::MCRichSegmentPacker>;
typedef DataPacking::Pack<LHCb::MCRichSegmentPacker> C;
DECLARE_ALGORITHM_FACTORY( C )

template class DataPacking::Pack<LHCb::MCRichTrackPacker>;
typedef DataPacking::Pack<LHCb::MCRichTrackPacker> D;
DECLARE_ALGORITHM_FACTORY( D )

template class DataPacking::Pack<LHCb::MCPrsHitPacker>;
typedef DataPacking::Pack<LHCb::MCPrsHitPacker> Ea;
DECLARE_ALGORITHM_FACTORY( Ea )

template class DataPacking::Pack<LHCb::MCSpdHitPacker>;
typedef DataPacking::Pack<LHCb::MCSpdHitPacker> Eb;
DECLARE_ALGORITHM_FACTORY( Eb )

template class DataPacking::Pack<LHCb::MCEcalHitPacker>;
typedef DataPacking::Pack<LHCb::MCEcalHitPacker> Ec;
DECLARE_ALGORITHM_FACTORY( Ec )

template class DataPacking::Pack<LHCb::MCHcalHitPacker>;
typedef DataPacking::Pack<LHCb::MCHcalHitPacker> Ed;
DECLARE_ALGORITHM_FACTORY( Ed )

template class DataPacking::Pack<LHCb::MCVeloHitPacker>;
typedef DataPacking::Pack<LHCb::MCVeloHitPacker> F;
DECLARE_ALGORITHM_FACTORY( F )

template class DataPacking::Pack<LHCb::MCVPHitPacker>;
typedef DataPacking::Pack<LHCb::MCVPHitPacker> Fp;
DECLARE_ALGORITHM_FACTORY( Fp )

template class DataPacking::Pack<LHCb::MCVLHitPacker>;
typedef DataPacking::Pack<LHCb::MCVLHitPacker> Fl;
DECLARE_ALGORITHM_FACTORY( Fl )

template class DataPacking::Pack<LHCb::MCPuVetoHitPacker>;
typedef DataPacking::Pack<LHCb::MCPuVetoHitPacker> G;
DECLARE_ALGORITHM_FACTORY( G )

template class DataPacking::Pack<LHCb::MCTTHitPacker>;
typedef DataPacking::Pack<LHCb::MCTTHitPacker> H;
DECLARE_ALGORITHM_FACTORY( H )

template class DataPacking::Pack<LHCb::MCUTHitPacker>;
typedef DataPacking::Pack<LHCb::MCUTHitPacker> Hu;
DECLARE_ALGORITHM_FACTORY( Hu )

template class DataPacking::Pack<LHCb::MCITHitPacker>;
typedef DataPacking::Pack<LHCb::MCITHitPacker> I;
DECLARE_ALGORITHM_FACTORY( I )

template class DataPacking::Pack<LHCb::MCOTHitPacker>;
typedef DataPacking::Pack<LHCb::MCOTHitPacker> J;
DECLARE_ALGORITHM_FACTORY( J )

template class DataPacking::Pack<LHCb::MCMuonHitPacker>;
typedef DataPacking::Pack<LHCb::MCMuonHitPacker> K;
DECLARE_ALGORITHM_FACTORY( K )

template class DataPacking::Pack<LHCb::MCRichDigitSummaryPacker>;
typedef DataPacking::Pack<LHCb::MCRichDigitSummaryPacker> L;
DECLARE_ALGORITHM_FACTORY( L )

template class DataPacking::Pack<LHCb::RichPIDPacker>;
typedef DataPacking::Pack<LHCb::RichPIDPacker> M;
DECLARE_ALGORITHM_FACTORY( M )

template class DataPacking::Pack<LHCb::MuonPIDPacker>;
typedef DataPacking::Pack<LHCb::MuonPIDPacker> N;
DECLARE_ALGORITHM_FACTORY( N )

template class DataPacking::Pack<LHCb::ParticlePacker>;
typedef DataPacking::Pack<LHCb::ParticlePacker> O;
DECLARE_ALGORITHM_FACTORY( O )

template class DataPacking::Pack<LHCb::VertexPacker>;
typedef DataPacking::Pack<LHCb::VertexPacker> P;
DECLARE_ALGORITHM_FACTORY( P )

template class DataPacking::Pack<LHCb::WeightsVectorPacker>;
typedef DataPacking::Pack<LHCb::WeightsVectorPacker> Q;
DECLARE_ALGORITHM_FACTORY( Q )

template class DataPacking::Pack<LHCb::CaloClusterPacker>;
typedef DataPacking::Pack<LHCb::CaloClusterPacker> R;
DECLARE_ALGORITHM_FACTORY( R )

template class DataPacking::Pack<LHCb::MCFTHitPacker>;
typedef DataPacking::Pack<LHCb::MCFTHitPacker> S;
DECLARE_ALGORITHM_FACTORY( S )

template class DataPacking::Pack<LHCb::MCSLHitPacker>;
typedef DataPacking::Pack<LHCb::MCSLHitPacker> T;
DECLARE_ALGORITHM_FACTORY( T )
