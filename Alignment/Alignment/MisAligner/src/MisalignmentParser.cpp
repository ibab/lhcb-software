// $Id: MisalignmentParser.cpp,v 1.4 2009-09-09 14:07:44 jblouw Exp $

#include "MisalignmentParser.h"

MisalignmentParser::MisalignmentParser(const std::string& optionFileName) 
{
  std::ifstream options(optionFileName.c_str(), std::ios::in);

  std::size_t nLines = 0;
  std::string line;
  while (getline(options, line)) {
    ++nLines;
    if (line.empty()) continue;
    // check for lines containing only comments
    // strip leading whitespace
    std::string tmp(line);
    for (std::string::iterator i = tmp.begin(); tmp.end() != i; ) {
      if (std::isspace(*i)) i = tmp.erase(i);
      else ++i;
    }
    // check for comment/empty line
    if (tmp.empty() || ('#' == tmp[0]) ||
	(2 <= tmp.size() && '/' == tmp[0] && '/' == tmp[1]))
      continue;
    // can append spaces to end without changing meaning
    line += ' ';
    // search for first space to break string into regexp and misalignment
    // specification
    std::size_t spacepos = line.find(' ', 0);
    if (std::string::npos != spacepos) {
      // found space, split into two substrings
      std::string regexpstr = line.substr(0, spacepos);
      std::string misalspecstr = line.substr(spacepos);
      try {
	m_rules.push_back(
	    std::make_pair(regexpstr, Misalignment(misalspecstr)));
      } catch (ParseException ex) {
	std::ostringstream os;
        os << "could not parse line " << nLines << " of file "
	  << optionFileName << ": " << ex.what() << "\n" <<
	  "line is: " << line << "\n";
	throw ParseException(os.str());
      }
      continue;
    }
  }
}

MisalignmentParser::~MisalignmentParser()
{ }

MisalignmentParser::AlignmentParameters
MisalignmentParser::getAlignmentParameters(
    const std::string& detelement)
{
  for (unsigned i = 0; i < m_rules.size(); ++i) {
    regexp e(m_rules[i].first, boost::regex::extended);
    // skip non-matching rules
    std::cout << "Checking " << detelement << " vs. " << e << std::endl;
    if (!boost::regex_match(detelement, e)) 
      continue;
    // ok, found one that fits
    std::cout << "Match " << detelement << " and " << e << std::endl;
    return m_rules[i].second.get();
  }

  // if we have not found anything, return no misalignments
  return AlignmentParameters(new AlignmentParametersImpl());
}

MisalignmentParser::Misalignment::Misalignment(const std::string& str) :
  m_rnd(0)
{
  // set everything to no misalignment
  for (int i = 6; i--; m_types[i] = none, m_hi[i] = m_lo[i] = 0.) {} ;
  // make copy for our own processing
  std::string mystr(str);
  // turn to upper case, remove whitespace
  for (std::string::iterator i = mystr.begin(); mystr.end() != i; ) {
    *i = std::toupper(*i);
    if (std::isspace(*i)) i = mystr.erase(i, i + 1);
    else ++i;
  }
  // no misalignments given?
  if (mystr.empty()) return;

  // parse mystr
  std::istringstream istr(mystr);
  std::cout << "Parsing... " << istr.str() << std::endl;
  while (!istr.eof()) {
    if (readstr(istr, "SHIFT=[")) {
      // read exactly three comma seperated misalignment specs
      // complain if it doesn't work
      if (!readspec(istr, 0)) throw ParseException("Invalid format in SHIFT");
      if (!readstr(istr, ",")) throw ParseException("Invalid format in SHIFT");
      if (!readspec(istr, 1)) throw ParseException("Invalid format in SHIFT");
      if (!readstr(istr, ",")) throw ParseException("Invalid format in SHIFT");
      if (!readspec(istr, 2)) throw ParseException("Invalid format in SHIFT");
      if (!readstr(istr, "]")) throw ParseException("Invalid format in SHIFT");
      continue;
    }
    // didn't work, clear error flags
    istr.clear();
    if (readstr(istr, "ROTATE=[")) {
      // read exactly three comma seperated misalignment specs
      // complain if it doesn't work
      if (!readspec(istr, 3)) throw ParseException("Invalid format in ROTATE");
      if (!readstr(istr, ",")) throw ParseException("Invalid format in ROTATE");
      if (!readspec(istr, 4)) throw ParseException("Invalid format in ROTATE");
      if (!readstr(istr, ",")) throw ParseException("Invalid format in ROTATE");
      if (!readspec(istr, 5)) throw ParseException("Invalid format in ROTATE");
      if (!readstr(istr, "]")) throw ParseException("Invalid format in ROTATE");
      continue;
    }
    istr.clear();
    if (readstr(istr, "#")) {
      // comment till end of line - ignore rest
      break;
    }
    istr.clear();
    if (readstr(istr, "//")) {
      // comment till end of line - ignore rest
      break;
    }
    // something we didn't expect, so fail
    if (!istr.eof()) throw ParseException("Invalid format.");
  }
}

