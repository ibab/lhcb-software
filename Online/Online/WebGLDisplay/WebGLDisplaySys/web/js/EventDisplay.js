var software = ! Detector.webgl;
//software = true;
var scene = new THREE.Scene();
//if (!software)
//  scene.fog = new THREE.FogExp2( 0x000000, 0.01 );


var logoscene = new THREE.Scene();
var logocamera = new THREE.OrthographicCamera( - window.innerWidth / 2.0,  window.innerWidth / 2.0,  window.innerHeight / 2.0, -  window.innerHeight / 2.0, 1.0, 10.0 );
logocamera.position.z = 10.0;
var logosprite, logomaterial, logowidth=0.2*window.innerWidth, logoheight=0.0;
var logomap = THREE.ImageUtils.loadTexture( "logo.png", {}, function() {
    logomaterial = new THREE.SpriteMaterial( { map: logomap, color: 0xffffff } );
    logosprite = new THREE.Sprite( logomaterial );
    logoheight = logowidth * logomaterial.map.image.height/logomaterial.map.image.width;
    logosprite.scale.set( logowidth, logoheight, 1.0 );
    logosprite.position.set( -window.innerWidth/2.0 + 0.55*logowidth,   window.innerHeight/2.0 - 0.55*logoheight, 1.0 );
    logoscene.add(logosprite);
} );
var textcanvas = document.createElement('canvas');
var textheight = 50;
var textcontext = textcanvas.getContext('2d');
textcontext.font = textheight + "px Lucida Grande, sans serif";
//textmetrics = textcontext.measureText("Thu, 30 Sep 2015 23:45:00 ");
textmetrics = textcontext.measureText("XXX, XX XXX 2015 XX:XX:XX");
var textwidth = textmetrics.width;
textcanvas.width = textwidth;
textcanvas.height = 4.0*textheight;
textcontext.textBaseline = "middle";
textcontext.fillStyle = "#ffffff";
//textcontext.fillStyle = logomaterial.color;
textcontext.font = textheight + "px Lucida Grande, sans serif";
textcontext.fillText("Event number N/A", 0.0, 0.5*textheight);
textcontext.fillText("Run number N/A", 0.0, 2.0*textheight);
textcontext.fillText("Time stamp N/A", 0.0, 3.5*textheight);
var texttexture = new THREE.Texture(textcanvas);
texttexture.needsUpdate = true;
var textmaterial = new THREE.SpriteMaterial( { map: texttexture, useScreenCoordinates: false });
var textsprite = new THREE.Sprite( textmaterial );
textsprite.scale.set(logowidth, logowidth*textcanvas.height/textcanvas.width, 1.0);
textsprite.position.set( -window.innerWidth/2.0 + 0.55*logowidth, window.innerHeight/2.0 - 1.0*logowidth, 1.0 );
logoscene.add(textsprite);

var ortho = false;
var orthowidth = 20.0;
var aspectratio = window.innerWidth/window.innerHeight;

var camera;
camera = new THREE.CombinedCamera( orthowidth, orthowidth/aspectratio, 75.0, 0.01, 1000, -10000.0, 10000.0);
camera.position.set(-7.5,7.5,2.5);
camera.up.set( 0.0, 1.0, 0.0);
camera.toPerspective();

var renderer;
//if ( ! Detector.webgl ) Detector.addGetWebGLMessage();
if ( !software )
    renderer = new THREE.WebGLRenderer( {antialias: true, preserveDrawingBuffer: true, clearColor: 0x000000, clearAlpha: 1  }); //  {antialias: true} { clearColor: 0x000000, clearAlpha: 1 } 
else 
    renderer = new THREE.CanvasRenderer();
renderer.autoClear = false;

renderer.setSize(window.innerWidth, window.innerHeight);
document.body.appendChild(renderer.domElement);

var softwarning = document.createElement('div');
softwarning.style.position = 'absolute';
softwarning.innerHTML = "You are using the software renderer. For better performance use a WebGL capable browser!";
softwarning.style.textAlign = 'center';
softwarning.style.top = '0px';
softwarning.style.width = '300px';
softwarning.style.marginLeft = '-150px';
softwarning.style.left = '50%';
softwarning.style.fontSize = '9.5px';
softwarning.style.color = '#fff';
softwarning.style.opacity = '0.97';
softwarning.style.fontFamily = 'Lucida Grande, sans-serif';
softwarning.style.padding = '5px';
if (software)      
    document.body.appendChild(softwarning);

//var cycletime = 5000;
var updatetime = 3000;//try all 3 sec
//var autoupdate = true;
var autorotate = false;
var displaydetector = true && !software;
var displaylogo = true;
//var displayplane = false && !software;
var wiremode = false;

//global material definitions
var otmaterial = new THREE.LineBasicMaterial( { color: 0x0000a0, opacity: 0.5, linewidth: 1, transparent: true, blending: THREE.AdditiveBlending, depthWrite: false, depthTest: true } );
var itmaterial = new THREE.LineBasicMaterial( { color: 0xa000a0, opacity: 0.5, linewidth: 1, transparent: true, blending: THREE.AdditiveBlending, depthWrite: false, depthTest: true  } );
var ttmaterial = new THREE.LineBasicMaterial( { color: 0xa000a0, opacity: 0.5, linewidth: 1, transparent: true, blending: THREE.AdditiveBlending, depthWrite: false, depthTest: true  } );
var velomaterial = new THREE.LineBasicMaterial( { color: 0xa000a0, opacity: 0.5, linewidth: 1, transparent: true, blending: THREE.AdditiveBlending, depthWrite: false, depthTest: true  } );
var muonmaterial = new THREE.MeshLambertMaterial({color: 0x00ff00, shading: THREE.FlatShading, wireframe: false, overdraw: false, doubleSided: true, transparent: false });
var ecalmaterial = new THREE.MeshLambertMaterial({color: 0x0000ff, shading: THREE.FlatShading, wireframe: false, overdraw: false, doubleSided: true, transparent: false });
var hcalmaterial = new THREE.MeshLambertMaterial({color: 0xff8000, shading: THREE.FlatShading, wireframe: false, overdraw: false, doubleSided: true, transparent: false });

var muonedges, ecaledges, hcaledges;

