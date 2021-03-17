#pragma once
//Dont edit this file manualy
const char dash[] =R"rawliteral(<!DOCTYPE HTML>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        html {
            font-family: Arial;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }

        h2 {
            font-size: 3.0rem;
        }

        button {
            font-size: 2.0rem
        }

        p {
            font-size: 3.0rem;
        }

        .units {
            font-size: 1.2rem;
        }
    </style>
</head>

<body>
    <h2>Kralj Artur - konrolni panel</h2>
    <p>
        <span class="status">Status otkljucanosti: </span>
        <span id="lock_status">%STATUS_MAC%</span>
        <button id="lock_command_button" onclick=sword_toogle()>Otkljucaj/Zakljucaj</button>
    </p>
    <p>
        <span class="dht-labels">Humidity</span>
        <span id="humidity">%HUMIDITY%</span>
        <sup class="units">%</sup>
    </p>

    <script>
        function sword_toogle() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("lock_status").innerHTML =
                        this.responseText;
                    if (this.responseText == 'Zakljucan') {
                        document.getElementById('lock_command_button').innerHTML = "Otkljucaj"
                    } else if (this.responseText == 'Otkljucan') {
                        document.getElementById('lock_command_button').innerHTML = "Zakljucaj"

                    }

                }
            };
            xhttp.open("GET", "/sword_toogle", true);
            xhttp.send();
        }

    </script>
</body>
<script>)rawliteral";