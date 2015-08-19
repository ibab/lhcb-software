#ifndef DICT_HLTBASEDICT_H
#define DICT_HLTBASEDICT_H 1
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning( disable                                                            \
                 : 2259 ) // non-pointer conversion ... may lose significant bits
#pragma warning( push )
#endif
// ============================================================================
#include "HltBase/IMatchTVeloTracks.h"
#include "HltBase/ITrackConfirmTool.h"
#include "HltBase/ITrackL0CaloMatchTool.h"
#include "HltBase/IMatchTVeloTracks.h"
#include "HltBase/ITrackUpgrade.h"
#include "HltBase/ITrackView.h"
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
#include "HltBase/IHltInspector.h"
#include "HltBase/HltSelection.h"


#include "Event/HltCandidate.h"
#include "Event/HltL0DiMuonCandidate.h"
#include "Event/HltMultiTrack.h"

#include "HltBase/HltUtils.h"
#include "HltBase/ITrack2CandidateMatch.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning( pop )
#endif
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning( disable : 177 ) //  variable ... was declared but never referenced
#pragma warning( disable : 191 ) // type qualifier is meaningless on cast type
#endif
// ============================================================================
namespace HltBaseDict
{
struct __Instantiations
{

    Hlt::Stage::Vector m_s_1;
    Hlt::Stage::ConstVector m_s_2;
    Hlt::Stage::Container m_s_3;
    Hlt::Stage::Range m_s_4;
    Hlt::Stage::Selection m_s_5;
    SmartRefVector<Hlt::Stage> m_s_6;
    SmartRef<Hlt::Stage> m_s_7;

    Hlt::Candidate::Vector m_c_1;
    Hlt::Candidate::ConstVector m_c_2;
    Hlt::Candidate::Container m_c_3;
    Hlt::Candidate::Range m_c_4;
    Hlt::Candidate::Selection m_c_5;

    Hlt::MultiTrack::Vector m_t_1;
    Hlt::MultiTrack::ConstVector m_t_2;
    Hlt::MultiTrack::Container m_t_3;
    Hlt::MultiTrack::Range m_t_4;
    Hlt::MultiTrack::Selection m_t_5;

    Hlt::L0DiMuonCandidate::Vector m_m_1;
    Hlt::L0DiMuonCandidate::ConstVector m_m_2;
    Hlt::L0DiMuonCandidate::Container m_m_3;
    Hlt::L0DiMuonCandidate::Range m_m_4;
    Hlt::L0DiMuonCandidate::Selection m_m_5;

    Hlt::TSelection<Hlt::Candidate> m_a_1;
    Hlt::TSelection<Hlt::Stage> m_a_2;
    Hlt::TSelection<Hlt::MultiTrack> m_a_3;
    Hlt::TSelection<Hlt::L0DiMuonCandidate> m_a_4;
    Hlt::TrackSelection m_a_5;
    Hlt::VertexSelection m_a_6;

    __Instantiations();
};
}

#endif // DICT_HLTBASEDICT_H

