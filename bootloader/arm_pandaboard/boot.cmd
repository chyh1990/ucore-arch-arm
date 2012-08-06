usb start
sleep 1
setenv serverip 192.168.0.100
setenv ipaddr   192.168.0.120
setenv ipmask   255.255.255.0
tftpboot 90000000 loader
go 90000000

