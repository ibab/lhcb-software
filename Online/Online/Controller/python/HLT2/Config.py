"""
     HLT2 monitoring parameter configuration

     \author  M.Frank
     \version 1.0
"""
import os

# ------------------------------------------------------------------------------
class Config:
  """
        \author  M.Frank
        \version 1.0
  """

  # ----------------------------------------------------------------------------
  def __init__(self, top='.', credentials='example.db', have_archive=False):
    self.configured              = False
    self.top_working_directory   = top
    self.failed                  = 'Failed'
    self.done                    = 'Done'
    self.todo                    = 'ToDo'
    if have_archive:
      self.archive = Config(top+os.sep+'Archive')
      self.daqarea_directory     = '/daqarea/lhcb/data/2015/RAW/FULL'
      self.conditions_directory  = "/group/online/hlt/conditions/LHCb/2015"
      self.running            = 'Running'
      self.st_failed             = 'FAILED'
      self.st_todo               = 'TODO'
      self.st_done               = 'DONE'
      self.st_initial            = 'CREATED'
      self.st_running            = 'RUNNING'

  # ----------------------------------------------------------------------------
  def config(self):
    if not self.configured:
      self.configured = True
      self.failed  = self.top_working_directory + os.sep + self.failed
      self.done    = self.top_working_directory + os.sep + self.done
      self.todo    = self.top_working_directory + os.sep + self.todo
      if hasattr(self,'archive'):
        self.running  = self.top_working_directory + os.sep + self.running
        self.archive.top_working_directory = self.top_working_directory+os.sep+'Archive'
        self.archive.config()
    return self