var pvmaterial = new THREE.MeshBasicMaterial( { color: 0xff00ff, shading: THREE.FlatShading, blending: THREE.AdditiveBlending, wireframe: false, overdraw: false, doubleSided: false, transparent: true, opacity: 0.8 } );
var fourprongmaterial = new THREE.MeshBasicMaterial( { color: 0xff00ff, shading: THREE.FlatShading, blending: THREE.AdditiveBlending, wireframe: false, overdraw: false, doubleSided: false, transparent: true, opacity: 0.8, visible: true } );
var threeprongmaterial = new THREE.MeshBasicMaterial( { color: 0xff00ff, shading: THREE.FlatShading, blending: THREE.AdditiveBlending, wireframe: false, overdraw: false, doubleSided: false, transparent: true, opacity: 0.8, visible: false} );
var twoprongmaterial = new THREE.MeshBasicMaterial( { color: 0xff00ff, shading: THREE.FlatShading, blending: THREE.AdditiveBlending, wireframe: false, overdraw: false, doubleSided: false, transparent: true, opacity: 0.8 , visible: false} );
//var pimaterial = new THREE.LineBasicMaterial( { color: 0xff8000, opacity: 1.0, linewidth: 2 } );
//var kmaterial = new THREE.LineBasicMaterial( { color: 0xff0000, opacity: 1.0, linewidth: 2 } );
//var pmaterial = new THREE.LineBasicMaterial( { color: 0xff00ff, opacity: 1.0, linewidth: 2 } );
//var mumaterial = new THREE.LineBasicMaterial( { color: 0x00ff00, opacity: 1.0, linewidth: 2 } );
//var ematerial = new THREE.LineBasicMaterial( { color: 0x0505ff, opacity: 1.0, linewidth: 2 } );

//var vtrackmaterial = new THREE.LineBasicMaterial( { color: 0x303030, opacity: 1.0, linewidth: 2 } );      
var pvsvfourmaterial = new THREE.LineDashedMaterial( { color: 0xff00ff, dashSize: 0.75e-3, gapSize: 0.5e-3, scale: 1.0, linewidth: 2, visible: true } );
var pvsvthreematerial = new THREE.LineDashedMaterial( { color: 0xff00ff, dashSize: 0.75e-3, gapSize: 0.5e-3, scale: 1.0, linewidth: 2, visible: false } );
var pvsvtwomaterial = new THREE.LineDashedMaterial( { color: 0xff00ff, dashSize: 0.75e-3, gapSize: 0.5e-3, scale: 1.0, linewidth: 2, visible: false } );

var detectoralpha = 0.05;
var detectormaterial = new THREE.MeshLambertMaterial({color: 0x40d0ff, blending: THREE.NormalBlending, shading: THREE.FlatShading, wireframe: wiremode, overdraw: false, doubleSided: true, transparent: true, opacity: detectoralpha, depthWrite: false, depthTest: true, side: THREE.DoubleSide });

var pimaterial = new THREE.ShaderMaterial( 
    {
        uniforms: {
            color: { type: "c", value: new THREE.Color( 0xff8000 ) }
        },
        attributes: {
            alpha: { type: 'f', value: [ ] }
        },
        vertexShader:   document.getElementById( 'vertexShader'   ).textContent,
        fragmentShader: document.getElementById( 'fragmentShader' ).textContent,
        side: THREE.FrontSide,      
        blending: THREE.NormalBlending, //THREE.AdditiveBlending,
        linewidth: 2,
        transparent: true,
        depthWrite: false,
        depthTest: true
    }   );
for (i = 0; i < 200; i++)
{
    pimaterial.attributes.alpha.value[i] = 1.0;
    if (i >= 170)//last 15% should fade out
  	pimaterial.attributes.alpha.value[i] = 1.0 - (i-169.0)/30.0;	
}

var kmaterial = pimaterial.clone();
kmaterial.uniforms = THREE.UniformsUtils.clone( pimaterial.uniforms );
kmaterial.attributes = THREE.UniformsUtils.clone( pimaterial.attributes );
kmaterial.uniforms.color.value = new THREE.Color( 0xff0000 );
var pmaterial = pimaterial.clone();
pmaterial.uniforms = THREE.UniformsUtils.clone( pimaterial.uniforms );
pmaterial.attributes = THREE.UniformsUtils.clone( pimaterial.attributes );
pmaterial.uniforms.color.value = new THREE.Color( 0xff00ff );
var mumaterial = pimaterial.clone();
mumaterial.uniforms = THREE.UniformsUtils.clone( pimaterial.uniforms );
mumaterial.attributes = THREE.UniformsUtils.clone( pimaterial.attributes );
mumaterial.uniforms.color.value = new THREE.Color( 0x00ff00 );
var ematerial = pimaterial.clone();
ematerial.uniforms = THREE.UniformsUtils.clone( pimaterial.uniforms );
ematerial.attributes = THREE.UniformsUtils.clone( pimaterial.attributes );
ematerial.uniforms.color.value = new THREE.Color( 0x0505ff );

//var vtrackmaterial = new THREE.LineBasicMaterial( { color: 0x303030, opacity: 1.0, linewidth: 2 } );      
var vtrackmaterial = pimaterial.clone();
vtrackmaterial.uniforms = THREE.UniformsUtils.clone( pimaterial.uniforms );
vtrackmaterial.attributes = THREE.UniformsUtils.clone( pimaterial.attributes );
vtrackmaterial.uniforms.color.value = new THREE.Color( 0x303030 );
for (i = 0; i < 200; i++)
{
    vtrackmaterial.attributes.alpha.value[i] = 1.0;
    if (i >= 5)
  	vtrackmaterial.attributes.alpha.value[i] = 0.0;		  
}
vtrackmaterial.attributes.alpha.value[3] = 0.66;		  
vtrackmaterial.attributes.alpha.value[4] = 0.33;		  
//vtrackmaterial.attributes.alpha.value[5] = 0.0;		  

//var pimaterial = new THREE.LineBasicMaterial( { color: 0xff8000, opacity: 1.0, linewidth: 2 } );
//var kmaterial = new THREE.LineBasicMaterial( { color: 0xff0000, opacity: 1.0, linewidth: 2 } );
//var pmaterial = new THREE.LineBasicMaterial( { color: 0xff00ff, opacity: 1.0, linewidth: 2 } );
//var mumaterial = new THREE.LineBasicMaterial( { color: 0x00ff00, opacity: 1.0, linewidth: 2 } );
//var ematerial = new THREE.LineBasicMaterial( { color: 0x0505ff, opacity: 1.0, linewidth: 2 } );

var controls = new THREE.OrbitControls( camera, renderer.domElement );
controls.target.set( 0.0, 0.0, 8.5);
//controls.addEventListener( 'change', render );
controls.maxPolarAngle = 0.5*Math.PI;
controls.autoRotate = autorotate;
controls.noPan = true;
controls.update();

//var logo = new THREE.Mesh();
var detector = new THREE.Mesh()
var loader = new THREE.BinaryLoader(true);
loader.load( "small.js", function(geometry, materials) {
    detector = new THREE.Mesh( geometry, detectormaterial );
    detector.geometry.computeFaceNormals();
    detector.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
    detector.rotation.set(0.0, Math.PI, 0.0);
    scene.add(detector);              
});

