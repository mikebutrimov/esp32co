const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
.card{
    display: inline-block;
     max-width: 3000px;
     min-height: 150px;
     background: #b2b2b2;
     padding: 20px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
</style>
<body>
<div class = "parent">
  <div class="card">
    <h3>MHZ19 reading:</h3>
    <h2>CO2: <span id="CO2">0</span> ppm.</h2>
    <h2>Temp: <span id="TEMP"></span> C</h2>
  </div>
  <div class="card">
    <h3>DHT 22 reading:</h3>
    <h2>Temp: <span id="DHT_TEMP"></span> C</h2>
    <h2>Humidity: <span id="DHT_HUMIDITY"></span> %</h2>
  </div>
</div>
<script>

setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 2000); //2000mSeconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("CO2").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "CO2", true);
  xhttp.send();
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TEMP").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "TEMP", true);
  xhttp.send();
  
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("DHT_HUMIDITY").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "DHT_HUMIDITY", true);
  xhttp.send();
  
  
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("DHT_TEMP").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "DHT_TEMP", true);
  xhttp.send();
}
</script>
</body>
</html>
)=====";