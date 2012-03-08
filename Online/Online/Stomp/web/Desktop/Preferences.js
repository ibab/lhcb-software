
Ext.app.Preferences = Ext.extend(Ext.app.Module, {
  moduleType : 'system/preferences',
  id:          'Preferences',
  menuPath:    'ToolMenu',

  init: function() {
    this.launcher =   {
      iconCls:         'pref-icon',
      shortcutIconCls: 'pref-shortcut-icon',
      text:            'Preferences',
      tooltip:       '<b>Preferences</b><br />Allows you to modify your desktop',
      handler:          this.createWindow,
      scope:            this
    }
  },

  actions : null,
  cards : [
        'pref-win-card-1', // navigation
        'pref-win-card-2', // quickstart
        'pref-win-card-3', // color and appearance
        'pref-win-card-4', // wallpaper
        'pref-win-card-5', // autorun
        'pref-win-card-6'  // shortcuts
  ],
  contentPanel : null,
  cardHistory : [
      'pref-win-card-1' // default
  ],
  layout: null,
  win : null,

  createWindow: function(){
    var launch = this.app.launcher;
    var desktop = this.app.getDesktop();
    this.win = desktop.getWindow(this.id);
    if( !this.win )   {
      var winWidth = 610;
      var winHeight = 460;
      this.contentPanel = new Ext.Panel({
        activeItem: 	0,
        border: 	false,
        id: 		'pref-win-content',
        layout: 'card',
        tbar: [{
                  disabled: 	true,
                  handler: 	this.navHandler.createDelegate(this, [-1]),
                  id: 		'back',
                  scope: 	this,
                  text: 	'Back'
                },
		{
                  disabled: 	true,
                  handler: 	this.navHandler.createDelegate(this, [1]),
                  id: 		'next',
                  scope: 	this,
                  text: 	'Next'
        }],
        items: [
           new Ext.app.Preferences.NavPanel(	{owner: this, id: 'pref-win-card-1'})
       	  ,new Ext.app.Preferences.Shortcuts(   {owner: this, id: 'pref-win-card-6', _items:launch.shortcut}  )
          ,new Ext.app.Preferences.AutoRun(	{owner: this, id: 'pref-win-card-5', _items:launch.autorun}   )
       	  ,new Ext.app.Preferences.QuickStart(  {owner: this, id: 'pref-win-card-2', _items:launch.quickstart})
          ,new Ext.app.Preferences.Appearance(  {owner: this, id: 'pref-win-card-3'})
          ,new Ext.app.Preferences.Background(  {owner: this, id: 'pref-win-card-4'})
        ]
      });

      this.win = desktop.createWindow({
              animCollapse: 	false,
              constrainHeader:	true,
              id: 		this.id,
              height: 		winHeight,
              iconCls: 		'settings16',
              items: 		[this.contentPanel],
              layout: 		'fit',
              shim: 		false,
              taskbuttonTooltip: '<b>Preferences</b><br />Allows you to modify your desktop',
              title: 		'Preferences',
              width: 		winWidth
       });
      this.layout = this.contentPanel.getLayout();
    }
    this.win.show();
  },
    
  handleButtonState : function(){
    var cards = this.cardHistory, activeId = this.layout.activeItem.id,
        items = this.contentPanel.getTopToolbar().items, back = items.get(0), next = items.get(1);
      
    for(var i = 0, len = cards.length; i < len; i++){
      if(cards[i] === activeId){
        if(i <= 0){
          back.disable();
          next.enable();
        }else if(i >= (len-1)){
          back.enable();
          next.disable();
        }else{
          back.enable();
          next.enable();
        }
        break;
      }
    }
  },
    
  navHandler : function(index){
    var cards = this.cardHistory,
        activeId = this.layout.activeItem.id,
        nextId;
      
    for(var i = 0, len = cards.length; i < len; i++){
      if(cards[i] === activeId){
        nextId = cards[i+index];
        break;
      }
    }
    this.layout.setActiveItem(nextId);
    this.handleButtonState();
  },
    
  save : function(params){      
    var desktop = this.app.getDesktop();
    var notifyWin = desktop.showNotification({ html: 'Saving your data...', title: 'Please wait'});
    var callback = params.callback || null;
    var callbackScope = params.callbackScope || this;
    
    params.id = this.id;
    // alert('Saving data:'+Ext.app.inspect(params));
    if ( this.app.savePreferences(params) )
      saveComplete('Finished', 'Save complete.');
    else
      saveComplete('Error', 'Errors while saving preferences of type:'+params.action.substr(4));

    function saveComplete(title, msg){
      notifyWin.setIconClass('x-icon-done');
      notifyWin.setTitle(title);
      notifyWin.setMessage(msg);
      desktop.hideNotification(notifyWin);
      if(callback){
        callback.call(callbackScope);
      }
    }
  },
    
  doClose : function()  {
    this.win.close();
  },

  expandNodes: function (ms, ids){
    var nodes = new Array();
    for(var i = 0, len = ms.length; i < len; i++){
      if(ms[i].moduleType === 'menu'){
        /* nodes.push({
          leaf: false,
          text: ms[i].launcher.text,
          children: this.expandNodes(o.menu._items, ids)
        }); */
      }
      else   {
        nodes.push({
           checked: 	this.isChecked(ms[i].id, ids) ? true : false,
           iconCls: 	ms[i].launcher.iconCls,
           id: 		ms[i].id,
           leaf: 	true,
           selected: 	true,
           text: 	ms[i].launcher.text
        });
      }
    }
    return nodes;
  },

  isChecked: function (id, ids){
    for(var i = 0, len = ids.length; i < len; i++){
      if(id == ids[i]){
        return true;
      }
    }
  },

  viewCard : function(card){
    this.layout.setActiveItem(card);
      if(this.cardHistory.length > 1){
        this.cardHistory.pop();
      }
      this.cardHistory.push(card);
      this.handleButtonState();
  }
});

