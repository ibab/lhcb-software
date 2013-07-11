//$Header: $
/*
 * Attempt to save Decays into ROOT Tree.
 *
 * AZ, SHM, MS, GK,  
 * PI Heidelberg 2008, 2009, 2010
 */

#ifndef ZOO_H
#define ZOO_H

#include <vector>
#include <bitset>
#include <limits>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <utility>
#include <string>

#include <TObject.h>
#include <TObjArray.h>		// per default, does not own its contents
#include <TClonesArray.h>	// always owns its contents
#include <TRefArray.h>		// never owns its contents
#include <TRef.h>
#include <TArrayF.h>
#include <Math/SMatrix.h>
#include <Math/Vector4D.h>
#include <Math/Point3D.h>

#ifndef __CINT__
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#else
#define BOOST_STATIC_ASSERT(x)
#endif

#include "ZooLikely.h"
#include "ZooObjectManager.h"
#include "ZooPacked.h"
#include "ZooPackedState.h"
#include "ZooPackedVertex.h"
#include "ZooPackedParticle.h"
#include "ZooPackedStates.h"
#include "ZooTriggerDecisions.h"
#include "ZooOccupancy.h"
#include "ZooLHCbID.h"
#include "ZooKeyValueBlock.h"
#include "ZooLoKiBlock.h"

typedef ROOT::Math::PxPyPzEVector     zooLorentzVector; 
typedef ROOT::Math::XYZPoint          XYZPoint;
class TMap;

/** @class ZooStable
 *    \brief Stable (tracked) particles information
 */
class ZooStable : public TObject 
{
 public:
  ZooStable() : m_trackType(-1), m_charge(0) { 
  }
  
  ZooStable(int trackType, int charge) :
    m_trackType(trackType), m_charge(charge) {
  }
  virtual ~ZooStable();
 
  ///Gives back the track type from which the particle was build:
  int trackType() const { return m_trackType; };
  int charge()    const { return m_charge; };
  
 private:
  Char_t   m_trackType;
  Char_t   m_charge;
  
  ClassDef(ZooStable,1);
};
      
      
/** @class ZooTreefitInfo
 *  \brief Information from fit of the complete decay tree
 */
class ZooTreefitInfo : public TObject
{
 public:
 ZooTreefitInfo() :
   m_particle(ZooPackedParticle()),
   m_measuredMass(std::numeric_limits<Float_t>::quiet_NaN()),
   m_measuredMassErr(std::numeric_limits<Float_t>::quiet_NaN()),
   m_fD(std::numeric_limits<Float_t>::quiet_NaN()),
   m_fDErr(std::numeric_limits<Float_t>::quiet_NaN()),
   m_ct(std::numeric_limits<Float_t>::quiet_NaN()),
   m_ctErr(std::numeric_limits<Float_t>::quiet_NaN()),
   m_ctFixed(std::numeric_limits<Float_t>::quiet_NaN()),
   m_ctErrFixed(std::numeric_limits<Float_t>::quiet_NaN()),
   m_decVtxChi2(std::numeric_limits<Float_t>::quiet_NaN()),
   m_decVtxNDoF(-1) {
  }
  ZooTreefitInfo(XYZPoint pos,
                 zooLorentzVector mom, 
                 ROOT::Math::SMatrix<double, 7, 7, ROOT::Math::MatRepSym<double, 7> > cov, 
                 double m, double mErr, double fD, double fDErr, 
                 double ct, double ctErr, double ctFixed, double ctErrFixed, double chi2, short ndof) : 
    m_particle(pos, mom, cov),
    m_measuredMass(m),
    m_measuredMassErr(mErr),
    m_fD(fD),
    m_fDErr(fDErr),
    m_ct(ct),
    m_ctErr(ctErr),
    m_ctFixed(ctFixed),
    m_ctErrFixed(ctErrFixed),
    m_decVtxChi2(chi2),
    m_decVtxNDoF(ndof) {
  }
  virtual ~ZooTreefitInfo();
  Float_t  measuredMass()    const { return m_measuredMass; }
  Float_t  measuredMassErr() const { return m_measuredMassErr; }
  Float_t  fDistance()       const { return m_fD; }
  Float_t  fDistanceErr()    const { return m_fDErr; }
  Float_t  ct()              const { return m_ct; }
  Float_t  ctErr()           const { return m_ctErr; }
  Float_t  ctFixed()              const { return m_ctFixed; }
  Float_t  ctErrFixed()           const { return m_ctErrFixed; }
  Float_t  chi2()            const { return m_decVtxChi2; }
  Short_t  nDoF()            const { return m_decVtxNDoF; }
  zooLorentzVector    momentum()         const { return m_particle.momentum(); }
  XYZPoint pos() const { return m_particle.pos(); }
  const ZooPackedParticle&  particle() const { return m_particle; }

 private:
  ZooPackedParticle m_particle;
  
  Float_t m_measuredMass;
  Float_t m_measuredMassErr;
  Float_t m_fD;
  Float_t m_fDErr;
  Float_t m_ct;
  Float_t m_ctErr;
  Float_t m_ctFixed;
  Float_t m_ctErrFixed;
  
  Float_t m_decVtxChi2;
  Short_t m_decVtxNDoF;
  
  ClassDef(ZooTreefitInfo,1);
};


/** @class ZooDecay
 *     \brief Decayed (composite) particles information
 */
class ZooDecay : public TObject
{
 public:
  
  ZooDecay() :
    m_measuredMass(std::numeric_limits<Float_t>::quiet_NaN()),
    m_measuredMassErr(std::numeric_limits<Float_t>::quiet_NaN()),
    m_fD(std::numeric_limits<Float_t>::quiet_NaN()),
    m_fDErr(std::numeric_limits<Float_t>::quiet_NaN()),
    m_fDChi2(std::numeric_limits<Float_t>::quiet_NaN()),
    m_ct(std::numeric_limits<Float_t>::quiet_NaN()),
    m_ctErr(std::numeric_limits<Float_t>::quiet_NaN()),
    m_ctChi2(std::numeric_limits<Float_t>::quiet_NaN()), 
    m_decVtxChi2(std::numeric_limits<Float_t>::quiet_NaN()),
    m_isolation(-999), m_decVtxNDoF(-1){ 
  }
	
  ZooDecay(double m, double merr, double fd, double fderr, double fdchi2,
	   double ct, double cterr, double ctChi2, double chi2, int iso, int ndof) :
    m_measuredMass(m), m_measuredMassErr(merr),
    m_fD(fd), m_fDErr(fderr), m_fDChi2(fdchi2), m_ct(ct), m_ctErr(cterr),
    m_ctChi2(ctChi2), m_decVtxChi2(chi2), m_isolation(iso), m_decVtxNDoF(ndof){ 
  }
  virtual ~ZooDecay();
  
  float measuredMass()    const { return m_measuredMass; }
  float measuredMassErr() const { return m_measuredMassErr; }
  float chi2()            const { return m_decVtxChi2; }
  float fDistance()       const { return m_fD; }
  float fDistanceErr()    const { return m_fDErr; }
  float fDistanceChi2()    const { return m_fDChi2; }
  float ct()              const { return m_ct; }
  float ctErr()           const { return m_ctErr; }
  float ctChi2()        const { return m_ctChi2; } 
  int nDoF()            const { return m_decVtxNDoF; }
  int isolation()       const { return m_isolation; }
  
 private:
  Float_t m_measuredMass;
  Float_t m_measuredMassErr;
  Float_t m_fD;
  Float_t m_fDErr;
  Float_t m_fDChi2;
  Float_t m_ct;
  Float_t m_ctErr;
  Float_t m_ctChi2; 
  Float_t m_decVtxChi2;
  Short_t m_isolation;
  Short_t m_decVtxNDoF;
	
  ClassDef(ZooDecay,1);
};


