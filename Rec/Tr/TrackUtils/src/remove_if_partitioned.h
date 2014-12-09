/**
 * @file remove_if_partitioned.h
 *
 * @author Manuel Schiller
 * @date 2014-12-09
 */

#ifndef REMOVE_IF_PARTITIONED_H
#define REMOVE_IF_PARTITIONED_H

/** remove elements if binary predicate is satisfied
 *
 * @author Manuel Schiller
 * @date 2014-11-19
 *
 * @param begin	start of range of elements to be processed
 * @param end		end of range of elements to be processed
 * @param p		binary predicate to decide which elements to remove
 *
 * The binary predicate p takes the current element being tested as first
 * argument, and an iterator pointing to the end of the range of elements
 * already processed as second argument. The second argument thus partitions
 * the range into the range of elements from begin to the second argument
 * which is known to contain elements that will not be removed, and the rest.
 *
 * This facilitates removal of equivalent elements from a range, for example:
 * @code
 * class Foo;
 * std::vector<Foo> v;
 * // assume v contains elements sorted according to some quality criterion,
 * // and Foo::equiv(const Foo&) allows to check for equivalence of some
 * // sort; then the following will keep the highest quality unique elements
 * auto newend = remove_if_partitioned(std::begin(v), std::end(v),
 *     [&v] (const Foo& el, decltype(std::begin(v)) doneend)
 *     { return doneend != std::find_if(std::begin(v), doneend,
 *           [&el] (const Foo& el2) { return el.equiv(el2); });
 *     });
 * v.erase(newend, std::end(v));
 * @endcode
 */
template <class ForwardIterator, class BinaryPredicate>
ForwardIterator remove_if_partitioned(
    ForwardIterator begin, ForwardIterator end,
    BinaryPredicate p)
{
  for (auto it = begin; end != it; ++it) {
    if (p(*it, begin)) continue;
    if (begin != it) *begin = std::move(*it);
    ++begin;
  }
  return begin;
}

#endif // REMOVE_IF_PARTITIONED_H

// vim: sw=2:tw=78:ft=cpp
