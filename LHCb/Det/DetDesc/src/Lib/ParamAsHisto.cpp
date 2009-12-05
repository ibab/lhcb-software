#include "DetDesc/ParamAsHisto.h"

//----------------------------------------------------------------------------
namespace DetDesc {
  namespace Params {
    //------------------------------------------------------------------------
    /// Get the value of TH1D parameter of a Condition
    const Histo1D *paramAsHisto1D(const Condition *cond, const std::string &name){
      return cond ? &( cond->param<DetDesc::Params::Histo1D>(name) ) : NULL;
    }
    //------------------------------------------------------------------------
    /// Get the value of TH2D parameter of a Condition
    const Histo2D *paramAsHisto2D(const Condition *cond, const std::string &name){
      return cond ? &( cond->param<DetDesc::Params::Histo2D>(name) ) : NULL;
    }
  }
}
//----------------------------------------------------------------------------

