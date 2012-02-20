#ifndef ZOOOBJECTMANAGER_H
#define ZOOOBJECTMANAGER_H

#include <cassert>
#include <utility>
#include <string>
#include <map>

#ifndef __CINT__
#include <boost/pool/pool_alloc.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#else
#define BOOST_STATIC_ASSERT(x)
#endif

#include <TObject.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TRef.h>
#include <TRefArray.h>
#include <TTree.h>

#include "ZooLikely.h"

/** @namespace ZooObjectID
 * this namespace serves as central repository of object ids and helps
 * with the mapping of object type to object id
 *
 * adding a new type of object consists of
 *
 * 1. writing a class, say NewInfo, for it which inherits from TObject
 * 2. adding an entry to the enum, say NEW_INFO, just before "maxid"
 * 3. put a line specifying the mapping between enum and class to the
 *    ones below: ZooObjectIDMapping(NewInfo, NEW_INFO);
 *    (this provides the mapping from object type to its id and
 *    vice-versa; a class or enum entry used twice won't compile)
 *
 * See below for examples.
 */
namespace ZooObjectID
{
    enum {
	ZOOP_OBJ = 0,
	ZOOMCP_OBJ,
	STABLE_INFO,
	DECAY_INFO,
	TRIGGER_INFO,
	TAGGING_INFO,
	DLL_INFO, 
	LINKS_INFO,
	BKG_INFO,
	PACKEDSTATES_INFO,
	TRACK_INFO,
	HITPATTERN_INFO,
	TRACKEXTRAINFO_INFO,
	GHOSTCATEGORY_INFO,
	PACKEDVERTEX_INFO,
	OCCUPANCY_INFO,
	ZOOLHCBIDBLOCK_INFO,
	ZOOMCGENEVENT_INFO,
	TREEFIT_INFO,
        ZOOPARTICLE_INFO,
  ZOOLOKIBLOCK_INFO,
	// ADD YOUR NEW INFO BLOCK IDS HERE!!!!
	maxid // this must always be last
    };

    // provide the fallback mapping (invalid id, invalid class)
    template<class T> struct ID { enum { id = ZooObjectID::maxid }; };
    template<unsigned I> struct TYPE { typedef void type; };
};

// define a nice macro which lets us add a mapping from info block id
// to its type and vice versa; also make a forward declaration of its
// type
#define ZooObjectIDMapping(x,y) class x; namespace ZooObjectID {\
    template<> struct ID<x>  { enum { id = ZooObjectID::y }; }; \
    template<> struct TYPE<ZooObjectID::y>  { typedef x type; }; }
ZooObjectIDMapping(ZooP, ZOOP_OBJ);
ZooObjectIDMapping(ZooMCP, ZOOMCP_OBJ);
ZooObjectIDMapping(ZooStable, STABLE_INFO);
ZooObjectIDMapping(ZooDecay, DECAY_INFO);
ZooObjectIDMapping(ZooTrigger, TRIGGER_INFO);
ZooObjectIDMapping(ZooTagging, TAGGING_INFO);
ZooObjectIDMapping(ZooDLL, DLL_INFO);
ZooObjectIDMapping(ZooLinks, LINKS_INFO);
ZooObjectIDMapping(ZooMCBkg, BKG_INFO);
ZooObjectIDMapping(ZooPackedStates, PACKEDSTATES_INFO);
ZooObjectIDMapping(ZooTrackInfo, TRACK_INFO);
ZooObjectIDMapping(ZooHitPattern, HITPATTERN_INFO);
ZooObjectIDMapping(ZooTrackExtraInfo, TRACKEXTRAINFO_INFO);
ZooObjectIDMapping(ZooGhostCategory, GHOSTCATEGORY_INFO);
ZooObjectIDMapping(ZooPackedVertex, PACKEDVERTEX_INFO);
ZooObjectIDMapping(ZooOccupancy, OCCUPANCY_INFO);
ZooObjectIDMapping(ZooLHCbIDBlock, ZOOLHCBIDBLOCK_INFO);
ZooObjectIDMapping(ZooMCGenEventInfo, ZOOMCGENEVENT_INFO);
ZooObjectIDMapping(ZooTreefitInfo, TREEFIT_INFO);
ZooObjectIDMapping(ZooParticleInfo, ZOOPARTICLE_INFO);
ZooObjectIDMapping(ZooLoKiBlock, ZOOLOKIBLOCK_INFO);
// ADD YOUR NEW ZOOOBJECTIDMAPPINGS HERE
#undef ZooObjectIDMapping

