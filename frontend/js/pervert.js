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
      settings["complete"] = function() { self.complete(); if (oldcomplete) { oldcomplete(); } }
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

    var __f_memscatter = {};
        
    var init = function(cont) {
      __pv.log("initting DB");      
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
    
    var f_context = function(frame, cont) {
      if (__f_context[frame]) {
        return cont(__f_context[frame]);
      } else {
        __ajaxqueue.ajax('/f/context_stack?exec='+__exec+'&frame='+frame, {
          success: function(data) { 
            __pv.log("f_context returned: " + data); 
            __f_context[frame] = data;
            cont(__f_context[frame]);
          },
          error: function() { __pv.log("f_context FAILED!"); },
         });          
      }
    }
    
    var f_memscatter = function(frame, cont) {
      if (__f_memscatter[frame]) {
        return cont(__f_memscatter[frame]);
      } else {
        __ajaxqueue.ajax('/f/context_events?exec='+__exec+'&frame='+frame, {
          success: function(data) { 
            __pv.log("f_memscatter returned: " + data); 
            __f_memscatter[frame] = data;
            cont(__f_memscatter[frame]);
          },
          error: function() { __pv.log("f_memscatter FAILED!"); },
         });          
      }
    }
    
    return {
      init: init,
      abort: abort,
      f_counts: f_counts,
      f_context: f_context,
      f_mem_status: f_mem_status,
      f_memscatter: f_memscatter,
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
    
    var __currentFrame = 0;
    var __maxFrame = 0;
    
    var __playing = false;
    var __playingTimer = null;
    
    function construct() {
      addEvent("frameslider_change");
      addEvent("frameslider_play");
      addEvent("frameslider_pause");
    }
    
    //Public API
    var reset = function() {
      setPlaying(false);
      setCurrentFrame(0);
    }

    var setFrameRange = function(maxframe) {
      __pv.log("Setting max frame to " + maxframe);
      __maxFrame = maxframe;
    }
    
    var setCurrentFrame = function(f) {
      if (f > __maxFrame) {
        setPlaying(false);
      } else {
        __currentFrame = f;
        fireEvent("frameslider_change", f, this);         
      }
    }
    
    var setPlaying = function(v) {
      __playing = v;
      if (__playing) {
        
        fireEvent("frameslider_play", true, this);
        if (__currentFrame == __maxFrame) {
          setCurrentFrame(0);
        }
        __playingTimer = setInterval(function() {
          setCurrentFrame(__currentFrame + 1);
        },100);        
        
      } else {
        
        clearInterval(__playingTimer);
        fireEvent("frameslider_pause", true, this);
      
      }
    }

    var togglePlaying = function() {
      setPlaying(!__playing);
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

    var obj = {
      reset: reset,
      setCurrentFrame: setCurrentFrame,
      setFrameRange: setFrameRange,
      setPlaying: setPlaying,
      togglePlaying: togglePlaying,
      addListener: addListener,
      addEvent: addEvent,
      fireEvent: fireEvent
    }
    construct(obj)
    return obj;
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

    function drawmockup(data) {
      var canvas = document.getElementById("pv_memmap_canvas");
      
      var ctx = canvas.getContext("2d");
      var canvasWidth  = canvas.width;
      var canvasHeight = canvas.height;
      ctx.clearRect ( 0 , 0 , canvasWidth , canvasHeight );

      var imageData = ctx.getImageData(0, 0, canvasWidth, canvasHeight);

      var dt = imageData.data;
      
      var biggest = 50
      var value = 255;
      var alpha = 255;
      
      for (var idx = 0; idx < data.addr.length; idx++) {
        var ob = data.addr[idx];
        
        for (var y = biggest/2; y >= -(biggest/2); y--) {
          for (var x = biggest/2; x >= -(biggest/2); x--) {
            var index = (1024*50)*4 + (ob + y * canvasWidth + x) * 4;
  
  
            if (data.events[idx] == "r") {
              dt[index];    // red
              dt[index+1] = dt[index+1]*0.5 + value*0.5;    // green
              dt[index+2] = dt[index+2] + 1;    // blue              
            } else {
              dt[index]   = dt[index]*0.5 + value*0.5;    // red
              dt[index+1];    // green
              dt[index+2] = dt[index+2] + 1;    // blue              
            }     
            dt[index+3] = alpha;      // alpha
          
          }
        }
        if (biggest > 1) {
          biggest -= 2;
        }
        if (value > 0)
          value--;
        if (alpha < 255)
          alpha++;
        
      }
      ctx.putImageData(imageData, 0, 0);

    }


    function create_controls_view() {
      $(__div_controls)
        .append("<div id='pv_controls_playpause'>4</div>")
        .append("<div id='pv_controls_slider_container'><div id='pv_controls_slider'></div></div>");
      
      __vS.addListener("init", function(eventname, event, caller) {
                
        __db.f_counts(function(counts) {
          __vS.setFrameRange(counts.event-1);
          $("#pv_controls_slider").slider("destroy");
          $("#pv_controls_slider").slider({
            range: false,
            min: 0,
            max: counts.event-1,
            value: 0,
            create: function() { __vS.setCurrentFrame(0,this); return true;},
            slide: function() { __vS.setCurrentFrame($("#pv_controls_slider").slider("value"),this); return true;},
            stop: function() { __vS.setCurrentFrame($("#pv_controls_slider").slider("value"),this); return true;}
          });
        });
        
      });
      
      __vS.addListener("frameslider_change", function(eventname, event, caller) {
        var pr = $("#pv_controls_slider").slider("value");
        if (pr != event) { $("#pv_controls_slider").slider("value", event); }
      });
      
      __vS.addEvent("frameslider_play");
      __vS.addEvent("frameslider_pause");
      
      $("#pv_controls_playpause").click(function() { 
        var v = $("#pv_controls_playpause").html();
        if (v == "4") { __vS.setPlaying(true); } else { __vS.setPlaying(false); }
      });
      __vS.addListener("frameslider_play",  function(eventname, event, caller) { $("#pv_controls_playpause").html("5"); });
      __vS.addListener("frameslider_pause", function(eventname, event, caller) { $("#pv_controls_playpause").html("4"); });
      
      KeyboardJS.bind.key("p", function() { __vS.togglePlaying(); }, function() {});
      
    }
    
    function create_mem_view() {
      var width = 1024;
      var height = 1024;
      $(__div_memmap).css("width", width);
      $(__div_memmap).css("height", width);
      $(__div_memmap).html("<canvas id='pv_memmap_canvas' width='"+width+"' height='"+height+"'></canvas>");
      __vS.addEvent("memmap_click");
      
      $("#pv_memmap_canvas").click(function(eventObj) {__vS.fireEvent("memmap_click", eventObj, this);})
      
      //__vS.addListener("memmap_click", function(eventname,event,caller) { drawanim();});
      
      __vS.addListener("frameslider_change", function(eventname, event, caller) {
        __db.f_mem_status(event,255,function(data) {
          
          drawmockup(data);
          
        })
        
        
      })

    }
    
    function create_context_view() {
      $(__div_context).css("border", "solid black 1px"); 
      __vS.addListener("frameslider_change", function(eventname, event, caller) { __db.f_context(event, function(data) {
          $(__div_context).html("");
          $.each(data.stack, function(idx, val) { 
            $(__div_context).append(idx + ": " + val.file + " " + val.line + "<br/>");
          });
        })
      });
    }
    
    var vis = null;
    var x = null;
    var y = null;

    function create_scatter_view() {
      $(__div_memscatter).css("border", "solid red 1px"); 


      __vS.addListener("init", function(eventname, event, caller) {
        __db.f_counts(function(counts) {          
          var xmax = counts.event-1,
              ymax = counts.max_addr,
              w = 450,
              h = 450,
              p = 20;

          x = d3.scale.linear().domain([0,xmax]).range([0, w]);
          y = d3.scale.linear().domain([0,ymax]).range([h, 0]);
          vis = d3.select(__div_memscatter)
            .append("svg")
               .attr("width", w + p * 2)
              .attr("height", h + p * 2)
            .append("g")
              .attr("transform", "translate(" + p + "," + p + ")");

          var xrule = vis.selectAll("g.x")
              .data(x.ticks(10))
            .enter().append("g")
              .attr("class", "x");

          xrule.append("line")
              .attr("x1", x)
              .attr("x2", x)
              .attr("y1", 0)
              .attr("y2", h);

          xrule.append("text")
              .attr("x", x)
              .attr("y", h + 3)
              .attr("dy", ".71em")
              .attr("text-anchor", "middle")
              .text(x.tickFormat(10));

          var yrule = vis.selectAll("g.y")
              .data(y.ticks(10))
            .enter().append("g")
              .attr("class", "y");

          yrule.append("line")
              .attr("x1", 0)
              .attr("x2", w)
              .attr("y1", y)
              .attr("y2", y);

          yrule.append("text")
              .attr("x", -3)
              .attr("y", y)
              .attr("dy", ".35em")
              .attr("text-anchor", "end")
              .text(y.tickFormat(10));

          vis.append("rect")
              .attr("width", w)
              .attr("height", h);
        });
      });

      __vS.addListener("frameslider_change", function(eventname, event, caller) { 
        __db.f_memscatter(event, function(data) {
              var points = vis.selectAll("path.dot")
                .data(data.events);
              points.enter().append("path")
                .attr("class", "dot")
                .attr("stroke", function(d, i) { return data.type == "w" ? "red" : "blue"; })
                .attr("transform", function(d) { return "translate(" + x(d.index) + "," + y(d.addr) + ")"; })
                .attr("d", d3.svg.symbol());
              points.transition().duration(0)
                .attr("class", "dot")
                .attr("stroke", function(d, i) { return data.type == "w" ? "red" : "blue"; })
                .attr("transform", function(d) { return "translate(" + x(d.index) + "," + y(d.addr) + ")"; })
                .attr("d", d3.svg.symbol());
              points.exit().remove();
        });
      });
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
