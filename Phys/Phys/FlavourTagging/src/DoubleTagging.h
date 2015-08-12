// $Id: DoubleTagging.h,v 1.2 2006-10-24 10:16:44 jpalac Exp $
#ifndef DOUBLETAGGING_H
#define DOUBLETAGGING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"

#include "ICombineTaggersTool.h"
#include "Kernel/IBTaggingTool.h"


/** @class DoubleTagCorrelation DoubleTagCorrelation.h
 *
 *  Algorithm to evaluate OS/SS tag correlation
 *  @author Chris Barnes
 *  @date   2006-01-28
 */

class DoubleTagging : public DaVinciAlgorithm {

public: 
  /// Standard constructor
  DoubleTagging( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DoubleTagging( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  std::string m_TagLocation;
  std::string m_TagLocationOS;
  
  ICombineTaggersTool* m_oscombine;
  
  //category 1
  double m_Wm;
  double m_SigmaWm;
  //category 2
  double m_We;
  double m_SigmaWe;
  //category 3
  double m_Wosk;
  double m_SigmaWosk;
  //category 4
  double m_Woskm;
  double m_SigmaWoskm;
  //category 5
  double m_Woske;
  double m_SigmaWoske;
  //category 6
  double m_Wv;
  double m_SigmaWv;
  //combination
  double m_Wcomb;
  double m_SigmaWcomb;

  int m_ntotal;
  int m_ndt_total;
  int m_nag_total;
  
  size_t ncategories;
  std::vector<double> inputW;
  std::vector<double> inputSigmaW;
  std::vector<int> ndoubletagged;
  std::vector<int> nagree;
  std::vector<double> Wk;
  std::vector<double> SigmaWk;
  
  double calculateWk(int ndt,int nag,double Wtagger);
  double calculateSigmaWk(int ndt,int nag,double Wtagger,double SigmaWtagger);
  void   combineMeasurements(std::vector<double>& Wks, std::vector<double>& SigmaWks, double& Wktot, double& SigmaWktot);

  void printInfo(int i);
};
#endif // DOUBLETAGGING_H