Ext.app.Preferences.NavPanel = Ext.extend(Ext.app.Panel, {
  title: 	'Preferences navigation panel',
  autoScroll: 	true,
  bodyStyle: 	'padding:15px',
  border: 	false,
  html: '<ul id="pref-nav-panel"> \
        <li> \
          <img src="'+Ext.BLANK_IMAGE_URL+'" class="icon-pref-autorun"/> \
          <a id="viewShortcuts" href="#">Shortcuts</a><br /> \
          <span>Choose which applications appear in your shortcuts.</span> \
        </li> \
        <li> \
          <img src="'+Ext.BLANK_IMAGE_URL+'" class="icon-pref-autorun"/> \
          <a id="viewAutoRun" href="#">Auto Run Apps</a><br /> \
          <span>Choose which applications open automatically once logged in.</span> \
        </li> \
        <li> \
          <img src="'+Ext.BLANK_IMAGE_URL+'" class="icon-pref-quickstart"/> \
          <a id="viewQuickstart" href="#">Quick Start Apps</a><br /> \
          <span>Choose which applications appear in your Quick Start panel.</span> \
        </li> \
        <li> \
          <img src="'+Ext.BLANK_IMAGE_URL+'" class="icon-pref-appearance"/> \
          <a id="viewAppearance" href="#">Window Color and Appearance</a><br /> \
          <span>Fine tune window color and style of your windows.</span> \
        </li> \
        <li> \
          <img src="'+Ext.BLANK_IMAGE_URL+'" class="icon-pref-wallpaper"/> \
          <a id="viewWallpapers" href="#">Desktop Background</a><br /> \
          <span>Choose from available wallpapers or colors to decorate you desktop.</span> \
        </li> \
      </ul>',
  
  actions: {
    'viewShortcuts': 	function(owner)	{      /* alert('Shortcuts'); */ owner.viewCard('pref-win-card-6');    },    
    'viewAutoRun': 	function(owner)	{      /* alert('Autorun');   */ owner.viewCard('pref-win-card-5');    },
    'viewQuickstart': 	function(owner)	{      /* alert('QuickStart');*/ owner.viewCard('pref-win-card-2');    },
    'viewAppearance': 	function(owner)	{      owner.viewCard('pref-win-card-3');    },
    'viewWallpapers': 	function(owner)	{      owner.viewCard('pref-win-card-4');    }
  },

  afterRender: function(){
    this.body.on({
      'mousedown': {
        fn: 		this.doAction,
        scope: 		this,
        delegate: 	'a'
      },
      'click': {
        fn: 		Ext.emptyFn,
        scope: 		null,
        delegate: 	'a',
        preventDefault: true
      }
    });    
    Ext.app.Preferences.NavPanel.superclass.afterRender.call(this); // do sizing calcs last
  },
  doAction : function(e, t){
    e.stopEvent();
    this.actions[t.id](this.owner);  // pass owner for scope
  }
});

