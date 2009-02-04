#ifndef RUNCHANGEINCIDENT_H_
#define RUNCHANGEINCIDENT_H_

#include "GaudiKernel/Incident.h"
 
namespace IncidentType
{
  const std::string RunChange = "RunChange"; ///< Change of run number
}

/** @class RunChangeIncident
 *
 *  Specialized incident class for run number change incidents.
 *
 */
class RunChangeIncident: public Incident {
public:
  /// Constructor
  RunChangeIncident(const std::string& source, // Source(service or alg) name)
                    unsigned long run          // New run number
                    ): Incident(source, IncidentType::RunChange), m_run(run) { }
  
  /// Get the new run number
  inline unsigned long runNumber() const 
  {
    return m_run;
  }

  /// Virtual destructor.
  virtual ~RunChangeIncident();

private:
  /// New run number
  unsigned long m_run;
};

#endif /*RUNCHANGEINCIDENT_H_*/
