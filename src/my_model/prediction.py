import pandas as pd
import joblib

# 1. モデルとスケーラーの読み込み
model_path = './rf_model3.pkl'
scaler_path = './scaler3.pkl'

loaded_rf = joblib.load(model_path)
loaded_scaler = joblib.load(scaler_path)

# 2. 新しいデータの読み込み
file_path = './研究用データセット2.xlsx'
df = pd.read_excel(file_path)

# 3. 特徴量の抽出と追加加工（buffer列）
features = df.iloc[:, :5].copy()
#features['buffer'] = features.iloc[:, 2] ** 2  # loss^2

# 4. 特徴量の標準化（保存済みスケーラーを使用）
features_standardized = loaded_scaler.transform(features)

# 5. DataFrame化（列名は訓練時と揃える）
standardized_df = pd.DataFrame(features_standardized, columns=df.columns[:5])

# 6. 予測対象データの選択（例：先頭6件）
new_data = standardized_df.iloc[:15]

# 7. 予測の実行
predictions = loaded_rf.predict(new_data)
print(f'予測結果: {predictions}')
