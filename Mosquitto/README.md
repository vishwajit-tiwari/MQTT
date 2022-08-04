# Mosquitto Brocker

## How to setup :
1. MQTT Brocker
2. MQTT Publisher
3. MQTT Subscriber

### 1. MQTT Brocker
- On either Ubuntu or on Raspberry Pi
- Command to install mosquitto brocker

```bash
sudo apt install mosquitto
sudo apt-get install mosquitto-clients # Optional
```
- Create the ".txt" file on home directory and 

```bash
cdac@ubuntu:~$ pwd
#/home/cdac
cdac@ubuntu:~$ touch mqtt.txt
```
- Write the following lines inside .txt file

```bash
nano mqtt.txt
# Write these lines 
listener 1883
allow_anonymous true
```

- Start the brocker

```bash 
# To enable Mosquitto 
sudo systemctl enable mosquitto.service
# Check status
sudo systemclt status mosquitto.service
# Run Mosquitto Broker
mosquitto -c mqtt.txt -v 
```

- If Mosquitto broker is not running, restart it.

```bash
# If not starting run these commands
sudo systemctl stop mosquitto.service
# Check Status
sudo systemctl status mosquitto.service
# Run Broker
mosquitto -c mqtt.txt -v
```
![Start Broker](https://tinypic.host/images/2022/08/04/Screenshot-from-2022-08-04-10-34-33.png "How to start Broker")

**Note:** 
1. Always use "ctrl+c" to stop broker
2. Don't use "ctrl+x" or "ctrl+z" 

![To Stop Broker](https://tinypic.host/images/2022/08/04/Screenshot-from-2022-08-04-11-15-45.png)

### 2. MQTT Publisher
- On Raspberry Pi or on Ubuntu
- Command to install mosquitto 

```bash
sudo apt-get install mosquitto # optional
sudo apt-get install mosquitto-clients
```
- Now start sending data 

```bash 
mosquitto_pub -h 192.168.43.75 -t test -m "Hello from Raspberry Pi" # ip of brocker
```
![How to Publish](https://tinypic.host/images/2022/08/04/Screenshot-from-2022-08-04-11-32-20.png)

### 3. MQTT Subscriber
- On other Raspberry Pi or Ubuntu terminals
- Command to install mosquitto 

```bash
sudo apt-get install mosquitto # Optional
sudo apt-get install mosquitto-clients
```
- Now start receiving data 

```bash
mosquitto_sub -h 192.168.43.75 -t test # ip of Brocker
```

![How to Subscribe](https://tinypic.host/images/2022/08/04/Screenshot-from-2022-08-04-11-26-43.png)

### MQTT Dashboard (Mobile Application)
- Enter the details of your brocker
  - Brocker Name: mosquitto (or any name)
  - Address of brocker: 192.168.43.75
  - Port number: 1883 or 8883
  - Client Id: MqttDashboard-1341078
  - topic: test

- Save all these details
- Now we have to enter topic name: test

### For More info 
[mosquitto_Auth](https://mosquitto.org/documentation/authentication-methods/)

