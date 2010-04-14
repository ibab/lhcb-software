
Ext.app.About = Ext.extend(Ext.app.Module, {
  moduleType : 'system/about',
  id:          'About',
  menuPath:    'ToolMenu',
  panel : 	null,
  win:          null,

  init: function() {
    this.launcher =   {
      iconCls:         'user',
      shortcutIconCls: 'user',
      text:            'About',
      tooltip:         '<b>About box</b><br />All the credits.',
      handler:          this.createWindow,
      scope:            this
    }
  },

  delegate: function() {
    this.app = this;
    this.getDesktop = function()   { 
      return { 
	windows: {},
        getWindow:    function(id)  {return this.windows[id];  },
	createWindow: function(cfg) {return new Ext.Window(cfg); }
      };
    };
    return this.createWindow();
  },

  createWindow: function(){
    var desktop = this.app.getDesktop();
    var win = desktop.getWindow(this.id);
    if( !win )   {
      var panel = new Ext.Panel({
        border: 	false,
        id: 		'about-panel',
        bodyStyle: 	'padding:20px',
        margins: 	'20 25 0 15',
        region:         'center',
        html:           'LHCb web desktop<br>\
Access to monitoring information<br>of the LHCb experiment.<br><br>\
<font size="-2">Comments and suggestions to<br>M.Frank CERN/LHCb</font>'
});
      panel.addButton({text:'Ok'}, function() { this.win.close();}, this);
      this.win = desktop.createWindow({
              id: 		this.id,
              title: 		this.id,
              //cls: 	       'pref-card-subpanel',//'x-icon-information',
              iconCls:         'user',
              maximizable:      false,
              minimizable:      false,
              //resizeable:       false,
              closable:         false,
	      plain:            false,
              shim:             false,
              animCollapse:     false,
              constrainHeader:  true,
              layout:          'fit'
       });
       this.win.add(panel);
    }
    this.win.setSize(300,200);
    //this.win.setPosition(500,300);
    this.win.show();
    return this.win;
  }
});
