# Fetch

## What is this project?
This is a project for UNL CSCE 438 IOT. The intention is there are two components: the web component and the hardware component. The hardware component consists of two Arduino-based microcontrollers. These controllers handle GPS information. One is a dog collar and one is the "home base" that sends information back to Microsoft Azure. The web component handles user interaction and access to the mapping information and interface.

## How to run
### Tool Installation
Ensure you are using `node 18.12.1` or later (this is the latest LTS build at the time of development). [NVM](https://github.com/nvm-sh/nvm) is a great tool for node version management. Following that, make sure you have the Ionic CLI installed. You can do this with `npm install -g @ionic/cli`, or Google "install ionic react" and it's the first link to `ionicframework.com`, and it will guide you through the command(s).

### Running the code
1. Go here: https://azure-samples.github.io/raspberry-pi-web-simulator/#Getstarted to simulate the collar/gateway setup
    1. Change the device connection string to `HostName=CadensIOTHub.azure-devices.net;DeviceId=FetchGateway;SharedAccessKey=VLwfetwWHHCp4OjRtDtJIPuhsxsEKIb0+9vxqtdWKYk=` 
    1. Click Run
1. Navigate to /Fetch/Backend and run these commands to spin up a node server that arbitrates access to Azure
    1. `npm install`
    1. `$env:IotHubConnectionString="HostName=CadensIOTHub.azure-devices.net;SharedAccessKeyName=service;SharedAccessKey=qkrdFA48kfHiha4kzBWm16IfwASohSEog2Kvk3UD+Ro="`
    1. `$env:EventHubConsumerGroup="cadensconsumergroup"`
    1. `npm start`
1. Navigate to /Fetch/Fetch and run these commands to spin up a web app that requests the node server for Azure data
    1. `npm install`
    1. `ionic serve`

### TLDR (assuming you're using NVM)
- `nvm install --lts` - installs latest LTS build of node (18.12.1 at time of development)
- `npm install -g @ionic/cli native-run cordova-res` - installs Ionic CLI and other native development tools

### Notes
- Maps API docs: https://react-google-maps-api-docs.netlify.app/ 