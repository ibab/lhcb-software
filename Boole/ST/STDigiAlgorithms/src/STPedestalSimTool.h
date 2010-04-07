// $Id: STPedestalSimTool.h,v 1.1 2010-04-07 09:27:38 mneedham Exp $
#ifndef STPEDESTALSIMTOOL_H
#define STPEDESTALSIMTOOL_H 1


// local interface
#include "ISTPedestalSimTool.h"

#include "GaudiAlg/GaudiTool.h"

/** @class STPedestalSimTool STPedestalSimTool.h
 *
 *  Tool for for simulating pedestal
 *
 *  @author M.Needham
 *  @date   14/3/2010
 */

class STPedestalSimTool : public GaudiTool, 
                            virtual public ISTPedestalSimTool {

public: 

  STPedestalSimTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

  /// destructer
  virtual ~STPedestalSimTool();

  virtual double pedestal(const LHCb::STChannelID& chan) const;

private:

  double pedvalue(const unsigned int i)  const;

  // Job option: param of pedestals
  std::vector<double> m_param; 

};

#endif // STCHARGESHARINGTOOL_H
