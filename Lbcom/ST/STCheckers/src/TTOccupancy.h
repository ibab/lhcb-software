// $Id: TTOccupancy.h,v 1.8 2010-04-05 09:49:19 mneedham Exp $
#ifndef TTOccupancy_H
#define TTOccupancy_H 1

#include "Kernel/STHistoAlgBase.h"


/** @class TTOccupancy TTOccupancy.h
 *
 *  Class for plotting the occupancy of TTDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */


template <class PBASE >
class TTOccupancy : public ST::HistoAlgBase {

public:
 
  /// constructer
  TTOccupancy( const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~TTOccupancy();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  const std::string& dataLocation() const;
 
  const std::string histoDirName() const;

  double defaultThreshold() const;


  void fillHistograms(const PBASE* obj);

  unsigned int weight(const PBASE* obj) const; 
  
  double SN(const PBASE* obj) const;

 
  int m_nBins; 
  int m_hMax;  

  // job options
  std::vector<double> m_threshold;   ///< List of threshold values

  int m_binSize;                     ///< Number of channels in each bin
  std::string m_dataLocation;        ///< Location of the digits

  std::string m_histoLocation;

};

#endif // TTOccupancy_H
