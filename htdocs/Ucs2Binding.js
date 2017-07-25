    var afb = new AFB("api", "mysecret");
    var ws;
    var ucs2_config="ConfigNotSelected";
    var evtidx=0;
    var numid=0;

    function syntaxHighlight(json) {
        if (typeof json !== 'string') {
             json = JSON.stringify(json, undefined, 2);
        }
        json = json.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
        return json.replace(/("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?|\b(true|false|null)\b|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?)/g, function (match) {
            var cls = 'number';
            if (/^"/.test(match)) {
                if (/:$/.test(match)) {
                    cls = 'key';
                } else {
                    cls = 'string';
                }
            } else if (/true|false/.test(match)) {
                cls = 'boolean';
            } else if (/null/.test(match)) {
                cls = 'null';
            }
            return '<span class="' + cls + '">' + match + '</span>';
        });
    }
    
    function basename(path) {
        return path.split('/').reverse()[0];
    }

    function getParameterByName(name, url) {
        if (!url) {
          url = window.location.href;
        }
        name = name.replace(/[\[\]]/g, "\\$&");
        var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
            results = regex.exec(url);
        if (!results) return null;
        if (!results[2]) return '';
        return decodeURIComponent(results[2].replace(/\+/g, " "));
    }
    
    // default soundcard is "PCH"
    var devid=getParameterByName("devid");
    if (!devid) devid="hw:1";
    
    var haldev=getParameterByName("haldev");
    if (!haldev) haldev="scarlett-usb";
    
    var sndname=getParameterByName("sndname");
    if (!sndname) sndname="PCH";
    
    var mode=getParameterByName("mode");
    if (!mode) mode="0";


    

    function replyok(obj) {
            console.log("replyok:" + JSON.stringify(obj));
            document.getElementById("output").innerHTML = "OK: "+ syntaxHighlight(obj);
    }
    
    function replyerr(obj) {
            console.log("replyerr:" + JSON.stringify(obj));
            document.getElementById("output").innerHTML = "ERROR: "+ syntaxHighlight(obj);
    }
    
    function gotevent(obj) {
            console.log("gotevent:" + JSON.stringify(obj));
            document.getElementById("outevt").innerHTML = (evtidx++) +": "+JSON.stringify(obj);
    }
    
    function send(message) {
            var api = document.getElementById("api").value;
            var verb = document.getElementById("verb").value;
            document.getElementById("question").innerHTML = "subscribe: "+api+"/"+verb + " (" + JSON.stringify(message) +")";
            ws.call(api+"/"+verb, {data:message}).then(replyok, replyerr);
    }
    

     // On button click from HTML page    
    function callbinder(api, verb, query) {
            console.log ("subscribe api="+api+" verb="+verb+" query=" +query);
            var question = urlws +"/" +api +"/" +verb + "?query=" + JSON.stringify(query);          
            document.getElementById("question").innerHTML = syntaxHighlight(question);
            ws.call(api+"/"+verb, query).then(replyok, replyerr);
    }


    // Retreive Select value and Text from the binder
    // Note: selection of value/text for a given context is huggly!!!
    function querySelectList (elemid, api, verb, query) {
        
        console.log("querySelectList elemid=%s api=%s verb=%s query=%s", elemid, api, verb, query);
        
        var selectobj = document.getElementById(elemid);
        if (!selectobj) {
            console.log ("****** elemid=%s does not exit in HTML page ****", elemid);
            return;
        }
        
        // onlick update selected HAL api
        selectobj.onclick=function(){
           ucs2_config= this.value; 
           console.log ("Default Selection=" + ucs2_config);           
        };

        function gotit (result) {
            
            // display response as for normal onclick action
            replyok(result);
            var response=result.response;
           
            // fulfill select with avaliable active HAL
            for (idx=0; idx<response.length; idx++) {
                var opt = document.createElement('option');
                var basename = response[idx].basename;
                var dirpath  = response[idx].dirpath;
                var ext= basename.split('.').pop();

                // Only propose XML files
                if (ext.toLowerCase() === "xml") {
                    opt.text   = basename;
                    opt.value  = dirpath + "/" + basename;
                    selectobj.appendChild(opt);
                }
            }
               
           ucs2_config= selectobj.value;
        }
                
        var question = urlws +"/"+api +"/" +verb + "?query=" + JSON.stringify(query);          
        document.getElementById("question").innerHTML = syntaxHighlight(question);

        // request lowlevel ALSA to get API list
        ws.call(api+"/"+verb, query).then(gotit, replyerr);
    }
    
        
    function init(elemid, api, verb, query) {
        
        function onopen() {
                // check for active HALs
                querySelectList (elemid, api, verb, query);
                
                document.getElementById("main").style.visibility = "visible";
                document.getElementById("connected").innerHTML = "Binder WS Active";
                document.getElementById("connected").style.background  = "lightgreen";
                ws.onevent("*", gotevent);
        }

        function onabort() {
                document.getElementById("main").style.visibility = "hidden";
                document.getElementById("connected").innerHTML = "Connected Closed";
                document.getElementById("connected").style.background  = "red";

        }           
        ws = new afb.ws(onopen, onabort);
    }
