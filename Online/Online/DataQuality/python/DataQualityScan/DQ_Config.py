"""
     DataQuality monitoring parameter configuration

     \author  M.Frank
     \version 1.0
"""

# ------------------------------------------------------------------------------
class Config:
  """
  Configuration setup for the Data quality monitoring system

  \author  M.Frank
  \version 1.0
  """

  # ----------------------------------------------------------------------------
  def __init__(self):
    import time, rundb_params, mondb_params
    # Intrinsic options:
    #
    # Database state names
    #
    self.st_failed             = 'FAILED'      # State name in the monitoring DB
    self.st_todo               = 'TODO'        # State name in the monitoring DB
    self.st_done               = 'DONE'        # State name in the monitoring DB
    self.st_initial            = 'CREATED'     # State name in the monitoring DB
    self.st_running            = 'RUNNING'     # State name in the monitoring DB
    #
    # User setup options:
    #
    self.announceService       = 'LHCb2_MONA10_R/Work'
    self.statusService         = 'MONA1001_R_Controller/status'
    #
    # Database access
    #
    ## Directory where to find the conditions files (HLT2Params.py)
    self.condDirectory         = "/group/online/hlt/conditions/LHCb/%s"%(time.ctime()[20:],)
    ## Directory where to place the data links to the files to be processed
    self.linkDirectory         = '/localdisk/DQ/DATAQUALITY_MON'
    ## Directory where to place processing options
    self.optsDirectory         = '/group/online/dataflow/cmtuser/DQ/DATAQUALITY_OPT'
    
    ## Run database login credentials
    self.rundb       = 'oracle:'+rundb_params.login+'@'+rundb_params.tns+'/'+rundb_params.pwd
    ## Monitoring database login credentials
    self.mondb       = 'oracle:'+mondb_params.login+'@'+mondb_params.tns+'/'+mondb_params.pwd
    ##self.mondb               = 'sqlite:/localdisk/DQ/DATAQUALITY_MON.db'
    ## Monitoring database table name
    self.mondb_table         = 'DQ_mon'
    #
    # Process steering:
    #
    ## Data stream type in the rundb which files to be processed
    self.file_stream_name    = 'TurCal'         # Stream name to pick up input files
    self.file_stream_name    = 'TURCAL'         # Stream name to pick up input files
    ## Minimal number of events in the data stream files per run (must be >0)
    self.min_event_count     = 1000           # Minimal event count in input files
    self.req_event_count     = 150000          # Optimum/max event count in input files
    ## Minimal fraction of data files already processed by HLT2
    self.min_files_processed = 0.9            # Minimal fraction of files processed