// add subtle ambient lighting
var ambientLight = new THREE.AmbientLight(0x040404);
scene.add(ambientLight);

// directional lighting
var directionalLighta = new THREE.DirectionalLight(0xffffff);
directionalLighta.position.set(1.0, 1.0, 1.0); //z is along beam axis, y is up, x is perpendicular, horizontal
scene.add(directionalLighta);

var directionalLightb = new THREE.DirectionalLight(0xffffff);
directionalLightb.position.set(-1.0, 1.0, -1.0);
scene.add(directionalLightb);

var currentevent = 0;
//this contains names of the currently available events on the server, first event is current
var eventlist = [];
//this is the current event
var theevent = {};

window.addEventListener( 'resize', onWindowResize, false );

var gui = new dat.GUI();
var params = {
    logo: displaylogo,
    detector: displaydetector,
    velohits: true, 
    tthits: true, 
    ithits: true, 
    othits: true, 
    ecalhits: true, 
    hcalhits: true, 
    muonhits: true, 
    wire: wiremode,
    rotate: autorotate,
    center : "Tracker",
    view : "Perspective",
    update: true, 
    alpha: detectoralpha,
    picolor: "#ff8000",
    kcolor: "#ff0000",
    pcolor: "#ff00ff",
    mucolor: "#00ff00",
    ecolor: "#0505ff",
    bkgcolor: "#000000",
    logocolor: "#ffffff",
    detcolor: "#40d0ff",
    width: 2.0,
    fov: 75.0, 
    screenshotscale: 1.0,
    pvs: true,
    vtracks: true,
    fourprongs: true,
    threeprongs: false,
    twoprongs: false,
    //screenshot: function() { window.open(renderer.domElement.toDataURL("image/jpeg")); },
    screenshot: function() { 

        renderer.setSize(params.screenshotscale*window.innerWidth, params.screenshotscale*window.innerHeight);
        //renderer.render( scene, camera );

        renderer.clear();
	renderer.render(scene, camera);
	if (displaylogo)
	{
	    renderer.clearDepth();
	    renderer.render( logoscene, logocamera );
	}


        window.open(renderer.domElement.toDataURL("image/png")); 
        renderer.setSize(window.innerWidth, window.innerHeight);
        //renderer.render( scene, camera );
        renderer.clear();
	renderer.render(scene, camera);
	if (displaylogo)
	{
	    renderer.clearDepth();
	    renderer.render( logoscene, logocamera );
	}
    },
    public: function() { 
        window.open("http://lhcb-public.web.cern.ch/lhcb-public"); 
    },
    next: function() { 
        params.update = false; 
        if (currentevent > 0)
	    currentevent -= 1;
	else
	    currentevent = eventlist.length-1;
	loadevent(eventlist[currentevent]);
    },
    prev: function() { 
        params.update = false; 
        if (currentevent < eventlist.length-1)
	    currentevent += 1;
	else
	    currentevent = 0;
	loadevent(eventlist[currentevent]);
    },			     
    download: function() { 
	//download current events json
        window.open(eventlist[currentevent]); 
    }
};

var epublic = gui.add(params, "public").name("LHCb public page");       
//epublic.domElement.parentNode.parentNode.style.backgroundPosition = "100px 50%";
//epublic.domElement.parentNode.parentNode.style.backgroundImage = "url(logo20.png)";
//epublic.domElement.parentNode.parentNode.style.backgroundRepeat = "no-repeat";
var fev = gui.addFolder('Event navigation');
var eupdate = fev.add(params, "update").name("Online mode").listen();//.onChange( function() { autoupdate = params.update; } );
var enext = fev.add(params, "next").name("Next event");
var eprev = fev.add(params, "prev").name("Previous event");

epublic.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
eupdate.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
enext.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
eprev.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
//enext.domElement.parentNode.parentNode.style.backgroundPosition = "100px 50%";
//enext.domElement.parentNode.parentNode.style.backgroundImage = "url(next16.png)";
//enext.domElement.parentNode.parentNode.style.backgroundRepeat = "no-repeat";
//eprev.domElement.parentNode.parentNode.style.backgroundPosition = "100px 50%";
//eprev.domElement.parentNode.parentNode.style.backgroundImage = "url(prev16.png)";
//eprev.domElement.parentNode.parentNode.style.backgroundRepeat = "no-repeat";
fev.open();

