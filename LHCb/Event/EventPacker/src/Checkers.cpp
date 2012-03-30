// $Id: Checkers.cpp,v 1.5 2010-05-18 09:03:21 jonrob Exp $

#include "CheckerBaseAlg.icpp"

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

template class DataPacking::Check<LHCb::MCRichHitPacker>;
typedef DataPacking::Check<LHCb::MCRichHitPacker> A;
DECLARE_ALGORITHM_FACTORY( A )

template class DataPacking::Check<LHCb::MCRichOpticalPhotonPacker>;
typedef DataPacking::Check<LHCb::MCRichOpticalPhotonPacker> B;
DECLARE_ALGORITHM_FACTORY( B )

template class DataPacking::Check<LHCb::MCRichSegmentPacker>;
typedef DataPacking::Check<LHCb::MCRichSegmentPacker> C;
DECLARE_ALGORITHM_FACTORY( C )

template class DataPacking::Check<LHCb::MCRichTrackPacker>;
typedef DataPacking::Check<LHCb::MCRichTrackPacker> D;
DECLARE_ALGORITHM_FACTORY( D )

template class DataPacking::Check<LHCb::MCPrsHitPacker>;
typedef DataPacking::Check<LHCb::MCPrsHitPacker> Ea;
DECLARE_ALGORITHM_FACTORY( Ea )

template class DataPacking::Check<LHCb::MCSpdHitPacker>;
typedef DataPacking::Check<LHCb::MCSpdHitPacker> Eb;
DECLARE_ALGORITHM_FACTORY( Eb )

template class DataPacking::Check<LHCb::MCEcalHitPacker>;
typedef DataPacking::Check<LHCb::MCEcalHitPacker> Ec;
DECLARE_ALGORITHM_FACTORY( Ec )

template class DataPacking::Check<LHCb::MCHcalHitPacker>;
typedef DataPacking::Check<LHCb::MCHcalHitPacker> Ed;
DECLARE_ALGORITHM_FACTORY( Ed )

template class DataPacking::Check<LHCb::MCVeloHitPacker>;
typedef DataPacking::Check<LHCb::MCVeloHitPacker> F;
DECLARE_ALGORITHM_FACTORY( F )

template class DataPacking::Check<LHCb::MCPuVetoHitPacker>;
typedef DataPacking::Check<LHCb::MCPuVetoHitPacker> G;
DECLARE_ALGORITHM_FACTORY( G )

template class DataPacking::Check<LHCb::MCVeloPixHitPacker>;
typedef DataPacking::Check<LHCb::MCVeloPixHitPacker> Fp;
DECLARE_ALGORITHM_FACTORY( Fp )

template class DataPacking::Check<LHCb::MCTTHitPacker>;
typedef DataPacking::Check<LHCb::MCTTHitPacker> H;
DECLARE_ALGORITHM_FACTORY( H )

template class DataPacking::Check<LHCb::MCITHitPacker>;
typedef DataPacking::Check<LHCb::MCITHitPacker> I;
DECLARE_ALGORITHM_FACTORY( I )

template class DataPacking::Check<LHCb::MCOTHitPacker>;
typedef DataPacking::Check<LHCb::MCOTHitPacker> J;
DECLARE_ALGORITHM_FACTORY( J )

template class DataPacking::Check<LHCb::MCMuonHitPacker>;
typedef DataPacking::Check<LHCb::MCMuonHitPacker> K;
DECLARE_ALGORITHM_FACTORY( K )

template class DataPacking::Check<LHCb::MCRichDigitSummaryPacker>;
typedef DataPacking::Check<LHCb::MCRichDigitSummaryPacker> L;
DECLARE_ALGORITHM_FACTORY( L )

template class DataPacking::Check<LHCb::RichPIDPacker>;
typedef DataPacking::Check<LHCb::RichPIDPacker> M;
DECLARE_ALGORITHM_FACTORY( M )

template class DataPacking::Check<LHCb::MuonPIDPacker>;
typedef DataPacking::Check<LHCb::MuonPIDPacker> N;
DECLARE_ALGORITHM_FACTORY( N )

template class DataPacking::Check<LHCb::ParticlePacker>;
typedef DataPacking::Check<LHCb::ParticlePacker> O;
DECLARE_ALGORITHM_FACTORY( O )

template class DataPacking::Check<LHCb::VertexPacker>;
typedef DataPacking::Check<LHCb::VertexPacker> P;
DECLARE_ALGORITHM_FACTORY( P )

template class DataPacking::Check<LHCb::WeightsVectorPacker>;
typedef DataPacking::Check<LHCb::WeightsVectorPacker> Q;
DECLARE_ALGORITHM_FACTORY( Q )

template class DataPacking::Check<LHCb::CaloClusterPacker>;
typedef DataPacking::Check<LHCb::CaloClusterPacker> R;
DECLARE_ALGORITHM_FACTORY( R )
