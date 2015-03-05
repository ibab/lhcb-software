/** @file VersionedObject.cc
 *
 * @brief class mapping a version object to some kind of payload object
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-04-15
 */

#include <iterator>

#include "VeloAnalysisFramework/VersionedObject.h"

VersionedObjectBase::VersionedObjectBase()
{ }

VersionedObjectBase::~VersionedObjectBase()
{ }

VersionedObjectBase::VersionedObjectBase(const VersionedObjectBase& /* other */)
{ }

VersionedObjectBase& VersionedObjectBase::operator=(
	const VersionedObjectBase& /* other */)
{
    return *this;
}

VersionedObjectBase::OutOfRangeException::OutOfRangeException(
	const char* msg) throw () : m_msg(msg)
{ }

VersionedObjectBase::OutOfRangeException::~OutOfRangeException() throw ()
{ }

const char* VersionedObjectBase::OutOfRangeException::what() const throw ()
{ return m_msg; }

template<class OBJ, class VEROBJ, class CMP>
VersionedObject<OBJ, VEROBJ, CMP>::VersionedObject()
{}

template<class OBJ, class VEROBJ, class CMP>
VersionedObject<OBJ, VEROBJ, CMP>::~VersionedObject()
{}

template<class OBJ, class VEROBJ, class CMP>
VersionedObject<OBJ, VEROBJ, CMP>::VersionedObject(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::my_type& other) :
    VersionedObjectBase(other), m_objs(other.m_objs)
{ }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::my_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator=(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::my_type& other)
{
    reinterpret_cast<VersionedObjectBase&>(*this) =
	reinterpret_cast<const VersionedObjectBase&>(other);
    m_objs = other.m_objs;
    return *this;
}

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::size_type
VersionedObject<OBJ, VEROBJ, CMP>::size() const
{ return m_objs.size(); }

template<class OBJ, class VEROBJ, class CMP>
bool VersionedObject<OBJ, VEROBJ, CMP>::empty() const
{ return m_objs.empty(); }

template<class OBJ, class VEROBJ, class CMP>
void VersionedObject<OBJ, VEROBJ, CMP>::clear()
{ m_objs.clear(); }

template<class OBJ, class VEROBJ, class CMP>
void VersionedObject<OBJ, VEROBJ, CMP>::swap(
	typename VersionedObject<OBJ, VEROBJ, CMP>::my_type& other)
{ m_objs.swap(other.m_objs); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator*()
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    return m_objs.begin()->second;
}

template<class OBJ, class VEROBJ, class CMP>
const typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator*() const
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    return m_objs.begin()->second;
}

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator->()
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    return m_objs.begin()->second;
}

template<class OBJ, class VEROBJ, class CMP>
const typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator->() const
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    return m_objs.begin()->second;
}

template<class OBJ, class VEROBJ, class CMP>
const typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::value() const
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance");
    return m_objs.begin()->second;
}

template<class OBJ, class VEROBJ, class CMP>
const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type&
VersionedObject<OBJ, VEROBJ, CMP>::active_version() const
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance");
    return m_objs.begin()->first;
}

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator[](
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver)
{ return m_objs[ver]; }

template<class OBJ, class VEROBJ, class CMP>
const typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator[](
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver) const
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    const_iterator it = find(ver);
    if (end() == it) throw OutOfRangeException(
	    "VersionedObject: requested version not found!");
    return it->second;
}

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::value_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator[](
	typename VersionedObject<OBJ, VEROBJ, CMP>::size_type nver)
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    if (nver >= size()) throw OutOfRangeException(
	    "VersionedObject: requested index out of range!");
    iterator it = begin();
    std::advance(it, nver);
    return *it;
}

template<class OBJ, class VEROBJ, class CMP>
const typename VersionedObject<OBJ, VEROBJ, CMP>::value_type&
VersionedObject<OBJ, VEROBJ, CMP>::operator[](
	typename VersionedObject<OBJ, VEROBJ, CMP>::size_type nver) const
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    if (nver >= size()) throw OutOfRangeException(
	    "VersionedObject: requested index out of range!");
    const_iterator it = begin();
    std::advance(it, nver);
    return *it;
}

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::at(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver)
{ return m_objs[ver]; }

template<class OBJ, class VEROBJ, class CMP>
const typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type&
VersionedObject<OBJ, VEROBJ, CMP>::at(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver) const
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    const_iterator it = find(ver);
    if (end() == it) throw OutOfRangeException(
	    "VersionedObject: requested version not found!");
    return it->second;
}

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::value_type&
VersionedObject<OBJ, VEROBJ, CMP>::at(
	typename VersionedObject<OBJ, VEROBJ, CMP>::size_type nver)
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    if (nver >= size()) throw OutOfRangeException(
	    "VersionedObject: requested index out of range!");
    iterator it = begin();
    std::advance(it, nver);
    return *it;
}