/** @class ZooMCP
 *   \brief Represend MC particles with relations to other MC particles
 */
class ZooMCP : public TObject
{
    private:
	ZooMCPArray m_mother;
	ZooMCPArray m_child;

	ZooPackedLorentzVector m_momentum;
	ZooPackedPosition      m_prodVertex;
	ZooPackedPosition      m_decayVertex;

	Int_t m_pid;
	Int_t m_key;
	UInt_t m_mcTrackInfo;

	enum { 
	    maskVeloR      = 0x00000001,
	    maskVeloPhi    = 0x00000002,
	    maskTT1        = 0x00000004,
	    maskTT2        = 0x00000008,
	    maskT1X        = 0x00000010,
	    maskT1S        = 0x00000020,
	    maskT2X        = 0x00000040,
	    maskT2S        = 0x00000080,
	    maskT3X        = 0x00000100,
	    maskT3S        = 0x00000200,

	    maskHasVelo    = maskVeloR | maskVeloPhi,
	    maskHasTT      = maskTT1 | maskTT2,
	    maskHasT1      = maskT1X | maskT1S,
	    maskHasT2      = maskT2X | maskT2S,
	    maskHasT3      = maskT3X | maskT3S,
	    maskHasT       = maskHasT1 | maskHasT2 | maskHasT3,
	    maskHasVeloAndT = maskHasVelo | maskHasT,

	    maskAccVeloR   = 0x00000400,
	    maskAccVeloPhi = 0x00000800,
	    maskAccTT1     = 0x00001000,
	    maskAccTT2     = 0x00002000,
	    maskAccT1X     = 0x00004000,
	    maskAccT1S     = 0x00008000,
	    maskAccT2X     = 0x00010000,
	    maskAccT2S     = 0x00020000,
	    maskAccT3X     = 0x00040000,
	    maskAccT3S     = 0x00080000,

	    maskAccVelo    = maskAccVeloR | maskAccVeloPhi,
	    maskAccTT      = maskAccTT1 | maskAccTT2,
	    maskAccT1      = maskAccT1X | maskAccT1S,
	    maskAccT2      = maskAccT2X | maskAccT2S,
	    maskAccT3      = maskAccT3X | maskAccT3S,
	    maskAccT       = maskAccT1 | maskAccT2 | maskAccT3,
	    maskAccVeloAndT = maskAccVelo | maskAccT,

	    multVeloR      = 20,
	    multVeloPhi    = 25,

	    maskMultVeloR  = 0x01F00000,  // Velo R CLuster multiplicity
	    maskMultVeloPhi= 0x3E000000   // VeloPhi cluster multipliity
	};

	ULong64_t m_flags;
    public:
	enum Flags {
	    hasOscillated = 1,
	    isFromPV = 2
	};
    private:

	ZooMCP &operator=(const ZooMCP&);
	ZooMCP(const ZooMCP&);

    public:
	ZooMCP() : m_pid(0), m_key(-1), m_mcTrackInfo(0), m_flags(0) { }
	virtual ~ZooMCP();
	
	const ZooMCP* Mo(unsigned i)          const { return m_mother[i]; }
	unsigned      NMo()                   const { return m_mother.size(); }
	const ZooMCP* Ch(unsigned i)          const { return m_child[i]; }
	unsigned      NCh()                   const { return m_child.size(); }
	const ZooMCP* operator[](UInt_t i)    const { return m_child[i]; }

	void  AddMother (ZooMCP *m)  { m_mother.push_back(m); }
	void  AddChild  (ZooMCP *c)  { m_child.push_back(c); }
	
	zooLorentzVector    momentum()         const { return m_momentum; }
	int                 pid()              const { return m_pid; }
	XYZPoint            prodVertex()       const { return m_prodVertex; }
	XYZPoint            decayVertex()      const { return m_decayVertex; }
	int                 key()              const { return m_key; }

	// these duplicate the information in MCTrackInfo
	bool hasVeloR()     const { return m_mcTrackInfo & maskVeloR; }
	bool hasVeloPhi()   const { return m_mcTrackInfo & maskVeloPhi; }
	bool hasTT1()       const { return m_mcTrackInfo & maskTT1; }
	bool hasTT2()       const { return m_mcTrackInfo & maskTT2; }
	bool hasT1X()       const { return m_mcTrackInfo & maskT1X; }
	bool hasT1S()       const { return m_mcTrackInfo & maskT1S; }
	bool hasT2X()       const { return m_mcTrackInfo & maskT2X; }
	bool hasT2S()       const { return m_mcTrackInfo & maskT2S; }
	bool hasT3X()       const { return m_mcTrackInfo & maskT3X; }
	bool hasT3S()       const { return m_mcTrackInfo & maskT3S; }
	bool hasVelo()      const { 
	  return maskHasVelo == (m_mcTrackInfo & maskHasVelo); }
	bool hasTT()        const { 
	  return maskHasTT == (m_mcTrackInfo & maskHasTT); }
	bool hasT()         const {
	  return maskHasT == (m_mcTrackInfo & maskHasT); }
	bool hasVeloAndT()   const {
	  return maskHasVeloAndT == (m_mcTrackInfo & maskHasVeloAndT); }
	bool hasT1()         const {
	  return maskHasT1 == (m_mcTrackInfo & maskHasT1); }
	bool hasT2()         const {
	  return maskHasT2 == (m_mcTrackInfo & maskHasT2); }
	bool hasT3()         const {
	  return maskHasT3 == (m_mcTrackInfo & maskHasT3); }
	bool accVeloR()      const { return m_mcTrackInfo & maskAccVeloR; }
	bool accVeloPhi()    const { return m_mcTrackInfo & maskAccVeloPhi; }
	bool accTT1()        const { return m_mcTrackInfo & maskAccTT1; }
	bool accTT2()        const { return m_mcTrackInfo & maskAccTT2; }
	bool accT1X()        const { return m_mcTrackInfo & maskAccT1X; }
	bool accT1S()        const { return m_mcTrackInfo & maskAccT1S; }
	bool accT2X()        const { return m_mcTrackInfo & maskAccT2X; }
	bool accT2S()        const { return m_mcTrackInfo & maskAccT2S; }
	bool accT3X()        const { return m_mcTrackInfo & maskAccT3X; }
	bool accT3S()        const { return m_mcTrackInfo & maskAccT3S; }
	bool accVelo()       const {
	  return maskAccVelo == (m_mcTrackInfo & maskAccVelo); }
	bool accTT()         const {
	  return maskAccTT == (m_mcTrackInfo & maskAccTT); }
	bool accT()          const {
	  return maskAccT == (m_mcTrackInfo & maskAccT); }
	bool accVeloAndT()   const {
	  return maskAccVeloAndT == (m_mcTrackInfo & maskAccVeloAndT); }
	bool accT1()         const {
	  return maskAccT1 == (m_mcTrackInfo & maskAccT1); }
	bool accT2()         const { 
	  return maskAccT2 == (m_mcTrackInfo & maskAccT2); }
	bool accT3()         const {
	  return maskAccT3 == (m_mcTrackInfo & maskAccT3); }
	unsigned nbVeloR()   const {
	  return (m_mcTrackInfo & maskMultVeloR) >> multVeloR; }
	unsigned nbVeloPhi() const {
	  return (m_mcTrackInfo & maskMultVeloPhi) >> multVeloPhi; }

	// check ZooMCP flags
	/** example:
	 * @code
	 * const ZooMCP* mcp = ...;
	 * if (mcp->checkFlag<ZooMCP::isFromPV>()) {
	 *     // do something..
	 * }
	 * @endcode
	 */
	template<unsigned flag>	bool checkFlag() const
	{ return m_flags & flag; }

	friend class ZooWriter;
	friend class ZooObjectManager;

	ClassDef(ZooMCP,2)
};

