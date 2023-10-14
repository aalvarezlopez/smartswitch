target remote localhost:3333
monitor reset halt
load
monitor reset halt
break HardFault_Handler
commands
monitor reset halt
c
end
break udpTransmit
