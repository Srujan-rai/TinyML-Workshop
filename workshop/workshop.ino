#include "irisModel.h"
#include <tflm_esp32.h>
#include <eloquent_tinyml.h>

#define ARENA_SIZE 2000
Eloquent::TF::Sequential<TF_NUM_OPS, ARENA_SIZE> tf;

float userInput[4];
String inputBuffer = "";

void setup()
{
    Serial.begin(115200);
    delay(10000);
    Serial.println("=== IRIS Interactive Classifier ===");
    Serial.println("Enter values: sepal_len sepal_wid petal_len petal_wid");
    Serial.println("Example: 5.1 3.5 1.4 0.2");
    Serial.println("-----------------------------------");

    tf.setNumInputs(4);
    tf.setNumOutputs(3);
    tf.resolver.AddFullyConnected();
    tf.resolver.AddSoftmax();

    while (!tf.begin(irisModel).isOk())
    {
        Serial.println(tf.exception.toString());
    }
}

void loop()
{
    if (Serial.available())
    {
        char c = Serial.read();

        if (c == '\n')
        {
            parseAndPredict(inputBuffer);
            inputBuffer = "";
        }
        else
        {
            inputBuffer += c;
        }
    }
}

void parseAndPredict(String line)
{
    line.trim();
    if (line.length() == 0)
        return;

    float values[4];
    int idx = 0;
    char *token = strtok((char *)line.c_str(), " ");

    while (token != NULL && idx < 4)
    {
        values[idx++] = atof(token);
        token = strtok(NULL, " ");
    }

    if (idx < 4)
    {
        Serial.println("❌ Error: Please enter 4 numbers separated by spaces.");
        return;
    }

    for (int i = 0; i < 4; i++)
        userInput[i] = values[i];

    if (!tf.predict(userInput).isOk())
    {
        Serial.println(tf.exception.toString());
        return;
    }

    int cls = tf.classification;

    Serial.print("Predicted class: ");
    if (cls == 0)
        Serial.println("Iris-setosa");
    else if (cls == 1)
        Serial.println("Iris-versicolor");
    else if (cls == 2)
        Serial.println("Iris-virginica");
    else
        Serial.println("Unknown");

    Serial.println("\nEnter next values:");
}