/** @class ZooTagging
 *    \brief tagging information information
 */
class ZooTagging : public TObject
{
    public:
	/// default constructor
	ZooTagging() :
	    m_omega(-0.1),m_osOmega(-0.1), m_elecOmega(-0.1), m_muonOmega(-0.1),
	    m_osKaonOmega(-0.1), m_ssKaonOmega(-0.1), m_ssPionOmega(-0.1), m_vtxOmega(-0.1),
	    m_ssKaon(reinterpret_cast<TObject*>(0)),
            m_tagDecision(0), m_osTagDecision(0), m_tagCategory(0), m_elecTagDecision(0),
	    m_muonTagDecision(0), m_osKaonTagDecision(0),
	    m_ssKaonTagDecision(0),  m_ssPionTagDecision(0), m_vtxTagDecision(0)
        { }
	virtual ~ZooTagging();

	int     tagDecision()       const { return m_tagDecision; };
	int     osTagDecision()     const { return m_osTagDecision; };
	float   omega()             const { return m_omega; };
	float   osOmega()           const { return m_osOmega; };
	int     tagCategory()       const { return m_tagCategory; };
	int     osTagCategory()     const { return m_osTagCategory; };
	int     elecTagDecision()   const { return m_elecTagDecision; };
	int     muonTagDecision()   const { return m_muonTagDecision; };
	int     osKaonTagDecision() const { return m_osKaonTagDecision; };
	int     ssKaonTagDecision() const { return m_ssKaonTagDecision; };
	int     ssPionTagDecision() const { return m_ssPionTagDecision; };
	int     vtxTagDecision()    const { return m_vtxTagDecision; };
	float   elecOmega()         const { return m_elecOmega; };
	float   muonOmega()         const { return m_muonOmega; };
	float   osKaonOmega()       const { return m_osKaonOmega; };
	float   ssKaonOmega()       const { return m_ssKaonOmega; };
	float   ssPionOmega()       const { return m_ssPionOmega; };
	float   vtxOmega()          const { return m_vtxOmega; };
	// return pointer to ZooP that caused SS Kaon tag (0 if unavailable)
	const ZooP* ssKaon() const
	{ return reinterpret_cast<const ZooP*>(m_ssKaon.GetObject()); }

	void setTagDecision(int d, float w)
	{ m_tagDecision = d; m_omega = w; }
	void setTagCategory(int c) { m_tagCategory = c; }
	void setOsTagDecision(int d, float w)
	{ m_osTagDecision = d; m_osOmega = w; }
	void setOsTagCategory(int c) { m_osTagCategory = c; }
	void setElecTagDecision(int d, float w)
	{ m_elecTagDecision = d; m_elecOmega = w; }
	void setMuonTagDecision(int d, float w)
	{ m_muonTagDecision = d; m_muonOmega = w; }
	void setOsKaonTagDecision(int d, float w)
	{ m_osKaonTagDecision = d; m_osKaonOmega = w; }
	void setSsPionTagDecision(int d, float w)
	{ m_ssPionTagDecision = d; m_ssPionOmega = w; }
	void setSsKaonTagDecision(int d, float w)
	{ m_ssKaonTagDecision = d; m_ssKaonOmega = w; }
	void setSsKaon(const ZooP* zp)
	{ m_ssKaon = reinterpret_cast<TObject*>(const_cast<ZooP*>(zp)); }
	void setVtxTagDecision(int d, float w)
	{ m_vtxTagDecision = d; m_vtxOmega = w; }

    private:
	Float_t m_omega;
	Float_t m_osOmega;
	Float_t m_elecOmega;
	Float_t m_muonOmega;
	Float_t m_osKaonOmega;
	Float_t m_ssKaonOmega;
	Float_t m_ssPionOmega;
	Float_t m_vtxOmega;
        TRef    m_ssKaon;
	Char_t  m_tagDecision;
	Char_t  m_osTagDecision;
	Char_t  m_tagCategory;
	Char_t  m_osTagCategory;
	Char_t  m_elecTagDecision;
	Char_t  m_muonTagDecision;
	Char_t  m_osKaonTagDecision;
	Char_t  m_ssKaonTagDecision;
	Char_t  m_ssPionTagDecision;
	Char_t  m_vtxTagDecision;

	ClassDef(ZooTagging,2)
};

/** @class ZooMCBkg
 *    \brief MC background information and flags
 */
class ZooMCBkg : public TObject
{  
    public:
	// bits for flags field
	enum {
	    DaughtersFromC = 1,
	    DaughtersFromB = 2,
	    isFromPV = 4
	};

	virtual ~ZooMCBkg();

	int Category()   const { return m_category; };
	unsigned flags() const {return m_flags; }

    private:
	Short_t m_category;
	UInt_t m_flags;

	friend class ZooWriter;

	ClassDef(ZooMCBkg,3);
};

/** @class ZooTrigger
 *    \brief trigger information
 */

class ZooTrigger : public TObject
{
    public:
	// default constructor
	ZooTrigger() :
	    m_L0Dec(0), m_L0TIS(0), m_L0TOS(0),
	    m_HLT1Dec(0), m_HLT1TIS(0), m_HLT1TOS(0),
	    m_HLT2Dec(0), m_HLT2TIS(0), m_HLT2TOS(0)
        {
	    std::fill(m_dec, m_dec + TriggerDecisions::NTriggerDecisionWords, 0);
	    std::fill(m_tis, m_tis + TriggerDecisions::NTriggerDecisionWords, 0);
	    std::fill(m_tos, m_tos + TriggerDecisions::NTriggerDecisionWords, 0);
	    std::fill(m_tps, m_tps + TriggerDecisions::NTriggerDecisionWords, 0);
	}
	virtual ~ZooTrigger();

	// backward compatible or of all bits
	bool L0Dec(void)   const { return m_L0Dec; }; 
	bool L0TIS(void)   const { return m_L0TIS; }; 
	bool L0TOS(void)   const { return m_L0TOS; };
	bool HLT1Dec(void) const { return m_HLT1Dec; }; 
	bool HLT1TIS(void) const { return m_HLT1TIS; }; 
	bool HLT1TOS(void) const { return m_HLT1TOS; };
	bool HLT2Dec(void) const { return m_HLT2Dec; }; 
	bool HLT2TIS(void) const { return m_HLT2TIS; }; 
	bool HLT2TOS(void) const { return m_HLT2TOS; };

