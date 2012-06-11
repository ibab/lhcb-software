// $Id: $
#ifndef GENERATORS_EVTGENINPHSPDECAY_H 
#define GENERATORS_EVTGENINPHSPDECAY_H 1

#include "EvtGenDecay.h"

/** @class EvtGenInPhSpDecay EvtGenInPhSpDecay.h "EvtGenInPhSpDecay.h"
 *  
 *  Tool to interface to EvtGen generator. Concrete implementation of 
 *  a IDecayTool and inherits from EvtGenDecay
 *
 *  @author Alex Shires
 *  @date   2011-08-16
 */
class EvtGenInPhSpDecay : public EvtGenDecay, virtual public IDecayTool {
public:
  /// Standard constructor
  EvtGenInPhSpDecay( const std::string& type, const std::string& name,
              const IInterface* parent);

  virtual ~EvtGenInPhSpDecay( ) ; ///< Destructor

  /** Initialize method.
   *  In initialization:
   *  -# Call EvtGenDecay::initialize()
   *  -# Read the parameter file
   */
  virtual StatusCode initialize() ;

  /// Finalize method
  virtual StatusCode finalize() ;

  /// Implements IDecayTool::generateSignalDecay
  virtual StatusCode generateSignalDecay( HepMC::GenParticle * theMother , 
                                          bool & flip ) const ;

 private:

  StatusCode doEvtGenLoop( EvtParticle* &thePart , 
                           HepMC::GenParticle * &theHepMCParticle ,
                           const EvtId & eid ) const ;
  
  /// load qsquared map
  StatusCode loadMap(void ) ;
  
  /// File name of parameter file
  boost::filesystem::path m_q2mapfile ;

  std::string m_q2mapfilename ;
  std::string m_prefix ;

  // number of bins in q2
  int m_q2nbins ;
  int m_q2nmax ;

  //vector for q2 values
  std::vector<double> m_q2mapvec ;

  //statistics
  double m_tot_calls ;
  double m_num_evts ;
  double m_ave_eff ;

  int m_limit ;
 
};


#endif // GENERATORS_EVTGENDECAYINPHSP_H 1
