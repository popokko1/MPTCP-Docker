# predict_rf.py
import joblib
import numpy as np

# モデルとスケーラーの読み込み
model = joblib.load("rf_model3.pkl")
scaler = joblib.load("scaler3.pkl")

def predict(features):
    # features: list or numpy array of shape (5,)
    features = np.array(features).reshape(1, -1)
    features_scaled = scaler.transform(features)
    prediction = model.predict(features_scaled)
    return int(prediction[0])
