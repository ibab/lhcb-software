/** @brief implement "multiple personality calls"
 *
 * @file MultiplePersonalityCall.h
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-05-25
 */

#ifndef MULTIPLEPERSONALITYCALL_H
#define MULTIPLEPERSONALITYCALL_H

#include <map>
#include <string>
#include <vector>
#include <exception>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/utility/enable_if.hpp>

/// hide implementation details
namespace MultiplePersonalityCallHelpers{
    /** @brief helper class for the MultiplePersonalityCall class
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2013-05-25
     *
     * DO NOT USE DIRECTLY.
     */
    class MultiplePersonalityCallBase
    {
	protected:
	    /// map from personality nickname to unsigned
	    std::map<std::string, unsigned> m_personalities;
	    /// list of personality nicknames
	    std::vector<std::string> m_nicknames;
	    /// current personality
	    unsigned m_personality;

	    /// exception to throw if something goes wrong
	    class MultiplePersonalityCallException : public std::exception
	    {
		public: 
		    /// constructor
		    MultiplePersonalityCallException(const char* what) throw() :
			m_what(what) { }
		    /// destructor
		    virtual ~MultiplePersonalityCallException() throw() { }
		    /// return C style string describing what went wrong
		    virtual const char* what() const throw() { return m_what; }
		private:
		    /// pointer to C style string describing what went wrong
		    const char* m_what;
	    };

	    /// forbid direct construction
	    MultiplePersonalityCallBase() : m_personality(0) { }
	    /// forbid copy construction
	    MultiplePersonalityCallBase(
		    const MultiplePersonalityCallBase& other) :
		m_personalities(other.m_personalities),
		m_nicknames(other.m_nicknames), m_personality(0) { }
	    /// forbid destruction
	    ~MultiplePersonalityCallBase() { }
	    /// forbid assignment
	    MultiplePersonalityCallBase& operator=(
		    const MultiplePersonalityCallBase& other)
	    {
		{
		    std::map<std::string, unsigned> tmp(other.m_personalities);
		    m_personalities.swap(tmp);
		}
		{
		    std::vector<std::string> tmp(other.m_nicknames);
		    m_nicknames.swap(tmp);
		}
		m_personality = other.m_personality;
		return *this;
	    }

	    /// clear all internal data structures
	    void clear()
	    {
		m_personalities.clear();
		m_nicknames.clear();
		m_personality = 0;
	    }

	    /// swap contents with other
	    void swap(MultiplePersonalityCallBase& other)
	    {
		m_personalities.swap(other.m_personalities);
		m_nicknames.swap(other.m_nicknames);
		unsigned tmp = other.m_personality;
		other.m_personality = m_personality;
		m_personality = tmp;
	    }
    };

    /// safe pointer to member function binder (arity 0)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<0 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst);
    }

    /// safe pointer to member function binder (arity 1)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<1 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1);
    }

    /// safe pointer to member function binder (arity 2)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<2 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1, _2);
    }

    /// safe pointer to member function binder (arity 3)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<3 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1, _2, _3);
    }

    /// safe pointer to member function binder (arity 4)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<4 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1, _2, _3, _4);
    }

    /// safe pointer to member function binder (arity 5)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<5 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1, _2, _3, _4, _5);
    }

    /// safe pointer to member function binder (arity 6)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<6 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1, _2, _3, _4, _5, _6);
    }

    /// safe pointer to member function binder (arity 7)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<7 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1, _2, _3, _4, _5, _6, _7);
    }

    /// safe pointer to member function binder (arity 8)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<8 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1, _2, _3, _4, _5, _6, _7, _8);
    }

    /// safe pointer to member function binder (arity 9)
    template <class R, class T1, class T2>
    typename boost::enable_if_c<9 == R::arity, R&>::type
    safe_ptr_to_memfn_binder(R& dst, T1* inst, T2 mfnptr)
    {
	using namespace boost;
	return dst = bind(mfnptr, inst, _1, _2, _3, _4, _5, _6, _7, _8, _9);
    }
}

/** @brief function calls with "multiple personalities"
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-05-25
 *
 * Often, one finds oneself in the situation that some kind of operation needs
 * to be performed in different manners, depending on the version/type of data
 * being processed or some other consideration. This class attempts to
 * facilitate this kind of application by providing a convenient way to
 * "multiplex" different implementations of the same operation. Thus, this
 * class provides calls with multiple "personalities".
 *
 * Maybe the concept is best illustrated with an example. Suppose you want to
 * select tracks for different types of data as in the example below:
 *
 * @code
 * class Track;
 *
 * // the 2010 track selection is a standalone function
 * bool select2010(const Track& track)
 * { return track.p() > 2000.; }
 *
 * // the 2011 track selection is a functor with the same signature
 * struct Selector2011 {
 *   bool operator()(const Track& track) const
 *   { return track.p() > 2000. && track.pt() > 300.; }
 * };
 *
 * // 2012 data uses a method of a class
 * class Selector2012 {
 *   public:
 *     // construct selector from a cut specification
 *     Selector2012(const std::string& cuts);
 *     // check if track passes cuts
 *     bool passesCuts(const Track& track) const;
 * };
 * @endcode
 *
 * To call these very different selection routines in a consistent manner, one
 * first has to create a MultiplePersonalityCall object with the right
 * signature (takes a const Track& as sole argument, returns a bool). Then, one
 * registers the different personalities:
 * 
 * @code
 * #include <MultiplePersonalityCall.h>
 *
 * MultiplePersonalityCall<boost::function<bool(const Track&)> > select;
 * // register the 2010 selection function
 * select.registerPersonality("2010", &select2010);
 * // register the 2011 track selector
 * select.registerPersonality("2011", Selector2010());
 * // create a 2012 selection object
 * Selector2012 selector2012defaultcuts(
 *     "p > 3 GeV && pt > 300 MeV && nHits >= 10");
 * // and register the passesCuts method of that instance as 2012 selection
 * select.registerPersonality("2012",
 *     &selector2012defaultcuts, &Selector2012::passesCuts);
 * @endcode
 *
 * Selecting personalities and performing the call is then quite simple:
 *
 * @code
 * Track aTrack;
 * select.setPersonality("2012");
 * if (select(aTrack)) {
 *   // do something
 * }
 * @endcode
 *
 * The class supports arbitrary callable entities which take between 0 and 9
 * arguments. Copy construction, assignment and swapping of contents are
 * supported. The clear() operation clears all registered personalities,
 * bringing the object back to its pristine state just after construction.
 *
 * Note that MultiplePersonalityCall objects are not equality comparable.
 * (The main reason is that the operation is unimplemented (and apparently
 * does not make too much sense) for the underlying boost::function objects
 * used in the implementation.)
 */
