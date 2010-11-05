var lhcb = null;
var _lhcb = function() {
  if ( lhcb == null ) lhcb = new Object();
  return lhcb;
};
var _isInternetExplorer = function() 
{  return navigator.appName == "Microsoft Internet Explorer"; };
var _debugLoading = false;
var _emulateBrowser = function()
{  return navigator.appCodeName == 'Envjs';                   };
_loadStatic = function(name)
{  document.write('<SCRIPT type="'+_javascriptType+'" src="'+name+'"></SCRIPT>');   };

var _loadScriptAbs = function(base,name) { _loadStatic(base+'/'+name);     };
var _loadScript    = function(name)      { _loadScriptAbs(_fileBase,name); };

function _loadFileAbs(base,filename, filetype)   {
  // this somehow does not work!!!!
  if (filetype=="cpp"){ //if filename is a external JavaScript file
    var fileref=document.createElement('script');
    fileref.setAttribute("type",_javascriptType);
    fileref.setAttribute("src", base+'/'+filename+'.'+filetype);
  }
  else if (filetype=="js"){ //if filename is a external JavaScript file
    var fileref=document.createElement('script');
    fileref.setAttribute("type",_javascriptType);
    fileref.setAttribute("src", base+'/'+filename+'.'+filetype);
  }
  else if (filetype=="css"){ //if filename is an external CSS file
    var fileref=document.createElement("link");
    fileref.setAttribute("rel", "stylesheet");
    fileref.setAttribute("type", "text/css");
    fileref.setAttribute("href", base+'/Style/'+filename+'.css');
  }
  if (typeof fileref!="undefined")
    document.getElementsByTagName("head")[0].appendChild(fileref);
};

function _loadFile(filename, filetype)   {  _loadFileAbs(_fileBase,filename,filetype); };

if ( _transport_in_use=='stomp' )  {
  TCPSocket = Orbited.TCPSocket;
  //var org_transport = Orbited.util.chooseTransport;
  //Orbited.util.chooseTransport = function() {
  //  return Orbited.CometTransports.LongPoll;
  //};
  _loadStatic('/static/protocols/stomp/stomp.js');
  _loadScriptAbs(_lhcbScriptBase,'lhcb.display.data.cpp');
}
else if ( _transport_in_use == 'amq' )  {
  _loadScriptAbs(_lhcbScriptBase,'lhcb.display.amq.cpp');
}
_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.constants.cpp');

var display_type = function()   {
  this.type = null;
  this.header = null;
  this.body = null;
  var url = document.location.toString();
  var pars = url.split('?');

  this.header = function() {
    var msg = 'The URL\n'+url+'\nis not a valid display URL!\n';
    alert(msg);
  };
  this.body = function()   {  };
  this.unload = function() { alert('Bye, Bye my friend....'); };

  if ( pars.length > 1 )  {
    var disp_func = '';
    var p = pars[1].split('&');
    this.params = p;
    for (var i=0; i<p.length; ++i) {
      var v = p[i].split('=');
      this[v[0]] = v[1];
    }
    this.url_base = pars[0];
    if ( this.type != null ) {
      eval("this.header = function()     { _loadScriptAbs(_fileBase,'lhcb.display."+this.type+".cpp'); }");
      eval("this.body   = function()     { return "+this.type+"_body(); }");
      eval("this.unload = function()     { return "+this.type+"_unload(); }");
    }
  }

  this.dump = function() {
    var text = '';
    for( p in this) {
      text += p.toString() + '=' + this[p] +'\n';
    }
    alert(text);
  };
};

var the_displayObject = new display_type();
the_displayObject.header();


if ( _debugLoading ) alert('Script lhcb.display.select.cpp loaded successfully');
