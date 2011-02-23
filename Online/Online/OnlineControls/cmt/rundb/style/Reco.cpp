
function recodb_notimpl() 
{  alert('This functionality is not implemented!'); };

function recodb_show_configs() 
{  setViewerLocation('/ReprocesssingManager?-recodb_show_configs=1');  };

function recodb_show_managers() 
{  setViewerLocation('/ReprocesssingManager?-recodb_show_managers=1');  };

function recodb_manip_manager(pid, name, action)
{  setViewerLocation('/ReprocesssingManager?-recodb_manip_manager=1&-pid='+pid+'&-name='+name+'&-action='+action);  };

function recodb_show_summary() 
{  setViewerLocation('/ReprocesssingManager?-recodb_show_summary=1');  };

function recodb_remove_file(fid) 
{  setViewerLocation('/ReprocesssingManager?-recodb_remove_file='+fid);  };

function recodb_setstate_file(fid, state) 
{  setViewerLocation('/ReprocesssingManager?-recodb_set_filestate='+fid+'&-state='+state);  };

function recodb_reschedule_file(fid, state) 
{  setViewerLocation('/ReprocesssingManager?-recodb_reschedule_file='+fid);  };

function recodb_reschedule_files_selection(mgr, prev, state)   { 
  var res = window.confirm('Are you sure you want to\nreschedule the files for manager '+mgr+'\nwhich are in state:'+state+'?');
  if ( res == true ) {
    var p = prev ? '&-prev_state='+prev : '';
    setViewerLocation('/ReprocesssingManager?-recodb_reschedule_file_selection=1&-manager='+mgr+'&-state='+state+p);
  }
};

function rundb_submit_run(runno,stream)  {  
  var e = document.getElementById('recodb_slice_selector');
  var s = e ? '&-recodb_slice='+e.value : '';
  setViewerLocation('/ReprocesssingManager?-rundb_submit_run='+runno+'&-stream='+stream+s);  
};

function recodb_show_files() 
{  setViewerLocation('/ReprocesssingManager?-recodb_show_files=1');    };

function recodb_show_files_selection(slice,state)   {  
  var url = '/ReprocesssingManager?-recodb_show_files=1';
  if ( slice ) url = url + '&-slice='+slice;
  if ( state ) url = url + '&-state='+state;
  setViewerLocation(url);
};

function recodb_cleandb() 
{  setViewerLocation('/ReprocesssingManager?-recodb_clean=1');   };

function rundb_submit_file(fid)  {
  var e = document.getElementById('recodb_slice_selector');
  var s = e ? '&-recodb_slice='+e.value : '';
  setViewerLocation('/ReprocesssingManager?-rundb_submit_file='+fid+s);  
};

function rundb_show_run(run)
{  setViewerLocation('/ReprocesssingManager?-rundb_show_run='+run);  };

function rundb_show_file(fid)
{  setViewerLocation('/ReprocesssingManager?-rundb_show_file='+fid);  };

function rundb_show_runs() 
{  setViewerLocation('/ReprocesssingManager?-rundb_recent_runs=1');   };

function rundb_show_fills()  { 
  var e = document.getElementById('rundb_fill_selector');
  var s = e ? '&-fill_selector='+e.value : '';
  var url = '/ReprocesssingManager?-rundb_recent_fills=1'+s;
  setViewerLocation(url);
};

function rundb_show_fills_by_select(fill_selector) {
  setViewerLocation('/ReprocesssingManager?-rundb_recent_fills=1&-fill='+fill_selector);
}

function rundb_show_runs_by_fill(fill_selector) {
  setViewerLocation('/ReprocesssingManager?-rundb_recent_runs=1&-fill='+fill_selector);
}
function rundb_show_fill() {
  var e = document.getElementById('fill_no');
  setViewerLocation('/ReprocesssingManager?-rundb_recent_runs=1'+(e ? '&-fill='+e.value : ''));
};

