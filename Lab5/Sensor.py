import random
import time
from iotc.models import Command, Property
from iotc import IoTCClient, IOTCConnectType, IOTCEvents
scopeId = '0ne007959ED'
device_id = 'g178ewxy9f'
device_key = 'o8BTVpJ5IyvrfXu5epr16IwAdQikiA8zWlSNJDWIONs='

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

iotc.send_property({
    "LastTurnedOn": time.time()
})

while iotc.is_connected():
    iotc.send_telemetry({
    'Temperature': str(random.randint(0, 40))
    })

time.sleep(60)