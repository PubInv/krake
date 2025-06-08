REM PingAll.bat

@echo off
Title Piniing all IP Addrsses & Mode 70,4 & color 0B
::-------------------------------------------------------------------------------------
REM First We Check The Status Of The Internet Connection
Call :Check_Connection
::-------------------------------------------------------------------------------------
:Main
Title Ping the IP Address

mode con: cols=160 lines=78

REM arp -a -v
arp -a

pause

FOR /L %%j IN (1, 1, 250) DO (
    echo Number: %%j
REM    Test-Connection -Ping -TargetName 192.168.1.%%j
REM    'Test-Connection' -Ping -TargetName 192.168.1.%%j
start ping -n 1 192.168.10.%%j

)

arp -a