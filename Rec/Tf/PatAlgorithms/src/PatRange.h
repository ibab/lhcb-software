// $Id: PatRange.h,v 1.1 2008-08-26 09:57:02 mschille Exp $
#ifndef PATRANGE_H
#define PATRANGE_H 1

// Include files
#include <utility>
#include <algorithm>

/// coordinate range
/** @class PatRange
 *
 * coordinate range which can be used to pass around and manipulate ranges in
 * a convenient way
 * (left endpoint included, right endpoint excluded, as with iterators)
 *
 * can of course be used for any other type of range you see fit
 *
 * @author Manuel Tobias Schiller
 * @date Aug 20 2008
 */
class PatRange : private std::pair<double, double>
{
  public:
    /// default constructor: empty range
    PatRange() : std::pair<double, double>(0.,0.) { }
    /// construct range from minmium and maximum
    PatRange(const double min, const double max) :
      std::pair<double, double>(min, max) { }
    /// copy range
    PatRange(const PatRange& other) :
      std::pair<double, double>(other) { }
    /// assignment operator
    const PatRange& operator=(const PatRange& other)
    { min() = other.min(), max() = other.max(); return *this; }

    double min() const { return first; } ///< minimum coordinate
    double max() const { return second; } ///< maximum coordinate
    double& min() { return first; } ///< minimum coordinate
    double& max() { return second; } ///< maximum coordinate
    double begin() const { return min(); } ///< begin of range
    double end() const { return max(); } ///< end of range
    bool empty() const { return min() >= max(); } ///< true if range is empty
    /// return true if x is inside range
    bool isInside(double x) const { return (min() <= x) && (x < max()); }
    /// intersect with other range (yields largest subrange contained in both)
    const PatRange& intersect(const PatRange& other)
    {
      if (min() < other.min()) min() = other.min();
      if (max() > other.max()) max() = other.max();
      return *this;
    }
    /// unite with other range (yields smallest continuous range containing both)
    const PatRange& unite(const PatRange& other)
    {
      // need to make sure that union of a range with empty range yields
      // correct result
      if (other.empty()) return *this;
      if (empty()) return *this = other;
      if (min() > other.min()) min() = other.min();
      if (max() < other.max()) max() = other.max();
      return *this;
    }

    /// widen range at both ends
    const PatRange& widen(const double tol)
    { min() -= tol, max() += tol; return *this; }

    /// translate range
    const PatRange& operator+=(const double x)
    { first += x, second += x; return *this; }
    /// translate range
    const PatRange& operator-=(const double x)
    { first -= x, second -= x; return *this; }
    /// scale range
    const PatRange& operator*=(const double x)
    {
      if (x < 0.) std::swap(first, second); first *= x, second *= x;
      return *this;
    }
    /// scale range
    const PatRange& operator/=(const double x)
    {
      if (x < 0.) std::swap(first, second); first /= x, second /= x;
      return *this;
    }

    /// translate range
    PatRange operator+(const double x) const { return PatRange(*this) += x; }
    /// translate range
    PatRange operator-(const double x) const { return PatRange(*this) -= x; }
    /// scale range
    PatRange operator*(const double x) const { return PatRange(*this) *= x; }
    /// scale range
    PatRange operator/(const double x) const { return PatRange(*this) /= x; }

    /// translate range
    const PatRange& operator+=(const PatRange& other)
    { first += other.first, second += other.second; return *this; }
    /// translate range
    const PatRange& operator-=(const PatRange& other)
    { first -= other.first, second -= other.second; return *this; }
    /// translate range
    PatRange operator+(const PatRange& other) const
    { return PatRange(*this)+=other; }
    /// translate range
    PatRange operator-(const PatRange& other) const
    { return PatRange(*this)-=other; }
};

/// intersection of two ranges
inline PatRange intersect(const PatRange& r1, const PatRange& r2)
{ return PatRange(r1).intersect(r2); }

/// union of two ranges
inline PatRange unite(const PatRange& r1, const PatRange& r2)
{ return PatRange(r1).unite(r2); }

/// return symmetric range around given center, extending for +/- width
inline PatRange symmetricRange(const double center, const double width)
{ return PatRange(center - width, center + width); }

#endif // PATRANGE_H

// vim:shiftwidth=2:tw=78
