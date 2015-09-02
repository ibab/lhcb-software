//_loadScript('lhcb.display.data.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadScript('lhcb.display.partition.cpp');
_loadScript('lhcb.display.zoom.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.status','css');
_loadFile('lhcb.display.fsm','css');

var s_display_font_size = null;
var s_org_display_font_size = null;


var status_unload = function()  {
  dataProviderReset();
};

var status_body = function()  {
  var prt  = the_displayObject['external_print'];
  var msg  = the_displayObject['messages'];
  var sys  = the_displayObject['system'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector = new PartitionSelector(msg);

  s_display_font_size = the_displayObject['size'];
  //if ( null == s_display_font_size && screen.width>1500 ) s_display_font_size = 2;

  body.appendChild(selector);
  body.className = 'MainBody';

  if ( msg > 0 )
    selector.logger = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'RunStatusLogger');
  else
    selector.logger = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'RunStatusLogger');
  if ( prt ) selector.logger.print = prt;
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  if ( sys == null ) {
    setWindowTitle('Run Status Display');
    selector.start('PARTITIONS','lbWeb.PARTITIONS');
  }
  else {
    setWindowTitle(sys+' Run Status');
    selector.selectBox.add(sys,sys,true);
    selector.createDisplay(); 
    selector.hideInput();
    if ( sys == 'LHCb' ) {
      selector.showLHCstate();
    }
  }
  selector.provider.start();
};


if ( _debugLoading ) alert('Script lhcb.display.status.cpp loaded successfully');
