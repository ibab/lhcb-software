#include <vector>
#include <string>
#include <utility>

namespace AIDA 
{
  class IHistogram1D;
  class IHistogram2D;
  class IProfile1D;
}

namespace HistogramUtilities 
{

  bool setBinLabels( AIDA::IHistogram1D* hist,
                     const std::vector<std::string>& labels ) ;
  bool setBinLabels( AIDA::IProfile1D*   hist, 
                     const std::vector<std::string>& labels ) ;

  bool setBinLabels( AIDA::IHistogram1D* hist, 
                     const std::vector<std::pair<unsigned,std::string> >& labels ) ;
  bool setBinLabels( AIDA::IProfile1D*   hist,
                     const std::vector<std::pair<unsigned,std::string> >& labels ) ;

  bool setBinLabels( AIDA::IHistogram2D* hist, 
                     const std::vector<std::string>& xlabels,
                     const std::vector<std::string>& ylabels) ;

  bool setAxisLabels( AIDA::IHistogram1D* hist, 
                      const std::string & xAxis,
                      const std::string & yAxis  ) ;
  bool setAxisLabels( AIDA::IProfile1D* prof, 
                      const std::string & xAxis,
                      const std::string & yAxis  ) ;

}