var fvis = gui.addFolder('Visualization');      
//var eplane = fvis.add(params, "plane").name("Show floor").onChange( function() { displayplane = params.plane; } );
var erotate = fvis.add(params, "rotate").name("Auto rotate").onChange( function() { autorotate = params.rotate; if (camera.inPerspectiveMode) controls.autoRotate = autorotate;} );
var ecenter = fvis.add(params, "center", ["VeLo", "Magnet", "Tracker", "Calorimeter"]).name("Center camera").onChange( function() { 
    if (params.center=="VeLo")
        controls.target.set( 0.0, 0.0, 0.0);
    if (params.center=="Magnet")
        controls.target.set( 0.0, 0.0, 5.0);
    if (params.center=="Tracker")
        controls.target.set( 0.0, 0.0, 8.5);
    if (params.center=="Calorimeter")
        controls.target.set( 0.0, 0.0, 13.5);
    if (params.view=="Top")
        camera.position.set(0.0, 5.0, controls.target.z);
    if (params.view=="Side")
        camera.position.set(-5.0, 0.0, controls.target.z);
    if (params.view=="Front")
        camera.position.set(0.0, 0.0, -10.0);
});
var eview = fvis.add(params, "view", ["Perspective", "Top", "Side", "Front"]).name("View").onChange( function() { 
    if (params.view=="Perspective")
    {
        //camera = new THREE.PerspectiveCamera(75, aspectratio, 0.1, 1000);
        camera.position.set(-10,0,-5);
        camera.up.set( 0.0, 1.0, 0.0);
        camera.toPerspective();
        //controls = new THREE.OrbitControls( camera, renderer.domElement );
        //controls.target.set( 0.0, 0.0, 8.5);
        if (params.center=="VeLo")
            controls.target.set( 0.0, 0.0, 0.0);
        if (params.center=="Magnet")
            controls.target.set( 0.0, 0.0, 5.0);
        if (params.center=="Tracker")
            controls.target.set( 0.0, 0.0, 8.5);
        if (params.center=="Calorimeter")
            controls.target.set( 0.0, 0.0, 13.5);
        controls.noPan = true;//true
        controls.noRotate = false;
        controls.autoRotate = autorotate;
        controls.update();
    }
    if (params.view=="Top")
    {
        camera.position.set(0.0, 5.0, 8.5);
        camera.up.set( 1.0, 0.0, 0.0);
        camera.toOrthographic();
        controls.target.set( 0.0, 0.0, 8.5);
        controls.noPan = false;
        controls.noRotate = true;
        controls.autoRotate = false;
        controls.update();
    }
    if (params.view=="Side")
    {
        camera.position.set(-5.0, 0.0, 8.5);
        camera.up.set( 0.0, 1.0, 0.0);
        camera.toOrthographic();
        controls.target.set( 0.0, 0.0, 8.5);
        controls.noPan = false;
        controls.noRotate = true;
        controls.autoRotate = false;
        controls.update();
    }
    if (params.view=="Front")
    {
        camera.position.set(0.0, 0.0, -10.0);
        camera.up.set( 0.0, 1.0, 0.0);
        camera.toOrthographic();
        controls.target.set( 0.0, 0.0, 8.5);
        controls.noPan = false;
        controls.noRotate = true;
        controls.autoRotate = false;
        controls.update();
    }
});
var efov = fvis.add(params, "fov", 40.0, 100.0).name("Field of view").onChange( function() { camera.fov = params.fov; if (camera.inPerspectiveMode) camera.toPerspective(); controls.update(); } );
var ebkgcolor = fvis.addColor(params, "bkgcolor").name("Bkg. color").onChange( function() { renderer.setClearColor( params.bkgcolor, 1 ); } );
var elogo = fvis.add(params, "logo").name("Show logo").onChange( function() { displaylogo = params.logo; } );
var elogocolor = fvis.addColor(params, "logocolor").name("Logo color").onChange( function() { 
    logomaterial.color.setHex(params.logocolor.replace( '#','0x' ));  
    //textcontext.fillStyle = params.logocolor;//logomaterial.color;

    logoscene.remove(textsprite);
    textcontext.clearRect ( 0 , 0 , textwidth, 4.0*textheight );
    textcontext.fillStyle = params.logocolor;//logomaterial.color;
    textcontext.fillText("Event "+theevent.eventNumber, 0.0, 0.5*textheight);
    textcontext.fillText("Run "+theevent.runNumber, 0.0, 2.0*textheight);
    textcontext.fillText(theevent.time, 0.0, 3.5*textheight);
    texttexture = new THREE.Texture(textcanvas);
    texttexture.needsUpdate = true;
    textmaterial = new THREE.SpriteMaterial( { map: texttexture, useScreenCoordinates: false });
    textsprite = new THREE.Sprite( textmaterial );
    textsprite.scale.set(logowidth, logowidth*textcanvas.height/textcanvas.width, 1.0);
    textsprite.position.set( -window.innerWidth/2.0 + 0.55*logowidth, window.innerHeight/2.0 - 1.0*logowidth, 1.0 );
    logoscene.add(textsprite);

} );
var escreenshot = fvis.add(params, "screenshot").name("Screenshot"); 
var escreenshotscale = fvis.add(params, "screenshotscale", 0.0, 5.0).name("Scaling"); 
var edownload = fvis.add(params, "download").name("Download .json");

//fvis.open();

var fdet = gui.addFolder('Detector');      
var edetector = fdet.add(params, "detector").name("Detector model").onChange( function() { displaydetector = params.detector;});
var ealpha = fdet.add(params, "alpha", 0, 1.0).name("Det. opacity").onChange( function() { detectoralpha = params.alpha; detectormaterial.opacity = params.alpha; } );
//
var edetcolor = fdet.addColor(params, "detcolor").name("Det. color").onChange( function() { 
    //detectormaterial.  params.detcolor; 
    detectormaterial.color.setHex(params.detcolor.replace( '#','0x' ));
} );

var ewire = fdet.add(params, "wire").name("Wireframe mode").onChange( function() { 
    wiremode = params.wire; 
    detectormaterial.wireframe = wiremode;
} );
var evelohits = fdet.add(params, "velohits").name("Vertex Locator hits").onChange( function() { velomaterial.visible = params.velohits;});
var etthits = fdet.add(params, "tthits").name("Trigger tracker hits").onChange( function() { ttmaterial.visible = params.tthits;});
var eithits = fdet.add(params, "ithits").name("Inner tracker hits").onChange( function() { itmaterial.visible = params.ithits;});
var eothits = fdet.add(params, "othits").name("Outer tracker hits").onChange( function() { otmaterial.visible = params.othits;});
var eecalhits = fdet.add(params, "ecalhits").name("ECal hits").onChange( function() { 
    ecalmaterial.visible = params.ecalhits;
    ecaledges.visible = params.ecalhits;
    //ecalmaterials[0].visible = params.ecalhits;	
    //ecalmaterials[1].visible = params.ecalhits;	
});
var ehcalhits = fdet.add(params, "hcalhits").name("HCal hits").onChange( function() { 
    hcalmaterial.visible = params.hcalhits;
    hcaledges.visible = params.hcalhits;
    //hcalmaterials[0].visible = params.hcalhits;
    //hcalmaterials[1].visible = params.hcalhits;
});
var emuonhits = fdet.add(params, "muonhits").name("Muon hits").onChange( function() { 
    muonmaterial.visible = params.muonhits;
    muonedges.visible = params.muonhits;
    //muon.material.visible = params.muonhits;
    //muonmesh.visible = params.muonhits;
    //muonmaterials[0].visible = params.muonhits;
    //muonmaterials[1].visible = params.muonhits;
});
//fdet.open();

edetector.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
ewire.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
evelohits.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
etthits.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
eithits.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
eothits.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
eecalhits.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
ehcalhits.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
emuonhits.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
efov.domElement.parentNode.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
elogo.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
erotate.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
ecenter.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
escreenshot.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
escreenshotscale.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
edownload.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
eview.domElement.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";

var fcolors = gui.addFolder('Particles');
var ewidth = fcolors.add(params, "width", 1.0, 10.0).name("Track width").onChange( function() { 
    pimaterial.linewidth = params.width;
    kmaterial.linewidth = params.width;
    ematerial.linewidth = params.width;
    pmaterial.linewidth = params.width;
    mumaterial.linewidth = params.width;
} );
var epicolor = fcolors.addColor(params, "picolor").name("Pions").onChange( function() { pimaterial.uniforms.color.value = new THREE.Color(params.picolor); } );
var ekcolor = fcolors.addColor(params, "kcolor").name("Kaons").onChange( function() { kmaterial.uniforms.color.value = new THREE.Color(params.kcolor); } );
var epcolor = fcolors.addColor(params, "pcolor").name("Protons").onChange( function() { pmaterial.uniforms.color.value = new THREE.Color(params.pcolor); } );
var emucolor = fcolors.addColor(params, "mucolor").name("Muons").onChange( function() { mumaterial.uniforms.color.value = new THREE.Color(params.mucolor); } );
var eecolor = fcolors.addColor(params, "ecolor").name("Electrons").onChange( function() { ematerial.uniforms.color.value = new THREE.Color(params.ecolor); } );
var epvs = fcolors.add(params, "pvs").name("Primary vertices").onChange( function() { pvmaterial.visible = params.pvs; } );
var evtracks = fcolors.add(params, "vtracks").name("Velo Tracks").onChange( function() { vtrackmaterial.visible = params.vtracks; } );
var efourprongs = fcolors.add(params, "fourprongs").name("Four prong decays").onChange( function() { fourprongmaterial.visible = params.fourprongs; pvsvfourmaterial.visible = params.fourprongs; } );
var ethreeprongs = fcolors.add(params, "threeprongs").name("Three prong decays").onChange( function() { threeprongmaterial.visible = params.threeprongs; pvsvthreematerial.visible = params.threeprongs; } );
var etwoprongs = fcolors.add(params, "twoprongs").name("Two prong decays").onChange( function() { twoprongmaterial.visible = params.twoprongs; pvsvtwomaterial.visible = params.twoprongs; } );

