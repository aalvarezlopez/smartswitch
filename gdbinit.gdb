target remote localhost:3333
monitor reset halt
load
monitor reset halt
break HardFault_Handler
set disassemble-next-line on
commands 1
monitor reset halt
c
end
