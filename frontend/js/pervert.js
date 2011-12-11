/*!
 * pervert.js Performance Visualization and Error Remediation Toolkit
 * frontend
 *
 * Copyright 2011, Niels Joubert, Eric Schkufza
 *
 * Dependencies: 
 *   - jQuery      http://jquery.com
 *   - JSON        https://github.com/douglascrockford/JSON-js
 *
 * This uses the Revealing Module pattern.
 * Do NOT use this pattern if you create lots of objects,
 * it is for revealing a single module with a public API.
 * If you create many objects, functions should live on prototypes,
 * so that you do not duplicate function implementations every time you create an object
 */

(function(global){ 
  var pervert = function(exec, toggleBusy) {
    return Pervert(exec, toggleBusy);
  }

  /* AJAX Requests Queue - Helper Object */
  var jqXHRQueue = function() {
    self = this;
    self.__data = [];
    self.add = function(xhr,settings) { self.__data.push(xhr); };
    self.complete = function(xhr,success) { 
      var idx = self.__data.indexOf(xhr);
      if (idx!=-1) self.__data.splice(idx, 1);
    };
    self.abort = function() { $.each(self.__data, function(id,xhr) { xhr.abort(); }); self.__data = []; };
    self.ajax = function(path, settings) {
      var oldcomplete = settings["complete"];
      settings["complete"] = function() { self.complete(); oldcomplete(); }
      var t = $.ajax(path, settings);
      self.add(t);
      return t; 
    } 
  }
  
  /* 
  * Back-end Cache Helper Module 
  *   This class handles all the server data interaction and caching.
  *   It gets blown away when we "init" something, so we can start fresh.
  */
  var DB = function(pv, exec) {
    var __pv = pv;
    var __exec = exec;
    var __ajaxqueue = new jqXHRQueue();
    
    var __f_counts = null;
    var __f_counts_pending = false;
    var __f_counts_continuations = [];
    
    var __f_mem_status = {};

    var __f_context = {};
        
    function getListOfExecs(cont) {
      __ajaxqueue.ajax('/pp/list', {
        success: function(data) { $("#pv_bullshit").html(JSON.stringify(data)); },
        complete: function() { cont(); }
       });
    }
    
    var init = function(cont) {
      __pv.log("initting DB");
      getListOfExecs(cont);

      
    }
    
    var abort = function() {
      __ajaxqueue.abort();
    }
    
    //safe to be called to your heart's delight
    var f_counts = function(cont) { 
      if (__f_counts) {
        cont(__f_counts);
      } else {
        __f_counts_continuations.push(cont);
        if (!__f_counts_pending) {
          __f_counts_pending = true;
          __ajaxqueue.ajax('/f/counts?exec='+__exec, 
          {
            success: function(data) { 
              __pv.log("f_counts returned: " + data); 
              __f_counts = data; 
              $.each(__f_counts_continuations, function(idx,el) { el(__f_counts); }); 
            },
            error: function() { __pv.log("/f/counts FAILED!"); },
            complete: function() { __f_counts_continuations = []; __f_counts_pending = false; }
           });          
        }
      }
    }
    
    var f_mem_status = function(frame, windw, cont) {
      if (__f_mem_status[frame]) {
        return cont(__f_mem_status[frame]);
      } else {
        __ajaxqueue.ajax('/f/mem_status?exec='+__exec+'&frame='+frame+"&window="+windw, {
          success: function(data) { 
            __pv.log("f_mem_status returned: " + data); 
            __f_mem_status[frame] = data;
            cont(__f_mem_status[frame]);
          },
          error: function() { __pv.log("f_mem_status FAILED!"); },
         });          
      }
    }
    
    var f_context = function(frame, windw, cont) {
      if (__f_context[frame]) {
        return cont(__f_context[frame]);
      } else {
        __ajaxqueue.ajax('/f/context?exec='+__exec+'&frame='+frame, {
          success: function(data) { 
            __pv.log("f_context returned: " + data); 
            __f_context[frame] = data;
            cont(__f_context[frame]);
          },
          error: function() { __pv.log("f_context FAILED!"); },
         });          
      }
    }
    
    return {
      init: init,
      abort: abort,
      f_counts: f_counts,
    }
  }
  
  /* 
  * Carries the view state around. 
  *   This interacts with the user and the UI,
  *   and keeps everything happy and up to date.
  *   The UI itself registers event-listeners to this
  */
  var ViewState = function(pv) {
    var self = this;
    var __pv = pv;
    var __listeners = {};
    
    //Public API
    var reset = function() {
      //reset to the original view state;
    }

    //EVENT architecture:
    var fireEvent = function(eventname,event,caller) {
      if (__listeners[eventname]) {
        $.each(__listeners[eventname], function(idx,func) {
          func(eventname,event,caller);
        })        
      } else {
        __pv.log("Fired an undefined event: " + eventname);
      }
    }
    var addEvent = function(eventname) {
      if (!__listeners[eventname]) {
        __listeners[eventname] = [];
        __listeners[eventname].push(function(eventname,event,caller) {__pv.log(eventname + " fired " + event)});
        __pv.log(eventname + " added.");
      }
      return this;
    }
    //calls func(event,caller) when event fires
    var addListener = function(eventname,func) {
      if (!__listeners[eventname]) {
        addEvent(eventname);
      }
      __listeners[eventname].push(func);
      return this;
    }
    return {
      reset: reset,
      addListener: addListener,
      addEvent: addEvent,
      fireEvent: fireEvent
    }
  }


  var Pervert = function(exec) {
    var self = this;
    var __exec = exec;
    var __toggleBusy = function() {return;};
    
    var __div_controls = null;
    var __div_memmap = null;
    var __div_context = null;
    var __div_memscatter = null;
    var __div_debug = null;
    
    var __vS = null;
    var __db = null;
  
    function construct(obj) {
      __vS = ViewState(obj);
    }
    //private functions:
  

    function drawshit(ex,ey) {
      var canvas = document.getElementById("pv_memmap_canvas");
      
      var ctx = canvas.getContext("2d");
      var canvasWidth  = canvas.width;
      var canvasHeight = canvas.height;
      var imageData = ctx.getImageData(0, 0, canvasWidth, canvasHeight);

      var data = imageData.data;

      for (var y = 0; y < canvasHeight; ++y) {
          for (var x = 0; x < canvasWidth; ++x) {
              var index = (y * canvasWidth + x) * 4;

              var value = x * y & 0xff;

              data[index]   = value;    // red
              data[++index] = value;    // green
              data[++index] = value;    // blue
              data[++index] = 255;      // alpha
          }
      }

      ctx.putImageData(imageData, ex-$("#pv_memmap_canvas").position().left, ey-$("#pv_memmap_canvas").position().top);
    }
    
    var __x = 0;
    var __y = 0;
    
    function drawanim() {
      var canvas = document.getElementById("pv_memmap_canvas");
      
      var ctx = canvas.getContext("2d");
      var canvasWidth  = canvas.width;
      var canvasHeight = canvas.height;
      var imageData = ctx.getImageData(0, 0, canvasWidth, canvasHeight);

      var data = imageData.data;

      __x += 1;
      __y += 1;
      if (__x > canvasWidth)
        __x = 0;
      if (__y > canvasHeight)
        __y = 0;
                
        
      for (var y = __y; y < __y+10; ++y) {
          for (var x = __x; x < __x+10; ++x) {

              var index = (y * canvasWidth + x) * 4;

              var value = 100;

              data[index]   = value;    // red
              data[++index] = value;    // green
              data[++index] = value;    // blue
              data[++index] = 255;      // alpha

          }
      }

      ctx.putImageData(imageData, 0, 0);

    }

    function create_controls_view() {
      __vS.addEvent("frameslider_change");
      $(__div_controls).html("<div id='pv_controls_slider'></div>");

      __vS.addListener("init", function(eventname, event, caller) {
        
        __db.f_counts(function(counts) {          
          log("Creating pv_controls_slider");
          $("#pv_controls_slider").slider("destroy");
            $("#pv_controls_slider").slider({
              range: false,
              min: 0,
              max: counts.event-1,
              value: 0,
              start: function() {  var pr = $("#pv_controls_slider").slider("value"); __vS.fireEvent("frameslider_change", pr, this); return true;}
              // slide: function(e,u) {var pr = $("#pv_controls_slider").slider("value");  __vS.fireEvent("slider_slide", pr, this); return true;},
              change: function() {  var pr = $("#pv_controls_slider").slider("value"); __vS.fireEvent("frameslider_change", pr, this); return true;}
            });
        });
      });

      
    }
    
    function create_mem_view() {
      var width = 1024;
      var height = 1024;
      $(__div_memmap).css("width", width);
      $(__div_memmap).css("height", width);
      $(__div_memmap).html("<canvas id='pv_memmap_canvas' width='"+width+"' height='"+height+"'></canvas>");
      __vS.addEvent("memmap_click");
      $("#pv_memmap_canvas").click(function(eventObj) {__vS.fireEvent("memmap_click", eventObj, this);})
      
      __vS.addListener("memmap_click", function(eventname,event,caller) { drawanim();});
      

    }
    
    function create_context_view() {
      
    }
    
    function create_scatter_view() {
      d3.select(__div_memscatter).text("CREATE SCATTER VIEW");
    }
     
    // Public API:  
    var init = function() {
      __toggleBusy(true);
      if (__db)
        __db.abort();
      log("Initializing PerVert...");
      __db = DB(this,__exec); //create a new DB
            
      __db.init(function() {__toggleBusy(false);});
      __vS.addEvent("init");
      __vS.fireEvent("init", null, this);
    }
    
    var clean = function() {
      __toggleBusy(true);

      __db.abort();
      __toggleBusy(false);
    }     
    
    var log = function log(msg) {
      if (__div_debug) {
        var d1 = new Date();
        var ds = d1.getHours() + ":" + d1.getMinutes() + ":" + d1.getSeconds() + "  ";
        $(__div_debug).prepend("<p>" + ds + msg + "</p>");
        
      }
    }

    var bindBusy = function(busy) {
      __toggleBusy = busy;
      return this;
    }

    var bindMemView = function(div_memmap, div_memslider) {
      __div_memmap = div_memmap;
      create_mem_view();
      return this;
    }

    var bindContextView = function(div_context) {
      __div_context = div_context;
      create_context_view();
      return this;
    }
    
    var bindControlsView = function(div_controls) {
      __div_controls = div_controls;
      create_controls_view();
      return this;
    }
    
    var bindMemScatterView = function(div_memscatter) {
      __div_memscatter = div_memscatter;
      create_scatter_view();
      return this;
    }

    var bindDebugView = function(div_debug) {
      __div_debug = div_debug;
      return this;
    }
    
    // Exports the public api:
    var obj = {
      init: init,
      clean: clean,
      log: log,
      bindBusy: bindBusy,
      bindMemView: bindMemView,
      bindContextView: bindContextView,
      bindControlsView: bindControlsView,
      bindMemScatterView: bindMemScatterView,
      bindDebugView: bindDebugView
    };
    construct(obj);
    
    return obj;
    
  }
  
  pervert.VERSION = '0.0.1';
  
  if (global.pervert) {
    throw new Error("pervert has already been defined.")
  } else {
    global.pervert = pervert;
  }
  
})(window);
