//------------------------------------------------------------------------------------------------------------------------------------------------
if ( !_lhcb().desktop ) {
  lhcb.desktop = function() {

    this.make_iframe = function(url)  {
      var ifr='<iframe id="myFrame" frameborder="0" vspace="0" hspace="0" marginwidth="0" marginheight="0" width="100%" height="100%" src="'+url+'" scrolling="auto" style="overflow:visible"></iframe>';
      return ifr;
    }

    this._makeIcon = function(img) {
      if ( img ) {
	if ( img.iconSize ) {
	  return '<img src="'+img.icon+'" width="'+img.iconSize.small.width+'" height="'+img.iconSize.small.height+'"/>';
	}
	return '<img src="'+img.icon+'" width="16" height="16"/>';
      }
      return '';
    }

    this._makeIconSize = function(img,size) {
      if ( size ) {
	return '<img src="'+img.icon+'" width="'+size.width+'" height="'+size.height+'"/>';
      }
      return lhcb.desktop._makeIcon(img);
    }

    this.URLModule = Ext.extend(Ext.app.Module, {
      url:        lhcb.constants.urls.empty,
      init:       function(){},
      _createWin: function()  {
	var desktop = this.app.getDesktop();
	var win = desktop.getWindow(this.url.nick);
	if(!win)   {
	  win = desktop.createWindow({id:              this.url.nick,
							 title:           this.url.title,
							 width:           this.url.size.width,
							 height:          this.url.size.height,
							 initHidden:      true,
							 html:            lhcb.desktop.make_iframe(this.url.src),
							 iconCls:         this.url.cls,
							 shim:            false,
							 animCollapse:    false,
							 autoScroll:      true,
							 layout:          'fit',
							 constrainHeader: true
							 });
	}
	win.show();
      },

      _launch: function(url) {
	this.url            = url;
	this.id             = url.nick;
	this.createWindow   = function(src) {this._createWin();};
	this.launcher       = {text:           '<img src="'+this.url.img.icon+'" width="16" height="16"/>&nbsp;&nbsp;'+this.url.title,
			       iconCls:        '',
			       handler:        this.createWindow,
			       scope:          this
	};
      }
    });

    this.menuWindowIndex = 0;

    this.URLMenuModule = Ext.extend(Ext.app.Module, {
      iconCls: '',
      img:     lhcb.constants.images.bogus,
      init: function()  {
	this.launcher = {
	  text:     'Window '+(++lhcb.desktop.menuWindowIndex),
	  iconCls:  'bogus',
	  handler : this.createWindow,
	  scope:    this,
	  windowId: lhcb.desktop.menuWindowIndex
	}
      },
      _launchSize: function(title,img,items,size) {
	this.img = img;
	if ( this.img ) {
	  this.text    = lhcb.desktop._makeIconSize(this.img,size)+'&nbsp;'+title;
	  this.iconCls = '';
	}
	else {
	  this.text    = title;
	  this.iconCls = 'bogus';
	}
	this.handler = function() { return false;  };
	this.launcher = {
	  text:    this.text,
	  iconCls: this.iconCls,
	  handler: function() { return false;  },
	  menu: {  items:  items  }
	}
      },
      _launch: function(title,img,items) {
	return this._launchSize(title,img,items,null);
      },
      createWindow : function(src){
	var desktop = this.app.getDesktop();
	var win = desktop.getWindow(src.id);
	if(!win){
	  win = desktop.createWindow({
          id:              src.id,
          title:           src.title,
	  width:           src.width,
          height:          src.height,
          html:            lhcb.desktop.make_iframe(src.url),
          iconCls:         'bogus',
          shim:            false,
          animCollapse:    false,
          constrainHeader: true
			     });
	}
	win.show();
      },
      menuItemId : function(title,id,url,width,height)   {
	var item = {
	  id:      id,
	  title:   title,
	  text:    lhcb.desktop._makeIcon(constants.images.bogus)+'&nbsp;&nbsp;&nbsp;'+title,
	  url:     url,
	  width:   width,
	  height:  height,
	  iconCls: 'bogus',
	  handler: this.createWindow,
	  scope:   this,
	  windowId: ++lhcb.desktop.menuWindowIndex,
	  
	  makeIcon: function(img) {
	    this.text = _makeIcon(img)+'&nbsp;'+this.title;
	    return this;
	  }
	}
	return item;
      },
      menuItem : function(title,url,width,height)   {
	var item = this.menuItemId(title,title,url,width,height);
	return item;
      },
      menuURL : function(url)   { 
	var item = this.menuItemId(url.title,url.nick,url.src,url.size.width,url.size.height);
	item.cls = '';
	return item;
      }
    });

    this.shortcut = function(url) {
      var w = 32, h = 32;
      var cut = document.createElement('dt');
      var ref = document.createElement('a');
      ref.href = '#';
      cut.id = url.nick+'-shortcut';
      if ( url.img.iconSize )  {
	w = url.img.iconSize.large.width;
	h = url.img.iconSize.large.height;
      }
      ref.innerHTML='<img src="'+url.img.src+'" width="'+w+'" height="'+h+'"/><div>'+url.title+'</div>';
      cut.appendChild(ref);
      return cut;
    }

    this.defineLogo = function(url,target) {
      var p = document.getElementById('x-desktop');
      var a = document.createElement('a');
      a.href   = target;
      a.target = '_blank';
      a.innerHTML    = '<img style="float:right;margin:15px;" src="'+url+'" width="80"/>';
      p.appendChild(a);
      return p;
    }

    return this;
  }();
}
