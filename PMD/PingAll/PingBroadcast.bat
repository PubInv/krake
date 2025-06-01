REM from https://stackoverflow.com/questions/13713318/ping-all-addresses-in-network-windows

ECHO Starting broadcast ping.

ping 192.168.1.255
arp -a

pause