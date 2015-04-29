<!DOCTYPE html>
<html>
        <head>
                <title>2Rect</title>
                <meta charset='utf-8'>
                <meta name='viewport' content='width=device-width, initial-scale=1'>
                <link rel='stylesheet' href='../jquery.mobile-1.3.2.min.css' />
                <script src='../jquery-1.9.1.min.js'></script>
                <script src='../jquery.mobile-1.3.2.min.js'></script>

                <link rel="stylesheet" type="text/css" href="spectrum.css">
                <script type='text/javascript' src="spectrum.js"></script>

                <style>
                        .ui-header .ui-title { margin-left: 1em; margin-right: 1em; text-overflow: clip; }
                </style>
        </head>
		<body>
<div data-role="page" id="page1">
    <div data-theme="a" data-role="header" data-position="fixed">
        <h3>
            2Rect Configuration
        </h3>
        <div class="ui-grid-a">
            <div class="ui-block-a">
                <input id="cancel" type="submit" data-theme="c" data-icon="delete" data-iconpos="left"
                value="Cancel" data-mini="true">
            </div>
            <div class="ui-block-b">
                <input id="save" type="submit" data-theme="b" data-icon="check" data-iconpos="right"
                value="Save" data-mini="true">
            </div>
        </div>
    </div>
    <div data-role="content">

      <div id="colorpickerdiv" data-role="fieldcontain">
        <fieldset data-role="controlgroup">
          <legend>Background Color (Color Pebble only)</legend>
            <input id="bgcolorpicker" name="bgcolorpicker">
        </fieldset>
        <fieldset data-role="controlgroup">
          <legend>Foreground Color (Color Pebble only)</legend>
            <input id="fgcolorpicker" name="fgcolorpicker">
        </fieldset>
      </div>


</div>


  <script>
<?php
  if (!isset($_GET['return_to'])) {
    echo 'var closeURL = "pebblejs://close#";';
  } else {
    echo 'var closeURL = "' . $_GET['return_to'] . '";';
  }
?>

      var curBGColor = "#ffffff";
      var curFGColor = "#000000";

      function setBGColor(color) {
        curBGColor = color.toHexString();
      }

      function setFGColor(color) {
        curFGColor = color.toHexString();
      }

      function saveOptions() {
        var options = {
          'bgcolor': curBGColor,
          'fgcolor': curFGColor
        }
        return options;
      }

      $().ready(function() {

        $("#bgcolorpicker").spectrum({
<?php
  if (!isset($_GET['bgcolor'])) {
    echo "              color: \"#ffffff\",\n";
  } else {
    echo "              color: \"" . $_GET['bgcolor'] . "\",\n" ;
  }
?>
            showPaletteOnly: true,
            hideAfterPaletteSelect:true,
            preferredFormat: "hex3",
            change: function(color) {
              setBGColor(color);
            },
            palette: [
              [ '000', '005', '00a', '00f', '050', '055', '05a', '05f' ],
              [ '500', '505', '50a', '50f', '550', '555', '55a', '55f' ],
              [ 'a00', 'a05', 'a0a', 'a0f', 'a50', 'a55', 'a5a', 'a5f' ],
              [ 'f00', 'f05', 'f0a', 'f0f', 'f50', 'f55', 'f5a', 'f5f' ],
              [ '0a0', '0a5', '0aa', '0af', '0f0', '0f5', '0fa', '0ff' ],
              [ '5a0', '5a5', '5aa', '5af', '5f0', '5f5', '5fa', '5ff' ],
              [ 'aa0', 'aa5', 'aaa', 'aaf', 'af0', 'af5', 'afa', 'aff' ],
              [ 'fa0', 'fa5', 'faa', 'faf', 'ff0', 'ff5', 'ffa', 'fff' ]
            ]
        });

        $("#fgcolorpicker").spectrum({
<?php
  if (!isset($_GET['fgcolor'])) {
    echo "              color: \"#000\",\n";
  } else {
    echo "              color: \"" . $_GET['fgcolor'] . "\",\n" ;
  }
?>
            showPaletteOnly: true,
            hideAfterPaletteSelect:true,
            preferredFormat: "hex3",
            change: function(color) {
              setFGColor(color);
            },
            palette: [
              [ '000', '005', '00a', '00f', '050', '055', '05a', '05f' ],
              [ '500', '505', '50a', '50f', '550', '555', '55a', '55f' ],
              [ 'a00', 'a05', 'a0a', 'a0f', 'a50', 'a55', 'a5a', 'a5f' ],
              [ 'f00', 'f05', 'f0a', 'f0f', 'f50', 'f55', 'f5a', 'f5f' ],
              [ '0a0', '0a5', '0aa', '0af', '0f0', '0f5', '0fa', '0ff' ],
              [ '5a0', '5a5', '5aa', '5af', '5f0', '5f5', '5fa', '5ff' ],
              [ 'aa0', 'aa5', 'aaa', 'aaf', 'af0', 'af5', 'afa', 'aff' ],
              [ 'fa0', 'fa5', 'faa', 'faf', 'ff0', 'ff5', 'ffa', 'fff' ]
            ]
        });

        $("#cancel").click(function() {
          console.log("Cancel");
          document.location = closeURL;
        });

        $("#save").click(function() {
          console.log("Submit");
          
          var location = closeURL + encodeURIComponent(JSON.stringify(saveOptions()));
          console.log("Close: " + location);
          console.log(location);
          document.location = location;
        });

        $('.sp-replacer').unwrap();
      });
    </script>
</body>
</html>
