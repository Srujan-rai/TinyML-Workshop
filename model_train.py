from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler
import numpy as np
import tensorflow as tf

# 1. Load dataset
dataset = load_iris()
X = dataset.data
y = dataset.target

# 2. Normalize
X = MinMaxScaler().fit_transform(X)

# 3. Split
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

# 4. Build model
model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(4,)),
    tf.keras.layers.Dense(8, activation='relu'),
    tf.keras.layers.Dense(3, activation='softmax')
])

model.compile(optimizer='adam',
              loss='sparse_categorical_crossentropy',
              metrics=['accuracy'])

model.fit(X_train, y_train, epochs=50, batch_size=4)

# 5. Convert to TFLite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# 6. Convert to C array
def convert_to_c_array(model_bytes):
    hex_array = ", ".join(["0x{:02x}".format(x) for x in model_bytes])
    return f"""
#pragma once
const unsigned char irisModel[] = {{
    {hex_array}
}};
const unsigned int irisModel_len = {len(model_bytes)};
"""

# 7. Write to file
with open("irisModel.h", "w") as f:
    f.write(convert_to_c_array(tflite_model))

print("✔ irisModel.h generated successfully!")
