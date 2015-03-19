/** @file LinParFit.h
 *
 * @brief chi^2 fit which is linear in fit parameters
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2015-03-11
 */
#ifndef _LINPARFIT_H
#define _LINPARFIT_H

#include <cmath>
#include <vector>
#include <string>
#include <limits>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "Math/CholeskyDecomp.h"

/** @brief chi^2 fit for problems linear in the fit parameters
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2015-03-04
 *
 * This class solves the linear system of equations arising from chi^2 fits
 * which are linear in the fit parameters (e.g. fitting the coefficients of a
 * polynomial). Specifically, it fits expressions where the chi^2 can be cast
 * in the following form:
 *
 * @f[\chi^2 = \sum_k \left(\frac{y_k - \sum_j p_j q_j}{\sigma_k}\right)^2 @f]
 *
 * Here, @f$p_j@f$ are the fit parameters, and @f$q_j@f$ are some
 * data-dependent quantities which do not depend on the fit parameters; for a
 * polynomial fit in a single variable @f$x@f$, one would have @f$q_j=x^j@f$,
 * for example. This formulation is rather flexible, and allows, among others,
 * fits which parameterise data points @f$y_k@f$ with polynomials in any number
 * of variables (e.g. @f$y_k\approx f(x_k, p_k) = \sum_{ij} c_{ij} x_k^i
 * p_k^j@f$).
 *
 * For simple fits which model measurements as a polynomial in one or more
 * variables, there is a simplified, less general interface available. 
 *
 * To simply fit a polynomial @f$p(x)@f$ to some measurements @f$y_k@f$ with
 * uncertainties @f$\sigma_k@f$ taken at positions @f$x_k@f$, the following
 * code will suffice:
 *
 * @code
 * LinParFit<double> fit(3); // parabola fit
 *
 * std::vector<double> y, sigma, x; // measurements, errors, x-coordinates
 * // fill x, y and sigma from somewhere
 *
 * for (auto ity = std::begin(y), its = std::begin(sigma), itx = std::begin(x);
 *         std::end(y) != ity; ++ity, ++its, ++itx) {
 *     fit.accumulate(*ity, *its, *itx);
 * }
 *
 * // print results
 * std::cout << fit << std::endl;
 * @endcode
 */
template <typename FLT = double>
class LinParFit
{
    private:
	std::vector<FLT> m_mat;	///< inverse covariance matrix (accumulated)
	std::vector<FLT> m_rhs;	///< right hand side (accumulated)
	std::vector<FLT> m_cov;	///< covariance matrix (after solve())
	std::vector<FLT> m_sol;	///< fit parameter solution (after solve())
	std::vector<std::string> m_names; ///< fit parameter names
	size_t m_npts;		///< number of data points accumulated
	size_t m_nptsSolved;	///< are m_cov/m_sol dirty? (last solve() call)

	// various little scratch areas, so we won't have to reallocate for
	// each accumulate call
	std::vector<FLT> m_wgrad;
	std::vector<size_t> m_multiidx;

    public:
	/** @brief constructor
	 *
	 * @param nparams	number of parameters in the fit
	 *
	 * Parameter names are generated according to the template "p_XXXX"
	 * where "XXXX" is replaced by the zero-padded number of the parameter;
	 * counting starts at zero.
	 */
	LinParFit(size_t nparams) :
	    m_mat((nparams * (nparams + 1)) / 2, FLT(0)),
	    m_rhs(nparams, FLT(0)),
	    m_cov((nparams * (nparams + 1)) / 2,
		    std::numeric_limits<FLT>::quiet_NaN()),
	    m_sol(nparams, std::numeric_limits<FLT>::quiet_NaN()),
	    m_npts(0), m_nptsSolved(-1)
        {
	    if (0 == nparams)
		throw std::invalid_argument("number of parameters must be positive");
	    // generate parameter names p_0000 and so forth
	    std::ostringstream tmpbuf;
	    for (unsigned i = 0; i < nparams; ++i) {
		tmpbuf << "p_" << std::setw(4) << std::setfill('0') << i;
		m_names.push_back(tmpbuf.str());
		tmpbuf.str("");
	    }
	}

	/** @brief constructor
	 *
	 * @param parnames	vector of names for parameters
	 *
	 * Constructs a LinParFit with as many parameters as there are names in
	 * parnames.
	 */
	LinParFit(const std::vector<std::string>& parnames) :
	    m_mat((parnames.size() * (parnames.size() + 1)) / 2, FLT(0)),
	    m_rhs(parnames.size(), FLT(0)),
	    m_cov((parnames.size() * (parnames.size() + 1)) / 2,
		    std::numeric_limits<FLT>::quiet_NaN()),
	    m_sol(parnames.size(), std::numeric_limits<FLT>::quiet_NaN()),
	    m_names(parnames), m_npts(0), m_nptsSolved(-1)
        {
	    if (0 == parnames.size())
		throw std::invalid_argument("number of parameters must be positive");
	}