	// these can get at an individual decision bit
	bool L0Dec(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_L0Dec >> idx) & 1; }
	bool L0TIS(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_L0TIS >> idx) & 1; }
	bool L0TOS(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_L0TOS >> idx) & 1; }
	bool HLT1Dec(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_HLT1Dec >> idx) & 1; }
	bool HLT1TIS(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_HLT1TIS >> idx) & 1; }
	bool HLT1TOS(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_HLT1TOS >> idx) & 1; }
	bool HLT2Dec(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_HLT2Dec >> idx) & 1; }
	bool HLT2TIS(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_HLT2TIS >> idx) & 1; }
	bool HLT2TOS(unsigned idx) const
	{ assert(LIKELY(idx < 64)); return (m_HLT2TOS >> idx) & 1; }

	// consistently set L0 decision, TIS and TOS
	void SetBitsL0(unsigned idx, bool L0Dec, bool L0TIS, bool L0TOS)
	{
	    assert(LIKELY(idx < 64));
	    if (L0Dec) m_L0Dec |= 1ULL << idx;
	    if (L0TIS) m_L0TIS |= 1ULL << idx;
	    if (L0TOS) m_L0TOS |= 1ULL << idx;
	}
	// consistently set HLT1 decision, TIS and TOS
	void SetBitsHLT1(unsigned idx, bool HLT1Dec, bool HLT1TIS, bool HLT1TOS)
	{
	    assert(LIKELY(idx < 64));
	    if (HLT1Dec) m_HLT1Dec |= 1ULL << idx;
	    if (HLT1TIS) m_HLT1TIS |= 1ULL << idx;
	    if (HLT1TOS) m_HLT1TOS |= 1ULL << idx;
	}
	// consistently set HLT2 decision, TIS and TOS
	void SetBitsHLT2(unsigned idx, bool HLT2Dec, bool HLT2TIS, bool HLT2TOS)
	{
	    assert(LIKELY(idx < 64));
	    if (HLT2Dec) m_HLT2Dec |= 1ULL << idx;
	    if (HLT2TIS) m_HLT2TIS |= 1ULL << idx;
	    if (HLT2TOS) m_HLT2TOS |= 1ULL << idx;
	}

	/// get trigger decision bit
	/** usage example:
	 * @code
	 * using namespace TriggerDecisions;
	 * ZooTrigger* tr = ... ;
	 * [...]
	 * if (tr->getDec<Hlt1Global>()) {
	 *   // do something
	 * }
	 * @endcode
	 * See ZooTriggerDecisions.h for a list of available decisions
	 */
	template<typename DEC> bool getDec(void) const
	{ return m_dec[DEC::word] & DEC::mask; }
	/// set/clear trigger decision bit
	template<typename DEC> void setDec(bool status)
	{ m_dec[DEC::word] = (m_dec[DEC::word] & ~DEC::mask) | (status ? DEC::mask : 0); }
	/// get trigger TIS bit
	template<typename DEC> bool getTIS(void) const
	{ return m_tis[DEC::word] & DEC::mask; }
	/// set/clear TIS decision bit
	template<typename DEC> void setTIS(bool status)
	{ m_tis[DEC::word] = (m_tis[DEC::word] & ~DEC::mask) | (status ? DEC::mask : 0); }
	/// get trigger TOS bit
	template<typename DEC> bool getTOS(void) const
	{ return m_tos[DEC::word] & DEC::mask; }
	/// set/clear trigger TOS bit
	template<typename DEC> void setTOS(bool status)
	{ m_tos[DEC::word] = (m_tos[DEC::word] & ~DEC::mask) | (status ? DEC::mask : 0); }
	/// get trigger TPS bit (trigger partially signal)
	template<typename DEC> bool getTPS(void) const
	{ return m_tps[DEC::word] & DEC::mask; }
	/// set/clear trigger TPS bit (trigger partially signal)
	template<typename DEC> void setTPS(bool status)
	{ m_tps[DEC::word] = (m_tps[DEC::word] & ~DEC::mask) | (status ? DEC::mask : 0); }
	/// get trigger TUS bit (trigger used signal: TOS | TPS)
	template<typename DEC> bool getTUS(void) const
	{ return (m_tos[DEC::word] | m_tps[DEC::word]) & DEC::mask; }

	/// get specified trigger decision word
	UInt_t getTriggerDecWord(unsigned int i) const
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    return m_dec[i]; 
	}

	/// get specified trigger TIS word
	UInt_t getTriggerTISWord(unsigned int i) const
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    return m_tis[i]; 
	}

	/// get specified trigger TOS word
	UInt_t getTriggerTOSWord(unsigned int i) const
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    return m_tos[i]; 
	}

	/// get specified trigger TPS word (trigger partially signal)
	UInt_t getTriggerTPSWord(unsigned int i) const
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    return m_tps[i]; 
	}

	/// get specified trigger TUS word (trigger used signal : TOS | TPS)
	UInt_t getTriggerTUSWord(unsigned int i) const
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    return m_tos[i] | m_tps[i]; 
	}

	/// set specified trigger decision word
	void setTriggerDecWord(unsigned int i, UInt_t val)
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    m_dec[i] = val;
	}

	/// set specified trigger TIS word
	void setTriggerTISWord(unsigned int i, UInt_t val)
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    m_tis[i] = val;
	}

	/// set specified trigger TOS word
	void setTriggerTOSWord(unsigned int i, UInt_t val)
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    m_tos[i] = val;
	}

	/// set specified trigger TPS word (trigger partially signal)
	void setTriggerTPSWord(unsigned int i, UInt_t val)
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    m_tps[i] = val;
	}

    private:
	ULong64_t m_L0Dec;
	ULong64_t m_L0TIS;
	ULong64_t m_L0TOS;
	ULong64_t m_HLT1Dec;
	ULong64_t m_HLT1TIS;
	ULong64_t m_HLT1TOS;
	ULong64_t m_HLT2Dec;
	ULong64_t m_HLT2TIS;
	ULong64_t m_HLT2TOS;

	UInt_t m_dec[TriggerDecisions::NTriggerDecisionWords];
	UInt_t m_tis[TriggerDecisions::NTriggerDecisionWords];
	UInt_t m_tos[TriggerDecisions::NTriggerDecisionWords];
	UInt_t m_tps[TriggerDecisions::NTriggerDecisionWords];

	ClassDef(ZooTrigger,3)
};

class ZooGhostCategory : public TObject
{
    public:
	virtual ~ZooGhostCategory();
	int ghostCategory() const { return m_ghostCategory; };

    private:
	Int_t m_ghostCategory;

	friend class ZooWriter;

	ClassDef(ZooGhostCategory,2)

};

class ZooDLL : public TObject
{
    public:
	virtual ~ZooDLL();
  ZooDLL() ;
	float  DLLe()   const { return m_DLLe; };
	float  DLLk()   const { return m_DLLk; };
	float  DLLmu()  const { return m_DLLmu; };
	float  DLLp()   const { return m_DLLp; };
	bool   isMuon() const { return m_isMuon; };
	bool   isMuonLoose() const { return m_isMuonLoose; };
	bool   hasRich() const { return m_hasRich; };



    private:
	Float_t m_DLLe;
	Float_t m_DLLk;
	Float_t m_DLLmu;
	Float_t m_DLLp;
	Short_t m_isMuon;
	Short_t m_isMuonLoose;
	Short_t m_hasRich;


	friend class ZooWriter;

	ClassDef(ZooDLL,2)
};

/// particle-particle linker table (based on hit content of tracks)
class ZooLinks : public TObject
{ 
    public:
	virtual ~ZooLinks();
	/// size of linker table
	unsigned NLinks() const { return m_fracs.size(); }
	/// size of linker table
	unsigned size() const { return NLinks(); }
	/// return ZooP at given index
	const ZooP* Link(unsigned i) const { return m_links[i]; }
	/// return fraction at given index (weight)
	float Frac(unsigned i) const
	{
	    return UNLIKELY(-128 == m_fracs[i]) ?
		std::numeric_limits<float>::quiet_NaN() :
		(float(m_fracs[i]) / 127.f);
	}
	/// add a link
	void AddLink(ZooP *p, float f)
	{
	    m_links.push_back(p);
	    f *= 127.f;
	    if (UNLIKELY(std::isnan(f) || std::isinf(f) || std::abs(f) > 127.f))
		m_fracs.push_back(-128);
	    else
		m_fracs.push_back(Char_t(f));
	}

	/// return pair of (const ZooP*, float)
	std::pair<const ZooP*, float> operator[](unsigned i) const
	{ return std::make_pair(Link(i), Frac(i)); }

	/// look up weight of a given ZooP
	float operator[](const ZooP* other) const
	{
	    for (unsigned i = 0; i < size(); ++i)
		if (other == Link(i)) return Frac(i);
	    return -1.f;
	}

	/// clear all links
	void clear() { m_links.clear(); m_fracs.clear(); }
	/// clear all links
	void Clear() { clear(); }

    private:
	/// weights are stored as char, accuracy better than 1% (should be sufficient)
	ZooPArray m_links;
	std::vector<Char_t> m_fracs;

	ClassDef(ZooLinks, 2)
};

