// $Id: VeloMonitorBase.h,v 1.2 2008-08-19 17:08:15 erodrigu Exp $
#ifndef VELORECMONITORS_VELOMONITORBASE_H 
#define VELORECMONITORS_VELOMONITORBASE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

/** @class VeloMonitorBase VeloMonitorBase.h Velo/VeloMonitorBase.h
 *  
 *  Header file for the VELO reconstruction monitoring
 *  algorithm base class
 *
 *  @author Eduardo Rodrigues
 *  @date   2008-08-15
 */

namespace Velo
{

  class VeloMonitorBase : public GaudiHistoAlg {

  public: 

    /// Standard constructor
    VeloMonitorBase( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloMonitorBase( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  protected:

    // pointer to the VELO detector element
    DeVelo* m_veloDet;

    // flag for DEBUG level
    bool m_debugLevel;

  private:

  };

} // end of Velo namespace

#endif // VELORECMONITORS_VELOMONITORBASE_H