Ext.app.Preferences.AutoRun =  Ext.extend(Ext.app.TreePanel, {
  title:           'Auto run applications at startup',
  bodyStyle:       'padding:10px',
  autoScroll:      true,
  border:          false,
  containerScroll: true,
  rootVisible: 	   false,
  frame:           true,
  cls: 		   'pref-card pref-check-tree',
  lines: 	   false,
  listeners: {
    'checkchange': function(node,checked) { this.onCheckChange(node,checked);}
  },

  init: function()  {
    this.app      = this.owner.app;
    this.nodes    = this.owner.expandNodes(this.app.modules, this._items);
    this.loader   = new Ext.tree.TreeLoader();
    var root      = new Ext.tree.AsyncTreeNode({
      text: 	'Auto Run Apps',
      children: this.nodes
    });
    this.setRootNode(root);
    this.addButton({text:      'Save',
		    disabled:  !this.app.isAllowedTo('saveAutorun', this.owner.id)},
 		    this.onSave,
		    this);
    this.addButton({text: 'Close'},this.owner.doClose,this.owner);
  },
  onCheckChange: function (node, checked){
    if(node.leaf && node.id){
      if(checked)
        this.app.getDesktop().addAutoRun(node.id, true);
      else
        this.app.getDesktop().removeAutoRun(node.id, true);
    }
    node.ownerTree.selModel.select(node);
  },
  onSave: function (button,event)  {
    button.disable();
    this.owner.save({
      action:        'saveAutorun',
      callback:      function(){  this.buttons[0].enable();   },
      callbackScope: this,
      ids:           Ext.encode(this._items)
    });
  }
});

Ext.app.Preferences.Shortcuts = Ext.extend(Ext.app.TreePanel, {
  title:           'Shortcuts',
  bodyStyle:       'padding:10px',
  autoScroll:      true,
  border:          false,
  containerScroll: true,
  rootVisible: 	   false,
  frame:           true,
  cls: 		   'pref-card pref-check-tree',
  lines: 	   false,
  listeners: { 	
    'checkchange': function(node,checked) { this.onCheckChange(node,checked);}
  },

  init: function(config)  {
    this.app      = this.owner.app;
    this.nodes    = this.owner.expandNodes(this.app.modules, this._items);
    this.loader   = new Ext.tree.TreeLoader();
    var root     = new Ext.tree.AsyncTreeNode({
      text: 'Shortcuts',
      children: this.nodes
    });
    this.setRootNode(root);
    this.addButton({text:      'Save',
		    disabled:  !this.app.isAllowedTo('saveShortcut', this.owner.id)},
		   this.onSave,
		   this);
    this.addButton({text: 'Close'},this.owner.doClose,this.owner);
  },

  onCheckChange: function (node, checked){
    if(node.leaf && node.id){
      if(checked)
        this.app.getDesktop().addShortcut(node.id, true);
      else
        this.app.getDesktop().removeShortcut(node.id, true);
    }
    node.ownerTree.selModel.select(node);
  },

  onSave: function ()   {
    this.buttons[0].disable();
    this.owner.save({
      action:         'saveShortcut',
      callback:       function(){   this.buttons[0].enable();     },
      callbackScope:  this,
      ids:            Ext.encode(this._items)
    });
  }
});

