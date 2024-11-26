# Welcome to my ESP32 Sensor


## Start on Windows with Docker and WSL

Make sure you have Docker installed for windows. 

1. **Only if your ESP32 isnt a shared device**
    1. Install [usbipd-win](https://github.com/dorssel/usbipd-win/releases) 
    2. Open your Windows Powershell as an administrator.
    3. First of all you need the busid for your ESP32. To get this you can run

            usbipd list

       Mostlikly it is a UART Brigde
    4. Run for example the following command 

            usbipd bind --busid 1-1
        
       Now your ESP32 is a shared device and can be linked to the WSL 
2. **Only if you haven't an image of this Docker Container**
    1. Go in the folder with the Dockerfile and run 

            docker image build -t esp_idf:v1 .

3. **To start a Dev Container run**

        docker run -t -i --privileged -v /dev/bus/usb:/dev/bus/usb esp_idf:v1 /bin/bash
    
    Now your is USB ESP32 connected with this newly start Dev Container