/// more well-behaved and type-safe TRefArray
template<typename OBJ> class ZooObjRefArray : public TObject
{
    private:
	TRefArray m_refs;
    public:
	ZooObjRefArray() { Class()->IgnoreTObjectStreamer(); }
	virtual ~ZooObjRefArray();
	unsigned size() const { return m_refs.GetEntriesFast(); }
	
	const OBJ* operator[](unsigned idx) const
	{ return reinterpret_cast<const OBJ*>(m_refs[idx]); }

	void push_back(OBJ* obj)
	{ m_refs.AddLast(reinterpret_cast<TObject*>(obj)); }

	void clear() { m_refs.Delete(); }

	ClassDef(ZooObjRefArray, 1);
};

/// more well-behaved TClonesArray
class ZooObjClonesArray : public TObject
{
    private:
	TClonesArray m_clones;
	mutable UInt_t m_sz;
	std::string m_name; //! transient member
    public:
	/// default constructor for ROOT - don't use
	ZooObjClonesArray() : m_sz(0) { }
	/// construct a clones array for objects of given class
	ZooObjClonesArray(TClass* c) :
	    m_clones(c, 1024), m_sz(0),
	    m_name(std::string(Class()->GetName()) + "_" + c->GetName())
        { }
	virtual ~ZooObjClonesArray();
	/// return a name that gives a clue as to what the contents are
	virtual const char* GetName() const { return m_name.c_str(); }
	/// clear the clones array
	void clear() { if (m_sz) { m_clones.Delete(); m_sz = 0; } }
	/// get its current size
	UInt_t size() const { return m_sz; }
	/// get access to the object at index idx
	TObject* operator[](UInt_t idx) const
	{
	    if (UNLIKELY(idx >= m_sz)) m_sz = idx + 1;
	    return m_clones[idx];
	}
	/// operator[] to be used to put an object into the clones array
	/**
	 * example code:
	 * 	ZooObjClonesArray<ZooP> arr;
	 *
	 * 	new(arr[arr.size()]) ZooP(...);
	 *      // now, we can use the ZooP
	 *      ZooP* zp = (ZooP*) arr[arr.size() - 1];
	 */
	TObject*& operator[](UInt_t idx)
	{
	    if (UNLIKELY(idx >= m_sz)) m_sz = idx + 1;
	    return *(&m_clones[idx]);
	}

	ClassDef(ZooObjClonesArray, 1);
};

/// CINT itself does not need to know about ZooObjectManager
#ifndef __CINT__
/// the ZooObjectManager creates, owns and writes to a TTree all ZooObjects
class ZooObjectManager
{
    private:
	typedef std::pair<const void*, void*> VoidPointerPair;
	/// type used as an allocator
	typedef boost::pool_allocator<VoidPointerPair> VoidPointerAlloc;
	/// type used for mapping one void* to another void*
	typedef std::map<const void *, void *, std::less<const void*>,
		VoidPointerAlloc> VoidPointerMap;
    public:
	/// constructor - NOTE: the manager must live longer than the tree
	ZooObjectManager(TTree& tree, TTree& perjobtree,
		const unsigned long& nevts);
	/// destructor
	virtual ~ZooObjectManager();

	/// create new zoo object (possibly copy-constructing it from pointer)
	template<typename I> I* zooObj()
	{
	    // force info blocks to have registered type
	    BOOST_STATIC_ASSERT(ZooObjectID::maxid - ZooObjectID::ID<I>::id > 0);
	    // force info blocks to inherit from TObject
	    typedef boost::is_base_and_derived<TObject, I> fromTObject;
	    BOOST_STATIC_ASSERT(fromTObject::value);
	    // get references to number of info blocks of type I and array
	    // containing them
	    ZooObjClonesArray& arr = *reinterpret_cast<ZooObjClonesArray*>(
		    (*m_objects)[ZooObjectID::ID<I>::id]);
	    // return pointer to newly created info block
	    return new(arr[arr.size()]) I;
	}
	/// create new zoo object (possibly copy-constructing it from pointer)
	template<typename I> I* zooObj(const I* other)
	{
	    // force info blocks to have registered type
	    BOOST_STATIC_ASSERT(ZooObjectID::maxid - ZooObjectID::ID<I>::id > 0);
	    // force info blocks to inherit from TObject
	    typedef boost::is_base_and_derived<TObject, I> fromTObject;
	    BOOST_STATIC_ASSERT(fromTObject::value);
	    assert(LIKELY(0 != other));
	    // get references to number of info blocks of type I and array
	    // containing them
	    ZooObjClonesArray& arr = *reinterpret_cast<ZooObjClonesArray*>(
		    (*m_objects)[ZooObjectID::ID<I>::id]);
	    // return pointer to newly created info block
	    return new(arr[arr.size()]) I(*other);
	}

