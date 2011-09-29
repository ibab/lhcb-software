//$Header: $
/*
 * Attempt to save Decays into ROOT Tree.
 *
 * AZ, SHM, MS, GK,  
 * PI Heidelberg 2008, 2009, 2010
 */

#include "Zoo.h"
#include <iostream>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

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

bool ZooTrackExtraInfo::exists(unsigned key) const
{
    return std::binary_search(m_idx.begin(), m_idx.end(), key);
}

float ZooTrackExtraInfo::value(unsigned key) const
{
    KeyVector::const_iterator it =
	std::lower_bound(m_idx.begin(), m_idx.end(), key);
    if (it != m_idx.end())
	return m_val[it - m_idx.begin()];
    else
	return std::numeric_limits<float>::quiet_NaN();
}

ZooTrackExtraInfo::ZooTrackExtraInfo(KeyValueVector& extrainfo)
{
    using namespace boost::lambda;
    m_idx.reserve(extrainfo.size());
    m_val.reserve(extrainfo.size());
    //the next two lines break with boost 1.44 so we have to specify the return type of bind explicitely
    std::sort(extrainfo.begin(), extrainfo.end(),
	    bind<unsigned int>(&KeyValuePair::first, _1) < bind<unsigned int>(&KeyValuePair::first, _2));
    for (std::size_t i = 0; i < extrainfo.size(); ++i) {
	// skip duplicate keys
	if (i && extrainfo[i - 1].first == extrainfo[i].first)
	    continue;
	m_idx.push_back(extrainfo[i].first);
	m_val.push_back(extrainfo[i].second);
    }
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

bool ZooParticleInfo::exists(unsigned key) const
{
    return std::binary_search(m_idx.begin(), m_idx.end(), key);
}

float ZooParticleInfo::value(unsigned key) const
{
    KeyVector::const_iterator it =
	std::lower_bound(m_idx.begin(), m_idx.end(), key);
    if (it != m_idx.end())
	return m_val[it - m_idx.begin()];
    else
	return std::numeric_limits<float>::quiet_NaN();
}

ZooParticleInfo::ZooParticleInfo(KeyValueVector& extrainfo)
{
    using namespace boost::lambda;
    m_idx.reserve(extrainfo.size());
    m_val.reserve(extrainfo.size());
    //the next two lines breake with boost 1.44 so we have to specify the return type of bind explicitely
    std::sort(extrainfo.begin(), extrainfo.end(),
	    bind<unsigned int>(&KeyValuePair::first, _1) < bind<unsigned int>(&KeyValuePair::first, _2));
    for (std::size_t i = 0; i < extrainfo.size(); ++i) {
	// skip duplicate keys
	if (i && extrainfo[i - 1].first == extrainfo[i].first)
	    continue;
	m_idx.push_back(extrainfo[i].first);
	m_val.push_back(extrainfo[i].second);
    }
}

int ZooParticleInfo::insert(unsigned key, float val)
{
    // okay, this looks ugly. I tried std algorithms and failed.
    unsigned lower = 0;
    for (unsigned k = 0 ; k < m_idx.size() ; ++k) {
	if (key == m_idx[k]) {
	    m_val[k] = val;
	    return 0;
	}
	if (key > m_idx[k]) lower++;
    }
    if (m_idx.size() == lower) {
	m_idx.push_back(key);
	m_val.push_back(val);
	return 0;
    }
    m_idx.push_back(m_idx.back());
    m_val.push_back(m_val.back()); 

    for (unsigned k = m_idx.size()-2 ; k > lower ; --k) {
	m_idx[k] = m_idx[k-1];
	m_val[k] = m_val[k-1];
    }
   m_idx[lower] = key;
   m_val[lower] = val;
   return 0;

}

bool ZooParticleInfo::insert_unsorted(unsigned key, float val) {
  if (exists(key)) return modify(key, val);
  m_idx.push_back(key);
  m_val.push_back(key);
  return true;
}

bool ZooParticleInfo::modify(unsigned key, float val) {
    for (unsigned k = 0 ; k < m_idx.size() ; ++k) {
      if (key == m_idx[k]) {
        m_val[k] = val;
        return true;
      }
    }
    return false;
}

void ZooParticleInfo::dump() {
  for (unsigned k = 0 ; k < m_idx.size() ; ++k) {
    printf("%d\t%f\n",m_idx[k],m_val[k]);
  }
}

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
