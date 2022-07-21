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
- Start the brocker

```bash 
sudo systemctl enable mosquitto.services
mosquitto -c user_file.txt -v 
```

### 2. MQTT Publisher
- On Raspberry Pi 
- Command to install mosquitto 

```bash
sudo apt-get install mosquitto
sudo apt-get install mosquitto-clients
```
- Now start sending data 

```bash 
mosquitto_pub -h 192.168.43.75 -t test -m "Hello from Raspberry Pi" # ip of brocker
```

### 3. MQTT Subscriber
- On other Raspberry Pi or Ubuntu terminals
- Command to install mosquitto 

```bash
sudo apt-get install mosquitto
sudo apt-get install mosquitto-clients
```
- Now start receiving data 

```bash
mosquitto_sub -h 192.168.43.75 -t test # ip of Brocker
```

### MQTT Dashboard (Mobile Application)
- Enter the details of your brocker
  - Brocker Name: mosquitto (or any name)
  - Address of brocker: 192.168.43.75
  - Port number: 1883 or 8883
  - Client Id: MqttDashboard-1341078
  - topic: test

- Save all these details
- Now we have enter topic name: test


### For More info 
[mosquitto_Auth](https://mosquitto.org/documentation/authentication-methods/)

