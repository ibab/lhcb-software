// $Id: CircularRangeUtils.h,v 1.1.1.1 2007-08-26 21:03:29 krinnert Exp $
#ifndef INCLUDE_TF_CIRCULARRANGEUTILS_H
#define INCLUDE_TF_CIRCULARRANGEUTILS_H 1

#include <utility>

namespace Tf {
  /** @class CircularRangeUtils CircularRangeUtils.h
   * Utilities for dealing with circular ranges.
   *
   * The most common circular range application are angles.  The are frewuently
   * mapped to either [-pi,pi] or [0,2pi].  However, this class supports circular
   * ranges in a more generic way. 
   *
   * A specific range is defined as a std::pair<T,T>.  This class supports
   * operations on these ranges: mapping T to the interval specified when
   * construction the utility, checking wether a T value is in a range, overlap
   * of ranges and so on.  
   *
   * With the exception of the mapToInterval() methods, all ranges and values
   * passed a parameters are expected to be properly mapped into the circular
   * interval.  A range implies a direction.  It is always interpreted as the
   * interval between first and second when going from first in the direction
   * of the circular interval maximum.  This implies that any range for which
   * second < first contains the boundary of the circular interval.
   *   
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-23
   */
  template<typename T>
  class CircularRangeUtils {

    public:

      typedef std::pair<T,T> Range;
      
    public:

      /// Construct with given circular interval
      CircularRangeUtils(const T& min, const T& max)
        : m_min(min<max ? min : max)
        , m_max(min<max ? max : min)
        , m_module(m_max-m_min)
        , m_halfModule(m_module/2)
        {;}

      /// Map a value to the circular interval
      void mapToInterval(T& value) const {
        while (m_max < value) value -= m_module;
        while (value < m_min) value += m_module;
      }
      
      /// Map a range to the circular interval
      void mapToInterval(Range& range) const {
        while (m_max < range.first)  range.first  -= m_module;
        while (range.first < m_min)  range.first  += m_module;
        while (m_max < range.second) range.second -= m_module;
        while (range.second < m_min) range.second += m_module;
      }
      
      /// Check whehter a range contains the circular baundary
      bool containsBoundary(const Range& range) const {
        return range.second  < range.first;
      }
      
      /// Check whether value is in range
      bool contains(const Range& range, const T& value) const {
        return (containsBoundary(range) && ((range.first<value) || ( value < range.second))) 
          || ((range.first < value) && (value < range.second));
      }

      /// Check whether the first range contains the entire second range
      bool contains(const Range& container, const Range& containee) const {
        return (contains(container,containee.first)) && (contains(container,containee.second));
      }
      
      /// Check whether value +/- tolerance is in range
      bool contains(const Range& range, const T& value, const T& tolerance) const {
        Range testRange(sub(value,tolerance),add(value,tolerance));
        return overlap(range,testRange);
      }

      /// Compute the overlap between to ranges with added tolerance. Returns false if the two ranges do not overlap.
      bool overlap (const Range& range1, const Range& range2, const T& tolerance, Range& overlapRange) const {
        Range testRange(sub(range1.first,tolerance),add(range1.second,tolerance));
        return overlap(testRange,range2,overlapRange);
      }
      
      /// Compute the overlap between two ranges. Returns false if the two ranges do not overlap.
      bool overlap(const Range& range1, const Range& range2, Range& overlapRange) const {
       overlapRange = range1;
       if (contains(range2,range1) || (range1 == range2) ) { return true; } 
       if (contains(range1,range2))                        { overlapRange = range2; return true; }
       if (contains(range1,range2.first))                  { overlapRange.first  = range2.first ; return true; }
       if (contains(range1,range2.second))                 { overlapRange.second = range2.second; return true; }
       return false;
      }

      /// Check whether two ranges overlap without computing the overlap range
      bool overlap(const Range& range1, const Range& range2) const {
        return contains(range2,range1) || (contains(range1,range2.first)) || (contains(range1,range2.second)) || (range1 == range2);
      }
      
      /** Compute the union of two ranges. 
       * The union is the largest range that can be constructed from
       * the input range boundaries.
       * This only makes sense if the ranges overlap, so we
       * return false if the two ranges do not overlap.
       * However, the union range will still be a valid range in this
       * case, namely the first input range1.
       */
      bool rangeUnion(const Range& range1, const Range& range2, Range& unionRange) const {
       unionRange = range1; 
       if (contains(range2,range1))        { unionRange = range2; return true; } 
       if (contains(range1,range2))        { return true; }
       if (contains(range1,range2.first))  { unionRange.second = range2.second; return true; }
       if (contains(range1,range2.second)) { unionRange.first  = range2.first ; return true; }
       return false;
      }

      /// Compute absolute shortest distance between two values.  This is NOT a value inside the circular interval. It has no direction!
      T distance(const T& value1, const T& value2) const {
        T dist = value1 < value2 ? value2-value1 : value1-value2;
        return m_halfModule < dist ? dist-m_halfModule : dist;
      }
      
      /// Add two values. The result is properly mapped into the circular interval.
      T add(const T& lhs, const T& rhs) const {
        T sum = lhs+rhs;
        mapToInterval(sum);
        return sum;
      }

      /// Substract two values. The result is properly mapped into the circular interval.
      T sub(const T& lhs, const T& rhs) const {
        T diff = lhs-rhs;
        mapToInterval(diff);
        return diff;
      }
      
      /** Compute the weighted mean of two values.  
       * This results in a value betwween the two input values along the
       * shortest path connecting the two in the circular interval.
       * The two weight values should add up to the neutral element of the
       * SCALAR*T operation.  Otherwise it is not guaranteed that the result
       * is between the two values.
       * The result is properly mapperd to the circular interval.
       */
      template<typename SCALAR>
        T mean(const T& value1, const SCALAR& weight1, const T& value2, const SCALAR& weight2) const {
          T mean = m_halfModule < value2-value1 
            ? weight1*(value1+m_module) + weight2*value2 
            : ( m_halfModule < value1-value2 ? weight1*(value1-m_module) + weight2*value2 : weight1*value1 + weight2*value2);
          mapToInterval(mean);
          return mean; 
        }
      
    private:

      T m_min;
      T m_max;
      T m_module;
      T m_halfModule;

  };
}
#endif // TF_INCLUDE_CIRCULARRANGEUTILS_H

