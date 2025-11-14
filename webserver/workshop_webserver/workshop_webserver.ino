#include <WiFi.h>
#include <WebServer.h>

#include "irisModel.h"
#include <tflm_esp32.h>
#include <eloquent_tinyml.h>

#define ARENA_SIZE 2000
Eloquent::TF::Sequential<TF_NUM_OPS, ARENA_SIZE> tf;

const char *ssid = "wifi name";
const char *password = "wifi password";

WebServer server(80);

String htmlPage = R"HTML(
<!DOCTYPE html>
<html>
<head>
<title>Iris Classifier</title>
<meta name="viewport" content="width=device-width, initial-scale=1"/>
<style>
body { font-family: Arial; padding: 20px; }
input { width: 80%; padding: 10px; margin: 6px 0; font-size: 18px; }
button { padding: 10px; width: 90%; font-size: 20px; }
</style>
</head>
<body>
<h2> ESP32 IRIS Classifier</h2>
<p>Enter the 4 iris flower features:</p>
<input id="a" placeholder="Sepal length">
<input id="b" placeholder="Sepal width">
<input id="c" placeholder="Petal length">
<input id="d" placeholder="Petal width">
<button onclick="predict()">Predict</button>

<h3 id="result"></h3>

<script>
function predict() {
    let a = document.getElementById('a').value;
    let b = document.getElementById('b').value;
    let c = document.getElementById('c').value;
    let d = document.getElementById('d').value;

    fetch(`/predict?a=${a}&b=${b}&c=${c}&d=${d}`)
        .then(res => res.text())
        .then(txt => document.getElementById('result').innerHTML = txt);
}
</script>

</body>
</html>
)HTML";

void handleRoot()
{
    server.send(200, "text/html", htmlPage);
}

void handlePredict()
{
    if (!server.hasArg("a") || !server.hasArg("b") ||
        !server.hasArg("c") || !server.hasArg("d"))
    {
        server.send(400, "text/plain", "Missing arguments");
        return;
    }

    float input[4];
    input[0] = server.arg("a").toFloat();
    input[1] = server.arg("b").toFloat();
    input[2] = server.arg("c").toFloat();
    input[3] = server.arg("d").toFloat();

    if (!tf.predict(input).isOk())
    {
        server.send(500, "text/plain", "Model error");
        return;
    }

    int cls = tf.classification;
    String result;

    if (cls == 0)
        result = "🌼 Iris-setosa";
    else if (cls == 1)
        result = "🌺 Iris-versicolor";
    else if (cls == 2)
        result = "🌸 Iris-virginica";
    else
        result = "Unknown";

    server.send(200, "text/plain", result);
}

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println("Starting WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(300);
        Serial.print(".");
    }

    Serial.print("\nConnected! IP address: ");
    Serial.println(WiFi.localIP());

    tf.setNumInputs(4);
    tf.setNumOutputs(3);
    tf.resolver.AddFullyConnected();
    tf.resolver.AddSoftmax();

    while (!tf.begin(irisModel).isOk())
    {
        Serial.println(tf.exception.toString());
    }
    Serial.println("Model loaded.");

    server.on("/", handleRoot);
    server.on("/predict", handlePredict);

    server.begin();
}

void loop()
{
    server.handleClient();
}
