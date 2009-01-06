// $Id: TTOccupancy.h,v 1.7 2009-01-06 13:47:56 mneedham Exp $
#ifndef TTOccupancy_H
#define TTOccupancy_H 1

#include "Kernel/STHistoAlgBase.h"

class ISTSignalToNoiseTool;

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
  
  ISTSignalToNoiseTool* m_sigNoiseTool;
 
  int m_nBins; 
  int m_hMax;  

  // job options
  std::vector<double> m_threshold;   ///< List of threshold values
  std::string m_sigNoiseToolName;    ///< Name of the S/N tool 
  int m_binSize;                     ///< Number of channels in each bin
  std::string m_dataLocation;        ///< Location of the digits

};

#endif // TTOccupancy_H
