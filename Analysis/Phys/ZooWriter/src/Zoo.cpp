//$Header: $
/*
 * Attempt to save Decays into ROOT Tree.
 *
 * AZ, SHM, MS, GK,  
 * PI Heidelberg 2008, 2009, 2010
 */

#include "Zoo.h"
#include <iostream>
#include <string.h>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <TObjString.h>
#include <TMap.h>

const ZooHitPattern& ZooHitPattern::operator |= (const ZooHitPattern& other)
{
    m_veloAR |= other.m_veloAR;
    m_veloCR |= other.m_veloCR;
    m_veloAPhi |= other.m_veloAPhi;
    m_veloCPhi |= other.m_veloCPhi;
    m_otMono1 |= other.m_otMono1;
    m_otMono2 |= other.m_otMono2;
    m_itTopBottom |= other.m_itTopBottom;
    m_itAC |= other.m_itAC;
    m_tt |= other.m_tt;
    m_muon |= other.m_muon;
    return *this;
}

const ZooHitPattern& ZooHitPattern::operator &= (const ZooHitPattern& other)
{
    m_veloAR &= other.m_veloAR;
    m_veloCR &= other.m_veloCR;
    m_veloAPhi &= other.m_veloAPhi;
    m_veloCPhi &= other.m_veloCPhi;
    m_otMono1 &= other.m_otMono1;
    m_otMono2 &= other.m_otMono2;
    m_itTopBottom &= other.m_itTopBottom;
    m_itAC &= other.m_itAC;
    m_tt &= other.m_tt;
    m_muon &= other.m_muon;
    return *this;
}

const ZooHitPattern& ZooHitPattern::operator ^= (const ZooHitPattern& other)
{
    m_veloAR ^= other.m_veloAR;
    m_veloCR ^= other.m_veloCR;
    m_veloAPhi ^= other.m_veloAPhi;
    m_veloCPhi ^= other.m_veloCPhi;
    m_otMono1 ^= other.m_otMono1;
    m_otMono2 ^= other.m_otMono2;
    m_itTopBottom ^= other.m_itTopBottom;
    m_itAC ^= other.m_itAC;
    m_tt ^= other.m_tt;
    m_muon ^= other.m_muon;
    return *this;
}

void ZooEv::Clear()
{
    m_particles = 0; m_mcParticles = 0; m_trackmult = 0; m_nbMCPVs = 0;
    m_L0Dec = 0;
    std::fill(m_triggerDecisions,
	    m_triggerDecisions + TriggerDecisions::NTriggerDecisionWords,
	    0);
    m_pvs.clear();
    m_usingEmergencyPV = false; m_eventNumber = ULong64_t(-1);
    m_gpsTime = ULong64_t(-1); m_detectorStatus = ULong64_t(-1);
    m_runNumber = UInt_t(-1); m_eventType = UInt_t(-1);
    m_orbitNumber = UInt_t(-1); m_bunchCurrent = UInt_t(-1);
    m_calibrationStep = UInt_t(-1); m_TCK = UInt_t(-1);
    m_bunchID = UShort_t(-1); m_triggerType = UChar_t(-1);
    m_readoutType = UChar_t(-1); m_bunchCrossingType = UChar_t(-1);
    m_calibrationType = UChar_t(-1); m_TAEWindow = UChar_t(-1);
    m_errorBits = UChar_t(-1); m_forceBit = false;
    m_occupancy = 0;
    m_nTracksVelo = 0; m_nTracksVeloBackward = 0; m_nTracksLong = 0;
    m_nTracksUpstream = 0; m_nTracksDownstream = 0; m_nTracksT = 0;
    m_spdMult = 0; m_prsMult = 0; m_ecalMult = 0; m_hcalMult = 0;
    m_mcGenEventInfo = 0;
    m_nMCPartInAcceptance = 0;
}

/// bitwise or
ZooHitPattern operator|(const ZooHitPattern& p1, const ZooHitPattern& p2)
{ return ZooHitPattern(p1) |= p2; }
/// bitwise and
ZooHitPattern operator&(const ZooHitPattern& p1, const ZooHitPattern& p2)
{ return ZooHitPattern(p1) &= p2; }
/// bitwise xor
ZooHitPattern operator^(const ZooHitPattern& p1, const ZooHitPattern& p2)
{ return ZooHitPattern(p1) ^= p2; }
/// bitwise complement
ZooHitPattern operator ~ (const ZooHitPattern& other)
{ return other.complement(); }
/// return bitwise complement
ZooHitPattern ZooHitPattern::complement() const
{
    ZooHitPattern p;
    p.m_veloAR = ~m_veloAR;
    p.m_veloCR = ~m_veloCR;
    p.m_veloAPhi = ~m_veloAPhi;
    p.m_veloCPhi = ~m_veloCPhi;
    p.m_otMono1 = ~m_otMono1;
    p.m_otMono2 = ~m_otMono2;
    p.m_itTopBottom = ~m_itTopBottom;
    p.m_itAC = ~m_itAC;
    p.m_tt = ~m_tt;
    p.m_muon = ~m_muon;
    return p;
}

TObject* ZooEv::perJobObject(const std::string& key) const
{
    // get map
    TMap* map = reinterpret_cast<TMap*>(m_perJobObjects.GetObject());
    if (!map) return 0;
    TObjString okey(key.c_str());
    TRef* objref = reinterpret_cast<TRef*>(map->GetValue(&okey));
    if (!objref) return 0;
    return objref->GetObject();
}

ZooDLL::ZooDLL() : 
     m_DLLe(0.f),
     m_DLLk(0.f),
     m_DLLmu(0.f),
     m_DLLp(0.f),
     m_isMuon(0),
     m_isMuonLoose(0),
     m_hasRich(0) {};

ZooP::~ZooP() { }
ZooMCP::~ZooMCP() { }
ZooEv::~ZooEv() { }
ZooStable::~ZooStable() { }
ZooDecay::~ZooDecay() { }
ZooTagging::~ZooTagging() { }
ZooMCBkg::~ZooMCBkg() { }
ZooTrigger::~ZooTrigger() { }
ZooDLL::~ZooDLL() { }
ZooLinks::~ZooLinks() { }
ZooTrackInfo::~ZooTrackInfo() { }
ZooTrackExtraInfo::~ZooTrackExtraInfo() { }
ZooHitPattern::~ZooHitPattern() { }
ZooMCGenEventInfo::~ZooMCGenEventInfo() { }
ZooParticleInfo::~ZooParticleInfo() { }
ZooTreefitInfo::~ZooTreefitInfo() { }
ZooGhostCategory::~ZooGhostCategory() { }

ClassImp(ZooP);
ClassImp(ZooMCP);
ClassImp(ZooEv);
ClassImp(ZooStable);
ClassImp(ZooDecay);
ClassImp(ZooTagging);
ClassImp(ZooMCBkg);
ClassImp(ZooTrigger);
ClassImp(ZooDLL);
ClassImp(ZooLinks);
ClassImp(ZooTrackInfo);
ClassImp(ZooTrackExtraInfo);
ClassImp(ZooHitPattern);
ClassImp(ZooMCGenEventInfo);
ClassImp(ZooParticleInfo);
ClassImp(ZooTreefitInfo);
ClassImp(ZooGhostCategory);

// vim: tw=78:sw=4:ft=cpp
