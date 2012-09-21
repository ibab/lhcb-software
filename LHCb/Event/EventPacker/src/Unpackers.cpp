// $Id: Unpackers.cpp,v 1.5 2010-05-18 09:03:21 jonrob Exp $

#include "UnpackerBaseAlg.icpp"

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

template class DataPacking::Unpack<LHCb::MCRichHitPacker>;
typedef DataPacking::Unpack<LHCb::MCRichHitPacker> A;
DECLARE_ALGORITHM_FACTORY( A )

template class DataPacking::Unpack<LHCb::MCRichOpticalPhotonPacker>;
typedef DataPacking::Unpack<LHCb::MCRichOpticalPhotonPacker> B;
DECLARE_ALGORITHM_FACTORY( B )

template class DataPacking::Unpack<LHCb::MCRichSegmentPacker>;
typedef DataPacking::Unpack<LHCb::MCRichSegmentPacker> C;
DECLARE_ALGORITHM_FACTORY( C )

template class DataPacking::Unpack<LHCb::MCRichTrackPacker>;
typedef DataPacking::Unpack<LHCb::MCRichTrackPacker> D;
DECLARE_ALGORITHM_FACTORY( D )

template class DataPacking::Unpack<LHCb::MCPrsHitPacker>;
typedef DataPacking::Unpack<LHCb::MCPrsHitPacker> Ea;
DECLARE_ALGORITHM_FACTORY( Ea )

template class DataPacking::Unpack<LHCb::MCSpdHitPacker>;
typedef DataPacking::Unpack<LHCb::MCSpdHitPacker> Eb;
DECLARE_ALGORITHM_FACTORY( Eb )

template class DataPacking::Unpack<LHCb::MCEcalHitPacker>;
typedef DataPacking::Unpack<LHCb::MCEcalHitPacker> Ec;
DECLARE_ALGORITHM_FACTORY( Ec )

template class DataPacking::Unpack<LHCb::MCHcalHitPacker>;
typedef DataPacking::Unpack<LHCb::MCHcalHitPacker> Ed;
DECLARE_ALGORITHM_FACTORY( Ed )

template class DataPacking::Unpack<LHCb::MCVeloHitPacker>;
typedef DataPacking::Unpack<LHCb::MCVeloHitPacker> F;
DECLARE_ALGORITHM_FACTORY( F )

template class DataPacking::Unpack<LHCb::MCVPHitPacker>;
typedef DataPacking::Unpack<LHCb::MCVPHitPacker> Fp;
DECLARE_ALGORITHM_FACTORY( Fp )

template class DataPacking::Unpack<LHCb::MCVLHitPacker>;
typedef DataPacking::Unpack<LHCb::MCVLHitPacker> Fl;
DECLARE_ALGORITHM_FACTORY( Fl )

template class DataPacking::Unpack<LHCb::MCPuVetoHitPacker>;
typedef DataPacking::Unpack<LHCb::MCPuVetoHitPacker> G;
DECLARE_ALGORITHM_FACTORY( G )

template class DataPacking::Unpack<LHCb::MCTTHitPacker>;
typedef DataPacking::Unpack<LHCb::MCTTHitPacker> H;
DECLARE_ALGORITHM_FACTORY( H )

template class DataPacking::Unpack<LHCb::MCUTHitPacker>;
typedef DataPacking::Unpack<LHCb::MCUTHitPacker> Hu;
DECLARE_ALGORITHM_FACTORY( Hu )

template class DataPacking::Unpack<LHCb::MCITHitPacker>;
typedef DataPacking::Unpack<LHCb::MCITHitPacker> I;
DECLARE_ALGORITHM_FACTORY( I )

template class DataPacking::Unpack<LHCb::MCOTHitPacker>;
typedef DataPacking::Unpack<LHCb::MCOTHitPacker> J;
DECLARE_ALGORITHM_FACTORY( J )

template class DataPacking::Unpack<LHCb::MCMuonHitPacker>;
typedef DataPacking::Unpack<LHCb::MCMuonHitPacker> K;
DECLARE_ALGORITHM_FACTORY( K )

template class DataPacking::Unpack<LHCb::MCRichDigitSummaryPacker>;
typedef DataPacking::Unpack<LHCb::MCRichDigitSummaryPacker> L;
DECLARE_ALGORITHM_FACTORY( L )

template class DataPacking::Unpack<LHCb::RichPIDPacker>;
typedef DataPacking::Unpack<LHCb::RichPIDPacker> M;
DECLARE_ALGORITHM_FACTORY( M )

template class DataPacking::Unpack<LHCb::MuonPIDPacker>;
typedef DataPacking::Unpack<LHCb::MuonPIDPacker> N;
DECLARE_ALGORITHM_FACTORY( N )

template class DataPacking::Unpack<LHCb::ParticlePacker>;
typedef DataPacking::Unpack<LHCb::ParticlePacker> O;
DECLARE_ALGORITHM_FACTORY( O )

template class DataPacking::Unpack<LHCb::VertexPacker>;
typedef DataPacking::Unpack<LHCb::VertexPacker> P;
DECLARE_ALGORITHM_FACTORY( P )

template class DataPacking::Unpack<LHCb::WeightsVectorPacker>;
typedef DataPacking::Unpack<LHCb::WeightsVectorPacker> Q;
DECLARE_ALGORITHM_FACTORY( Q )

template class DataPacking::Unpack<LHCb::CaloClusterPacker>;
typedef DataPacking::Unpack<LHCb::CaloClusterPacker> R;
DECLARE_ALGORITHM_FACTORY( R )