template <class CALLABLE> class MultiplePersonalityCall :
    protected MultiplePersonalityCallHelpers::MultiplePersonalityCallBase,
    public CALLABLE
{
    private:
	/// hold array of callable objects, one for each personality
	std::vector<CALLABLE> m_callables;

    public:
	/// constructor
	MultiplePersonalityCall() { }
	/// copy constructor
	MultiplePersonalityCall(
		const MultiplePersonalityCall<CALLABLE>& other) :
	    MultiplePersonalityCallBase(other), m_callables(other.m_callables)
	{ CALLABLE::operator=(m_callables[m_personality]); }
	/// destructor
	~MultiplePersonalityCall() { }
	/// assignment operator
	MultiplePersonalityCall<CALLABLE>& operator=(
		const MultiplePersonalityCall<CALLABLE>& other)
	{
	    MultiplePersonalityCallBase::operator=(other);
	    {
		std::vector<CALLABLE> tmp(other.m_callables);
		tmp.swap(m_callables);
	    }
	    CALLABLE::operator=(m_callables[m_personality]);
	    return *this;
	}
	
	/// register new personality (function type)
	template<class FN>
	void registerPersonality(const std::string& personality, FN fn)
	{
	    std::map<std::string, unsigned>::iterator it = 
		m_personalities.find(personality);
	    if (m_personalities.end() != it)
		throw MultiplePersonalityCallException(
			"Personality already registered");
	    m_personalities[personality] = m_callables.size();
	    m_nicknames.push_back(personality);
	    m_callables.push_back(CALLABLE(fn));
	}

	/// register new personality (bound member function type)
	template<class CLASS, class FNPTR>
	void registerPersonality(const std::string& personality, CLASS* iptr, FNPTR fnptr)
	{
	    std::map<std::string, unsigned>::iterator it = 
		m_personalities.find(personality);
	    if (m_personalities.end() != it)
		throw MultiplePersonalityCallException(
			"Personality already registered");
	    m_personalities[personality] = m_callables.size();
	    m_nicknames.push_back(personality);
	    // bind the pointer-to-member-function pointer fnptr to class instance iptr
	    m_callables.push_back(CALLABLE());
	    MultiplePersonalityCallHelpers::safe_ptr_to_memfn_binder(
		    m_callables.back(), iptr, fnptr);
	}

	/// get number if personalities
	unsigned getNumberOfPersonalities() const
	{ return m_callables.size(); }
	/// get current personality
	unsigned getPersonality() const { return m_personality; }
	/// get current personality nickname
	const std::string& getPersonalityName() const
	{ return m_nicknames[m_personality]; }

	/// set personality of call
	void setPersonality(unsigned personality)
	{
	    if (personality >= m_callables.size())
		throw MultiplePersonalityCallException("Unknown Personality");
	    m_personality = personality;
	    CALLABLE::operator=(m_callables[m_personality]);
	}

	/// set personality of call
	void setPersonality(const std::string& personality)
	{
	    std::map<std::string, unsigned>::iterator it = 
		m_personalities.find(personality);
	    if (m_personalities.end() == it)
		throw MultiplePersonalityCallException("Unknown Personality");
	    setPersonality(it->second);
	}

	/// clear all registered personalities
	void clear()
	{
	    MultiplePersonalityCallBase::clear();
	    CALLABLE::clear();
	    m_callables.clear();
	}

	/// swap contents with other
	void swap(MultiplePersonalityCall<CALLABLE>& other)
	{
	    MultiplePersonalityCallBase::swap(other);
	    CALLABLE::swap(other);
	    m_callables.swap(other.m_callables);
	}
};

/// disallow comparison of same MultiplePersonalityCall types
template<class CALLABLE>
void operator==(const MultiplePersonalityCall<CALLABLE>&a,
	const MultiplePersonalityCall<CALLABLE>& b);

/// disallow comparison of same MultiplePersonalityCall types
template<class CALLABLE>
void operator!=(const MultiplePersonalityCall<CALLABLE>&a,
	const MultiplePersonalityCall<CALLABLE>& b);

#endif // MULTIPLEPERSONALITYCALL_H

// vim: sw=4:tw=78:ft=cpp