Ext.app.Preferences.QuickStart = Ext.extend(Ext.app.TreePanel, {
  title: 	   'Quick Start Apps',
  autoScroll: 	   true,
  bodyStyle: 	   'padding:10px',
  border: 	   false,
  containerScroll: true,
  frame:           true,
  cls: 		   'pref-card pref-check-tree',
  lines: 	   false,
  rootVisible:     false,
  listeners: {
    'checkchange': function(node,checked) { this.onCheckChange(node,checked);}
  },

  init: function()  {
    this.app      = this.owner.app;
    this.nodes    = this.owner.expandNodes(this.app.modules, this._items);
    this.loader   = new Ext.tree.TreeLoader();
    var root     = new Ext.tree.AsyncTreeNode({
      text: 'Quick Start Apps',
      children: this.nodes
    });
    this.setRootNode(root);
    this.addButton({text:      'Save',
		    disabled:  !this.app.isAllowedTo('saveQuickstart', this.owner.id)},
		   this.onSave,
		   this);
    this.addButton({text: 'Close'},this.owner.doClose,this.owner);
  },
  onCheckChange: function(node, checked)  {
    if(node.leaf && node.id){
      if(checked)
        this.app.getDesktop().addQuickStartButton(node.id, true);
      else
        this.app.getDesktop().removeQuickStartButton(node.id, true);
    }
    node.ownerTree.selModel.select(node);
  },
  onSave: function(button,event){
    button.disable();
    this.owner.save({
        action:        'saveQuickstart',
	callback:      function(){ this.buttons[0].enable();    },
        callbackScope: this,
        ids:           Ext.encode(this._items)
    });
  }
});

