Ext.state.Manager.setProvider(new Ext.state.CookieProvider());

Ext.LogonPanel = function() {

  var help_text = 
'<H1>Configuration selector:</H1>\
You may use several configurations.<br>\
However, you have to select one for a given session.\
Each configuration may later be changed using\
the preferences window.<br>\
If you logout from a given session, you need to\
supply the proper key when reconnecting.';

  this.panel = null;
  this.win = null;

  if(!this.win)   {
    var config_store = new Ext.data.ArrayStore({
    fields: ['config-name'],
    data:   [['LHCb'], ['VELO'], ['CALO'], ['Default'] ]
    });

    win = new Ext.Window({
      id:            'logon-win',
      title:         'Logon Window',
      layout:        'fit',
      expandOnShow:   true,
      monitorResize:  true,
      closeAction:   'close',
      iconCls:       'user'
    });
    this.panel = new Ext.app.FormPanel({
         frame:        true,
         border:      'none',
	 id:          'logon-form',
         url:         'save-form.php',
         header:       false,
	 defaultType: 'textfield',
         style:       'padding: 0px',
	 window:       win,
         onLogon:     function()  {
	   var session = '';
	   var url = 'desk.htm';
	   var config = this.items.items[0].getValue();
	   /*
	     var session = this.items.items[0].getValue();
	     //var name = this.items.items[1].getValue();
	     Ext.state.Manager.set('session_name',session);
	   */
	   Ext.state.Manager.set('session_config',config);
	   if ( config.length > 0 ) {
	     if ( config != 'Default' ) {
	       url += '?config='+config;
	     }
	     if ( session.length > 0 ) url += '&session='+session;
	   }
	   else if ( session.length > 0 )  {
	     url += '?session='+session;
	   }
	   //alert(url);
	   window.location.assign(url);
	 },

	 onAbout: function() {
           var w = new Ext.app.About();
	   w.delegate();
	 },

  	 init:         function() {
	   var itm = new Ext.form.ComboBox({
                       fieldLabel:   'Config',
                       store:         config_store,
  	  	       emptyText:    'Select...',
                       displayField: 'config-name',
                       name:         'config',
                       typeAhead:     true,
                       mode:         'local',
                       triggerAction:'all',
		       stateful:      true,
		       stateId:      'selected_config',
                       labelStyle:   'padding: 0 0 0 20px',
                       selectOnFocus: true,
                       value:         Ext.state.Manager.get('session_config','')
		     });
	   this.add(itm);
	   itm = new Ext.Panel({
                       fieldLabel:   '',
                       title:        'Help',
		       width:         270,
		       html:          help_text,
		       parent:        this,
                       collapsible:   true,
		       collapsed:     true,
		       autoScroll:    true
		       });
           itm.on('beforecollapse', function() {this.setHeight(150);return true;}, win);
	   itm.on('beforeexpand',   function() {this.setHeight(250);return true},  win);
	   this.add(itm);
	   this.addButton({text:'Logon'}, this.onLogon, this);
	   this.addButton({text:'Back'},  function(){window.history.back();}, this);
	   this.addButton({text:'About'}, this.onAbout, this);
	 }
     });
    win.add(this.panel);
    win.setSize(350,150);
    win.show(this);
  }
  return this;
};

Ext.onReady(function(){
  var panel = null;
  if ( !panel ) panel = new Ext.LogonPanel();
});
