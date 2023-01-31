

# Custom RPI Image installation

 - Using RPI Imager to install RPiOS Lite 64 Bit.
 ```ini
 hostname=croissant2842 or croissant2843
 user=access
 pwd=<you know it>
 keyboard=de
 ``` 
 - edit /boot/config.txt and add screen config:
 ```ini
 hdmi_group=2
 hdmi_mode=87
 hdmi_cvt 800 480 60 6 0 0 0
 hdmi_ignore_edid=0xa5000080
 ```
 - manually configuring the time might be required if ntp doesn't work:
 ```bash
 sudo timedatectl set-time '2023-01-25 15:21:10'
 ```
 - update system and install git
 ```bash
 sudo apt update
 sudo apt upgrade
 sudo apt install git
 ```
 - install libwallaby following the build instructions in the readme
 
 