ewidth.domElement.parentNode.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
ealpha.domElement.parentNode.parentNode.parentNode.style.borderLeft = "3px solid #fffffe";
//fcolors.open();


var timebefore = -1.0;
//var cycledelta = 0.0;
var updatedelta = 0.0;

//render function

function render(catalog_url) {
    detector.visible = displaydetector;

    function render_wrapper() {
	render(catalog_url);
    }
    requestAnimationFrame(render_wrapper);
    controls.update();
    //stats.update();
    
    var timenow = Date.now();// this is in ms
    if (timebefore < 0.0)
	timebefore = timenow;
    else
    {
	var dt = timenow - timebefore;
	//cycledelta += dt;
	updatedelta += dt;
	timebefore = timenow;
    }

    if (updatedelta > updatetime && params.update)//autoupdate)
    {
	updatedelta = 0.0;
	loadeventlist(catalog_url);
    }

    renderer.clear();
    renderer.render(scene, camera);
    if (displaylogo)
    {
	renderer.clearDepth();
	renderer.render( logoscene, logocamera );
    }
};

//load names of events into eventlist, first event is to be displayed, remaining events can be traversed via next/prev
function loadeventlist( name ) {//this is polled every X seconds
    var namefile = new XMLHttpRequest();
    var fname = name+"?hash="+Math.floor(Math.random()*1.0e+10);
    namefile.open("GET", fname, true);
    namefile.onreadystatechange = function() {
        if (namefile.readyState == 4) {
            if (namefile.status == 200 || namefile.status == 0) {
		var parsedname = JSON.parse(namefile.responseText);            
		var i = 0;
		if (parsedname.length > 0)
	            eventlist = [];
		for (i in parsedname)
		{
	            eventlist.push(parsedname[i]);
		}      
		//alert(theevent.fileName+":"+eventlist[0]);
		if (eventlist.length > 0 && theevent.fileName != eventlist[0])
		{
	            //alert(theevent.fileName+":"+eventlist[0]);
	            currentevent = 0;
	            loadevent(eventlist[currentevent]);
		}
		//else
		//  alert("SAME");

            }
            else
		alert("Error loading file "+name+"\n");
        }
    }
    namefile.send(null);       
};

//remove current event from scene    
function addtoscene( ) {
    var i =0;
    for (i in theevent.lines)
	scene.add(theevent.lines[i]);
    for (i in theevent.meshes)
	scene.add(theevent.meshes[i]);        

    logoscene.remove(textsprite);
    textcontext.clearRect ( 0 , 0 , textwidth, 4.0*textheight );
    textcontext.fillStyle = params.logocolor;//logomaterial.color;
    textcontext.fillText("Event "+theevent.eventNumber, 0.0, 0.5*textheight);
    textcontext.fillText("Run "+theevent.runNumber, 0.0, 2.0*textheight);
    textcontext.fillText(theevent.time, 0.0, 3.5*textheight);
    texttexture = new THREE.Texture(textcanvas);
    texttexture.needsUpdate = true;
    textmaterial = new THREE.SpriteMaterial( { map: texttexture, useScreenCoordinates: false });
    textsprite = new THREE.Sprite( textmaterial );
    textsprite.scale.set(logowidth, logowidth*textcanvas.height/textcanvas.width, 1.0);
    textsprite.position.set( -window.innerWidth/2.0 + 0.55*logowidth, window.innerHeight/2.0 - 1.0*logowidth, 1.0 );
    logoscene.add(textsprite);

};

function removefromscene( ) {
    var i=0;
    for (i in theevent.lines)
        scene.remove(theevent.lines[i]);
    for (i in theevent.meshes)
        scene.remove(theevent.meshes[i]);        
};

function onWindowResize() {    
    camera.setSize(window.innerWidth, window.innerHeight);
    camera.updateProjectionMatrix();      

    logosprite.position.set( -window.innerWidth/2.0 + 0.55*logowidth,   window.innerHeight/2.0 - 0.55*logoheight, 1.0 );
    textsprite.position.set( -window.innerWidth/2.0 + 0.55*logowidth, window.innerHeight/2.0 - 1.0*logowidth, 1.0 );
    logocamera.left = - window.innerWidth / 2.0;
    logocamera.right = window.innerWidth / 2.0;
    logocamera.top = window.innerHeight / 2.0;
    logocamera.bottom = - window.innerHeight / 2.0;
    logocamera.updateProjectionMatrix();      

    renderer.setSize( window.innerWidth, window.innerHeight );      
    //controls.handleResize();      
};

