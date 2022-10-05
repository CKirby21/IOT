import logging
import azure.functions as func
import pandas as pd
import pickle
import requests

iotc_sub_domain = "cadens-iot-central" 
device_id = "2dshe37ngau" 
api_key = "SharedAccessSignature sr=c1debb04-0148-4abe-a92c-257f2839c085&sig=dRxAyy%2By1KDqzZK4fX3Tf134pGgiFst%2FNXfgX7wnEsQ%3D&skn=tolkien&se=1696083732330="

def main(req: func.HttpRequest) -> func.HttpResponse:
    logging.info('Python HTTP trigger function processed a request.')

    # Get data from IOT Central
    req_body = req.get_json()
    telemetry = req_body.get('telemetry')
    logging.info(telemetry)
    telemetry_list = []
    telemetry_list.append(telemetry)
    X = pd.DataFrame(telemetry_list)

    # Make prediction on telemetry data
    with open("HttpTrigger/iot_model", "rb") as model_file:
        model = pickle.load(model_file)
        y_pred_new = model.predict(X)  
    logging.info(y_pred_new)
    
    # It the prediction is rain, then send command to listener
    if (y_pred_new[0] == 1):
        resp = requests.post(
        _command_url(), 
        json={}, 
        headers={"Authorization": api_key}     
        ) 
        logging.info(resp.json())
        
    return func.HttpResponse(
        "This HTTP triggered function executed successfully.",
        status_code=200
    )

def _command_url(): 
    return f"https://{iotc_sub_domain}.azureiotcentral.com/api/devices/{device_id}/commands/SendData?api-version=2022-05-31" 