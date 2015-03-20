// $Id: FwdParameters.h,v 1.2 2009-08-19 14:16:15 ocallot Exp $
#ifndef FWDPARAMETERS_H 
#define FWDPARAMETERS_H 1

#include <string>
#include <vector>
#include <algorithm>

// Include files
// from Gaudi
#include "GaudiKernel/MsgStream.h"

#include "LinParFit.h"

/** @class FwdParameters FwdParameters.h cmt/FwdParameters.h
 *  Class to store and compute/update parameters for the Forward Tracking
 *
 *  @author Olivier Callot
 *  @date   27/11/2001
 */
class FwdParameters {
  public:
    /// Standard constructor
    FwdParameters(); 

    virtual ~FwdParameters(); ///< Destructor

    /// Initialise the parameter for computation
    void init(const std::string& title, const std::vector<double>& param);

    /// Add an event: specify delta and the functions
    void addEvent(double delta);

    /// Solve and update the parameters
    bool updateParameters(MsgStream& log);

    /// Print on cout the parameters in a jobOption format
    void printParams (const std::string& prefix);

    /// Print on cout the parameters in a python format
    void printPythonParams (const std::string& prefix);

    /// return the specified parameter
    double param(unsigned i) const  { return m_par.at(i); }

    /// set a function
    void setFun(unsigned i, double value) { m_grad.at(i) = value; }

    /// return the whole sum
    double sum() const noexcept
    { return std::inner_product(std::begin(m_par), std::end(m_par),
	std::begin(m_grad), 0.); }

  protected:

  private:
    std::string m_title;
    std::vector<double> m_par;
    std::vector<double> m_grad;
    LinParFit<double> m_fit;
};
#endif // FWDPARAMETERS_H
