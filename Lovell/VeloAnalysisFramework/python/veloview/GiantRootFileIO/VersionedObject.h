/** @file VersionedObject.h
 *
 * @brief class mapping a version object to some kind of payload object
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-04-15
 */

#ifndef VERSIONEDOBJECT_H
#define VERSIONEDOBJECT_H

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <functional>

#include "TimeStamp.h"

/** @brief base class to keep a version history of objects
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-04-15
 *
 * This is a helper class never meant to be instantiated. It is used to keep
 * common traits of the templated family of VersionedObject classes in a
 * common place without code bloat more massive than necessary due to the
 * templated nature of VersionedObject.
 */
class VersionedObjectBase
{
    public:
	/** @brief exception used in operator[] of VersionedObject<...> & friends
	 *
	 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
	 * @date 2013-04-15
	 */
	class OutOfRangeException : public std::exception
        {
            public:
                /// constructor
                OutOfRangeException(const char* msg) throw ();
                /// destructor
                virtual ~OutOfRangeException() throw ();
                /// description
                virtual const char* what() const throw ();
	    private:
		/// message description
		const char* m_msg;
        };

	/// default constructor
	VersionedObjectBase();
	/// copy constructor
	VersionedObjectBase(const VersionedObjectBase& other);
	/// assignment operator
	VersionedObjectBase& operator=(const VersionedObjectBase& other);
	/// destructor
	virtual ~VersionedObjectBase();
};

/** @brief class to keep a version history of objects
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-04-15
 *
 * This class is templated with up to three parameters:
 * - T		type of object to be versioned
 * - VEROBJ	type of object to hold version information (e.g. a time stamp)
 * - CMP	default ordering of versions
 *
 * The VEROBJ type must be comparable (less than, greater than) to other
 * objects of that type so the individual revisions of an object can be sorted
 * internally. When ordered by the ordering CMP, the first version according
 * to that order becomes the "active" version, i.e. the version you get when
 * you do not ask for a specific version.
 *
 * The VersionedObject class tries to stay as close as possible to the
 * std::map interface of C++, with the following modifications:
 * - Payload objects can not only be retrieved by version (VEROBJ) as in
 *   std::map, they can also be retrieved by index (just like an array). The
 *   latter access mode goes through the versions in the order given by CMP.
 * - There is the notion of an "active" version, i.e. the version you get when
 *   you do not ask for a specific version. You can access it with a
 *   deferencing operation, just like a pointer, or the value() and
 *   active_version() methods.
 */
template <class OBJ, class VEROBJ = TimeStamp,
	 class CMP = std::greater<VEROBJ> >
class VersionedObject : public VersionedObjectBase
{
    private:
	/// type used for the version-object mapping
	typedef std::map<VEROBJ, OBJ, CMP> map_type;

    public:
	/// type of payload to be stored
	typedef typename map_type::mapped_type mapped_type;
	/// type of payload to be stored
	typedef mapped_type obj_type;
	/// type used for versioning (time stamp, version number, whatever)
	typedef typename map_type::key_type key_type;
	/// type used for versioning (time stamp, version number, whatever)
	typedef key_type version_type;
	/// type of entries (version, obj)
	typedef typename map_type::value_type value_type;
	/// type to represent sizes
	typedef typename map_type::size_type size_type;
	/// type to represent (size) differences
	typedef typename map_type::difference_type difference_type;
	// iterator type
	typedef typename map_type::iterator iterator;
	// const iterator type
	typedef typename map_type::const_iterator const_iterator;
	// reverse iterator type
	typedef typename map_type::reverse_iterator reverse_iterator;
	// const reverse iterator type
	typedef typename map_type::const_reverse_iterator const_reverse_iterator;
	/// type of version conparison function
	typedef typename map_type::key_compare key_compare;
	/// type of version conparison function
	typedef key_compare version_compare;
	/// type of allocator in underlying map
	typedef typename map_type::allocator_type allocator_type;
	/// shorthand for the type of the class itself
	typedef VersionedObject<
	    obj_type, version_type, version_compare> my_type;
	/// shorthand for type returned by insert methods
	typedef std::pair<iterator, bool> insert_return_type;

	/// default constructor
	VersionedObject();
	/// copy constructor
	VersionedObject(const my_type& other);
	/// assignment operator
	my_type& operator=(const my_type& other);
	/// destructor
	virtual ~VersionedObject();

	/// return "size" of the version object (i.e. number of versions)
	size_type size() const;
	/// return if empty
	bool empty() const;

	/// clear all stored objects/versions
	void clear();

	/// swap the contents of two VersionedObjects
	void swap(my_type& other);
	
