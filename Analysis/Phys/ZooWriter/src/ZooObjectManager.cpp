#include "ZooObjectManager.h"
#include "Zoo.h"

/// template class used to initialize m_objects in a ZooObjectsManager
template<unsigned i> class Initializer
{
    private:
	Initializer<i - 1> m_dummy;
	typedef typename ZooObjectID::TYPE<i - 1>::type containedType;
    public:
	template<typename T> Initializer(T& obj) : m_dummy(obj)
        {
	    obj[i - 1] = new ZooObjClonesArray(containedType::Class());
	    TRef r(obj[i - 1]);
       	}
};
/// template class used to initialize m_objects in a ZooObjectsManager
/** termination condition (i == 0) specialization
 */
template<> class Initializer<0u>
{ public: template<typename T> Initializer(const T&) { } };

ZooObjectManager::ZooObjectManager(TTree& tree) :
    m_objects(new TObjArray(ZooObjectID::maxid))
{
    // make sure auto-loading references on demand works
    tree.BranchRef();
    m_objects->SetOwner(kTRUE);
    // add TClonesArrays for each type of Zoo object
    Initializer<ZooObjectID::maxid> dummy(*m_objects);
    // register Zoo objects with tree
    tree.Branch(m_objects, 1 << 16, 99, "ZooObjBank");
}

ZooObjectManager::~ZooObjectManager()
{
    Clear();
    delete m_objects;
}

void ZooObjectManager::Clear()
{
    for (unsigned i = 0; ZooObjectID::maxid != i; ++i) {
	m_mapping[i].clear();
	ZooObjClonesArray& arr =
	    *reinterpret_cast<ZooObjClonesArray*>((*m_objects)[i]);
	if (0 != arr.size()) arr.clear();
    }
}

ZooObjClonesArray::~ZooObjClonesArray() { }
template<typename OBJ> ZooObjRefArray<OBJ>::~ZooObjRefArray() { }

// force template instatiation
template class ZooObjRefArray<ZooP>;
template class ZooObjRefArray<ZooMCP>;
template class ZooObjRefArray<ZooPackedVertex>;

ClassImp(ZooObjClonesArray);
ClassImp(ZooPArray);
ClassImp(ZooMCPArray);
ClassImp(ZooVertexArray);

// vim:sw=4:tw=78:ft=cpp
