_loadScript('lhcb.tools.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('dom.print.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.subfarm','css');

var SelectionBox = function() {
  select = document.createElement('select');
  select.add = function(tag, text, selected) {
    var o = document.createElement('option');
    var i = document.createTextNode(text);
    if ( selected ) o.setAttribute('selected',selected);
    o.setAttribute('value',tag);
    o.appendChild(i);
    this.appendChild(o);
    return this;
  };

  select.get_value = function() {
    return this.options[this.selectedIndex].value;
  };
  return select;
};

add_row = function(label,data) {
  var tr = document.createElement('tr');
  td = document.createElement('td');
  td.innerHTML = label;
  tr.appendChild(td);
  td = document.createElement('td');
  td.innerHTML = data;
  tr.appendChild(td);
  return tr;
};

var ClusterData = function(e) {
  var cluster = new Object();
  cluster.name = e.getAttribute('name');
  cluster.status = e.getAttribute('status');
  cluster.lastUpdate = e.getAttribute('time');
  cluster.nodes = new Array();
  cluster.pvss_OK = true;
  cluster.pvss_error = 'OK';
  var elts = e.childNodes;
  var arr = cluster.nodes;
  var txt = '';
  for(var i=0, leni=elts.length; i<leni;++i) {
    var m = elts[i];
    if ( m.nodeName != 'Node' ) continue;

    var n = new Object();
    n.ok = true;
    n.error = '';
    n.tasks = new Array();
    n.projects = new Array();
    n.connections = new Array();

    n.tasks.count = 0;
    n.tasks.missing = 0;
    n.projects.count = 0;
    n.projects.missing = 0;
    n.connections.count = 0;
    n.connections.missing = 0;

    var mm = m.childNodes;
    arr.push(n);
    //alert('1 '+cluster.name+'  '+m.parentNode.nodeName+'  type:'+m.nodeType+' name:'+m.nodeName+' value:'+m.nodeValue);
    n.name = m.getAttribute('name');
    n.status = m.getAttribute('status');
    n.lastUpdate = m.getAttribute('time');
    txt = txt + '+++ Node:'+n.name+'\n';
    for(var j=0, lenj=mm.length; j<lenj;++j) {
      var ni = mm[j];
      if ( ni.nodeName != '#text' )  txt = txt + ni.nodeName+'\n';
      //alert('Item:'+n.name+' '+ni.nodeName);
      if ( ni.nodeName == 'Tasks' ) {
	var tasks = ni.childNodes;
	n.tasks.ok = ni.getAttribute('ok');
	n.tasks.count = ni.getAttribute('count');
	n.tasks.missing = ni.getAttribute('missing');
	if ( n.tasks.missing > 0 )   {
	  n.ok = false;
	  n.error = 'Tasks missing';
	}
	for(var k=0, lenk=tasks.length;k<lenk;++k) {
	  var dt   = tasks[k];
	  if ( dt.nodeName == 'Task' ) {
	    var t    = new Object();
	    t.name   = dt.getAttribute('name');
	    t.status = dt.getAttribute('status');
	    n.tasks.push(t);
	  }
	}
      }
      else if ( ni.nodeName == 'Connections' ) {
	var conns = ni.childNodes;
	n.connections.ok = ni.getAttribute('ok');
	n.connections.count = ni.getAttribute('count');
	n.connections.missing = ni.getAttribute('missing');
	if ( n.connections.missing > 0 )  {
	  n.ok = false;
	  n.error = 'Connections missing';
	}
	for(var k=0, lenk=conns.length;k<lenk;++k) {
	  var dt   = conns[k];
	  if ( dt.nodeName == 'Connection' ) {
	    var c    = new Object();
	    c.name   = dt.getAttribute('name');
	    c.status = dt.getAttribute('status');
	    n.connections.push(c);
	  }
	}
      }
      else if ( ni.nodeName == 'Projects' ) {
	var pros = ni.childNodes;
	n.projects.ok = ni.getAttribute('ok');
	n.projects.count = ni.getAttribute('count');
	n.projects.missing = ni.getAttribute('missing');
	if ( cluster.pvss_OK && n.projects.missing > 0 ) {
	  if ( n.ok ) {
	    n.ok = false;
	    n.error = 'PVSS does not behave properly....';
	  }
	  cluster.pvss_OK = false;
	  cluster.pvss_error = 'Projects missing';
	}
	for(var k=0, lenk=pros.length;k<lenk;++k) {
	  var dt   = pros[k];
	  if ( dt.nodeName == 'Project' ) {
	    var p    = new Object();
	    p.name   = dt.getAttribute('name');
	    p.status = dt.getAttribute('status');
	    p.event  = dt.getAttribute('event');
	    p.data   = dt.getAttribute('data');
	    p.dist   = dt.getAttribute('dist');
	    if ( !(p.event=='RUNNING' && p.data=='RUNNING' && p.dist=='RUNNING') ) {
	      cluster.pvss_OK = false;
	      cluster.pvss_error = 'Managers not running';
	    }
	    p.fsmSrv = dt.getAttribute('fsmSrv');
	    p.devHandler = dt.getAttribute('devHandler');
	    n.projects.push(p);
	  }
	}
      }
      else if ( ni.nodeName == 'Boot' ) {
	n.boottime = ni.getAttribute('time');
      }
      else if ( ni.nodeName == 'System' ) {
	n.perc_cpu = ni.getAttribute('perc_cpu');
	n.perc_mem = ni.getAttribute('perc_mem');
	n.vsize    = ni.getAttribute('vsize');
	n.rss      = ni.getAttribute('rss');
	n.vsize    = ni.getAttribute('data');
	n.stack    = ni.getAttribute('stack');	
      }
    }
  }
  //alert('Summary:'+cluster.name+':\n'+txt);
  return cluster;
};


var Task = function(t) {
  var task = document.createElement('table');
  task.body = document.createElement('tbody');
  var tr = document.createElement('tr');
  tr.appendChild(Cell(t.name,1,'NodeItem'));
  tr.appendChild(Cell(t.status,1,'NodeItem'));
  task.body.appendChild(tr);
  task.appendChild(task.body);
  return task;
};

  
var Projects = function(node) {
  var p = node.projects;
  var tr = document.createElement('tr');
  var projects = document.createElement('table');
  projects.body = document.createElement('tbody');
  projects.width = '100%';

  this._status = function(val) {
    if ( val == 'RUNNING' ) return 'OK';
    return 'NOTOK';
  };

  tr.appendChild(Cell(p.length+' PVSS projects on '+node.name,12,'SubfarmItemHeader'));
  tr.width='100%';
  projects.body.appendChild(tr);
  if ( p.length > 0 ) {
    for(var j=0, lenj=p.length;j<lenj;++j) {
      var pro = p[j];
      tr = document.createElement('tr');
      tr.appendChild(Cell(pro.name,      1,'NodeItem'));
      tr.appendChild(Cell(pro.status,    1,pro.status));
      tr.appendChild(Cell('Event Mgr:',  1,'NodeItem'));
      tr.appendChild(Cell(pro.event,     1,pro.event  =='RUNNING' ? 'OK' : 'NOTOK'));
      tr.appendChild(Cell('Data Mgr:',   1,'NodeItem'));
      tr.appendChild(Cell(pro.data,      1,pro.data   =='RUNNING' ? 'OK' : 'NOTOK'));
      tr.appendChild(Cell('Dist.Mgr:',   1,'NodeItem'));
      tr.appendChild(Cell(pro.dist,      1,pro.dist   =='RUNNING' ? 'OK' : 'NOTOK'));
      tr.appendChild(Cell('FSM Server:', 1,'NodeItem'));
      tr.appendChild(Cell(pro.fsmSrv,    1,pro.fsmSrv =='RUNNING' ? 'OK' : 'NOTOK'));
      tr.appendChild(Cell('Dev Handler:',1,'NodeItem'));
      tr.appendChild(Cell(pro.devHandler,1,pro.devHandler =='RUNNING' ? 'OK' : 'NOTOK'));
      projects.body.appendChild(tr);
      tr = document.createElement('tr');
    }
  }
  else {
    tr.appendChild(Cell('No PVSS projects are required on:'+node.name,8,'NodeItem'));
  }
  projects.appendChild(projects.body);
  return projects;
};

var Connections = function(node) {
  var c = node.connections;
  var tr = document.createElement('tr');
  var conns = document.createElement('table');

  conns.width = '100%';
  conns.body = document.createElement('tbody');
  tr.appendChild(Cell(c.length+' Network connections on '+node.name,8,'SubfarmItemHeader'));
  tr.width='100%';
  conns.body.appendChild(tr);
  tr = document.createElement('tr');
  if ( c.length > 0 ) {
    for(var j=0, lenj=c.length;j<lenj;++j) {
      var con = c[j];
      tr.appendChild(Cell(con.name,1,'NodeItem'));
      tr.appendChild(Cell(con.status,1,con.status));
      if ( 0 == ((j+1)%4) )   {
	conns.body.appendChild(tr);
	tr = document.createElement('tr');
      }
    }
    if ( j>0 && 0 != ((j+1)%4) ) {
      conns.body.appendChild(tr);
    }
  }
  else {
    tr.appendChild(Cell('No connections are required on:'+node.name,8,'NodeItem'));
  }
  conns.appendChild(conns.body);
  return conns;
};

var Tasks = function(node) {
  var t = node.tasks;
  var tr = document.createElement('tr');
  var tasks = document.createElement('table');

  tasks.width = '100%';
  tasks.body = document.createElement('tbody');
  tr.appendChild(Cell(t.length+' Tasks executing on '+node.name,8,'SubfarmItemHeader'));
  tasks.body.appendChild(tr);
  tr = document.createElement('tr');
  if ( t.length > 0 ) {
    for(var j=0, lenj=t.length;j<lenj;++j) {
      var tsk = t[j];
      tr.appendChild(Cell(tsk.name,1,'NodeItem'));
      tr.appendChild(Cell(tsk.status,1,tsk.status));
      if ( 0 == ((j+1)%4) ) {
	tasks.body.appendChild(tr);
	tr = document.createElement('tr');
      }
    }
    if ( j>0 && 0 != ((j+1)%4) ) {
      tasks.body.appendChild(tr);
    }
  }
  else {
    tr.appendChild(Cell('No tasks are required to execute on:'+node.name,8,'NodeItem'));
  }
  tasks.appendChild(tasks.body);
  return tasks;
};

var NodeSummaryHeader = function() {
  var hdr = document.createElement('tr');
  hdr.appendChild(Cell('Node<BR/>Name',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('<BR/>Status',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('Tasks<BR/>OK/Req',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('Connections<BR/>OK/Req',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('<BR/>RSS',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('<BR/>Stack',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('<BR/>VSize',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('<BR/>% CPU',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('<BR/>% Mem',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('<BR/>Boottime',1,'SubfarmItemLabel'));
  hdr.appendChild(Cell('<BR/>Last update',1,'SubfarmItemLabel'));
  return hdr;
};

var NodeSummaryBody = function(n) {
  var t = n.tasks;
  var c = n.connections;
  var row = document.createElement('tr');
  row.appendChild(Cell(n.name,1,'SubfarmItem'));
  row.appendChild(Cell(n.status,1,n.status=='ALIVE' ? 'SubfarmItem' : 'NOTOK'));
  row.appendChild(Cell(t.ok+'/'+t.count,1,t.ok==t.count ? 'SubfarmItem' : 'NOTOK'));
  row.appendChild(Cell(c.ok+'/'+c.count,1,c.ok==c.count ? 'SubfarmItem' : 'NOTOK'));
  row.appendChild(Cell(n.rss,1,'SubfarmItem'));
  row.appendChild(Cell(n.stack,1,'SubfarmItem'));
  row.appendChild(Cell(n.vsize,1,'SubfarmItem'));
  row.appendChild(Cell(n.perc_cpu,1,'SubfarmItem'));
  row.appendChild(Cell(n.perc_mem,1,'SubfarmItem'));
  row.appendChild(Cell(n.boottime,1,'SubfarmItem'));
  row.appendChild(Cell(n.lastUpdate,1,'SubfarmItem'));
  return row;
};

var NodeSummaryTrailer = function(n) {
  var t = n.tasks;
  var c = n.connections;
  var p = n.projects;
  var row = document.createElement('tr');
  var msg = 'Node looks fine. Nothing missing. ', cls = 'OK';
  if ( !(t.missing == 0 && p.missing == 0 && c.missing) ) {
    msg = 'Node has some problems. ';
    cls = 'NOTOK';
  }
  msg += (t.missing > 0) ? t.missing+' tasks missing. ' : 'Tasks present. ';
  msg += (c.missing > 0) ? c.missing+' connections missing. ' : 'Connections present. ';
  if ( p.count > 0 ) {
    msg += (p.missing > 0) ? p.missing+' projects missing. ' : 'All PVSS projects present. ';
  }
  row.appendChild(Cell(msg,11,cls));
  return row;
};

var Node = function(n) {
  var t = n.tasks;
  var c = n.connections;
  var node = document.createElement('table');
  node.body = document.createElement('tbody');
  node.className = 'NodeTable';
  node.body.className = 'NodeTable';
  node.body.border = 'none';

  this.addCell = function(n, ncol, c) {
    var row = document.createElement('tr');
    var cell = document.createElement('td');
    cell.colSpan = ncol;
    cell.appendChild(c);
    row.appendChild(cell);
    n.body.appendChild(row);
    return cell;
  };

  node.body.appendChild(NodeSummaryHeader());
  node.body.appendChild(node.status_row=NodeSummaryBody(n));
  node.tasks_cell = this.addCell(node,11,Tasks(n));
  node.conns_cell = this.addCell(node,11,Connections(n));
  if ( n.projects.length > 0 ) {
    node.projects_cell = this.addCell(node,11,Projects(n));
  }
  node.body.appendChild(node.status_trl=NodeSummaryTrailer(n));
  node.appendChild(node.body);
  return node;
};

var ClusterSummary = function(c) {
  var td, tr   = document.createElement('tr');
  var summary  = document.createElement('table');
  summary.body = document.createElement('tbody');
  summary.className      = 'NodeTable';
  summary.body.className = 'NodeTable';
  summary.body.border    = 'none';

  summary.header = document.createElement('table');
  summary.header.body = document.createElement('tbody');
  tr.appendChild(Cell('Task control monitoring on <B>'+c.name+'</B> ['+c.lastUpdate+']',1,'NodeTableHeader'));
  tr.appendChild(Cell('Status:',1,'NodeTableHeader'));
  tr.appendChild(Cell(c.status,1,c.status=='ALIVE' ? 'OK' : 'NOTOK'));
  summary.header.body.appendChild(tr);
  summary.header.appendChild(summary.header.body);

  summary.nodes = document.createElement('table');
  summary.nodes.body = document.createElement('tbody');
  summary.nodes.className = 'NodeTable';
  summary.nodes.body.className = 'NodeTable';
  summary.nodes.body.appendChild(NodeSummaryHeader());
  for(var i=0; i<c.nodes.length;++i) {
    summary.nodes.body.appendChild(NodeSummaryBody(c.nodes[i]));
  }
  summary.nodes.appendChild(summary.nodes.body);

  // Add header to display
  tr = document.createElement('tr');
  td = document.createElement('td');
  td.appendChild(summary.header);
  tr.appendChild(td);
  summary.body.appendChild(tr);
  // Add body
  tr = document.createElement('tr');
  td = document.createElement('td');
  td.appendChild(summary.nodes);
  tr.appendChild(td);
  summary.body.appendChild(tr);

  /// Add close button
  tr = document.createElement('tr');
  td = document.createElement('td');
  td.innerHTML = 'Close';
  tr.className = 'DisplayButton';
  tr.appendChild(td);
  summary.body.appendChild(tr);
  summary.appendChild(summary.body);
  return summary;
};

var ClusterSubdisplay = function(c) {
  var td, tr;
  var sum  = document.createElement('table');
  sum.body = document.createElement('tbody');

  sum.className      = 'ClusterSubdisplay';
  sum.body.className = 'ClusterSubdisplay';
  sum.body.border    = 'none';
  tr = document.createElement('tr');
  tr.appendChild(Cell('<B>'+c.name+'</B>',1,'NodeItem'));
  tr.appendChild(Cell(c.status,1,c.status=='ALIVE' ? 'OK' : 'NOTOK'));
  tr.appendChild(Cell('PVSS '+c.pvss_error,1,c.pvss_OK ? 'OK' : 'NOTOK'));
  tr.appendChild(Cell('['+c.lastUpdate+']',1,'NodeItem'));
  sum.body.appendChild(tr);

  sum.row2 = document.createElement('tr');
  sum.body.appendChild(sum.row2);

  tr   = document.createElement('tr');
  td   = document.createElement('td');
  td.colSpan = 4;
  tr.appendChild(td);
  sum.body.appendChild(tr);
  sum.nodes = document.createElement('table');
  td.appendChild(sum.nodes);
  sum.nodes.body = document.createElement('tbody');
  sum.nodes.className = 'ClusterSubdisplayNodes';
  sum.nodes.body.className = 'ClusterSubdisplayNodes';
  sum.nodes.appendChild(sum.nodes.body);

  tr   = document.createElement('tr');
  sum.nodes.body.appendChild(tr);
  var cn = c.name.toLowerCase();
  var has_errors = !c.pvss_OK;
  var err = 'No obvious errors detected.';
  var badConn=0, totConn=0, badTask=0, totTask=0;
  for(var i=0; i<c.nodes.length;++i) {
    var n = c.nodes[i];
    totTask += parseInt(n.tasks.count);
    badTask += parseInt(n.tasks.missing);
    totConn += parseInt(n.connections.count);
    badConn += parseInt(n.connections.missing);
    if ( ((i+1)%13) == 0 ) {
      tr = document.createElement('tr');
      sum.nodes.body.appendChild(tr);
      tr.appendChild(Cell('',1,'NodeItem'));
    }
    if ( n.name == cn ) {
      tr.appendChild(Cell(n.name,1,n.ok ? 'OK' : 'NOTOK'));
    }
    else {
      tr.appendChild(Cell(n.name.substring(n.name.length-2),1,n.ok ? 'OK' : 'NOTOK'));
    }
    if ( !n.ok )  {
      has_errors = true;
      err = n.error;
    }
  }
  var msg = c.nodes.length+' Nodes '+totTask+' Tasks/'+badTask+' bad  '+totConn+' Connections/'+badConn+' bad';
  sum.row2.appendChild(Cell(msg,4,'NodeItem'));
  // Final comment
  tr = document.createElement('tr');
  tr.appendChild(Cell(err,4,has_errors ? 'NOTOK' : 'NodeItem'));
  sum.body.appendChild(tr);

  //sum.body.onclick = function() { alert('hello');};
  sum.appendChild(sum.body);
  return sum;
};

var Cluster = function(c) {
  var cluster = document.createElement('table');
  cluster.body = document.createElement('tbody');

  for(var i=0; i<c.nodes.length;++i) {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    td.colSpan = 2;
    td.appendChild(Node(c.nodes[i]));
    tr.appendChild(td);
    cluster.body.appendChild(tr);
  }
  cluster.appendChild(cluster.body);
  return cluster;
};

var SubfarmDisplay = function(provider, logger, name) {
  var table = document.createElement('table');
  table.body = document.createElement('tbody');
  table.provider = provider;
  table.logger = logger;
  table.name = name;

  table.className = 'ClusterSubdisplay';
  table.body.className = 'ClusterSubdisplay';

  var tr = document.createElement('tr');
  table.display = document.createElement('td');
  table.item = RawItem(table.provider,'/'+table.name+'/TaskSupervisor/Status',null,table);
  tr.appendChild(table.display);
  table.body.appendChild(tr);
  table.appendChild(table.body);

  table.logger.error('added child '+table.name);
  table.set = function(data) {
    var parser;
    this.logger.error('filling child '+this.name);
    if (document.implementation.createDocument) {
      var parser = new DOMParser();
      doc = parser.parseFromString(data[0], "text/xml");
    }
    else if (window.ActiveXObject) {
      doc = new ActiveXObject("Microsoft.XMLDOM");
      doc.async="false";
      doc.loadXML(data[0]);
    }
    var clusters = doc.getElementsByTagName('Cluster');
    disp = document.createElement('table');
    disp.body = document.createElement('tbody');

    for (var i=0; i<clusters.length; ++i)  {
      var c = clusters[i];
      var tr = document.createElement('tr');
      var td = document.createElement('td');
      var c_data = ClusterData(c);
      td.appendChild(ClusterSubdisplay(c_data));
      tr.appendChild(td);
      disp.body.appendChild(tr);
    }
    disp.appendChild(disp.body);
    if ( this.display.content ) {
      this.display.removeChild(this.display.content);
    }
    this.display.appendChild(disp);
    this.display.content = disp;
    this.logger.error('filling child '+this.name+'....done');
    return this;
  };
  return table;
};

var SubfarmSelector = function(msg,sys) {
  var table = document.createElement('table');
  table.body = document.createElement('tbody');
  table.inheritsFrom = ItemListener;
  table.inheritsFrom(null,null);
  table.select = SelectionBox();
  table.messages = msg;
  table.provider = null;
  table.logger = null;

  table.add = function() {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    td.setAttribute('colSpan',4);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  };

  table.body.cellpadding = 0;
  if ( sys != 'all' )  {
    var tr = document.createElement('tr');
    tr.appendChild(StatusText('Select subfarm:',1));
    
    var td = document.createElement('td');
    td.width = 135;
    td.appendChild(table.select);
    tr.appendChild(td);
    
    table.change = document.createElement('td');
    table.change.handler   = table;
    table.change.innerHTML = 'Show';
    table.change.className = 'DisplayButton';
    table.change.onclick   = function() { this.handler.createDisplay();  };
    tr.appendChild(table.change);
    
    table.update = document.createElement('td');
    table.update.handler   = table;
    table.update.innerHTML = 'Update';
    table.update.className = 'DisplayButton';
    table.update.onclick   = function() { _dataProvider.update();  };
    tr.appendChild(table.update);
    table.body.appendChild(tr);
  }
  table.display = table.add();
  table.display.attached = false;
  table.logDisplay = table.add();

  table.appendChild(table.body);

  table.createDisplay = function()   {
    if ( this.display.attached ) {
      this.display.removeChild(this.display.child);
    }
    var d = SubfarmDisplay(this.provider,this.logger,this.select.get_value());
    this.display.child = d;
    this.display.appendChild(d);
  };
  return table;
};

var buildAll = function(selector) {
  var table = document.createElement('table');
  table.body = document.createElement('tbody');
  table.className = 'ClusterTable';
  table.body.className = 'ClusterTable';

  var count = 0;
  var tr = document.createElement('tr');
  for(var i=0xA; i<0xF; ++i) {
    for(var j=1; j<=11; ++j) {
      if ( j==5 ) continue;

      if ( count == 4 ) {
	count = 0;
	table.body.appendChild(tr);
	tr = document.createElement('tr');
      }
      count++;
      var nam = sprintf('HLT%1X%02d',i,j);
      var td = document.createElement('td');
      var itm = SubfarmDisplay(selector.provider,selector.logger,nam);
      td.appendChild(itm);
      tr.appendChild(td);
    }
  }
  table.body.appendChild(tr);
  table.appendChild(table.body);
  selector.display.appendChild(table);
  //printDOMTree(document,null);
};

var subfarm_unload = function()  {
  dataProviderReset();
};

var subfarm_body = function()  {
  var msg = the_displayObject['messages'];
  var sys = the_displayObject['system'];
  var body = document.getElementsByTagName('body')[0];
  var selector = new SubfarmSelector(msg,sys);
  body.appendChild(selector);

  if ( msg > 0 )
    selector.logger = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'RunStatusLogger');
  else
    selector.logger = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'RunStatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/farm';
  if ( sys == null ) {
    selector.select.add('MONA08','MONA08',true);
    selector.select.add('MONA09','MONA09',false);
    selector.select.add('HLTA01','HLTA01',false);
    selector.select.add('HLTA02','HLTA02',false);
    selector.select.add('STORECTL01','STORECTL01',false);
  }
  else if ( sys == 'all' ) {
    buildAll(selector);
  }
  else {
    selector.select.add(sys,sys,true);
    selector.createDisplay(); 
  }
  selector.provider.start();
};


if ( _debugLoading ) alert('Script lhcb.display.status.cpp loaded successfully');