	/// clear (reset) fit to initial state
	void clear() noexcept
	{
	    std::fill(std::begin(m_mat), std::end(m_mat), FLT(0));
	    std::fill(std::begin(m_rhs), std::end(m_rhs), FLT(0));
	    std::fill(std::begin(m_cov), std::end(m_cov), std::numeric_limits<FLT>::quiet_NaN());
	    std::fill(std::begin(m_sol), std::end(m_sol), std::numeric_limits<FLT>::quiet_NaN());
	    m_npts = 0;
	    m_nptsSolved = -1;
	}

	/// return the number of data points accumulated so far
	size_t npoints() const noexcept
	{ return m_npts; }

	/// return the number of degrees of freedom (npoints() - number of parameters)
	ssize_t ndf() const noexcept
	{ return m_npts - m_rhs.size(); }

	/** @brief accumulate a new data point
	 *
	 * @param wy	"weighted" measured value (@f$y_k/\sigma_k@f$)
	 * @param wgrad	"weighted" gradient @f$(q_0, \ldots, q_n)^T/\sigma_k@f$
	 *
	 * @tparam FLTVECT	type of floating point vector used for wgrad
	 *
	 * This routine accumulates a new data point into the fit.
	 */
	template <typename FLTVECT>
	void accumulate(FLT wy, const FLTVECT& wgrad) noexcept(
		noexcept(wgrad[0]))
	{
	    if (wgrad.size() != m_rhs.size()) {
		throw std::invalid_argument("dimensions of wgrad "
			"must match number of parameters.");
	    }
	    for (unsigned i = 0; i < m_rhs.size(); ++i) {
		m_rhs[i] += wy * wgrad[i];
		for (unsigned j = 0; j <= i; ++j)
		    m_mat[(i * (i + 1)) / 2 + j] += wgrad[i] * wgrad[j];
	    }
	    ++m_npts;
	}

	/** @brief accumulate new data point (simplified interface)
	 *
	 * @param y		measurement
	 * @param sigma		error on measurement
	 * @param x		variable of polynomial
	 *
	 * This fits the coefficients of a one-dimensional polynomial p(x) to a
	 * set of measurements (y(x), sigma).
	 */
	void accumulate(FLT y, FLT sigma, FLT x) noexcept
	{
	    typedef struct {
		FLT m_x, m_sigma;
		size_t m_sz;

		size_t size() const noexcept { return m_sz; }

		FLT operator[](unsigned k) const noexcept
		{ return std::pow(m_x, k++) / m_sigma; }
	    } wgrad_helper;
	    accumulate(y / sigma, wgrad_helper{x, sigma, m_rhs.size()});
	}

