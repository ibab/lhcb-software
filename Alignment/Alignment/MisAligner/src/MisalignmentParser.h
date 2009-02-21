// $Id: MisalignmentParser.h,v 1.1 2009-02-21 22:21:17 jblouw Exp $
#ifndef INCLUDE_MISALIGNMENTPARSER_H
#define INCLUDE_MISALIGNMENTPARSER_H 1

#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <exception>

#include "boost/shared_ptr.hpp"
#include "boost/regex.hpp"

#include "TRandom3.h"

/** @class MisalignmentParser MisalignmentParser.h
 *  smart class to read option file containing specification on how/what
 *  to misalign
 *  will generate specified misalignments on the fly
 *
 *  the option file is a set of zero or more rules of the form
 *
 *  regexp misalignment_specification
 *
 *  regexp is a POSIX extended regular expression that the name of the
 *  detector element has to match
 *  
 *  the format of misalignment_specification is described in class
 *  Misalignment
 *
 *  rules are applied on a first match wins basis
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2008-07-18
 */
class MisalignmentParser {

  public:
    class AlignmentParametersImpl;
    typedef boost::shared_ptr<AlignmentParametersImpl> AlignmentParameters;
    typedef boost::regex regexp;

    /// Standard Constructor
    MisalignmentParser(const std::string& optionFileName);

    virtual ~MisalignmentParser(); ///< Destructor

    /** given the name of a detector element, return a set of alignment
     * parameters conforming to the first matching rule in the option
     * file
     */
    virtual AlignmentParameters getAlignmentParameters(
	const std::string& detelement);

    /** @class AlignmentParametersImpl
     *  class to hold the misalignments generated according to a misalignment
     *  description file
     *
     * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
     * @date   2008-07-18
     */
    class AlignmentParametersImpl
    {
      public:
	/// initialize with true misalignments
	AlignmentParametersImpl(const double deltas[6]) : m_misaligned(true)
        { std::copy(&deltas[0], &deltas[6], m_deltas); }
	/// initialize with no misalignments
	AlignmentParametersImpl() : m_misaligned(false)
        { std::fill(&m_deltas[0], &m_deltas[6], 0.); }

	/// destructor
	virtual ~AlignmentParametersImpl();

	virtual bool isMisaligned() const;
	virtual std::vector<double> getShifts() const;
	virtual std::vector<double> getRotations() const;
	virtual double dx() const;
	virtual double dy() const;
	virtual double dz() const;
	virtual double dalpha() const;
	virtual double dbeta() const;
	virtual double dgamma() const;

      private:
	double m_deltas[6];
	bool m_misaligned;
    };

  protected:
    /** @class Misalignment
     * class used to parse the misalignment specification given as string,
     * handing out alignment parameters generated according to that
     * specification
     *
     * the misalignment string may consist of shifts ("SHIFT=[...]") and/or
     * rotations ("ROTATE=[...]") (or none of the above in which case we
     * do not misalign)
     *
     * shifts and rotations are given as vector of three components each.
     * a component may consist of a number for fixed misalignments or one
     * of FIXED(number), FLAT(low,high), GAUSS(mean, sigma) or NONE.
     * components are seperated by commas.
     *
     * the string passed is converted to all uppercase letters and stripped
     * of all white space.
     *
     * the string may end in a comment which is started by either "#" or
     * "//", both of which cause the remainder of the line to be ignored
     *
     * all shifts are in units of millimeters, all rotations in units of
     * radians.
     *
     * example:
     *
     * SHIFT = [ 5.4, FLAT(-0.3, 0.3), GAUSS(0.0, 5.0) ]
     *
     * this would shift a detector element by 5.4 mm in x direction, draw the
     * shift in y direction from a flat distribution between -0.3 mm and
     * 0.3 mm., and use a gaussian shift centered around 0 mm with a sigma of
     * 5 mm in z
     *
     * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
     * @date   2008-07-18
     */
    class Misalignment
    {
      public:
	/// constructor reading a string with desired misalignments
	Misalignment(const std::string& misal);
	virtual ~Misalignment() { }
	
	/// get back object with a specific instance of these misalignments
	virtual AlignmentParameters get();

      private:
	typedef enum { none, fixed, flat, gauss } type;

	type m_types[6];
	double m_lo[6];
	double m_hi[6];
	TRandom3 m_rnd;

	/// read in a misalignment spec for one DOF
	virtual bool readspec(std::istringstream& istr, int nr);
	
	/// consume a given string from istr, return true on success
	virtual bool readstr(std::istringstream& istr, const std::string& str) const;
    };


  protected:
    std::vector<std::pair<std::string, Misalignment> > m_rules;
    class ParseException : public std::exception
    {
      public:
	ParseException(const std::string& str) : m_str(str) { }
	const char* what() const throw()
	{ return m_str.c_str(); }
	virtual ~ParseException() throw() { }
      private:
	std::string m_str;
    };

  private:

};

#endif // INCLUDE_MISALIGNMENTPARSER_H
