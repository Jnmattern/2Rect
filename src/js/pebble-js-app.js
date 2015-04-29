var bgcolor;
var fgcolor;

function readConfig() {
  bgcolor = localStorage.getItem("bgcolor");
  if (!bgcolor) {
    bgcolor = "#000000";
  }

  fgcolor = localStorage.getItem("fgcolor");
  if (!fgcolor) {
    fgcolor = "#ffffff";
  }
}

function logVariables(msg) {
  console.log(msg);
  console.log("	bgcolor: " + bgcolor);
  console.log("	fgcolor: " + fgcolor);
};

Pebble.addEventListener("ready", function() {
  logVariables("Ready Event");
  readConfig();
	Pebble.sendAppMessage(JSON.parse('{"bgcolor":"'+bgcolor+'","fgcolor":"'+fgcolor+'"}'));
});

Pebble.addEventListener("showConfiguration", function(e) {
	logVariables("showConfiguration Event");
  var url = "http://www.famillemattern.com/jnm/pebble/2Rect/2Rect.php?bgcolor=" + encodeURIComponent(bgcolor) + "&fgcolor=" + encodeURIComponent(fgcolor);
  console.log(url);
	Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("Configuration window closed");
  console.log(e.type);
  console.log("Response: " + decodeURIComponent(e.response));

	var configuration = JSON.parse(decodeURIComponent(e.response));
	Pebble.sendAppMessage(configuration);

  bgcolor = configuration["bgcolor"];
	localStorage.setItem("bgcolor", bgcolor);

  fgcolor = configuration["fgcolor"];
	localStorage.setItem("fgcolor", fgcolor);

});

