#ifndef PVSSMANAGER_HOTLINKCALLBACK_H
#define PVSSMANAGER_HOTLINKCALLBACK_H
// $ID= $

class Variable;

/*
 *    Namespace PVSS
 */
namespace PVSS {

  /* @class HotLinkCallback  HotLinkCallback.h PVSSManager/HotLinkCallback.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class HotLinkCallback  {
  public:
    HotLinkCallback() {}
    virtual ~HotLinkCallback() {}
    virtual void setValue(const DpIdentifier& dpid, int typ, const Variable* val) = 0;
    virtual void handleDataUpdate() = 0;
  };
}       /*  End namespace PVSS         */
#endif  /*  PVSSMANAGER_HOTLINKCALLBACK_H  */