	/** @brief accumulate new data point (simplified interface)
	 *
	 * @param y		measurement
	 * @param sigma		error on measurement
	 * @param x		vector of variables in polynomial (length N)
	 * @param maxdeg	maximal degree for each variable in polynomial
	 * @param totmaxdeg	total maximal degree for terms in polynomial
	 *
	 * This routine fits a polynomial to a set of data points; the
	 * polynomial can be a polynomial in more than one variable. The
	 * function minimised can be written as:
	 *
	 * @f[ \chi^2 = \sum_k \left(
	 * \frac{
	 * y_k - \sum_{
	 * \begin{array}{c}
	 * j_0=0,1,\ldots, n_0 \\
	 * j_1=0,1,\ldots, n_1 \\
	 * \ldots \\
	 * j_{N-1}=0,1,\ldots, n_{N-1} \\
	 * j_0+j_1+\ldots+j_N \le M \\
	 * \end{array}} c_{j_0, j_1, \ldots, j_{N-1}} \cdot
	 * (x_0^{j_0})_k \cdot
	 * (x_1^{j_1})_k \cdots
	 * (x_{N-1}^{j_{N-1}})_k
	 * }{ \sigma_k}\right)^2 @f]
	 *
	 * Here, the index k labels the individual measurements @f$y_k@f$ with
	 * uncertainties @f$\sigma_k@f$. The polynomial is a polynomial in N
	 * different variables @f$x_0,x_1,\ldots,x_{N-1}@f$. The degree of the
	 * polynomial is limited to @f$n_k@f$ for the variable @f$x_k@f$, and
	 * the @f$n_k@f$ are given in the vector maxdeg. The total degree of
	 * the polynomial M, i.e. the maximal total power @f$j_0 + j_1 + \ldots
	 * + j_{N-1}@f$ of terms @f$x_0^{j_0}\cdot x_1^{j_1}\cdots
	 * x_{N-1}^{j_{N-1}}@f$, is limited to be less than or equal to
	 * totmaxdeg.
	 *
	 * The default is to not limit totmaxdeg, so that all variables run up
	 * to the degree specified in maxdeg. Specifying totmaxdeg can
	 * nevertheless be useful: Consider an example where you want to fit
	 * with a polynomial in two variables up to second order terms, but you
	 * want to keep the "mixed" terms to second order as well (i.e.
	 * monomials @f$1, x_0, x0_2, x_1, x_1^2, x_0 x_1@f$, but not e.g.
	 * @f$x_0^2x_1@f$). Then you would specify maxdeg as vector { 2, 2 },
	 * and also totmaxdeg = 2. The code to perform the fit described in
	 * last paragraph is given below:
	 *
	 * @code
	 * const std::vector<size_t> maxdeg{2, 2};
	 *
	 * std::vector<double> y, sigma, x0, x1;
	 * // fill y, sigma, x0, x1 somehow...
	 *
	 * LinParFit<double> fit(6);
	 *
	 * std::vector<double> xv(2, 0.);
	 * for (unsigned k = 0; y.size() != k; ++k) {
	 *     xv[0] = x0[k], xv[1] = x1[k];
	 *     fit.accumulate(y[k], sigma[k], xv, maxdeg, 2);
	 * }
	 *
	 * // print fit result
	 * std::cout << fit << std::endl;
	 * @endcode
	 */
	void accumulate(FLT y, FLT sigma, const std::vector<FLT>& x,
		const std::vector<size_t>& maxdeg, size_t totmaxdeg = -1)
	{
	    /// verify input arguments
	    if (x.empty()) {
		throw std::invalid_argument("need at least one variable in x");
	    } else if (x.size() != maxdeg.size()) {
		throw std::invalid_argument(
			"lengths of vectors x and maxdeg must match");
	    }
	    // use a "multiindex" to enumerate all terms in polynomial
	    m_multiidx.clear(); m_multiidx.assign(maxdeg.size(), size_t(0));
	    // need a way to enumerate all multiindices
	    const auto next = [this, &maxdeg, totmaxdeg] () {
	       	const auto begidx = std::begin(m_multiidx);
		const auto endidx = std::end(m_multiidx);
		auto max = std::begin(maxdeg);
		auto idx = begidx;
		while (endidx != idx) {
		    // increment lowest index
		    ++(*idx);
		    // see if we have a valid multiindex
		    if (*max >= *idx && totmaxdeg >=
			    std::accumulate(begidx, endidx, size_t(0))) {
			return true;
		    }
		    // no, have to increment at index idx + 1, so zero lower
		    // order indices
		    ++idx, ++max;
		    std::fill(begidx, idx, size_t(0));
		}
		// no valid multiindices left, signal end of iteration
		return false;
	    };
	    // okay, compute the various terms in polynomial
	    m_wgrad.clear(); m_wgrad.reserve(m_rhs.size());
	    do {
		// q = x_0^multiidx[0] * x_1^multiidx[1] * ...
		const FLT q = std::inner_product(std::begin(x), std::end(x),
			std::begin(m_multiidx), FLT(1),
			[] (FLT x, FLT y) { return x * y; },
			[] (FLT x, size_t k) { return std::pow(x, k); });
		m_wgrad.push_back(q / sigma);
	    } while(next());
	    if (m_wgrad.size() != m_rhs.size()) {
		throw std::invalid_argument("degree of polynomial does "
			"not match declared number of parameters");
	    }
	    // okay, we have all the ingredients now to accumulate the data
	    // point
	    accumulate(y / sigma, m_wgrad);
	}

	/** @brief Solve the fit
	 *
	 * @returns true if successful, false if resulting matrix is not
	 * positive definite.
	 */
	bool solve() noexcept
	{
	    if (m_nptsSolved == m_npts) return !std::isnan(m_cov[0]);
	    ROOT::Math::CholeskyDecompGenDim<FLT> decomp(m_rhs.size(), &m_mat[0]);
	    if (!decomp) {
		std::fill(std::begin(m_cov), std::end(m_cov),
			std::numeric_limits<FLT>::quiet_NaN());
		std::fill(std::begin(m_sol), std::end(m_sol),
			std::numeric_limits<FLT>::quiet_NaN());
		return false;
	    }
	    std::copy(std::begin(m_rhs), std::end(m_rhs), std::begin(m_sol));
	    decomp.Solve(m_sol);
	    decomp.Invert(&m_cov[0]);
	    m_nptsSolved = m_npts;
	    return true;
	}

	/// return if solving the fit was successful
	operator bool() noexcept
	{
	    if (m_nptsSolved == m_npts) return !std::isnan(m_cov[0]);
	    else return solve();
	}

