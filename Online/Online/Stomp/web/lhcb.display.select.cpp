var _fileBase = 'http://frankm.web.cern.ch/frankm/test/Online/Stomp/web';
var _debugLoading = false;

var _loadScript = function(name) 
{  document.write('<SCRIPT language="JavaScript" src="'+_fileBase+'/'+name+'"></SCRIPT>');   }
var _loadStatic = function(name)
{  document.write('<SCRIPT language="JavaScript" src="'+name+'"></SCRIPT>');   }

function _loadFile(filename, filetype)   {
  // this somehow does not work!!!!
  if (filetype=="cpp"){ //if filename is a external JavaScript file
    var fileref=document.createElement('script');
    fileref.setAttribute("type","text/javascript");
    fileref.setAttribute("src", _fileBase+'/'+filename+'.'+filetype);
  }
  else if (filetype=="js"){ //if filename is a external JavaScript file
    var fileref=document.createElement('script');
    fileref.setAttribute("type","text/javascript");
    fileref.setAttribute("src", _fileBase+'/'+filename+'.'+filetype);
  }
  else if (filetype=="css"){ //if filename is an external CSS file
    var fileref=document.createElement("link");
    fileref.setAttribute("rel", "stylesheet");
    fileref.setAttribute("type", "text/css");
    fileref.setAttribute("href", _fileBase+'/Style/'+filename+'.css');
  }
  if (typeof fileref!="undefined")
    document.getElementsByTagName("head")[0].appendChild(fileref);
}

if ( _stomp_in_use )  {
  TCPSocket = Orbited.TCPSocket;
  _loadStatic('/static/protocols/stomp/stomp.js');
  _loadScript('lhcb.display.data.cpp');
}

var display_type = function()   {
  this.type = null;
  this.header = null;
  this.body = null;
  var url = document.location.toString();
  var pars = url.split('?');

  this.header = function() {
    var msg = 'The URL\n'+url+'\nis not a valid display URL!\n';
    alert(msg);
  }
  this.body = function()   {  }
  this.unload = function() { alert('Bye, Bye my friend....'); }

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
      eval("this.header = function()     { _loadScript('lhcb.display."+this.type+".cpp'); }");
      eval("this.body   = function()     { "+this.type+"_body(); }");
      eval("this.unload = function()     { "+this.type+"_unload(); }");
    }
  }

  this.dump = function() {
    var text = '';
    for( p in this) {
      text += p.toString() + '=' + this[p] +'\n';
    }
    alert(text);
  }

}

var the_displayObject = new display_type();
the_displayObject.header();


if ( _debugLoading ) alert('Script lhcb.display.select.cpp loaded successfully');
