#ifndef GAUCHO_GAUCHOHISTOALG_H
#define GAUCHO_GAUCHOHISTOALG_H 1

#include "GaudiAlg/GaudiHistoAlg.h"
#include "Gaucho/IGauchoMonitorSvc.h"

/** @class GauchoHistoAlg
    Trivial class to override the declareInfo method in Algorithm.
    We use here the interface IGauchoMonitorSvc which has extended 
    declareInfo method to suppport MonObjects
    @author Juan Martin Otalora Goicochea, 04/06/2008
    
*/

class GauchoHistoAlg : public GaudiHistoAlg {
protected:

  /** Standard constructor (protected)
   *  @see  GauchoHistAlg
   *  @see  GaudiAlgorithm
   *  @see       Algorithm
   *  @param name           name of the algorithm
   *  @param pSvcLocator    pointer to Service Locator
   */
  GauchoHistoAlg ( const std::string& name,
                  ISvcLocator*       pSvc );

  /** @brief Access the monitor service
   *   
   *   @attention Note that this method will return a NULL pointer if no monitor service is
   *              configured to be present. You must take this possibility into account when
   *              using the pointer
   *   @return Pointer to the Monitor service
   *   @retval NULL No monitor service is present
   *   @retval non-NULL A monitor service is present and available to be used
   */
  inline IGauchoMonitorSvc* monitorSvc() const
  {
    // If not already located try to locate it without forcing a creation
    if ( !m_pGauchoMonitorSvc ){
      service_i( "MonitorSvc", false, 
                 IGauchoMonitorSvc::interfaceID(), pp_cast<void>(&m_pGauchoMonitorSvc) );
    
    }
    return m_pGauchoMonitorSvc;
  }
  
  /** Declare monitoring information
      @param name Monitoring information name known to the external system
      @param var  Monitoring Listener address (the item to monitor...)
      @param desc Textual description of the information being monitored
  */
  template <class T>
  void declareInfo( const std::string& name,
                    const T& var,
                    const std::string& desc ) const 
  {
    IGauchoMonitorSvc* mS = monitorSvc();
    if ( mS ) mS->declareInfo(name, var, desc, this);
  }

  /// virtual destructor
  virtual ~GauchoHistoAlg( );

  /** standard initialization method
   *  @see  Algorithm
   *  @see      IAlgorithm
   *  @return status code
   */
  virtual StatusCode initialize ();

  /** standard finalization method
   *  @see       Algorithm
   *  @see      IAlgorithm
   *  @return status code
   */
  virtual StatusCode finalize  ();

private:

  mutable IGauchoMonitorSvc *m_pGauchoMonitorSvc;
  
  // default constructor  is disabled
  GauchoHistoAlg ();
  // copy constructor     is disabled
  GauchoHistoAlg           ( const  GaudiHistoAlg& );
  // assignment operator is disabled
  GauchoHistoAlg& operator=( const  GaudiHistoAlg& );

  // This is because the service_i method in Algorith is private..
  StatusCode service_i(const std::string& svcName,
                       bool createIf,
                       const InterfaceID& iid,
                       void** ppSvc) const;  
  
};

#endif    // GAUCHO_GAUCHOHISTOALG_H