Ext.app.Preferences.Appearance = Ext.extend(Ext.app.Panel, {
  title:        'Window Color And Appearance',
  bodyStyle: 	'padding:50px',
  border:       false,
  cls: 	        'pref-card',
  layout:       'border',

  init: function() {
    this.app = this.owner.app;
    this.desktop = this.app.getDesktop();
    this.store = new Ext.data.JsonStore({
      id: 	'id',
      baseParams: {
        action: 'viewThemes',
        id: 	this.owner.id
      },
      fields:    ['id', 'name', 'pathtothumbnail', 'pathtofile'],
      root: 	'Images',
      url: 	'http://lbcomet.cern.ch/Online/Desktop/config/themes.json'
    });

    this.store.on('load', function(store, records){
      if(records)   {
        this.defaults.setTitle('Themes Available (' + records.length + ')');
        var id = this.app.styles.theme.id;
        if(id){
          this.view.select('theme-'+id);
        }
      }
    }, this);


    var tpl = new Ext.XTemplate(
      '<tpl for=".">',
        '<div class="pref-view-thumb-wrap" id="theme-{id}">',
          '<div class="pref-view-thumb"><img src="{pathtothumbnail}" title="{name}" /></div>',
        '<span>{shortName}</span></div>',
      '</tpl>',
      '<div class="x-clear"></div>'
    );
    this.view = new Ext.DataView({
      autoHeight:		true,
      cls: 			'pref-thumnail-view',
      emptyText: 		'No themes to display',
      itemSelector:		'div.pref-view-thumb-wrap',
      loadingText: 		'loading themes...',
      singleSelect: 		true,
      overClass:		'x-view-over',
      prepareData: 		function(data){
        data.shortName = Ext.util.Format.ellipsis(data.name, 17);
        return data;
      },
      store: 			this.store,
      tpl: 			tpl
    });
    this.view.on('selectionchange', this.onSelectionChange, this);
    
    this.defaults = new Ext.Panel({
      title: 		'Default Themes',
      bodyStyle: 	'padding:10px',
      id: 		'pref-theme-view',
      titleCollapse: 	true,
      animCollapse: 	false,
      baseCls:		'collapse-group',
      border: 		false,
      cls: 		'pref-thumbnail-viewer',
      collapsible: 	true,
      hideCollapseTool: true,
      items: 		[this.view],
      region:           'center'
    });
  
    this.themes = new Ext.Panel({
      id: 		'themes',
      autoScroll: 	true,
      bodyStyle: 	'padding:10px',
      border: 		true,
      cls: 		'pref-card-subpanel',
      items: 		[this.defaults],
      margins: 		'10 15 0 15',
      region: 		'center'
    });

    this.formPanel = new Ext.FormPanel({
      id:     'pref-theme-transparency-slider',
      border: false,
      height: 70,
      bodyStyle: 	'padding:10px',
      layout: 'absolute',
      split:   false,
      region: 'south'
    });
    this.insert(0,this.themes);
    this.insert(1,this.formPanel);

    this.slider = this.createSlider({
      handler: new Ext.util.DelayedTask(this.updateTransparency, this)
      , min: 0
      , max: 100
      , x: 15
      , y: 35
      , width: 100
    });
    this.formPanel.add({x: 15, y: 15, xtype: 'label', text: 'Taskbar Transparency'});
    this.formPanel.add(this.slider.slider);
    this.formPanel.add(this.slider.display);
    this.addButton({text:      'Save',
		    disabled:  !this.app.isAllowedTo('saveAppearance', this.owner.id)},
		   this.onSave,
		   this);
    this.addButton({text: 'Close'},this.owner.doClose,this.owner);
  },

  // private functions
  createSlider: function(config){
    var handler = config.handler, min = config.min, max = config.max
      , width = config.width || 100, x = config.x, y = config.y;

    var slider = new Ext.Slider({
        minValue: 	  min
      , maxValue: 	  max
      , width: 		  width
      , x: 		  x
      , y:  		  y
    });
    
    var display =  new Ext.form.NumberField({
        cls: 		'pref-percent-field'
      , enableKeyEvents: true
      , maxValue: 	 max
      , minValue: 	 min
      , width: 		 45
      , x: 		 x + width + 15
      , y: 		 y - 1
    });
      
    var sliderHandler = function(slider){
      var v = this.slider.slider.getValue();
      display.setValue(v);
      handler.delay(100, null, null, [v]); // delayed task prevents IE bog
    }

    slider.on({
      'change': { fn: sliderHandler, scope: this }
      , 'drag': { fn: sliderHandler, scope: this }
    });
    
    display.on({
      'keyup': {
        fn: function(field){
          var v = field.getValue();
          if(v !== '' && !isNaN(v) && v >= field.minValue && v <= field.maxValue){
            this.slider.slider.setValue(v);
          }
        }
        , buffer: 350
        , scope: this
      }
    });
    return { slider: slider, display: display }
  },

  onSave: function()  {
    var c = this.app.styles;    
    this.buttons[0].disable();
    this.owner.save({
          action:               'saveAppearance'
        , callback:             function(){  this.buttons[0].enable(); }
        , callbackScope: 	this
        , backgroundcolor: 	c.backgroundcolor
        , fontcolor: 		c.fontcolor
        , theme: 		c.theme.id
        , transparency: 	c.transparency
        , wallpaper: 		c.wallpaper.id
        , wallpaperposition: 	c.wallpaperposition
      });
  },
  onSelectionChange: function(view, sel)   {
    if(sel.length > 0){
      var cId = this.app.styles.theme.id,
        r = view.getRecord(sel[0]),
        d = r.data;
      
      if(parseInt(cId) !== parseInt(r.id)){
        if(r && r.id && d.name && d.pathtofile){
          this.desktop.setTheme({
            id: r.id,
            name: d.name,
            pathtofile: d.pathtofile
          });
        }
      }
    }
  },
  updateTransparency: function(v)   {
    this.desktop.setTransparency(v);
  },
  afterRender: function()  {
    Ext.app.Preferences.Appearance.superclass.afterRender.call(this);
    this.on('show', this.loadStore, this, {single: true});
  },  
  loadStore : function()   {
    this.store.load();
    var t = this.app.styles.transparency;
    this.slider.slider.setValue(t);
  }
});

