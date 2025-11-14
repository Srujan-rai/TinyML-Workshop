# ESP32 IRIS TensorFlow Lite Micro Demo

This project demonstrates how to run a TensorFlow Lite Micro (TFLM) neural
network model on an ESP32 to classify Iris flowers using 4 numeric inputs:

- Sepal length
- Sepal width
- Petal length
- Petal width

The model is trained in Python using TensorFlow and exported as a `.h`
header file containing a byte array that can be compiled inside Arduino IDE.

---

## 1. Model Training (Python)

The model is trained using the classic Iris dataset from scikit-learn.

### Output

Running the script produces:

```
irisModel.h
```

This file contains:

- The TFLite model byte array
- Model length
- Ready to be included in Arduino projects

---

## 2. Model Header Format

The final model file used in this project includes additional helper macros:

```cpp
#define TF_NUM_OPS 2
#define TF_NUM_INPUTS 4
#define TF_NUM_OUTPUTS 3
#define TF_OP_FULLYCONNECTED
#define TF_OP_SOFTMAX
```

These are automatically used by EloquentTinyML to configure:

- Number of model operations
- Input and output dimensions
- Required resolver ops

You may also include convenience test vectors:

```cpp
float x0[4] = { ... };
float x1[4] = { ... };
float x2[4] = { ... };
```

These are optional.

---

---

## 3. Interactive Serial Input (Optional)

You can make the ESP32 wait for user input via the serial monitor:

```
4 values separated by commas:
5.1, 3.5, 1.4, 0.2
```

Then parse and classify them.

---

## 4. Replacing the Model

To replace the ML model:

1. Train a new model in Python
2. Export a new `irisModel.h`
3. Replace the existing file in your Arduino sketch
4. Recompile and upload

Make sure:

- Input size matches (`TF_NUM_INPUTS`)
- Output size matches (`TF_NUM_OUTPUTS`)
- You add required ops to the resolver (e.g. Dense, Softmax)

---

## Notes

- Model memory (ARENA_SIZE) may need adjustment
- TFLite Micro does not support all TensorFlow ops
- Keep networks small for ESP32 (Dense layers only)
- Normalize your inputs exactly the same way as during training

---

## ✔ Works On

- ESP32
- ESP32-S3
- ESP32-C3
- Arduino IDE
- PlatformIO
