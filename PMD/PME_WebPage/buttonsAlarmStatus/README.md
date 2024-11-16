# Summary
With MQTT this web page controlls an ESP32 on homework2 PCB running firmware: FT_PMD_MQTT V0.5 

## Function
Pressing any of the buttons for "Emergency Level" sends an MQTT message for that alarm level and the LED on the hardware lights. 

![image](https://github.com/user-attachments/assets/6a7e2e53-4bc7-4ebf-973f-cc95af48d7f2)

## Some Details
Hard coded MQTT client  
client = mqtt.connect('wss://public:public@public.cloud.shiftr.io');
