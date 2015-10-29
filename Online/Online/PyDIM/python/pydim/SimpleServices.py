import pydim

#----------------------------------------------------------------------------------------
class SimpleService:
  #--------------------------------------------------------------------------------------
  def __init__(self, name, format, init_state):
    self.__name  = name
    self.__value = init_state
    self.__svcID = pydim.dis_add_service(self.__name, format, self.service, 1)
    pydim.dis_update_service(self.__svcID, (self.__value,))

  #--------------------------------------------------------------------------------------
  def update(self, new_state):
    self.__value = new_state
    pydim.dis_update_service(self.__svcID, (self.__value,))

  #--------------------------------------------------------------------------------------
  def service(self,tag):
    return (self.__value,)

def integerService(name, init_state):
  return SimpleService(name,'I',int(init_state))

def floatService(name, init_state):
  return SimpleService(name,'F',float(init_state))

def stringService(name, init_state):
  return SimpleService(name,'C',str(init_state))

def startServing(utgid):
  pydim.dis_start_serving(utgid)