class ZooHitPattern : public TObject
{
    public:
	ZooHitPattern() :
	    m_veloAR(0), m_veloCR(0),
	    m_veloAPhi(0), m_veloCPhi(0),
	    m_otMono1(0), m_otMono2(0),
	    m_itTopBottom(0), m_itAC(0), m_tt(0), m_muon(0) { 
	}

	ZooHitPattern(int veloAR, int veloCR, int veloAPhi, int veloCPhi,
		int otMono1, int otMono2, int itTopBottom, int itAC,
		int tt, int muon) :
	    m_veloAR(veloAR), m_veloCR(veloCR),
	    m_veloAPhi(veloAPhi), m_veloCPhi(veloCPhi),
	    m_otMono1(otMono1), m_otMono2(otMono2),
	    m_itTopBottom(itTopBottom), m_itAC(itAC),
	    m_tt(tt), m_muon(muon){ 
	}
	virtual ~ZooHitPattern();

	/// get pattern Velo A side R sensors
	std::bitset<23> veloAR() const { return std::bitset<23>(m_veloAR); }
	/// get pattern Velo C side R sensors
	std::bitset<23> veloCR() const { return std::bitset<23>(m_veloCR); }
	/// get pattern Velo A side Phi sensors
	std::bitset<23> veloAPhi() const { return std::bitset<23>(m_veloAPhi); }
	/// get pattern Velo C side Phi sensors
	std::bitset<23> veloCPhi() const { return std::bitset<23>(m_veloCPhi); }

	/// get bit pattern OT Monolayer 1
	std::bitset<12> otMono1() const { return std::bitset<12>(m_otMono2); }
	/// get bit pattern OT Monolayer 2
	std::bitset<12> otMono2() const { return std::bitset<12>(m_otMono1); }

	/// get bit pattern IT top/bottom box
	std::bitset<12> itTopBottom() const { return std::bitset<12>(m_itTopBottom); }
	/// get bit pattern IT A/C side box
	std::bitset<12> itAC() const { return std::bitset<12>(m_itAC); }

	/// get bit pattern TT
	std::bitset<4> tt() const { return std::bitset<4>(m_tt); }

	/// get bit pattern muon stations
	std::bitset<5> muon() const { return std::bitset<5>(m_muon); }

	// bitwise operations on ZooHitPattern
	/// bitwise or
	const ZooHitPattern& operator |= (const ZooHitPattern& other);
	/// bitwise and
	const ZooHitPattern& operator &= (const ZooHitPattern& other);
	/// bitwise xor
	const ZooHitPattern& operator ^= (const ZooHitPattern& other);
	/// bitwise complement (mostly helper for non-member operator~)
	ZooHitPattern complement() const;

    private:
	Int_t m_veloAR;
	Int_t m_veloCR;
	Int_t m_veloAPhi;
	Int_t m_veloCPhi;
	Short_t m_otMono1;
	Short_t m_otMono2;
	Short_t m_itTopBottom;
	Short_t m_itAC;
	Char_t m_tt;
	Char_t m_muon;

	ClassDef(ZooHitPattern, 2);
};

/// bitwise or
ZooHitPattern operator|(const ZooHitPattern& p1, const ZooHitPattern& p2);
/// bitwise and
ZooHitPattern operator&(const ZooHitPattern& p1, const ZooHitPattern& p2);
/// bitwise xor
ZooHitPattern operator^(const ZooHitPattern& p1, const ZooHitPattern& p2);
/// bitwise complement
ZooHitPattern operator~(const ZooHitPattern& other);

/// ExtraInfo map of tracks (key-value pairs mapping unsigned to float)
class ZooTrackExtraInfo: public ZooKeyValueBlock
{
 public:
  typedef ZooKeyValueBlock::KeyValuePair KeyValuePair;
  typedef ZooKeyValueBlock::KeyValueVector KeyValueVector;
  /// destructor
  virtual ~ZooTrackExtraInfo();
  /// default constructed (needed for ROOT I/O)
  ZooTrackExtraInfo() { }
  /// constructor to be used by anyone but ROOT
  ZooTrackExtraInfo(KeyValueVector& extrainfo) :
      ZooKeyValueBlock(extrainfo) { }
  /// copy constructor
  ZooTrackExtraInfo(const ZooTrackExtraInfo& extrainfo) :
      ZooKeyValueBlock(extrainfo) { }
  /// assignment
  const ZooTrackExtraInfo& operator=(const ZooTrackExtraInfo& extrainfo)
  { ZooKeyValueBlock::operator=(extrainfo); return *this; }
  /// enforce read-only access
  r_proxy operator[](unsigned key) { return get_r_proxy(key); }
  
 private:
  /// forbidden
  ZooTrackExtraInfo(const ZooKeyValueBlock&);
  /// forbidden
  const ZooTrackExtraInfo& operator=(const ZooKeyValueBlock& extrainfo);
  /// forbidden
  float erase(unsigned);
  /// forbidden
  float insert(unsigned, float);
  /// forbidden
  float modify(unsigned, float);
  /// forbidden
  const_iterator erase(const const_iterator&);
  /// forbidden
  unsigned capacity() const;
  /// forbidden
  void reserve(unsigned);
  /// forbidden
  void clear();

  ClassDef(ZooTrackExtraInfo, 3);
};

/** @class ZooTrackInfo
 * track information. We store one ZooTrackInfo per track. i.e. if a track is used for several ZooParticles in the event (a kaon and a pion) then both of them point to the same ZooTrackInfo.
 */
class ZooTrackInfo : public TObject
{
    public:

	ZooTrackInfo() :
	    m_chi2(-1.), m_cloneDist(-1), m_flags(UShort_t(-1)), m_ndf(-1),
	    m_nmeas(0), m_nlhcbids(0), m_nVeloIDs(0), m_nTTIDs(0), m_nITIDs(0),
	    m_nOTIDs(0), m_nMuonIDs(0), m_type(UChar_t(-1)),
	    m_history(UChar_t(-1)), m_patstate(UChar_t(-1)),
	    m_fitstate(UChar_t(-1)), m_ghostprob(-1.) { }

	ZooTrackInfo(double chi2, int ndf, int nmeas, int nlhcbids,
		int nVeloIDs, int nTTIDs, int nITIDs, int nOTIDs, int nMuonIDs,
		int type, int flags, int history, int patstate, int fitstate,
		int cloneDist, double ghostprob) :
	    m_chi2(chi2), m_cloneDist(cloneDist), m_flags(flags), m_ndf(ndf),
	    m_nmeas(nmeas), m_nlhcbids(nlhcbids), m_nVeloIDs(nVeloIDs),
	    m_nTTIDs(nTTIDs), m_nITIDs(nITIDs), m_nOTIDs(nOTIDs),
	    m_nMuonIDs(nMuonIDs), m_type(type), m_history(history),
	    m_patstate(patstate), m_fitstate(fitstate), m_ghostprob(ghostprob) { }

	virtual ~ZooTrackInfo();

	double chi2()       const { return m_chi2; }
	int cloneDist()  const{ return m_cloneDist;}
	int ndf()           const { return m_ndf; }//for compatibility reasons
	int nDoF()           const { return m_ndf; }
	int nmeas()         const { return m_nmeas; }
	int nLHCbIDs()      const { return m_nlhcbids; }
	int nVeloIDs()      const { return m_nVeloIDs; }
	int nTTIDs()        const { return m_nTTIDs; }
	int nITIDs()        const { return m_nITIDs; }
	int nSTIDs()        const { return nTTIDs() + nITIDs(); }
	int nOTIDs()        const { return m_nOTIDs; }
	int nTIDs()         const { return nITIDs() + nOTIDs(); }
	int nMuonIDs()      const { return m_nMuonIDs; }
	int type()          const { return m_type; }
	int flags()         const { return m_flags; }
	int history()       const { return m_history; }
	int patRecoStatus() const { return m_patstate; }
	int fitStatus()     const { return m_fitstate; }
  double ghostProbability() const { return m_ghostprob; }