	/// copy construct new zoo object from existing one
	template<typename I> I* zooObj(const I& other)
	{ return zooObj<I>(&other); }

	/// fill a ZooObjRefArray with references to the content of a bank
	template<typename I> const TRef getBankRef() const
	{
	    // force info blocks to have registered type
	    BOOST_STATIC_ASSERT(ZooObjectID::maxid - ZooObjectID::ID<I>::id > 0);
	    // force info blocks to inherit from TObject
	    typedef boost::is_base_and_derived<TObject, I> fromTObject;
	    BOOST_STATIC_ASSERT(fromTObject::value);
	    // get references to number of info blocks of type I and array
	    // containing them
	    return TRef(reinterpret_cast<ZooObjClonesArray*>(
			(*m_objects)[ZooObjectID::ID<I>::id]));
	}

	/// clear all info blocks (e.g. after an event was written to a tree)
	void Clear();

	/// look up if there is a mapping for a given LHCb object
	template<typename I> I* getMappingFor(const void * lhcbobj) const
	{
	    const VoidPointerMap& map =
		m_mapping[ZooObjectID::ID<I>::id];
	    VoidPointerMap::const_iterator it = map.find(lhcbobj);
	    if (UNLIKELY(map.end() == it)) return 0;
	    return reinterpret_cast<I*>(it->second);
	}
	/// get or create a mapping for a given LHCb object
	template<typename I> I*& getOrCreateMappingFor(const void * lhcbobj)
	{
	    VoidPointerMap& map = m_mapping[ZooObjectID::ID<I>::id];
	    return *reinterpret_cast<I**>(&map[lhcbobj]);
	}

	/** @brief associate a string with a per job objects
	 *
	 * @param str	string under which to register object
	 * @param obj	object to register (ZooObjectManager takes ownership)
	 * @returns	old mapping if it exists (0 otherwise)
	 *
	 * per job objects all have to be allocated before the first event is
	 * being filled
	 */
	TObject* zooPerJobObject(const std::string& str, TObject* obj);
	
	/** @brief associate a string with a per job objects
	 *
	 * @param str	string under which to register object
	 * @param obj	object to register (ZooObjectManager takes ownership)
	 * @returns	old mapping if it exists (0 otherwise)
	 *
	 * per job objects all have to be allocated before the first event is
	 * being filled
	 */
	template<class T>
	TObject* zooPerJobObject(const std::string& str, const T& obj)
	{
	    // disallow registration of new objects if first event has begun
	    if (m_nevts) return 0;
	    return zooPerJobObject(str, dynamic_cast<TObject*>(new T(obj)));
	}

	/** @brief lookup per-job object associated with a string
	 *
	 * @param str	string under which to register object
	 * @returns	old mapping if it exists (0 otherwise)
	 *
	 * per job objects all have to be allocated before the first event is
	 * being filled
	 */
	TObject* zooPerJobObject(const std::string& str) const;

	/// return number of elements in per job object map
	unsigned long perJobObjMapSize() const
	{ return m_perJobMap.size(); }

	/// return map of per-job objects
	const std::map<std::string, TObject*>& perJobObjMap() const
	{ return m_perJobMap; }

    private:
	/// array for arrays of zoo objects (one per type)
	TObjArray *m_objects;
	/// mapping to LHCb software objects
	VoidPointerMap m_mapping[ZooObjectID::maxid];
	/// mapping of per-job-Objects
	std::map<std::string, TObject*> m_perJobMap;
	/// reference to owner's event counter
	const unsigned long& m_nevts;
};
#else
/// forward declaration
class ZooObjectManager;
#endif

typedef ZooObjRefArray<ZooP> ZooPArray;
typedef ZooObjRefArray<ZooMCP> ZooMCPArray;
typedef ZooObjRefArray<ZooPackedVertex> ZooVertexArray;

#endif // ZOOOBJECTMANAGER_H

// vim:tw=78:sw=4:ft=cpp
