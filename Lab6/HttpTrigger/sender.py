import requests 
 
iotc_sub_domain = "cadens-iot-central" 
device_id = "2dshe37ngau" 
api_key = "SharedAccessSignature sr=c1debb04-0148-4abe-a92c-257f2839c085&sig=dRxAyy%2By1KDqzZK4fX3Tf134pGgiFst%2FNXfgX7wnEsQ%3D&skn=tolkien&se=1696083732330=" 
 
def _command_url(): 
    return f"https://{iotc_sub_domain}.azureiotcentral.com/api/devices/{device_id}/commands/SendData?api-version=2022-05-31" 
    
def send_command(): 
    resp = requests.post(
        _command_url(), 
        json={ "ItRained": "1", "RainTomorrow": "1" }, 
        headers={"Authorization": api_key}     
    ) 
    print(resp.json()) 
    
print(send_command())