Ext.app.Preferences.Background = Ext.extend(Ext.app.Panel,  {
  title: 'Desktop Background',
  border: false,
  cls: 'pref-card',
  layout: 'border',
  
  init: function()  {
    this.app = this.owner.app;
    this.desktop = this.app.getDesktop();
    this.store = new Ext.data.JsonStore({
      baseParams: {
        action: 'viewWallpapers',
        id: this.owner.id
      },
      fields: ['id', 'name', 'pathtothumbnail', 'pathtofile'],
      id: 	'id',
      root: 	'Images',
      url: 	'http://lbcomet.cern.ch/Online/Desktop/config/wallpapers.1280x1024.json'
    });
  
    this.store.on('load', function(store, records){
      //alert('records:'+records);
      if(records){
        defaults.setTitle('Default Wallpapers (' + records.length + ')');      
        var id = this.app.styles.wallpaper.id;
        if(id){
          view.select('wallpaper-'+id);
        }
      }        
    }, this);

    var tpl = new Ext.XTemplate(
      '<tpl for=".">',
        '<div class="pref-view-thumb-wrap" id="wallpaper-{id}">',
          '<div class="pref-view-thumb"><img src="{pathtothumbnail}" title="{name}" /></div>',
        '<span>{shortName}</span></div>',
      '</tpl>',
      '<div class="x-clear"></div>'
    );

    var view = new Ext.DataView({
      autoHeight:	true,
      cls: 		'pref-thumnail-view',
      emptyText: 	'No wallpapers to display',
      itemSelector:	'div.pref-view-thumb-wrap',
      loadingText: 	'loading...',
      singleSelect: 	true,
      overClass:	'x-view-over',
      store: 		this.store,
      tpl: 		tpl,
      prepareData: 	function(data){
	//alert(data);
        data.shortName = Ext.util.Format.ellipsis(data.name, 17);
        return data;
      }
    });
    view.on('selectionchange', this.onSelectionChange, this);
  
    var defaults = new Ext.Panel({
      id: 		'pref-wallpaper-view',
      animCollapse: 	false,
      baseCls:		'collapse-group',
      border: 		false,
      cls: 		'pref-thumbnail-viewer',
      collapsible: 	true,
      hideCollapseTool: true,
      items: 		view,
      title: 		'Default Wallpapers',
      titleCollapse: 	true
    });
  
    var wallpapers = new Ext.Panel({
      id: 		'wallpapers',
      autoScroll: 	true,
      bodyStyle: 	'padding:10px',
      border: 		true,
      cls: 		'pref-card-subpanel',
      items: 		defaults,
      margins: 		'10 15 0 15',
      region: 		'center'
    });
  
    var wpp = this.app.styles.wallpaperposition;
    var tileRadio = this.createRadio('tile', wpp == 'tile' ? true : false, 90, 40);
    var centerRadio = this.createRadio('center', wpp == 'center' ? true : false, 200, 40);
  
    var position = new Ext.FormPanel({
      border: false,
      height: 140,
      id: 'position',
      items: [
        {  items: {border: false, html:'How should the wallpaper be positioned?'},
           x: 15, y: 15, border: false
        },
        {  items: {border: false, html: '<img class="bg-pos-tile" src="'+Ext.BLANK_IMAGE_URL+'" width="64" height="44" border="0" alt="" />'},
           x: 15, y: 40, border: false
        },
        tileRadio,
        {  items: {border: false, html: '<img class="bg-pos-center" src="'+Ext.BLANK_IMAGE_URL+'" width="64" height="44" border="0" alt="" />'},
           x: 125, y: 40, border: false
        },
        centerRadio,
        {  items: {border: false, html:'Choose a background color'},
           x: 245, y: 15, border: false
        },
        {
          border: false,
        items: new Ext.Button({
          text: 	'Background color',
          handler: 	this.onChangeBgColor,
          scope: 	this
        }),
        x: 245,
        y: 40
      },
      {  items: {border: false, html:'Choose a font color'},
         x: 425, y: 15, border: false
      },{
        items: new Ext.Button({
          text: 	'Background color',
          handler: 	this.onChangeFontColor,
          scope: 	this
        }),
        x: 425, y: 40, border: false
        
    }],
    layout: 'absolute',
    region: 'south',
    split: false
    });

    this.insert(0,wallpapers);
    this.insert(1,position);
    this.addButton({text:      'Save',
		    disabled:  !this.app.isAllowedTo('saveBackground', this.owner.id)},
		   this.onSave,
		   this);
    this.addButton({text: 'Close'},this.owner.doClose,this.owner);
  },

  createRadio : function(value, checked, x, y)   {
    if(value){
      radio = new Ext.form.Radio({
        name: 'position',
        inputValue: value,
        checked: checked,
        x: x,
        y: y,
        parent: this
      });
      
      radio.on('check', this.togglePosition, radio);
      
      return radio;
    }
  },
    
  onChangeBgColor:  function()   {
      var dialog = new Ext.ux.ColorDialog({
      border: false
      , closeAction: 'close'
      , listeners: {
        'select': { fn: this.onColorSelect, scope: this, buffer: 350 }
      }
      , manager: this.desktop.getManager()
      , resizable: false
      , title: 'Color Picker'
    });
    dialog.show(this.app.styles.backgroundcolor);
  },
    
  onColorSelect:  function (p, hex){
    this.desktop.setBackgroundColor(hex);
  },
  
 onChangeFontColor:  function (){
      var dialog = new Ext.ux.ColorDialog({
      border: false
      , closeAction: 'close'
      , listeners: {
        'select': { fn: this.onFontColorSelect, scope: this, buffer: 350 }
      }
      , manager: this.app.getDesktop().getManager()
      , resizable: false
      , title: 'Color Picker'
    });
    dialog.show(this.app.styles.fontcolor);
    },
  
  onFontColorSelect : function (p, hex){
    desktop.setFontColor(hex);
  },
  
  onSave: function (){
    var c = this.app.styles;
    this.buttons[0].disable();
      this.owner.save({
        action: 		'saveBackground',
        callback: 		function(){ this.buttons[0].enable(); },
        callbackScope: 		this,
        backgroundcolor: 	c.backgroundcolor,
        fontcolor: 		c.fontcolor,
        theme: 			c.theme.id,
        transparency: 		c.transparency,
        wallpaper: 		c.wallpaper.id,
        wallpaperposition: 	c.wallpaperposition
      });
  },
  
  onSelectionChange: function (view, sel){
    if(sel.length > 0){
      var cId = this.app.styles.wallpaper.id,
        r = view.getRecord(sel[0]),
        d = r.data;
      
      if(parseInt(cId) !== parseInt(r.id)){
        if(r && r.id && d.name && d.pathtofile){
          this.desktop.setWallpaper({
            id: 	r.id,
            name: 	d.name,
            pathtofile: d.pathtofile
          });
        }
      }
    }
  },
  
  togglePosition: function (field, checked){
    if(checked === true)   {
      field.parent.desktop.setWallpaperPosition(field.inputValue);
    }
  },

  afterRender : function(){
    Ext.app.Preferences.Background.superclass.afterRender.call(this);    
    this.on('show', this.loadStore, this, {single: true});
  },
  
  loadStore : function(){
    this.store.load();
  }
});


/* 
 * Will ensure that the checkchange event is fired on 
 * node double click
 */
Ext.override(Ext.tree.TreeNodeUI, {
  toggleCheck : function(value){
    var cb = this.checkbox;
    if(cb){
      cb.checked = (value === undefined ? !cb.checked : value);
      this.fireEvent('checkchange', this.node, cb.checked);
    }
  }
});
