/** @file PolyEval.h
 *
 * @brief evaluate polynomials with given coefficients
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2015-02-04
 */

#ifndef POLYEVAL_H
#define POLYEVAL_H

#include <cstddef>
#include <type_traits>

namespace PolyEval { ///< routines to evaluate polynomials
    namespace PolyEvalImpl { ///< implementation details
	/// check if range or container T has a size() method
	template <typename T>
	class has_size {
	    private:
		typedef char Yes;
		typedef Yes No[2];

		template<typename C> static auto Test(void*)
		    -> decltype(size_t{std::declval<C const>().size()}, Yes{});

		template<typename> static No& Test(...);

	    public:
		static constexpr bool const value = sizeof(Test<T>(nullptr)) == sizeof(Yes);
	};

	/// check if iterator type T is a random access iterator
	template <typename T>
	struct is_random_access_iterator :
	    std::is_same<typename std::iterator_traits<T>::iterator_category,
	    std::random_access_iterator_tag>
	{};
    }

    /** @brief evaluate polynomial with given coefficients
     *
     * @param x	x at which to evaluate @f$p(x)=\sum_{k=0}^{n - 1} c_k x^k@f$
     * @param begin	iterator pointing to @f$c_0@f$
     * @param end	iterator pointing one element behind @f$c_{n - 1}@f$
     *
     * @returns @f$p(x)=\sum_{k=0}^{n - 1} c_k x^k@f$
     *
     * This code will be optimised well by current and future compilers: The code
     * will sum up to eight terms without branch instructions in the assembly by
     * using a computed goto or a jump table in the guise of a switch statement.
     * Care is taken that machines with a fused multiply-and-add instruction (FMA)
     * can take advantage of their capability.
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-04
     */
    template<class FLT, class IT>
    inline FLT eval(const FLT x, const IT begin, const IT end) noexcept
    {
	// a smart compiler will turn the switch statement in the loop
	// below into a computed goto into a hand-unrolled loop; for short
	// polynomials, this results in two compares and a computed goto -
	// one cannot do much better when n is not known at compile time;
	// also be friendly to processors supporting a fused
	// multiply-and-add (FMA) instruction by formulating things that
	// way
	FLT sum = 0;
	for (auto it = end; begin != it--;) {
	    const size_t rem = std::distance(begin, it) & 7;
	    it -= rem;
	    switch (rem) {
		case 7: sum = sum * x + *(it + 7); // fallthrough intended
		case 6: sum = sum * x + *(it + 6); // fallthrough intended
		case 5: sum = sum * x + *(it + 5); // fallthrough intended
		case 4: sum = sum * x + *(it + 4); // fallthrough intended
		case 3: sum = sum * x + *(it + 3); // fallthrough intended
		case 2: sum = sum * x + *(it + 2); // fallthrough intended
		case 1: sum = sum * x + *(it + 1); // fallthrough intended
		case 0: sum = sum * x + *(it + 0); break;
	    }
	};
	return sum;
    }

