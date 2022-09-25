import random 
import time
 
from iotc.models import Command, Property 
from iotc import IoTCClient, IOTCConnectType, IOTCEvents 
 
scopeId = '0ne00795B6C'
device_id = '2k2l5n15fp8' 
device_key = 'OeJ2vvnUXyLLlEASBjrZqBBwekpPEvYTTuVa+eeA4n8=' 
 
    
def on_commands(command: Command): 
    iotc.send_telemetry({ 
            'Temperature': str(random.randint(0, 40)), 
            'Humudity': str(random.randint(0, 40)),
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
            'Humudity': str(random.randint(0, 40)),
            'Pressure': str(random.randint(0, 40))  
        }) 
        time.sleep(60)