	const ZooPackedStates* states() const { 
	  return reinterpret_cast<const ZooPackedStates*>(m_states.GetObject()); }
	void setStates(ZooPackedStates* states){ 
	  m_states = states; }

	const ZooHitPattern* hitPattern() const { 
	  return reinterpret_cast<const ZooHitPattern*>(m_hitPattern.GetObject()); }
	const ZooHitPattern* expectedHitPattern() const { 
	  return reinterpret_cast<const ZooHitPattern*>(m_expectedHitPattern.GetObject()); }
	const ZooHitPattern* collectedHitPattern() const { 
	  return reinterpret_cast<const ZooHitPattern*>(m_collectedHitPattern.GetObject()); }
	
	void setHitPattern(ZooHitPattern* hitPattern){ 
	  m_hitPattern = hitPattern; }
	void setExpectedHitPattern(ZooHitPattern* hitPattern){ 
	  m_expectedHitPattern = hitPattern; }
	void setCollectedHitPattern(ZooHitPattern* hitPattern){ 
	  m_collectedHitPattern = hitPattern; }

	const ZooTrackExtraInfo* extraInfo() const { 
	  return reinterpret_cast<const ZooTrackExtraInfo*>( m_extraInfo.GetObject()); }
	
	void setExtraInfo(ZooTrackExtraInfo* extraInfo){ 
	  m_extraInfo = extraInfo; }

	
	unsigned nAncestors() const { return m_ancestors.GetEntriesFast(); }
	
	const ZooTrackInfo* operator[](unsigned idx) const { 
	  return reinterpret_cast<const ZooTrackInfo*>(m_ancestors.At(idx)); }
	
	void addAncestor(ZooTrackInfo* ancestor) { 
	  m_ancestors.AddLast(ancestor); }

	const ZooLHCbIDBlock* lhcbids() const
	{ return reinterpret_cast<const ZooLHCbIDBlock*>(m_lhcbids.GetObject()); }
	const ZooLHCbIDBlock* expectedLhcbids() const
	{ return reinterpret_cast<const ZooLHCbIDBlock*>(m_expectedLhcbids.GetObject()); }
	const ZooLHCbIDBlock* collectedLhcbids() const
	{ return reinterpret_cast<const ZooLHCbIDBlock*>(m_collectedLhcbids.GetObject()); }

	void setLhcbids(ZooLHCbIDBlock* ids) {  m_lhcbids = ids; }
	void setExpectedLhcbids(ZooLHCbIDBlock* ids) {  m_expectedLhcbids = ids; }
	void setCollectedLhcbids(ZooLHCbIDBlock* ids) {  m_collectedLhcbids = ids; }

	/// history of tracks
	enum History {
	    HistoryUnknown = 0, TrackIdealPR = 1, TrackSeeding = 2, PatVelo = 3,
	    PatVeloTT = 4, TrackVeloTT = 5, PatForward = 6, TrackMatching = 7,
	    PatKShort = 8, TsaTrack = 9, MuonID = 10, PatVeloR = 11,
	    PatVeloGeneric = 12, PatVeloGeneral = 13, PatVeloOpen = 14, PatSeeding = 15,
	    PatMatch = 16, PatDownstream = 17, TTGeneric = 18
	};

    private:
	Float_t m_chi2;	
	//FIXME: for the moment save this as an int but in reality this is a float
	//check if we need floating point precision
	Int_t m_cloneDist;
	Short_t m_flags;	
	Char_t m_ndf;		
	UChar_t m_nmeas;	
	UChar_t m_nlhcbids;
	UChar_t m_nVeloIDs;
	UChar_t m_nTTIDs;
	UChar_t m_nITIDs;
	UChar_t m_nOTIDs;
	UChar_t m_nMuonIDs;
	


	// these can be made into Int_t members if ever required
	UChar_t m_type;		
	UChar_t m_history;	
	UChar_t m_patstate;	
	UChar_t m_fitstate;	
  Float_t m_ghostprob;

	/// these need not be filled
	TRef m_states;		
	TRef m_extraInfo;	
	TRef m_hitPattern;	
	TRef m_expectedHitPattern;	
	TRef m_collectedHitPattern;	
	TRefArray m_ancestors;	

	TRef m_lhcbids;
	TRef m_expectedLhcbids;
	TRef m_collectedLhcbids;

	ClassDef(ZooTrackInfo, 5)
};

/// generator level info on generated event
class ZooMCGenEventInfo : public TObject
{
    public:
	/// default constructor (needed by ROOT)
	ZooMCGenEventInfo() { }
	/// constructor to be used by anyone else
	ZooMCGenEventInfo(const std::vector<Short_t>& processTypes) :
	    m_processTypes(processTypes)
        { }
	virtual ~ZooMCGenEventInfo();

	/// return vector of process types
	const std::vector<Short_t>& processTypes() const { return m_processTypes; }

    private:
	/// generated process types (e.g. single and double diffractive events)
	/** Comprehensive list of subprocesses can be found in
	 * http://home.thep.lu.se/~torbjorn/pythia/lutp0613man2.pdf
	 */
	std::vector<Short_t> m_processTypes;

  ClassDef(ZooMCGenEventInfo, 1);
};

/** @class ZooEv
 *  \brief Base class that contains all information about the event
 *
 *	Represent the event (collection of all particles)
 */
class ZooEv : public TObject
{
    private:
	// forbid assignment and copying
	ZooEv& operator=(const ZooEv&);
	ZooEv(const ZooEv&);
	
	/// TRefs to the ZooP and ZooMCP banks in the tuple
	TRef m_particles, m_mcParticles;

	Short_t m_trackmult; 
	Short_t m_nbMCPVs;
  Short_t m_nInteractions;

	UInt_t m_L0Dec;
	UInt_t m_triggerDecisions[TriggerDecisions::NTriggerDecisionWords];

	ZooVertexArray m_pvs;
	Bool_t m_usingEmergencyPV;
	
	// ODIN quantities
	ULong64_t m_eventNumber;
	/// us since 1. 1. 1970 (might or might not include leap seconds)
	ULong64_t m_gpsTime;
	ULong64_t m_detectorStatus;
	UInt_t m_runNumber;
	UInt_t m_eventType;
	UInt_t m_orbitNumber;
	UInt_t m_bunchCurrent;
	UInt_t m_calibrationStep;
	UInt_t m_TCK;
	UShort_t m_bunchID;
	UChar_t m_triggerType;
	UChar_t m_readoutType;
	UChar_t m_bunchCrossingType;
	UChar_t m_calibrationType;
	UChar_t m_TAEWindow;
	UChar_t m_errorBits;
	Bool_t m_forceBit;

	TRef m_occupancy; ///< occupancy info block

	Short_t m_nTracksVelo; ///< total # velo tracks
	Short_t m_nTracksVeloBackward; ///< only backward velo tracks
	Short_t m_nTracksLong;
	Short_t m_nTracksUpstream;
	Short_t m_nTracksDownstream;
	Short_t m_nTracksT;

	Short_t m_spdMult;
	Short_t m_prsMult;
	Short_t m_ecalMult;
	Short_t m_hcalMult;

  Short_t m_veloMult;
  Short_t m_ttMult;
  Short_t m_itMult;
  Short_t m_otMult;

	TRef m_mcGenEventInfo;

	UInt_t m_nMCPartInAcceptance;

	Short_t m_polarity;

	TRef m_perJobObjects;

	friend class ZooWriter;

 public:

	/// get trigger decision bits
	/** usage example:
	 * @code
	 * using namespace TriggerDecisions;
	 * ZooEv* pev = ... ;
	 * [...]
	 * if (pev->getTriggerDec<Hlt1Global>()) {
	 *   // do something
	 * }
	 * @endcode
	 * See ZooTriggerDecisions.h for a list of available decisions
	 */
	template<typename DEC> bool getTriggerDec() const
	{ return m_triggerDecisions[DEC::word] & DEC::mask; }

	UInt_t getTriggerWord(unsigned int i) const
	{ 
	    assert(LIKELY(i < TriggerDecisions::NTriggerDecisionWords)); 
	    return m_triggerDecisions[i]; 
	}

	unsigned NP() const { return GetP().size(); } 
	unsigned NMCP() const { return GetMCP().size(); }
	const ZooObjClonesArray& GetP() const
	{ return *reinterpret_cast<ZooObjClonesArray*>(m_particles.GetObject()); }
	const ZooObjClonesArray& GetMCP() const
	{ return *reinterpret_cast<ZooObjClonesArray*>(m_mcParticles.GetObject()); }
	void SetP(const TRef& ref) { m_particles = ref; }
	void SetMCP(const TRef& ref) { m_mcParticles = ref; }
	const ZooP   *P(UInt_t i)
	{ return reinterpret_cast<const ZooP*>(GetP()[i]); }
	const ZooMCP *MCP(UInt_t i)
	{ return reinterpret_cast<const ZooMCP*>(GetMCP()[i]); }

	///returns the track multiplicity
	int trackmult() const { return m_trackmult; }
	///returns the number of MC PV's
  int nbMCPVs()   const { return m_nbMCPVs; }
  ///return number of MC Interactions
  int nInteractions() const { return m_nInteractions; }
	///returns the L0Decision of the event
	int L0Dec()             const { return m_L0Dec;}
	///returns an array of reconstructed PV's in the event
	const ZooVertexArray& pvs() const { return m_pvs; }
      
	enum ReadoutTypes { ZeroSuppressed = 0, NonZeroSuppressed = 1 };
	enum CalibrationTypes { A = 0, B = 1, C = 2, D = 3 };
	enum BXTypes { NoBeam = 0, Beam1 = 1, Beam2 = 2, BeamCrossing = 3 };
	enum TriggerTypes { PhysicsTrigger, BeamGasTrigger, LumiTrigger,
		TechnicalTrigger, AuxiliaryTrigger, NonZSupTrigger, 
		TimingTrigger, CalibrationTrigger };

	unsigned runNumber() const { return m_runNumber; }
	unsigned eventType() const { return m_eventType; }
	unsigned orbitNumber() const { return m_orbitNumber; }
	std::size_t eventNumber() const { return m_eventNumber; }
	std::size_t gpsTime() const {return m_gpsTime; }
	std::size_t detectorStatus() const { return m_detectorStatus; }
	unsigned errorBits() const { return m_errorBits; }
	unsigned bunchID() const { return m_bunchID; }
	TriggerTypes triggerType() const { return static_cast<TriggerTypes>(m_triggerType); }
	ReadoutTypes readoutType() const { return static_cast<ReadoutTypes>(m_readoutType); }
	bool forceBit() const { return m_forceBit; }
	BXTypes bxType() const { return static_cast<BXTypes>(m_bunchCrossingType); }
	unsigned bunchCurrent() const { return m_bunchCurrent; }
	unsigned calibrationStep() const { return m_calibrationStep; }
	unsigned TCK() const { return m_TCK; }
	unsigned TAEWindow() const { return m_TAEWindow; }
	CalibrationTypes calibrationType() const { return static_cast<CalibrationTypes>(m_calibrationType); }

	bool usingEmergencyPV() const { return m_usingEmergencyPV; }
	const ZooOccupancy* occupancy() const
	{ return reinterpret_cast<const ZooOccupancy*>(m_occupancy.GetObject());; }

	void Clear();

	ZooEv() { Clear(); } 

	virtual ~ZooEv();

	int nTracksVelo() const { return m_nTracksVelo; }
	int nTracksVeloBackward() const { return m_nTracksVeloBackward; }
	int nTracksLong() const { return m_nTracksLong; }
	int nTracksUpstream() const { return m_nTracksUpstream; }
	int nTracksDownstream() const { return m_nTracksDownstream; }
	int nTracksT() const { return m_nTracksT; }

	/// set MC generator level event info
	void setMCGenEventInfo(ZooMCGenEventInfo* evinfo)
	{ m_mcGenEventInfo = evinfo; }
	/// get MC generator level event info block (if available)
	const ZooMCGenEventInfo* mcGenEventInfo() const
	{ return reinterpret_cast<const ZooMCGenEventInfo*>(m_mcGenEventInfo.GetObject()); }
	
	//track multiplicity on generator level, based on some cuts
	//cuts can be defined via job options in ZooWriter	
	unsigned nMCPartInAcceptance() const { return m_nMCPartInAcceptance; }
	
	int nSPDMult() const { return m_spdMult; }
	int nPRSMult() const { return m_prsMult; }
	int nECALMult() const { return m_ecalMult; }
  int nHCALMult() const { return m_hcalMult; }

  int nVELOMult() const { return m_veloMult; }
  int nTTMult() const { return m_ttMult; }
  int nITMult() const { return m_itMult; }
  int nOTMult() const { return m_otMult; }

  int polarity() const { return m_polarity; }

	/// access per-job objects by their key
	TObject* perJobObject(const std::string& key) const;
	/// set map of per-job objects 
	void setPerJobObjects(TMap* perJobObjMap)
	{ m_perJobObjects = reinterpret_cast<TObject*>(perJobObjMap); }

	ClassDef(ZooEv,11)
};


/** @class ZooP
 *  \brief Represent particle with relation to other particles
 */
class ZooP : public TObject
{
 private:
  Int_t   m_pid;
  Float_t m_ip;
  Float_t m_ipSig;
  
  Float_t m_second_ipSig;
  Float_t m_second_ip;
  
  ZooPackedParticle m_particle;
  
  ZooPArray m_mother; 
  ZooPArray m_child; 
  TRefArray m_info; 
  
  ZooP &operator=(const ZooP&);
  ZooP(const ZooP&);

 public:
  
  ZooP() : m_info(ZooObjectID::maxid) { }
  virtual ~ZooP();
  
  const ZooP* Mo(unsigned i) const { return m_mother[i]; }
  unsigned    NMo()          const { return m_mother.size(); }
  const ZooP* Ch(unsigned i) const { return m_child[i]; }
  unsigned    NCh()          const { return m_child.size(); }
  const ZooP* operator[](UInt_t i) const { return m_child[i]; }
  
  void  AddMother(ZooP *m) { m_mother.push_back(m); }
  void  AddChild(ZooP *c) { m_child.push_back(c); }
  
  zooLorentzVector          momentum()    const { return m_particle.momentum(); }
  int                       pid()         const { return m_pid; }
  float                     ip()          const { return m_ip; }
  float                     ipSig()       const { return m_ipSig; }
  float			    second_ip()	  const { return m_second_ip; };
  float			    second_ipSig()	  const { return m_second_ipSig; };
  const ZooPackedVertex&    pv() const { return *Info<ZooPackedVertex>(); }
  const ZooPackedParticle&  particle() const { return m_particle; }
  const ZooMCP*             assocMCP()    const { return Info<ZooMCP>(); }

  /** return info block of specified type if available, 0 otherwise
   * usage:
   *
   * const ZooDecay* dc = zoop->Info<ZooDecay>();
   */
  template<class I> const I* Info() const {
    // force info blocks to have registered type
    BOOST_STATIC_ASSERT(ZooObjectID::maxid - ZooObjectID::ID<I>::id > 0);
    typedef boost::is_base_and_derived<TObject, I> fromTObject;
    BOOST_STATIC_ASSERT(fromTObject::value);
    return reinterpret_cast<I*>(m_info.At(ZooObjectID::ID<I>::id));
  }

