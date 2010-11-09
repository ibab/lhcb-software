function recodb_remove_file(fid) 
{  setViewerLocation('/ReprocesssingManager?-recodb_remove_file='+fid);  };

function recodb_setstate_file(fid, state) 
{  setViewerLocation('/ReprocesssingManager?-recodb_set_filestate='+fid+'&-state='+state);  };

function rundb_submit_run(runno,stream) 
{  setViewerLocation('/ReprocesssingManager?-rundb_submit_run='+runno+'&-stream='+stream);  };

function rundb_submit_file(fid) 
{  setViewerLocation('/ReprocesssingManager?-rundb_submit_file='+fid);  };

function rundb_show_runs() 
{  setViewerLocation('/ReprocesssingManager?-rundb_recent_runs=1');   };

function recodb_show_files() 
{  setViewerLocation('/ReprocesssingManager?-recodb_show_files=1');    };

function recodb_cleandb() 
{  setViewerLocation('/ReprocesssingManager?-recodb_clean=1');   };