template<class OBJ, class VEROBJ, class CMP>
const typename VersionedObject<OBJ, VEROBJ, CMP>::value_type&
VersionedObject<OBJ, VEROBJ, CMP>::at(
	typename VersionedObject<OBJ, VEROBJ, CMP>::size_type nver) const
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    if (nver >= size()) throw OutOfRangeException(
	    "VersionedObject: requested index out of range!");
    const_iterator it = begin();
    std::advance(it, nver);
    return *it;
}

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::iterator
VersionedObject<OBJ, VEROBJ, CMP>::begin()
{ return m_objs.begin(); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::const_iterator
VersionedObject<OBJ, VEROBJ, CMP>::begin() const
{ return m_objs.begin(); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::iterator
VersionedObject<OBJ, VEROBJ, CMP>::end()
{ return m_objs.end(); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::const_iterator
VersionedObject<OBJ, VEROBJ, CMP>::end() const
{ return m_objs.end(); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::reverse_iterator
VersionedObject<OBJ, VEROBJ, CMP>::rbegin()
{ return m_objs.rbegin(); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::const_reverse_iterator
VersionedObject<OBJ, VEROBJ, CMP>::rbegin() const
{ return m_objs.rbegin(); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::reverse_iterator
VersionedObject<OBJ, VEROBJ, CMP>::rend()
{ return m_objs.rend(); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::const_reverse_iterator
VersionedObject<OBJ, VEROBJ, CMP>::rend() const
{ return m_objs.rend(); }

template<class OBJ, class VEROBJ, class CMP>
std::pair<typename VersionedObject<OBJ, VEROBJ, CMP>::iterator, bool>
VersionedObject<OBJ, VEROBJ, CMP>::insert(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::value_type& val)
{ return m_objs.insert(val); }

template<class OBJ, class VEROBJ, class CMP>
std::pair<typename VersionedObject<OBJ, VEROBJ, CMP>::iterator, bool>
VersionedObject<OBJ, VEROBJ, CMP>::insert(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& version,
	const typename VersionedObject<OBJ, VEROBJ, CMP>::obj_type& obj)
{ return insert(value_type(version, obj)); }

template<class OBJ, class VEROBJ, class CMP>
void VersionedObject<OBJ, VEROBJ, CMP>::erase(
	typename VersionedObject<OBJ, VEROBJ, CMP>::iterator pos)
{ m_objs.erase(pos); }

template<class OBJ, class VEROBJ, class CMP>
void VersionedObject<OBJ, VEROBJ, CMP>::erase(
	typename VersionedObject<OBJ, VEROBJ, CMP>::iterator first,
	typename VersionedObject<OBJ, VEROBJ, CMP>::iterator last)
{ m_objs.erase(first, last); }

template<class OBJ, class VEROBJ, class CMP>
void VersionedObject<OBJ, VEROBJ, CMP>::erase(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver)
{ m_objs.erase(ver); }

template<class OBJ, class VEROBJ, class CMP>
void VersionedObject<OBJ, VEROBJ, CMP>::erase(
	typename VersionedObject<OBJ, VEROBJ, CMP>::size_type nver)
{
    if (empty()) throw OutOfRangeException(
	    "Empty VersionedObject instance, index out of range!");
    if (nver >= size()) throw OutOfRangeException(
	    "VersionedObject: requested index out of range!");
    iterator it = begin();
    std::advance(it, nver);
    erase(it);
}

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::iterator
VersionedObject<OBJ, VEROBJ, CMP>::find(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver)
{ return m_objs.find(ver); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::const_iterator
VersionedObject<OBJ, VEROBJ, CMP>::find(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver) const
{ return m_objs.find(ver); }

template<class OBJ, class VEROBJ, class CMP>
bool VersionedObject<OBJ, VEROBJ, CMP>::contains(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver) const
{ return end() != find(ver); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::iterator
VersionedObject<OBJ, VEROBJ, CMP>::lower_bound(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver)
{ return m_objs.lower_bound(ver); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::const_iterator
VersionedObject<OBJ, VEROBJ, CMP>::lower_bound(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver) const
{ return m_objs.lower_bound(ver); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::iterator
VersionedObject<OBJ, VEROBJ, CMP>::upper_bound(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver)
{ return m_objs.upper_bound(ver); }

template<class OBJ, class VEROBJ, class CMP>
typename VersionedObject<OBJ, VEROBJ, CMP>::const_iterator
VersionedObject<OBJ, VEROBJ, CMP>::upper_bound(
	const typename VersionedObject<OBJ, VEROBJ, CMP>::version_type& ver) const
{ return m_objs.upper_bound(ver); }

// force instantiation of templated code for most important cases
#define INSTANTIATE_VERSIONEDOBJECTS_NOW
#include "VeloAnalysisFramework/VersionedObjectInstantiations.h"
#undef INSTANTIATE_VERSIONEDOBJECTS_NOW

// vim: sw=4:tw=78:ft=cpp
