# 1. Sensor
<br>

# 2. Arduino
<br>

# 3. Raspberrypi
<br>




# 4. ScadaBR (Syssec82)
## 4.1 Installation
***Intall ScadaBR in VMs and run it on bridge model, lanuch browser to access the control pages.***
<br>

## 4.2 Add data source
* ***Add new data source with modbus IP protocol.***
* ***Then configuring ip address.***
* ***Port number for modbus IP is 502***

![image](./img/img_ScadaBR_1.jpg)
<br>
## 4.3 Add point (data item)
***Offset here corresponding to modbus addresses, ScadaBR will connect to OpenPLC via Modbus ip protocol and read from the modbus addresses. <br>
%IW100 -> 100 (pressure) <br>
%IW101 -> 101 (temperature) <br>***
<br>
![image](./img/img_ScadaBR_2.jpg)
![image](./img/img_ScadaBR_3.jpg)
<br>

# 5 OpenHistorian (Syssec82)
<br>

# syssec82
**ip: 10.176.150.82 <br>
user: syssec82 <br>
pass: syssec82 <br>
<br>
ScadaBR: localhost:9090/ScadaBR <br>
account: admin <br>
pass: admin <br>**

