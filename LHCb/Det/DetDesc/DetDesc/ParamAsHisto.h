#include "DetDesc/HistoParam.h"
#include "DetDesc/Condition.h"
// #include "DetDesc/ParamValidDataObject.h"

namespace DetDesc {
  namespace Params {

  /** Get a read accessor to 1d histogram parameter of the Condition
   *
   *  @param cond pointer to the Condition object
   *  @param name name of the parameter
   *  @return pointer to DetDesc::Histo1D on success, NULL if cond == NULL
   *
   *  @author Dmitry Golubkov
   *  @date   2009-12-01
   */
  const Histo1D *paramAsHisto1D(const Condition *cond, const std::string &name);

  /** Get a read accessor to 2d histogram parameter of the Condition
   *
   *  @param cond pointer to the Condition object
   *  @param name name of the parameter
   *  @return pointer to DetDesc::Histo2D on success, NULL if cond == NULL
   *
   *  @author Dmitry Golubkov
   *  @date   2009-12-01
   */
  const Histo2D *paramAsHisto2D(const Condition *const, const std::string &name);

  }
}
