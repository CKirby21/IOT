# Load all the libraries
import pandas as pd
import numpy as np
import pickle
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score, classification_report
from sklearn.linear_model import LogisticRegression
import time
from iotc.models import Command, Property
from iotc import IoTCClient, IOTCConnectType, IOTCEvents

# Load the data
dataset = 'weatherAUS.csv'
rain = pd.read_csv(dataset)
# Reduce the cardinality of date by splitting it into year month and day
rain['Date'] = pd.to_datetime(rain['Date'])
rain['year'] = rain['Date'].dt.year
rain['month'] = rain['Date'].dt.month
rain['day'] = rain['Date'].dt.day
rain.drop('Date', axis = 1, inplace = True)

# Classify feature type
categorical_features = [
    column_name
    for column_name in rain.columns
    if rain[column_name].dtype == 'O'
]
numerical_features = [
    column_name
    for column_name in rain.columns
    if rain[column_name].dtype != 'O'
]
# Fill missing categorical values with the highest frequency value in the column
categorical_features_with_null = [
    feature
    for feature in categorical_features
    if rain[feature].isnull().sum()
]
for each_feature in categorical_features_with_null:
    mode_val = rain[each_feature].mode()[0]
    rain[each_feature].fillna(mode_val,inplace=True)

 # Before treating the missing values in numerical values, treat the outliers
features_with_outliers = [
    'MinTemp',
    'MaxTemp',
    'Rainfall',
    'Evaporation',
    'WindGustSpeed',
    'WindSpeed9am',
    'WindSpeed3pm',
    'Humidity9am',
    'Pressure9am',
    'Pressure3pm',
    'Temp9am',
    'Temp3pm'
]
for feature in features_with_outliers:
    q1 = rain[feature].quantile(0.25)
    q3 = rain[feature].quantile(0.75)
    IQR = q3 - q1
    lower_limit = q1 - (IQR * 1.5)
    upper_limit = q3 + (IQR * 1.5)
    rain.loc[rain[feature]<lower_limit,feature] = lower_limit
    rain.loc[rain[feature]>upper_limit,feature] = upper_limit

 # Treat missing values in numerical features
numerical_features_with_null = [
    feature
    for feature in numerical_features
    if rain[feature].isnull().sum()
]
for feature in numerical_features_with_null:
    mean_value = rain[feature].mean()
    rain[feature].fillna(mean_value,inplace=True)
# Encoding categorical values as integers
direction_encoding = {
    'W': 0, 'WNW': 1, 'WSW': 2, 'NE': 3, 'NNW': 4,
    'N': 5, 'NNE': 6, 'SW': 7, 'ENE': 8, 'SSE': 9,
    'S': 10, 'NW': 11, 'SE': 12, 'ESE': 13, 'E': 14, 'SSW': 15
}

location_encoding = {
 'Albury': 0,
 'BadgerysCreek': 1,
 'Cobar': 2,
 'CoffsHarbour': 3,
 'Moree': 4,
 'Newcastle': 5,
 'NorahHead': 6,
 'NorfolkIsland': 7,
 'Penrith': 8,
 'Richmond': 9,
 'Sydney': 10,
 'SydneyAirport': 11,
 'WaggaWagga': 12,
 'Williamtown': 13,
 'Wollongong': 14,
 'Canberra': 15,
 'Tuggeranong': 16,
 'MountGinini': 17,
 'Ballarat': 18,
 'Bendigo': 19,
 'Sale': 20,
 'MelbourneAirport': 21,
 'Melbourne': 22,
 'Mildura': 23,
 'Nhil': 24,
 'Portland': 25,
 'Watsonia': 26,
 'Dartmoor': 27,
 'Brisbane': 28,
 'Cairns': 29,
 'GoldCoast': 30,
 'Townsville': 31,
 'Adelaide': 32,
 'MountGambier': 33,
 'Nuriootpa': 34,
 'Woomera': 35,
 'Albany': 36,
 'Witchcliffe': 37,
 'PearceRAAF': 38,
 'PerthAirport': 39,
 'Perth': 40,
 'SalmonGums': 41,
 'Walpole': 42,
 'Hobart': 43,
 'Launceston': 44,
 'AliceSprings': 45,
 'Darwin': 46,
 'Katherine': 47,
 'Uluru': 48
}
boolean_encoding = {'No': 0, 'Yes': 1}

rain['RainToday'].replace(boolean_encoding, inplace = True)
rain['RainTomorrow'].replace(boolean_encoding, inplace = True)
rain['WindGustDir'].replace(direction_encoding,inplace = True)
rain['WindDir9am'].replace(direction_encoding,inplace = True)
rain['WindDir3pm'].replace(direction_encoding,inplace = True)
rain['Location'].replace(location_encoding, inplace = True)
# See the distribution of the dataset
print(rain['RainTomorrow'].value_counts())
# Split features and target value as X and Y
X = rain.drop(['RainTomorrow'],axis=1)
y = rain['RainTomorrow']
# Split training and test split
X_train, X_one_percent, y_train, y_one_percent = train_test_split(X,y, test_size = 0.01, random_state = 0)
X_train, X_test, y_train, y_test = train_test_split(X_train,y_train, test_size = 0.2, random_state = 0)
# Scale input using just the training set to prevent bias
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)
# Train the model
classifier_logreg = LogisticRegression(solver='liblinear', random_state=0)
classifier_logreg.fit(X_train, y_train)
# Test the model accuracy
y_pred = classifier_logreg.predict(X_test)
print(f"Accuracy Score: {accuracy_score(y_test,y_pred)}")
print("Classifcation report", classification_report(y_test,y_pred))

with open("iot_model", "wb") as model_file:
    pickle.dump(classifier_logreg, model_file)

# Send 1% of data to IoT Central
scopeId = '0ne007959ED'
device_id = '1srvwcyznuq'
device_key = 'sgZ3E+xh+SLfE3ayqoFkN1sur8nqQ+VZgtu5cIeWxyc='

def on_commands(command: Command):
    print(f"{command.name} command was sent")
    command.reply()

iotc = IoTCClient(
        device_id,
        scopeId,
               IOTCConnectType.IOTC_CONNECT_DEVICE_KEY,
               device_key)
iotc.connect()

iotc.on(IOTCEvents.IOTC_COMMAND, on_commands)

# Send 1% of the dataset to IOT Central
for index, row in X_one_percent.iterrows():
    telemetry_dict = dict(zip(X_one_percent.columns.values, row))
    iotc.send_telemetry(telemetry_dict)
    time.sleep(1)
    # break

