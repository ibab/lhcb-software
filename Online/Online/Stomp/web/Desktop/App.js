/*!
 * Ext JS Library 3.1.1
 * Copyright(c) 2006-2010 Ext JS, LLC
 * licensing@extjs.com
 * http://www.extjs.com/license
 */
Ext.app.__dump0 = function(o)  {
  m = 'Content of object:'+o+'\n';
  s = '';
  var cnt = 0;
  for (var i in o)   {
    s = s + i + '\n';
    cnt++;
    if ( cnt == 60 )  {
      alert(s);
      s = '';
      cnt = 0;
    }
  }
    alert(s);
}
Ext.app.inspect = function(o)  {
  m = 'Content of object:'+o+'\n';
  s = '';
  var cnt = 0;
  for (var i in o)   {
    s = s + i + ':' + o[i] + '\n';
    cnt++;
    if ( cnt == 60 )  {
      alert(s);
      s = '';
      cnt = 0;
    }
  }
  return s;
}
Ext.app.inspectObj = function(o)  {
  m = 'Content of object:'+o+'\n';
  s = '';
  var cnt = 0;
  for (var i in o)   {
    if ( (''+o[i]).indexOf('object')==0 ) continue;
    s = s + i + ':' + o[i] + '\n';
    cnt++;
    if ( cnt == 60 )  {
      alert(s);
      s = '';
      cnt = 0;
    }
  }
  return s;
}
Ext.app.__dump = function(o)  {
  alert(Ext.app.inspect(o));
}

Ext.state.Manager.setProvider(new Ext.state.CookieProvider());

Ext.app.App = function(cfg){
  Ext.apply(this, cfg);
  this.addEvents({
	'ready': 	true,
	'beforeunload': true
  });

  Ext.onReady(this.initApp, this);
};

