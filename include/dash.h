#pragma once
//Dont edit this file manualy
const char dash[] =R"rawliteral(<!DOCTYPE HTML>
<html>

<head>
    <title> Kralj Artur </title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        html {
            font-family: Arial, Helvetica, sans-serif;
            text-align: center;
        }

        h1 {
            font-size: 1.8rem;
            color: white;
        }

        h2 {
            font-size: 1.5rem;
            font-weight: bold;
            color: #143642;
        }

        .topnav {
            overflow: hidden;
            background-color: #143642;
        }

        body {
            margin: 0;
        }

        span {
            border-radius: 5px;
            padding: 2px;
        }

        .content {
            padding: 30px;
            max-width: 600px;
            margin: 0 auto;
        }

        .card {
            background-color: #F8F7F9;
            ;
            box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, .5);
            padding-top: 10px;
            padding-bottom: 20px;
        }

        .button {
            padding: 15px 50px;
            font-size: 20px;
            text-align: center;
            outline: none;
            color: rgb(0, 0, 0);
            background-color: #97e2d4;

        }

        /*.button:hover {background-color: #0f8b8d}*/
        .button:active {
            background-color: #0f8b8d;
            box-shadow: 2 2px #CDCDCD;
            transform: translateY(2px);
            margin-top: 5px;
        }

        .state {
            font-size: 1.5rem;
            color: #373d49;
            font-weight: bold;
            display: flex;
            justify-content: space-evenly;
            flex-flow: wrap;
            border-radius: 5px;
            padding: 2px
        }
    </style>
</head>

<body>
    <div class="topnav">
        <h1>Kontrola Mehanizma</h1>
    </div>

    <div class="content">
        <div class="card">
            <h2>Trajno Otkljucaj/Zakljucaj MAC u kamenu </h2>
            <p class="state"><span id="lock_status">%STATUS_MAC%</span></p>
            <button class="button" id="lock_command_button" onclick=sword_toogle()>Toogle</button>
        </div>
    </div>
    <div class="content">
        <div class="card">
            <h2>Postotak Kraljeva</h2>
            <p class="state"><span>Trenutni postotak: </span><span id="probability">%POSTOTAK% &#37;</span>
            </p>
            <p class="state">
                <input id="new_probability" type="number" max=100 min=0 defaultValue="20">
                <button class="button" id="submit_button" onclick=promijeni_postotak()>Promijeni</button>
            </p>
        </div>
    </div>
    <div class="content">
        <div class="card">
            <h2></h2>
            <p class="state"><span>Covjek na platformi:</span> <span id=pir_sensor
                    style="background-color:lightcoral;">%PIR_SENSOR%</span></p>
            <p class="state"><span>Mac u kamenu:</span> <span id=ir_sensor
                    style="background-color:lightcoral;">%IR_SENSOR%</span></p>
        </div>
    </div>



    <script>
        function sword_toogle() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    if (this.responseText == '0') {
                        document.getElementById("lock_status").innerHTML =
                            "Zakljucan"
                    } else {
                        document.getElementById("lock_status").innerHTML =
                            "Otkljucan"
                    }
                }
            };
            xhttp.open("GET", "/sword_toogle", true);
            xhttp.send();
        };


        setInterval(function () {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    var ir_pir = this.responseText.split(";");
                    if (ir_pir[1] == '0') {
                        document.getElementById('pir_sensor').setAttribute('style', 'background-color:lightcoral');
                        document.getElementById("pir_sensor").innerHTML = "FALSE";
                    }
                    else {
                        document.getElementById('pir_sensor').setAttribute('style', 'background-color:greenyellow');
                        document.getElementById("pir_sensor").innerHTML = "TRUE";
                    }
                    if (ir_pir[0] == '0') {
                        document.getElementById('ir_sensor').setAttribute('style', 'background-color:lightcoral');
                        document.getElementById("ir_sensor").innerHTML = "FALSE"
                    }
                    else {
                        document.getElementById('ir_sensor').setAttribute('style', 'background-color:greenyellow');
                        document.getElementById("ir_sensor").innerHTML = "TRUE";
                    }
                }
            };
            xhttp.open("GET", "/ir_pir_sensor", true);
            xhttp.send();
        }, 1000);

        function promijeni_postotak() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("probability").innerHTML =
                        this.responseText;
                }
            };
            xhttp.open("GET", "/change_probability?p=" + document.getElementById("new_probability").value, true);
            xhttp.send();
        }

    </script>
</body>
<script>)rawliteral";