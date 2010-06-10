_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** ST web widgets
 *
 *  @author  M.Frank
 */
var StStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('ST Operation State');

  table.options = {logo:    lhcb_logo('http://cern.ch/lhcb-public/Objects/Detector/Tracking-system-diagram-2.jpg'),
        	   logo_url:'http://cern.ch/lhcb-public/en/Detector/Trackers-en.html',
		   title:   'ST Detector Status',
		   tips:    'ST operations status:<br>-- High/Low voltage status<br>-- Tell1 Board Status<br>-- Infrastructure'
  };


  lhcb.widgets.st = new Object();

  /** ST web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.st.Systems = function(sys, options) {
    var tab = lhcb.widgets.SubdetectorSystem(options);
    tab.sys = sys;
    tab.addStations = function(title,id) {
      var tb = this.body(title);
      if ( this.sys == 'IT' ) {
	tb.appendChild(tab.addFSMRow(['IT_'+id+'_ST1A', 'IT_'+id+'_ST1C']));
	tb.appendChild(tab.addFSMRow(['IT_'+id+'_ST2A', 'IT_'+id+'_ST2C']));
	tb.appendChild(tab.addFSMRow(['IT_'+id+'_ST3A', 'IT_'+id+'_ST3C']));
      }
      if ( this.sys == 'TT' ) {
	tb.appendChild(tab.addFSMRow(['TT_'+id+'_A', 'TT_'+id+'_C']));
	tb.appendChild(tab.addFSMRow(['TT_'+id+'_C', id=='HV'?null:'TT_'+id+'_MRT']));
      }
      return this;
    };
    tab.addHV = function() { return this.addStations(this.sys+' High Voltage per Station','HV'); };
    tab.addLV = function() { return this.addStations(this.sys+' Low Voltage per Station', 'DCS_LV'); };
    return tab;
  };

  table.beforeComment = function() {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    var ttDAQ = lhcb.widgets.SystemSummary({style:'Arial12pt',
						    system: 'TT',
						    title: 'TT Subsystem State',
						    split: false,
						    items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
						    legend:true,
						    rowing:true,
						    logger:this.logger});
    var itDAQ = lhcb.widgets.SystemSummary({style:'Arial12pt',
						    system: 'IT',
						    title: 'IT Subsystem State',
						    split: false,
						    items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
						    legend:true,
						    rowing:true,
						    logger:this.logger});
    this.addRow([this.hvSummary({system:'IT',hv:true,lv:true,nosplit:true}),
		 this.hvSummary({system:'TT',hv:true,lv:true,nosplit:true})]);
    this.addRow([lhcb.widgets.st.Systems('IT',opts).addHV(),
		 lhcb.widgets.st.Systems('TT',opts).addHV()]);
    this.addRow([lhcb.widgets.st.Systems('IT',opts).addLV(),
		 lhcb.widgets.st.Systems('TT',opts).addLV()]);
    this.addRow([lhcb.widgets.SafetySummary({it:true,logger:this.logger}),
		 lhcb.widgets.SafetySummary({tt:true,logger:this.logger})]);
    this.addRow([lhcb.widgets.DSS.CaVPlantStatus('It',opts),
		 lhcb.widgets.DSS.CaVPlantStatus('Tt',opts)]);
    this.addRow([itDAQ, ttDAQ]);
  };
  return table;
};

var st_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var st_body = function()   { return lhcb.widgets.SubdetectorPage.start(StStatus); };
