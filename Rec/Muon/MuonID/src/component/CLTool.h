// $Id: CLTool.h,v 1.2 2009-07-07 22:02:41 polye Exp $
#ifndef CLTOOL_H 
#define CLTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "ICLTool.h"            // Interface
#include "Interpolator.h"

/** @class CLTool CLTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-02
 */
class CLTool : public GaudiTool, virtual public ICLTool {
public: 
  /// Standard constructor
  CLTool( const std::string& type, 
          const std::string& name,
          const IInterface* parent);

  virtual ~CLTool( ); ///< Destructor

  StatusCode initialize();
  
  StatusCode cl(double value, double& cls, double& clb,
                        double& clr, double mom = 0.,int region = 0);
  
  

  StatusCode finalize() {m_init.ignore(); return GaudiTool::finalize();} //tool finalization

private:
  double minRange(){return m_minMomentum;}
  double maxRange(){return 1000000.;}
  
  enum class sb { sig, bkg };

  StatusCode findMomRange(const double& mom,int& p_r,sb sig_bkg);
  double valFromUnif(double value, double mom, int p_r, sb sig_bkg);

  
  StatusCode getClValues(sb sig_bkg);

  
  std::string m_leftRight;
  bool m_overlap;
  bool m_applyLast;

  int m_nrange;
  int m_nrangeNmuons;
  std::vector<double> m_range;
  std::vector<double> m_rangeNmuons;
  std::vector<double> m_mombinsCenter;
  std::vector<double> m_mombinsCenterNmuons;
  std::vector<double> m_yvals;
  
  int m_nvals;
  double m_minMomentum;
  double m_lbinCenter;
  double m_lbinCenterNmuons;

  std::vector<double> m_signal;
  std::vector<double> m_bkg;
  
  std::vector< std::vector<double> > m_vsignal;
  std::vector< std::vector<double> > m_vbkg;

  std::vector<Uniformer> m_unifsignal;
  std::vector<Uniformer> m_unifbkg;
  
  StatusCode m_init;

  
private:

};
#endif // CLTOOL_H