    /** @brief evaluate polynomial with given coefficients
     *
     * @param x	x at which to evaluate @f$p(x)=\sum_{k=0}^{n - 1} c_k x^k@f$
     * @param n	number of coefficients
     * @param c	pointer to first coefficient
     *
     * @returns @f$p(x)=\sum_{k=0}^{n - 1} c_k x^k@f$
     *
     * This code will be optimised well by current and future compilers: If the
     * number of coefficients is known at compile time, this produces branchless
     * code. If the number of coefficients is only known at run time, the code
     * will sum up to eight terms without branch instructions in the assembly by
     * using a computed goto or a jump table in the guise of a switch statement.
     * Care is taken that machines with a fused multiply-and-add instruction (FMA)
     * can take advantage of their capability.
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-04
     */
    template<class FLT, class SZ>
    inline FLT eval(const FLT x, SZ n, const FLT* c) noexcept 
    {
	// a smart compiler will turn the switch statement in the loop
	// below into a computed goto into a hand-unrolled loop; for short
	// polynomials, this results in three compares and a computed goto -
	// one cannot do much better when n is not known at compile time;
	// also be friendly to processors supporting a fused
	// multiply-and-add (FMA) instruction by formulating things that
	// way.
	// The optimisation of pre-loading sum with the highest order
	// coefficient is there mainly in case the number of coefficients is
	// known at compile time (and most compilers will produce branchless
	// code with no superfluous operations). It costs a single compare and
	// branch at the beginning when the number of coefficients is only
	// known at run time.
	if (n--) {
	    FLT sum = c[n];
	    while (n--) {
		const SZ rem = n & 7;
		n -= rem;
		switch (rem) {
		    case 7: sum = sum * x + c[n + 7]; // fallthrough intended
		    case 6: sum = sum * x + c[n + 6]; // fallthrough intended
		    case 5: sum = sum * x + c[n + 5]; // fallthrough intended
		    case 4: sum = sum * x + c[n + 4]; // fallthrough intended
		    case 3: sum = sum * x + c[n + 3]; // fallthrough intended
		    case 2: sum = sum * x + c[n + 2]; // fallthrough intended
		    case 1: sum = sum * x + c[n + 1]; // fallthrough intended
		    case 0: sum = sum * x + c[n + 0]; break;
		}
	    };
	    return sum;
	}
	return static_cast<FLT>(0);
    }

    /** @brief evaluate polynomial with given coefficients
     *
     * @param x	x at which to evaluate @f$p(x)=\sum_{k=0}^{n - 1} c_k x^k@f$
     * @param c	container or range with coefficients
     *
     * @returns @f$p(x)=\sum_{k=0}^{n - 1} c_k x^k@f$
     *
     * This code will be optimised well by current and future compilers: If the
     * number of coefficients is known at compile time, this produces branchless
     * code. If the number of coefficients is only known at run time, the code
     * will sum up to eight terms without branch instructions in the assembly by
     * using a computed goto or a jump table in the guise of a switch statement.
     * Care is taken that machines with a fused multiply-and-add instruction (FMA)
     * can take advantage of their capability.
     *
     * @note DO NOT CALL THIS ROUTINE WITH CONTAINERS OR RANGES WHICH DO NOT STORE
     * ELEMENTS IN A CONTIGUOUS CHUNK OF MEMORY. The assumption here is that
     * ranges or containers which have random access iterators and a size() method
     * are suitable for this function, but until C++ gives us some way to check
     * for contiguous storage layout in containers, there is very little that
     * can be done to protect against users of really strange containers.
     * std::vector<FLT> and std::array<FLT> are safe unless FLT happens to be
     * bool.
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-04
     */
    template<class FLT, class C>
    inline typename std::enable_if<(PolyEvalImpl::has_size<C>::value &&
	    PolyEvalImpl::is_random_access_iterator<decltype(
		std::begin(C()))>::value), FLT>::type eval(
	    const FLT x, const C& coeffs) noexcept
    { return eval(x, coeffs.size(), &*std::begin(coeffs)); }

    /** @brief evaluate polynomial with given coefficients
     *
     * @param x	x at which to evaluate @f$p(x)=\sum_{k=0}^{n - 1} c_k x^k@f$
     * @param c	container or range with coefficients
     *
     * @returns @f$p(x)=\sum_{k=0}^{n - 1} c_k x^k@f$
     *
     * This code will be optimised well by current and future compilers: If the
     * number of coefficients is known at compile time, this produces branchless
     * code. If the number of coefficients is only known at run time, the code
     * will sum up to eight terms without branch instructions in the assembly by
     * using a computed goto or a jump table in the guise of a switch statement.
     * Care is taken that machines with a fused multiply-and-add instruction (FMA)
     * can take advantage of their capability.
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-04
     */
    template<class FLT, class C>
    inline typename std::enable_if<!(PolyEvalImpl::has_size<C>::value &&
	    PolyEvalImpl::is_random_access_iterator<decltype(
		std::begin(C()))>::value), FLT>::type eval(
	    const FLT x, const C& coeffs) noexcept
    { return eval(x, std::begin(coeffs), std::end(coeffs)); }
}

#endif // POLYEVAL_H

// vim: sw=4:tw=78:ft=cpp