bool MisalignmentParser::Misalignment::readspec(
    std::istringstream& istr, int nr)
{
  if (readstr(istr, "NONE")) {
    // no misalignment
    m_types[nr] = none;
    return istr;
  }
  istr.clear();
  if (readstr(istr, "FIXED(")) {
    // object fixed
    m_types[nr] = fixed;
    if (!(istr >> m_lo[nr])) return istr;
    if (0. == m_lo[nr])
      m_types[nr] = none;
    return readstr(istr,")");
  }
  istr.clear();
  if (readstr(istr, "FLAT(")) {
    // misalign by random amount, flat distribution
    m_types[nr] = flat;
    if (!(istr >> m_lo[nr])) return false;
    if (!readstr(istr, ",")) return false;
    if (!(istr >> m_hi[nr])) return false;
    if (m_lo[nr] > m_hi[nr]) std::swap(m_lo[nr], m_hi[nr]);
    return readstr(istr, ")");
  }
  istr.clear();
  if (readstr(istr, "GAUSS(")) {
    // misalign by random amount, gauss distribution
    m_types[nr] = gauss;
    if (!(istr >> m_lo[nr])) return false;
    if (!readstr(istr, ",")) return false;
    if (!(istr >> m_hi[nr])) return false;
    return readstr(istr, ")");
  }
  istr.clear();
  if (istr >> m_lo[nr]) {
    // fixed misalignment, shorthand
    m_types[nr] = fixed;
    if (0. == m_lo[nr])
      m_types[nr] = none;
    return istr;
  }
  // something we didn't expect: return istr which will signal the error
  return istr;
}

bool MisalignmentParser::Misalignment::readstr(
    std::istringstream& istr, const std::string& str) const
{
  std::size_t sz = 0;
  std::istringstream::pos_type pos = istr.tellg();
  while (sz < str.size()) {
    char c;
    // read error?
    if (!(istr >> c)) break;
    // does c match?
    if (c != str[sz]) break;
    ++sz;
  }
  // if compared to end, return success
  if (sz == str.size()) return true;
  // no match, restore old position
  istr.seekg(pos);
  return false;
}

MisalignmentParser::AlignmentParametersImpl::~AlignmentParametersImpl()
{ }

bool MisalignmentParser::AlignmentParametersImpl::isMisaligned() const
{ return m_misaligned; }

std::vector<double> MisalignmentParser::AlignmentParametersImpl::getShifts() const
{ return std::vector<double>(&m_deltas[0], &m_deltas[3]); }

std::vector<double> MisalignmentParser::AlignmentParametersImpl::getRotations() const
{ return std::vector<double>(&m_deltas[3], &m_deltas[6]); }

double MisalignmentParser::AlignmentParametersImpl::dx() const
{ return m_deltas[0]; }

double MisalignmentParser::AlignmentParametersImpl::dy() const
{ return m_deltas[1]; }

double MisalignmentParser::AlignmentParametersImpl::dz() const
{ return m_deltas[2]; }

double MisalignmentParser::AlignmentParametersImpl::dalpha() const
{ return m_deltas[3]; }

double MisalignmentParser::AlignmentParametersImpl::dbeta() const
{ return m_deltas[4]; }

double MisalignmentParser::AlignmentParametersImpl::dgamma() const
{ return m_deltas[5]; }

MisalignmentParser::AlignmentParameters
	MisalignmentParser::Misalignment::get()
{
  double values[6];
  int nnone = 0;
  // loop over degrees of freedom and generate fixed/random
  // misalignments
  for (int i = 0; i < 6; ++i) {
    std::cout << "m_types = " << m_types[i] << std::endl;
    switch (m_types[i]) {
      case none:
	values[i] = 0.;
	++nnone;
	break;
      case fixed:
	values[i] = m_lo[i];
	break;
      case flat:
	values[i] = m_rnd.Uniform(m_lo[i], m_hi[i]);
	break;
      case gauss:
	values[i] = m_rnd.Gaus(m_lo[i], m_hi[i]);
	break;
      default:
	// this must never happen
	throw;
    }
  }

  // if not misaligned, say so, otherwise, return object with
  // parameters
  if (6 == nnone)
    return AlignmentParameters(new AlignmentParametersImpl());
  else
    return AlignmentParameters(new AlignmentParametersImpl(values));
}
