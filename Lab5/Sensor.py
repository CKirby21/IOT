import random 
import time
 
from iotc.models import Command, Property 
from iotc import IoTCClient, IOTCConnectType, IOTCEvents 
 
device_key = 'hIBq7isjmSBB91hFlsqJAVycNTHgzJJO9t+gOx8ilC0='
device_id =  '8mj9oyfuia' 
scopeId = '0ne00795B6C'
 
    
def on_commands(command: Command): 
    iotc.send_telemetry({ 
            'Temperature': str(random.randint(0, 40)), 
            'Humidity': str(random.randint(0, 40)),
            'Pressure': str(random.randint(0, 40))  
        }) 
    print(f"{command.name} command was sent") 
    iotc.send_property({ 
    "LastCommandReceived": time.time() 
    }) 
    command.reply() 
 
 
 
iotc = IoTCClient(
        device_id,  
        scopeId, 
               IOTCConnectType.IOTC_CONNECT_DEVICE_KEY, 
               device_key) 
 
iotc.connect() 
 
 
iotc.on(IOTCEvents.IOTC_COMMAND, on_commands) 
 
iotc.send_property({ 
    "LastTurnedOn": time.time() 
}) 
 
 
while iotc.is_connected(): 
        iotc.send_telemetry({ 
            'Temperature': str(random.randint(0, 40)), 
            'Humidity': str(random.randint(0, 40)),
            'Pressure': str(random.randint(0, 40))  
        }) 
        time.sleep(60)