function loadevent( filename ) {
    var jsonfile = new XMLHttpRequest();
    jsonfile.open("GET", filename, true);
    jsonfile.onreadystatechange = function() {
        if (jsonfile.readyState == 4) {
            if (jsonfile.status == 200 || jsonfile.status == 0) {
		var parsedevent = JSON.parse(jsonfile.responseText);            
		var i = 0;
		var l = 0;
		var event = {};
		var alllines = []; //just store everything in here...
		var allmeshes = [];
		//PARSE EVENT INFO
		event["fileName"] = filename;
		event["runNumber"] = parsedevent.runNumber;
		event["eventNumber"] = parsedevent.eventNumber;
		event["time"] = parsedevent.time;
		
		//PARSE PVS
		pvs = parsedevent.PVS;
		for (i in pvs)
		{
		    var pvgeometry = new THREE.SphereGeometry( 1.0, 24, 24 );//assuming this is in m
  	            var pv = pvs[i];						  
		    var mtot = new THREE.Matrix4();
		    var factor = 10.0;
		    var sigma0 = factor*1.0e-3*Math.sqrt(pv.eval_0);
		    var sigma1 = factor*1.0e-3*Math.sqrt(pv.eval_1);
		    var sigma2 = factor*1.0e-3*Math.sqrt(pv.eval_2);
                    //var mtot = new THREE.Matrix4
		    mtot.set(pv.evec_0x*sigma0, pv.evec_1x*sigma1, pv.evec_2x*sigma2, 1.0e-3*pv.pv_x,
			     pv.evec_0y*sigma0, pv.evec_1y*sigma1, pv.evec_2y*sigma2, 1.0e-3*pv.pv_y,
			     pv.evec_0z*sigma0, pv.evec_1z*sigma1, pv.evec_2z*sigma2, 1.0e-3*pv.pv_z,
			     0.0, 0.0, 0.0, 1.0);

     	            pvgeometry.applyMatrix( mtot );
		    //alert("m: "+m.elements[0]+", "+m.elements[1]+", "+m.elements[2]+", "+m.elements[3]+", "+m.elements[4]+", "+m.elements[5]+", "+m.elements[6]+", "+m.elements[7]+", "+m.elements[8]+", "+m.elements[9]+", "+m.elements[10]+", "+m.elements[11]+", "+m.elements[12]+", "+m.elements[13]+", "+m.elements[14]+", "+m.elements[15]);				  
	            allmeshes.push(new THREE.Mesh(pvgeometry, pvmaterial));
		    //alert("PVS!");
		}

		//PARSE SVS
		//fourprongs = parsedevent.FOURPRONG;
		//console.log(parsedevent.FOURPRONG);
		fourprongs = parsedevent.FOURPRONG;
		for (i in fourprongs)
		{
		    var svgeometry = new THREE.SphereGeometry( 1.0, 24, 24 );//assuming this is in m
  	            var sv = fourprongs[i];
		    var mtot = new THREE.Matrix4();
		    var factor = 10.0;
		    var sigma0 = factor*1.0e-3*Math.sqrt(sv.eval_0);
		    var sigma1 = factor*1.0e-3*Math.sqrt(sv.eval_1);
		    var sigma2 = factor*1.0e-3*Math.sqrt(sv.eval_2);
		    mtot.set(sv.evec_0x*sigma0, sv.evec_1x*sigma1, sv.evec_2x*sigma2, 1.0e-3*sv.sv_x,
			     sv.evec_0y*sigma0, sv.evec_1y*sigma1, sv.evec_2y*sigma2, 1.0e-3*sv.sv_y,
			     sv.evec_0z*sigma0, sv.evec_1z*sigma1, sv.evec_2z*sigma2, 1.0e-3*sv.sv_z,
			     0.0, 0.0, 0.0, 1.0);
     	            svgeometry.applyMatrix( mtot );
	            allmeshes.push(new THREE.Mesh(svgeometry, fourprongmaterial));
		    //adding line connecting SV with PV
		    var pvsvgeometry = new THREE.Geometry(); 
                    pvsvgeometry.vertices.push( new THREE.Vector3(1.0e-3*sv.pv_x, 1.0e-3*sv.pv_y, 1.0e-3*sv.pv_z) );
                    pvsvgeometry.vertices.push( new THREE.Vector3(1.0e-3*sv.sv_x, 1.0e-3*sv.sv_y, 1.0e-3*sv.sv_z) );
		    pvsvgeometry.computeLineDistances();
		    var pvsvline = new THREE.Line( pvsvgeometry, pvsvfourmaterial, THREE.LinePieces );
		    alllines.push(pvsvline);
		    //alert("CHECK");
		}

		threeprongs = parsedevent.THREEPRONG;
		for (i in threeprongs)
		{
		    var svgeometry = new THREE.SphereGeometry( 1.0, 24, 24 );//assuming this is in m
  	            var sv = threeprongs[i];
		    var mtot = new THREE.Matrix4();
		    var factor = 10.0;
		    var sigma0 = factor*1.0e-3*Math.sqrt(sv.eval_0);
		    var sigma1 = factor*1.0e-3*Math.sqrt(sv.eval_1);
		    var sigma2 = factor*1.0e-3*Math.sqrt(sv.eval_2);
		    mtot.set(sv.evec_0x*sigma0, sv.evec_1x*sigma1, sv.evec_2x*sigma2, 1.0e-3*sv.sv_x,
			     sv.evec_0y*sigma0, sv.evec_1y*sigma1, sv.evec_2y*sigma2, 1.0e-3*sv.sv_y,
			     sv.evec_0z*sigma0, sv.evec_1z*sigma1, sv.evec_2z*sigma2, 1.0e-3*sv.sv_z,
			     0.0, 0.0, 0.0, 1.0);
     	            svgeometry.applyMatrix( mtot );
	            allmeshes.push(new THREE.Mesh(svgeometry, threeprongmaterial));
		    //adding line connecting SV with PV
		    var pvsvgeometry = new THREE.Geometry(); 
                    pvsvgeometry.vertices.push( new THREE.Vector3(1.0e-3*sv.pv_x, 1.0e-3*sv.pv_y, 1.0e-3*sv.pv_z) );
                    pvsvgeometry.vertices.push( new THREE.Vector3(1.0e-3*sv.sv_x, 1.0e-3*sv.sv_y, 1.0e-3*sv.sv_z) );
		    pvsvgeometry.computeLineDistances();
		    var pvsvline = new THREE.Line( pvsvgeometry, pvsvthreematerial, THREE.LinePieces );
		    alllines.push(pvsvline);
		    //alert("CHECK");
		}

		twoprongs = parsedevent.TWOPRONG;
		for (i in twoprongs)
		{
		    var svgeometry = new THREE.SphereGeometry( 1.0, 24, 24 );//assuming this is in m
  	            var sv = twoprongs[i];
		    var mtot = new THREE.Matrix4();
		    var factor = 10.0;
		    var sigma0 = factor*1.0e-3*Math.sqrt(sv.eval_0);
		    var sigma1 = factor*1.0e-3*Math.sqrt(sv.eval_1);
		    var sigma2 = factor*1.0e-3*Math.sqrt(sv.eval_2);
		    mtot.set(sv.evec_0x*sigma0, sv.evec_1x*sigma1, sv.evec_2x*sigma2, 1.0e-3*sv.sv_x,
			     sv.evec_0y*sigma0, sv.evec_1y*sigma1, sv.evec_2y*sigma2, 1.0e-3*sv.sv_y,
			     sv.evec_0z*sigma0, sv.evec_1z*sigma1, sv.evec_2z*sigma2, 1.0e-3*sv.sv_z,
			     0.0, 0.0, 0.0, 1.0);
     	            svgeometry.applyMatrix( mtot );
	            allmeshes.push(new THREE.Mesh(svgeometry, twoprongmaterial));
		    //adding line connecting SV with PV
		    var pvsvgeometry = new THREE.Geometry(); 
                    pvsvgeometry.vertices.push( new THREE.Vector3(1.0e-3*sv.pv_x, 1.0e-3*sv.pv_y, 1.0e-3*sv.pv_z) );
                    pvsvgeometry.vertices.push( new THREE.Vector3(1.0e-3*sv.sv_x, 1.0e-3*sv.sv_y, 1.0e-3*sv.sv_z) );
		    pvsvgeometry.computeLineDistances();
		    var pvsvline = new THREE.Line( pvsvgeometry, pvsvtwomaterial, THREE.LinePieces );
		    alllines.push(pvsvline);
		    //alert("CHECK");
		}
		
		//PARSE OT
		othits = parsedevent.OT;
		var otgeometry = new THREE.Geometry(); 
		for (i in othits)
		{
                    var ab = othits[i];
                    otgeometry.vertices.push( new THREE.Vector3(ab[0], ab[1], ab[2]) );
                    otgeometry.vertices.push( new THREE.Vector3(ab[3], ab[4], ab[5]) );
		}
		var otline = new THREE.Line( otgeometry, otmaterial, THREE.LinePieces );
		otline.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
		alllines.push(otline);

		//PARSE IT
		ithits = parsedevent.IT;
		var itgeometry = new THREE.Geometry(); 
		for (i in ithits)
		{
                    var ab = ithits[i];
                    itgeometry.vertices.push( new THREE.Vector3(ab[0], ab[1], ab[2]) );
                    itgeometry.vertices.push( new THREE.Vector3(ab[3], ab[4], ab[5]) );
		}
		var itline = new THREE.Line( itgeometry, itmaterial, THREE.LinePieces );
		itline.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
		alllines.push(itline);

		//PARSE TT
		tthits = parsedevent.TT;
		var ttgeometry = new THREE.Geometry(); 
		for (i in tthits)
		{
                    var ab = tthits[i];
                    ttgeometry.vertices.push( new THREE.Vector3(ab[0], ab[1], ab[2]) );
                    ttgeometry.vertices.push( new THREE.Vector3(ab[3], ab[4], ab[5]) );
		}
		var ttline = new THREE.Line( ttgeometry, ttmaterial, THREE.LinePieces );
		ttline.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
		alllines.push(ttline);

		//PARSE VELO PHI
		velophihits = parsedevent.VELOPHI;
		var velophigeometry = new THREE.Geometry(); 
		for (i in velophihits)
		{
                    var ab = velophihits[i];
                    velophigeometry.vertices.push( new THREE.Vector3(ab[0], ab[1], ab[2]) );
                    velophigeometry.vertices.push( new THREE.Vector3(ab[3], ab[4], ab[5]) );
		}
		var velophiline = new THREE.Line( velophigeometry, velomaterial, THREE.LinePieces );
		velophiline.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
		alllines.push(velophiline);

		//PARSE VELO PHI
		velorhits = parsedevent.VELOR;
		//var velorgeometry = new THREE.Geometry(); 
		for (i in velorhits)
		{
                    var ab = velorhits[i];
		    var a = new THREE.Vector3(ab[0], ab[1], ab[2]);
		    var b = new THREE.Vector3(ab[3], ab[4], ab[5]);
		    //var pivot = new THREE.Vector3(0.0, 0.0, ab[5]);
		    var r = Math.sqrt(a.x*a.x+a.y*a.y);
		    var phia = Math.atan2(a.y, a.x);
		    var phib = Math.atan2(b.y, b.x);
		    var dphi = phib-phia;
		    if (dphi > Math.PI)
			dphi = (2.0*Math.PI-dphi);
		    if (dphi < -Math.PI)
			dphi = (2.0*Math.PI+dphi);
		    //console.log(phia+";"+phib+";"+dphi);
		    var velorgeometry = new THREE.Geometry();
		    for (j = 0; j < 11; j++)
			velorgeometry.vertices.push(new THREE.Vector3(r*Math.cos(phia+dphi/10.0*j), r*Math.sin(phia+dphi/10.0*j), a.z));

                    //velorgeometry.vertices.push( new THREE.Vector3(ab[0], ab[1], ab[2]) );
                    //velorgeometry.vertices.push( new THREE.Vector3(ab[3], ab[4], ab[5]) );
		    var velorline = new THREE.Line( velorgeometry, velomaterial, THREE.LineStrip );
		    velorline.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
		    alllines.push(velorline);
		}

		/*
		//PARSE VELO
		velohits = parsedevent.VELO;
		for (i in velohits)
		{
		//just save two different types, r and phi sensors
                var ab = velohits[i];
		var velogeometry = new THREE.Geometry();
		var a = new THREE.Vector3(ab[0], ab[1], ab[2]);
		var b = new THREE.Vector3(ab[3], ab[4], ab[5]);
		var pivot = new THREE.Vector3(0.0, 0.0, ab[5]);
		
                velogeometry.vertices.push( new THREE.Vector3(ab[0], ab[1], ab[2]) );
                //velogeometry.vertices.push( new THREE.Vector3(ab[3], ab[4], ab[5]) );
		var r = velogeometry.vertices[0].
		var geometry = new THREE.Geometry();
		for (j = 0; j < 10; j++)
		geometry.vertices.push(new THREE.Vector3())
		}
		var veloline = new THREE.Line( velogeometry, velomaterial, THREE.LinePieces );
		veloline.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
		alllines.push(veloline);
		*/

		//PARSE MUON
		muonboxes = parsedevent.MUON;			     
		mergedmuongeometry = new THREE.Geometry();
		mergedmuonlines = new THREE.Geometry();
		for (i in muonboxes)
		{
	            var mb = muonboxes[i];
		    var muongeometry = new THREE.BoxGeometry(2.0*mb[1], 2.0*mb[3], 2.0*mb[5]);
		    //var muongeometry = new THREE.BoxHelper(2.0*mb[1], 2.0*mb[3], 2.0*mb[5]);
	            var muonmesh = new THREE.Mesh(muongeometry, muonmaterial); 
	            //var muonmesh = new THREE.Mesh(muongeometry, muonmaterials[0]);  
                    muonmesh.position.set(1.0e-3*mb[0], 1.0e-3*mb[2], 1.0e-3*mb[4]); 
                    muonmesh.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
                    //THREE.GeometryUtils.merge(mergedmuongeometry, muonmesh);
		    muonmesh.updateMatrix();
		    mergedmuongeometry.merge(muonmesh.geometry, muonmesh.matrix);
		}
		var mergedmuonmesh = new THREE.Mesh(mergedmuongeometry, muonmaterial);
		allmeshes.push(mergedmuonmesh);
		muonedges = new THREE.EdgesHelper(mergedmuonmesh, 0xa0ffa0 );
		allmeshes.push(muonedges);

		//PARSE ECAL
		ecalboxes = parsedevent.ECAL;			     
		mergedecalgeometry = new THREE.Geometry();
		for (i in ecalboxes)
		{
	            var eb = ecalboxes[i];
		    var ez = 1.0e+3*(1.0-Math.exp(-1.0e-4*eb[0]));
	            //var escale = 0.05; //20gev is 1m?
		    //var ecalgeometry = new THREE.BoxGeometry(eb[4], eb[4], eb[0]*escale);
	            //var ecalmesh = new THREE.Mesh(ecalgeometry, ecalmaterials[0]); 
                    //ecalmesh.position.set(1.0e-3*eb[1], 1.0e-3*eb[2], 1.0e-3*(eb[3]-0.5*eb[0]*escale)); 
		    var ecalgeometry = new THREE.BoxGeometry(eb[4], eb[4], ez);
	            var ecalmesh = new THREE.Mesh(ecalgeometry, ecalmaterial); 
                    ecalmesh.position.set(1.0e-3*eb[1], 1.0e-3*eb[2], 1.0e-3*(eb[3]+400.0-0.5*ez)); 
                    ecalmesh.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
                    //THREE.GeometryUtils.merge(mergedecalgeometry, ecalmesh);
		    ecalmesh.updateMatrix();
		    mergedecalgeometry.merge(ecalmesh.geometry, ecalmesh.matrix);
		}
		//allmeshes.push(new THREE.Mesh(mergedecalgeometry, ecalmaterial));
		//allmeshes.push(new THREE.SceneUtils.createMultiMaterialObject(mergedecalgeometry, ecalmaterials));
		var mergedecalmesh = new THREE.Mesh(mergedecalgeometry, ecalmaterial);
		allmeshes.push(mergedecalmesh);
		ecaledges = new THREE.EdgesHelper(mergedecalmesh, 0xa0a0ff );
		allmeshes.push(ecaledges);



		//PARSE HCAL
		hcalboxes = parsedevent.HCAL;			     
		mergedhcalgeometry = new THREE.Geometry();
		for (i in hcalboxes)
		{
	            var hb = hcalboxes[i];
	            var ez = 1.5e+3*(1.0-Math.exp(-0.5e-4*hb[0]));
		    //var escale = 0.05; //20gev is 1m?
		    //var hcalgeometry = new THREE.BoxGeometry(hb[4], hb[4], hb[0]*escale);
	            //var hcalmesh = new THREE.Mesh(hcalgeometry, hcalmaterials[0]); 
                    //hcalmesh.position.set(1.0e-3*hb[1], 1.0e-3*hb[2], 1.0e-3*(hb[3]+0.5*hb[0]*escale)); 
		    var hcalgeometry = new THREE.BoxGeometry(hb[4], hb[4], ez);
	            var hcalmesh = new THREE.Mesh(hcalgeometry, hcalmaterial); 
                    hcalmesh.position.set(1.0e-3*hb[1], 1.0e-3*hb[2], 1.0e-3*(hb[3]+0.5*ez)); 
                    hcalmesh.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
                    //THREE.GeometryUtils.merge(mergedhcalgeometry, hcalmesh);
		    hcalmesh.updateMatrix();
		    mergedhcalgeometry.merge(hcalmesh.geometry, hcalmesh.matrix);
		}
		//allmeshes.push(new THREE.Mesh(mergedhcalgeometry, hcalmaterial));
		//allmeshes.push(new THREE.SceneUtils.createMultiMaterialObject(mergedhcalgeometry, hcalmaterials));
		var mergedhcalmesh = new THREE.Mesh(mergedhcalgeometry, hcalmaterial);
		allmeshes.push(mergedhcalmesh);
		hcaledges = new THREE.EdgesHelper(mergedhcalmesh, 0xffffa0 );
		allmeshes.push(hcaledges);

		//PARSE VELO TRACKS
		
		vtracks = parsedevent.VTRACKS;
		for (i in vtracks)
		{
                    var track = vtracks[i]; 
		    var geometry = new THREE.Geometry(); //this will be the new line
                    for (j in track)
                    {
			var point = track[j];
			geometry.vertices.push( new THREE.Vector3(point[0], point[1], point[2]) );
                    }
		    var line = new THREE.Line( geometry, vtrackmaterial, THREE.LineStrip );
                    line.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
                    alllines.push(line);
		}
		
		//PARSE TRACKS
		particles = parsedevent.PARTICLES
		for (i in particles)
		{
                    //var j = 0;
                    var track = particles[i]; 
                    var name = track.name;
                    if (name=="pi+" || name=="pi-" || name=="K+" || name=="K-" || name=="p+" || name=="p-" || name=="mu+" || name=="mu-" || name=="e+" || name=="e-")
                    {						  
			var points = [];
			for (j in track.track)
			{
			    var point = track.track[j];
			    points.push( new THREE.Vector3(1.0e-3*point[0], 1.0e-3*point[1], 1.0e-3*point[2]) );//important to get these as float!
			}
			var spline = new THREE.SplineCurve3(points);
			var geometry = new THREE.Geometry();
			geometry.vertices = spline.getPoints(200);//these are divisions over full length

			var line;
			if (name=="pi+" || name=="pi-")
			    line = new THREE.Line( geometry, pimaterial, THREE.LineStrip );
			//line = new THREE.Line( geometry, linematerial, THREE.LineStrip );
			else if (name=="K+" || name=="K-")
			    line = new THREE.Line( geometry, kmaterial, THREE.LineStrip );
			else if (name=="p+" || name=="p-")
			    line = new THREE.Line( geometry, pmaterial, THREE.LineStrip );
			else if (name=="mu+" || name=="mu-")
			    line = new THREE.Line( geometry, mumaterial, THREE.LineStrip );
			else if (name=="e+" || name=="e-")
			    line = new THREE.Line( geometry, ematerial, THREE.LineStrip );
			else
			    line = new THREE.Line( geometry, pimaterial, THREE.LineStrip );

			//var linematerial = new THREE.LineBasicMaterial( { color: color, opacity: 1.0, linewidth: 3 } );
			//var line = new THREE.Line( spline, linematerial, THREE.LineStrip );
			//line.scale.set(1.0e-3, 1.0e-3, 1.0e-3);
			alllines.push(line);
                    }
		}
		event["lines"] = alllines;
		event["meshes"] = allmeshes;
		//allevents.push(event);
		removefromscene();
		theevent = event;
		addtoscene(); 
            }
            //else
	    //{
	    //  params.update = true; 
	    //  autoupdate = true; 
	    //}
            //  alert("Error loading file "+filename+"\n");
        }
        //else
	//{
	//  params.update = true; 
	//  autoupdate = true; 
	//}
    }
    jsonfile.send(null);
    //return 0;
};