	/// access the object of the "active" version
	obj_type& operator*();
	/// access the object of the "active" version
	const obj_type& operator*() const;
	/// access the object of the "active" version
	obj_type& operator->();
	/// access the object of the "active" version
	const obj_type& operator->() const;
	/// access the object of the "active" version
	const obj_type& value() const;
	/// get the active version
	const version_type& active_version() const;

	/// access to elements by version
	obj_type& operator[](const version_type& ver);

	/// access to elements by version
	const obj_type& operator[](const version_type& ver) const;

	/// access to elements by index (0, ..., size())
	value_type& operator[](const size_type nver);

	/// access to elements by index (0, ..., size())
	const value_type& operator[](const size_type nver) const;

	/// access to elements by version
	obj_type& at(const version_type& ver);

	/// access to elements by version
	const obj_type& at(const version_type& ver) const;

	/// access to elements by index (0, ..., size())
	value_type& at(const size_type nver);

	/// access to elements by index (0, ..., size())
	const value_type& at(const size_type nver) const;

	/// return iterator to first entry in map
	iterator begin();
	/// return iterator to first entry in map (const)
	const_iterator begin() const;
	/// return iterator one element past the last element
	iterator end();
	/// return iterator one element past the last element const
	const_iterator end() const;
	/// return reverse iterator to last entry in map
	reverse_iterator rbegin();
	/// return reverse iterator to last entry in map (const)
	const_reverse_iterator rbegin() const;
	/// return reverse iterator one element past the last element
	reverse_iterator rend();
	/// return reverse iterator one element past the last element (const)
	const_reverse_iterator rend() const;

	/** @brief insert a pair (version, object)
	 *
	 * @return a pair of (iterator, bool) where the iterator points to the
	 * element inserted (or not inserted), and the bool specifies if the
	 * insertion took place
	 *
	 * Insertion can fail if that version is already present in the
	 * container, if you intend to update it, erase the old version of the
	 * version in question and re-insert. HOWEVER, THAT SHOULD NEVER BE
	 * NECESSARY, SINCE A NEW VERSION SHOULD BE CREATED WITH UPDATED
	 * VALUES, WE SHOULD NEVER OVERWRITE OLD VERSIONS!
	 */ 
	insert_return_type insert(const value_type& value);
	/** @brief insert a pair (version, object)
	 *
	 * @return a pair of (iterator, bool) where the iterator points to the
	 * element inserted (or not inserted), and the bool specifies if the
	 * insertion took place
	 *
	 * Insertion can fail if that version is already present in the
	 * container, if you intend to update it, erase the old version of the
	 * version in question and re-insert. HOWEVER, THAT SHOULD NEVER BE
	 * NECESSARY, SINCE A NEW VERSION SHOULD BE CREATED WITH UPDATED
	 * VALUES, WE SHOULD NEVER OVERWRITE OLD VERSIONS!
	 */ 
	insert_return_type insert(
		const version_type& ver, const obj_type& obj);
	/// insert a range of pairs (version, object)
	template<class input_iterator>
	inline void insert(input_iterator first, input_iterator last)
	{ m_objs.insert(first, last); }

	/// erase element at given position
	void erase(iterator pos);
	/// erase range of elements
	void erase(iterator begin, iterator last);
	/// erase a specific version
	void erase(const version_type& ver);
	/// erase the n-th version in the order specified by CMP
	void erase(const size_type nver);

	/// return an iterator to given version
	iterator find(const version_type& ver);
	/// return a const iterator to given version
	const_iterator find(const version_type& ver) const;

	/// check if a given version is in the container
	bool contains(const version_type& ver) const;

	/// return iterator to first element with version ver or greater
	iterator lower_bound(const version_type& ver);
	/// return iterator to first element with version ver or greater
	const_iterator lower_bound(const version_type& ver) const;

	/// return iterator one after last element with version ver or smaller 
	iterator upper_bound(const version_type& ver);
	/// return iterator one after last element with version ver or smaller
	const_iterator upper_bound(const version_type& ver) const;

    private:
	/// keep the version information around
	map_type m_objs;
};

#if defined(__CINT__) || defined(__GCCXML__)
// instantiate most important versions of templated VersionedObject, so that
// the CINT/GCCXML/Reflex dictionary generation process sees them
#define INSTANTIATE_VERSIONEDOBJECTS_NOW
#include "VersionedObjectInstantiations.h"
#undef INSTANTIATE_VERSIONEDOBJECTS_NOW
#endif

#endif // VERSIONEDOBJECT_H

// vim: sw=4:tw=78:ft=cpp