  /** add an info block of the specified type to this ZooP and return
   * a pointer to the new block; the new block is copy constructed from
   * *other (if other is non-null) or a previously block is overwritten
   * (if other is non-null)
   *
   * the memory for the new info block is managed by the
   * ZooObjectManager pointed to by f
   *
   * usage: ZooStable *st = zoop->AddInfo<ZooStable>(*f);
   */
  template<class I> I* AddInfo(ZooObjectManager& f){
    BOOST_STATIC_ASSERT(ZooObjectID::maxid - ZooObjectID::ID<I>::id > 0);
    typedef boost::is_base_and_derived<TObject, I> fromTObject;
    BOOST_STATIC_ASSERT(fromTObject::value);
    I* info = reinterpret_cast<I*>(m_info[ZooObjectID::ID<I>::id]);
    if (LIKELY(0 == info)) {
      info = f.zooObj<I>();
      m_info.AddAt(info, ZooObjectID::ID<I>::id);
    } else 
      *info = I();
    return info;
  }

  /** add an info block of the specified type to this ZooP and return
   * a pointer to the new block; the new block is copy constructed from
   * *other (if other is non-null) or a previously block is overwritten
   * (if other is non-null)
   *
   * the memory for the new info block is managed by the
   * ZooObjectManager pointed to by f
   *
   * usage: ZooStable *st = zoop->AddInfo<ZooStable>(*f);
   */
  template<class I> I* AddInfo(ZooObjectManager& f, const I* other){
    BOOST_STATIC_ASSERT(ZooObjectID::maxid - ZooObjectID::ID<I>::id > 0);
    typedef boost::is_base_and_derived<TObject, I> fromTObject;
    BOOST_STATIC_ASSERT(fromTObject::value);
    assert(LIKELY(0 != other));
    I* info = reinterpret_cast<I*>(m_info[ZooObjectID::ID<I>::id]);
    if (LIKELY(0 == info)) {
      info = f.zooObj<I>(other);
      m_info.AddAt(info, ZooObjectID::ID<I>::id);
    } else 
      *info = *other;
    return info;
  }
  
  /** add an info block of the specified type to this ZooP and return
   * a pointer to the new block; the new block is copy constructed from
   * other (or the info block's contents are overwritten with an assignment
   * if the info block already exists)
   *
   * the memory for the new info block is managed by the
   * ZooObjectManager pointed to by f
   *
   * usage:
   *     ZooStable temp;
   *     // do something with temp
   *     ZooStable *st = zoop->AddInfo<ZooStable>(*f, temp);
   */
  template<class I> I* AddInfo(ZooObjectManager& f, const I& other){ 
    return AddInfo<I>(f, &other); 
  }
  
  /** assign an info block at its predefined location
   * returns pointer to Object referenced previously
   */
  template<class I> I* AssignInfo(I* info){
    BOOST_STATIC_ASSERT(ZooObjectID::maxid - ZooObjectID::ID<I>::id > 0);
    typedef boost::is_base_and_derived<TObject, I> fromTObject;
    BOOST_STATIC_ASSERT(fromTObject::value);
    I* oldinfo = reinterpret_cast<I*>(m_info[ZooObjectID::ID<I>::id]);
    m_info.AddAt(info, ZooObjectID::ID<I>::id);
    return oldinfo;
  }

  const ZooStable         *St()           const { return Info<ZooStable>(); }
  const ZooDecay          *Dc()           const { return Info<ZooDecay>(); }
  const ZooTrigger        *TrigDec()      const { return Info<ZooTrigger>(); }
  const ZooTagging        *Tag()          const { return Info<ZooTagging>(); }
  const ZooDLL            *DLL()          const { return Info<ZooDLL>(); } 
  const ZooLinks          *Links()        const { return Info<ZooLinks>(); }
  const ZooMCBkg          *Bkg()          const { return Info<ZooMCBkg>(); }
  const ZooTrackInfo      *TrInfo()       const { return Info<ZooTrackInfo>(); }
  const ZooTrackExtraInfo *TrExtraInfo()  const { return TrInfo()->extraInfo(); }
  const ZooHitPattern     *HitPattern()   const { return TrInfo()->hitPattern(); }
  const ZooGhostCategory  *GhostCat()     const { return Info<ZooGhostCategory>(); }
  const ZooTreefitInfo    *Treefit()      const { return Info<ZooTreefitInfo>(); }
  const ZooParticleInfo   *ParticleInfo() const { return Info<ZooParticleInfo>(); }
  const ZooLoKiBlock      *LoKiBlock()    const { return Info<ZooLoKiBlock>(); }

  float  measuredMass()       const { return Dc()->measuredMass(); };
  float  measuredMassErr()    const { return Dc()->measuredMassErr(); };
  float  fDistance()          const { return Dc()->fDistance(); };
  float  fDistanceErr()       const { return Dc()->fDistanceErr(); }; 
  float  ct()                 const { return Dc()->ct(); };
  float  ctErr()              const { return Dc()->ctErr(); };
  float  chi2()               const { return Dc()->chi2(); };
  int    nDoF()               const { return Dc()->nDoF(); };
  int    isolation()          const { return Dc()->isolation(); };
  
  XYZPoint decayVertex() const {
    return LIKELY(Dc())?m_particle.pos():
      XYZPoint(std::numeric_limits<double>::quiet_NaN(),
	       std::numeric_limits<double>::quiet_NaN(),
	       std::numeric_limits<double>::quiet_NaN());
  }
	
  ZooPackedVertex decayVertexPacked() const {
    const ZooDecay* dc = Dc();
    return LIKELY(dc) ? ZooPackedVertex(m_particle.pos(),
				m_particle.cov().Sub<ZooPackedVertex::Matrix>(0,0),
				dc->chi2(), dc->nDoF()):ZooPackedVertex();
  }
  
  int   trackType() const { return St()->trackType(); };
  int   charge()    const
  {
      const ZooStable* st = St();
      if (st) return st->charge();
      int charge = 0;
      for (unsigned i = 0; i < NCh(); ++i)
	  charge += Ch(i)->charge();
      return charge;
  };
  
  friend class ZooWriter;
  friend class ZooObjectManager;
  
  ClassDef(ZooP,1);
};


/** @class ZooParticleInfo: public TObject
 *  \brief Class that holds informations which were added to a particle in DaVinci
 *
 *  flexible methods of adding float type information to a zoontuple
 *  enables you to put infos on the tuple which the maintainer wouldn't allow you
 *  to implement for public release.
 *  reads info(int ,double) method of LHCb::Particle
 */
class ZooParticleInfo: public ZooKeyValueBlock
{
 public:
  typedef ZooKeyValueBlock::KeyValuePair KeyValuePair;
  typedef ZooKeyValueBlock::KeyValueVector KeyValueVector;
  /// destructor
  virtual ~ZooParticleInfo();
  /// default constructed (needed for ROOT I/O)
  ZooParticleInfo() { }
  /// constructor to be used by anyone but ROOT
  ZooParticleInfo(KeyValueVector& extrainfo) :
      ZooKeyValueBlock(extrainfo) { }
  /// copy constructor
  ZooParticleInfo(const ZooParticleInfo& extrainfo) :
      ZooKeyValueBlock(extrainfo) { }
  /// assignment
  const ZooParticleInfo& operator=(const ZooParticleInfo& extrainfo)
  { ZooKeyValueBlock::operator=(extrainfo); return *this; }
  
 private:
  /// forbidden
  ZooParticleInfo(const ZooKeyValueBlock&);
  /// forbidden
  const ZooParticleInfo& operator=(const ZooKeyValueBlock& extrainfo);

  ClassDef(ZooParticleInfo, 3);
};

#endif /* ZOO_H */

// vim: tw=78:sw=4:ft=cpp
