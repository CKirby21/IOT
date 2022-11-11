# Fetch

## What is this project?

- This is a project for UNL CSCE 438 IOT. The intention is there are two components: the web component and the hardware component. The hardware component consists of two Arduino-based microcontrollers. These controllers handle GPS information. One is a dog collar and one is the "home base" that sends information back to Microsoft Azure. The web component handles user interaction and access to the mapping information and interface.

## How to run
### Tool Installation
Ensure you are using `node 18.12.1` or later (this is the latest LTS build at the time of development). [NVM](https://github.com/nvm-sh/nvm) is a great tool for node version management. Following that, make sure you have the Ionic CLI installed. You can do this with `npm install -g @ionic/cli`, or Google "install ionic react" and it's the first link to `ionicframework.com`, and it will guide you through the command(s).

### Running the code
After installing the necessary tools, use the `npm install` command in the directory that contains the source code (so in `/Fetch/Fetch`) to install the necessary dependencies, and then run `ionic serve` to run the react startup script and open a browser window with the site. (`npm start` will also do that, just with a slightly different URL and port assignment)

### TLDR (assuming you're using NVM)
- `nvm install --lts` - installs latest LTS build of node (18.12.1 at time of development)
- `npm install -g @ionic/cli native-run cordova-res` - installs Ionic CLI and other native development tools
- `cd /Fetch/Fetch`, then run `npm install` to install dependencies
- in that same directory, run `ionic serve` to open a browser window and serve the site