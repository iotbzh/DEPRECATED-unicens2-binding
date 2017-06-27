    var afb = new AFB("api", "mysecret");
    var ws;
    var evtidx=0;

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
    if (!devid) devid="hw:0";

    var sndname=getParameterByName("sndname");
    if (!sndname) sndname="PCH";

    var quiet=getParameterByName("quiet");
    if (!quiet) quiet="99";

    function init() {
            ws = new afb.ws(onopen, onabort);
    }

    function onopen() {
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

    function replyok(obj) {
            console.log("replyok:" + JSON.stringify(obj));
            document.getElementById("output").innerHTML = "OK: "+JSON.stringify(obj);
    }

    function replyerr(obj) {
            console.log("replyerr:" + JSON.stringify(obj));
            document.getElementById("output").innerHTML = "ERROR: "+JSON.stringify(obj);
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


    function callbinder(api, verb, query) {
            console.log ("subscribe api="+api+" verb="+verb+" query=" +query);
            document.getElementById("question").innerHTML = "apicall: " + api+"/"+verb +" ("+ JSON.stringify(query)+")";
            ws.call(api+"/"+verb, query).then(replyok, replyerr);
    }