	/// return the vector of fitted parameters
	const std::vector<FLT>& solution()
	{
	    if (!solve())
		throw std::domain_error("matrix not positive definite");
	    return m_sol;
	}
	
	/** @brief return the (packed) covariance matrix
	 *
	 * @returns	flat vector containing the lower half of the
	 * 		(symmetric) covariance matrix
	 *
	 * Indices in the packed storage format are assigned in reading order.
	 * The scheme is as follows:
	 *
	 *  0
	 *  1  2
	 *  3  4  5
	 *  6  7  8  9
	 * 10 11 12 13 14
	 * ...           ...
	 */
	const std::vector<FLT>& covariance()
	{
	    if (!solve())
		throw std::domain_error("matrix not positive definite");
	    return m_cov;
	}

	/// return a vector of parameter names
	const std::vector<std::string>& names() const noexcept
	{ return m_names; }

	/// access the i-th fit parameter
	FLT operator[](unsigned idx)
	{
	    if (!solve())
		throw std::domain_error("matrix not positive definite");
	    return m_sol.at(idx);
	}

	/// access the (i, j) element of the covariance matrix
	FLT operator()(unsigned i, unsigned j)
	{
	    if (!solve())
		throw std::domain_error("matrix not positive definite");
	    if (i < j) std::swap(i, j);
	    return m_cov.at((i * (i + 1)) / 2 + j);
	}

	/// access the correlation coefficient @f$\rho_{ij}@f$
	FLT correl(unsigned i, unsigned j)
	{
	    if (!solve())
		throw std::domain_error("matrix not positive definite");
	    if (i < j) std::swap(i, j);
	    return m_cov.at((i * (i + 1)) / 2 + j) /
		std::sqrt(m_cov.at((i * (i + 1)) / 2 + i) *
			m_cov.at((j * (j + 1)) / 2 + j));
	}

	/// represent the result of a fit as string
	operator std::string()
	{
	    bool okay = solve();
	    std::ostringstream tmpbuf;
	    tmpbuf << "FIT RESULT: FIT " << (okay ? "CONVERGED" :
		    "FAILED, COVARIANCE MATRIX NOT POS. DEF.") <<
		" NDF = " << std::setw(5) << ndf() << std::endl;
	    if (!okay) return tmpbuf.str();
	    tmpbuf << std::endl;
	    // figure out width of parameter column
	    size_t namew = 9;
	    for (const auto& n: m_names) namew = std::max(namew, n.size());
	    const size_t fltp = std::numeric_limits<FLT>::max_digits10;
	    const size_t fltw = 7 + fltp;
	    tmpbuf << std::setw(4) << std::right << "NR" << " " <<
		std::setw(namew) << std::left << "PARAMETER" << " " <<
		std::setw(fltw) << std::setprecision(fltp) << std::scientific <<
		std::right << "VALUE" << " +/- " <<
		std::setw(fltw) << std::setprecision(fltp) << std::scientific <<
		std::right << "ERROR" << std::endl;

	    for (unsigned i = 0; i != m_sol.size(); ++i) {
		tmpbuf << std::setw(4) << std::right << i << " " <<
		    std::setw(namew) << std::left << m_names[i] << " " <<
		    std::setw(fltw) << std::setprecision(fltp) << std::scientific <<
		    std::right << m_sol[i] << " +/- " <<
		    std::setw(fltw) << std::setprecision(fltp) << std::scientific <<
		    std::right << std::sqrt(m_cov[(i * (i + 1)) / 2 + i]) <<
		    std::endl;
	    }
	    tmpbuf << std::endl << "CORRELATION MATRIX" << std::endl <<
		std::setw(4) << "";
	    for (unsigned i = 0; i != m_sol.size(); ++i)
		tmpbuf << " " << std::setw(6) << i;
	    tmpbuf << std::endl;
	    for (unsigned i = 0; i != m_sol.size(); ++i) {
		tmpbuf << std::setw(4) << i;
		for (unsigned j = 0; j <= i; ++j) {
		    tmpbuf << std::setw(6) << std::setprecision(3) << std::fixed <<
			std::right << (m_cov[(i * (i + 1)) / 2 + j] /
				std::sqrt(m_cov[(i * (i + 1)) / 2 + i] *
				    m_cov[(j * (j + 1)) / 2 + j]));
		}
		tmpbuf << std::endl;
	    }
	    
	    return tmpbuf.str();
	}
};

/// output of fit to some kind of output stream (e.g. std::cout << fit;)
template <typename OS, typename FLT>
OS& operator<<(OS& os, LinParFit<FLT>& fit)
{ os << std::string(fit); return os; }

#endif // _LINPARFIT_H

// vim: sw=4:tw=78:ft=cpp