Ext.extend(Ext.app.App, Ext.util.Observable, {
  isReady: false,

  startMenu: 		null,
  /**
   * Read-only. This app's launchers
   * @type object
   */
  launcher : 		null,
  /**
   * Read-only. This app's modules
   * @type array
   */
  modules : 		null,
   /**
   * Read-only. This app's styles
   * @type object
   */
  styles : 		null,
  /**
   * Read-only. This app's Start Menu config
   * @type object
   */
  startConfig : 	null,
  /**
   * Read-only. This app's Start Menu's items and toolItems configs.
   * @type object
   */
  startItemsConfig : 	null,
  /**
   * Read-only. This app's logout button config
   * @type object
   */
  logoutButtonConfig : 	null,

  savePreferences:  	function(params) {  return false; },
  initPreferences:  	Ext.emptyFn,

  getStyles :     	Ext.emtpyFn,
  getLogoutConfig : 	Ext.emptyFn,
  getModules :    	Ext.emptyFn,
  getStartConfig :  	Ext.emptyFn,

  initApp : function(){
    this.startConfig 	= this.startConfig   || this.getStartConfig();
    this.logoutConfig 	= this.logoutConfig || this.getLogoutConfig();
    this.desktop  	= new Ext.Desktop(this);
    this.launcher 	= this.desktop.taskbar.startMenu;
    this.launcher.quickstart = [];
    this.launcher.shortcut = [];
    this.launcher.autorun = [];
    this.styles = this.styles || this.getStyles();
    this.initPreferences();
      this.initStyles();

    this.modules = this.getModules();
    if(this.modules)   {
      this.initModules(this.modules);
    }
    this.startItemsConfig = this.startItemsConfig || this.getStartItemsConfig();
    this.initLaunchers();
    this.initLogout();
    this.init();

    Ext.EventManager.on(window, 'beforeunload', this.onUnload, this);
    this.fireEvent('ready', this);
    this.isReady = true;
  },

  init : Ext.emptyFn,

  initLogout : function(){
    if(this.logoutConfig){
      this.desktop.taskbar.startMenu.addTool(this.logoutConfig);
    }
    //alert('App.initLogout: Done.');
  },

  initStyles : function()   {
    var s = this.styles;
    if(!s){
      return false;
    }
    this.desktop.setBackgroundColor(s.backgroundcolor);
    this.desktop.setFontColor(s.fontcolor);
    this.desktop.setTheme(s.theme);
    this.desktop.setTransparency(s.transparency);
    this.desktop.setWallpaper(s.wallpaper);
    this.desktop.setWallpaperPosition(s.wallpaperposition);      
    //alert('App.initStyles: Done.');
    return true;
  },

  initModules : function(ms){
    var s = '';
    for(var i = 0, len = ms.length; i < len; i++){
      var m = ms[i];
      s = s + m.launcher.text + '\n';
      m.loaded = true;
      m.app = this;
    }
    //alert('Initialized the following modules:\n'+s);
  },
    
  initLaunchers : function(){
    //alert('App.initLaunchers: Starting...');
    var l = this.launcher;
    if(!l){
      return false;
    }
    if(l.contextmenu){
      this.initContextMenu(l.contextmenu);
    }
    if(l.quickstart){
      this.initQuickStart(l.quickstart);
    }
    if(l.shortcut){
      this.initShortcut(l.shortcut);
    }
    if(l.autorun){
      this.onReady(this.initAutoRun.createDelegate(this, [l.autorun]), this);
    }
    //alert('App.initLaunchers: Done.');
    return true;
    },

  /**
   * @param {array} mIds An array of the module ids to add to the Desktop Context Menu
   */
  initContextMenu : function(mIds){
    if(mIds){
      for(var i = 0, len = mIds.length; i < len; i++){
        this.desktop.addContextMenuItem(mIds[i]);
      }
    }
  },
  /**
   * @param {array} mIds An array of the module ids to add to the Desktop Shortcuts
   */
  initShortcut : function(mIds){
    if(mIds){
      //alert('adding '+mIds.length+' shortcuts...');
      for(var i = 0, len = mIds.length; i < len; i++){
        this.desktop.addShortcut(mIds[i], false);
      }
    }
  },
  /**
   * @param {array} mIds An array of the modulId's to add to the Quick Start panel
   */
  initQuickStart : function(mIds){
    //alert('initQuickStart:'+mIds.length);
    if(mIds){
      for(var i = 0, len = mIds.length; i < len; i++){
        this.desktop.addQuickStartButton(mIds[i], false);
      }
    }
  },
  /**
   * @param {array} mIds An array of the module ids to run when this app is ready
   */
  initAutoRun : function(mIds){
    //alert('initAutoRun:'+mIds.length);
    if(mIds){
      for(var i = 0, len = mIds.length; i < len; i++){
        var m = this.getModule(mIds[i]);
        if(m){
          m.autorun = true;
	  //alert('Start autorun app:'+m);
          this.createWindow(m.id);
        }
      }
    }
  },

  savePreferencesWithAJAX : function(params)  {
    if ( false )  {
    Ext.Ajax.request({
      url: this.app.connection,
      /* Could also pass moduleId and action in querystring like this
       * instead of in the params config option.
       *
       * url: this.app.connection+'?moduleId='+this.id+'&action=myAction', */
      params: params,
      success: function(o){
        if(o && o.responseText && Ext.decode(o.responseText).success){
          saveComplete('Finished', 'Save complete.');
        }else{
          saveComplete('Error', 'Errors encountered on the server.');
        }
      },
      failure: function(){
        saveComplete('Error', 'Lost connection to server.');
      },
      scope: this
    });
    }
    return false;
  },

  loadPreferencesWithCookie: function(tag)  {
    var i, p, v;
    var s = this.styles;
    p = Ext.state.Manager.get('lhcb_extjs_'+tag+'_Shortcut','');
    if ( p.length > 0 ) eval('this.launcher.shortcut='+p);
    p = Ext.state.Manager.get('lhcb_extjs_'+tag+'_Autorun','');
    if ( p.length > 0 ) eval('this.launcher.autorun='+p);
    p = Ext.state.Manager.get('lhcb_extjs_'+tag+'_Quickstart','');
    if ( p.length > 0 ) eval('this.launcher.quickstart='+p);
    p = Ext.state.Manager.get('lhcb_extjs_'+tag+'_Theme','');
    if ( p.length > 0 ) eval('s.theme='+p);
    p = Ext.state.Manager.get('lhcb_extjs_'+tag+'_Appearance','');
    if ( p.length > 0 )   {
      eval('v='+p);
      s.fontcolor = v.fontcolor;
      s.transparency = v.transparency;
      s.backgroundcolor = v.backgroundcolor;
    }
    p = Ext.state.Manager.get('lhcb_extjs_'+tag+'_Wallpaper','');
    if ( p.length > 0 )   {
      eval('s.wallpaper='+p);
      s.wallpaperposition = s.wallpaper.position;
    }
    return true;
  },

  savePreferencesWithCookie: function(params,tag)   {
    var save_all = false;
    var ms = this.modules;
    var auto_start = '[ ';
    var quick_start = '[ ';
    var short_cut = '[ ';
    for(var i = 0, len = ms.length; i < len; i++)   {
      var m = ms[i];
      if ( m.quickStartButton ) quick_start += '"'+m.id+'",';
      if ( m.autorun          ) auto_start += '"'+m.id+'",';
      if ( m.shortcut         ) short_cut += '"'+m.id+'",';
    }
    quick_start = quick_start.substr(0, quick_start.length-1)+']';
    auto_start  = auto_start.substr(0, auto_start.length-1)+']';
    short_cut   = short_cut.substr(0, short_cut.length-1)+']';

    //alert('Style:'+Ext.app.inspectObj(this.styles)+'\n'+Ext.app.inspectObj(this.styles.theme)+'\n'+Ext.app.inspectObj(this.styles.wallpaper)+'\n');
    if ( save_all || params.action == 'saveAutorun' )  {
      Ext.state.Manager.set('lhcb_extjs_'+tag+'_Autorun',auto_start);
      if ( !save_all ) return true;
    }
    if ( params.action == 'saveShortcut' )  {
      Ext.state.Manager.set('lhcb_extjs_'+tag+'_Shortcut',short_cut);
      if ( !save_all ) return true;
    }
    if ( params.action == 'saveQuickstart' )  {
      Ext.state.Manager.set('lhcb_extjs_'+tag+'_Quickstart',quick_start);
      if ( !save_all ) return true;
    }
    if ( params.action == 'saveAppearance' )  {
      var s = this.styles;
      var app='{ backgroundcolor:"'+s.backgroundcolor+'",fontcolor:"'+s.fontcolor+'",transparency:'+s.transparency+'}';
      var theme='{ id:'+s.theme.id+', name:"'+s.theme.name+'", pathtofile:"'+s.theme.pathtofile+'"}';
      Ext.state.Manager.set('lhcb_extjs_'+tag+'_Appearance',app);
      Ext.state.Manager.set('lhcb_extjs_'+tag+'_Theme',theme);
      if ( !save_all ) return true;
    }
    if ( params.action == 'saveBackground' )  {
      var s = this.styles.wallpaper;
      var bg='{ id:'+s.id+', name:"'+s.name+'", pathtofile:"'+s.pathtofile+'", pathtothumbnail:"'+s.pathtothumbnail+'",position:"'+this.styles.wallpaperposition+'"}';
      Ext.state.Manager.set('lhcb_extjs_'+tag+'_Wallpaper',bg);
      if ( !save_all ) return true;
    }
    if ( save_all ) return true;
    return false;
  },

  /**
   * @param {Ext.app.Module} m The module to register
   */
  registerModule: function(m){
    if(!m){ return false; }
      this.modules.push(m);
      m.launcher.handler = this.createWindow.createDelegate(this, [m.moduleId]);
      m.app = this;
  },

  /** 
   * @param {string} v The moduleId or moduleType you want returned
   * @param {Function} cb The Function to call when the module is ready/loaded
   * @param {object} scope The scope in which to execute the function
   */
  requestModule : function(v, cb, scope){
    var m = this.getModule(v);    
    if(m){
      if(m.loaded === true){
       	cb.call(scope, m);
      }else{
	alert(m.moduleId+' cannot be loaded');
      }
    }
  },
  /**
   * @param {string} moduleId
   * 
   * Provides the handler to the placeholder's launcher until the module it is loaded.
   * Requests the module.  Passes in the callback and scope as params.
   */
  createWindow : function(moduleId){
      var m = this.requestModule(moduleId, function(m){
      if(m) m.createWindow();
    }, this);
  },
    
  getModule : function(name){
    var ms = this.modules;
    for(var i = 0, len = ms.length; i < len; i++){
      if(ms[i].id == name || ms[i].appType == name){
        return ms[i];
      }
    }
    return '';
  },

    /**
     * @param {string} action The module action
     * @param {string} moduleId The moduleId property
     */
    isAllowedTo : function(action, moduleId){
/*
    	var p = this.privileges, a = p[action];
    	if(p && a){
    		for(var i = 0, len = a.length; i < len; i++){
    			if(a[i] === moduleId){
    				return true;
    			}
    		}
    	}
*/    	
    	return true;
    },
    
    getDesktop : function(){
        return this.desktop;
    },
    
    /**
     * @param {Function} fn The function to call after the app is ready
     * @param {object} scope The scope in which to execute the function
     */
    onReady : function(fn, scope){
        if(!this.isReady){
            this.on('ready', fn, scope);
        }else{
            fn.call(scope, this);
        }
    },

    onReady : function(fn, scope){
      if(!this.isReady){
        this.on('ready', fn, scope);
      }else{
        fn.call(scope, this);
      }
    },

    getDesktop : function(){
      return this.desktop;
    },

    onUnload : function(e){
    if(this.fireEvent('beforeunload', this) === false){
        e.stopEvent();
      }
    }
});
