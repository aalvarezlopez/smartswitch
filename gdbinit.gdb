target remote localhost:3333
monitor reset halt
load
monitor reset halt
break HardFault_Handler
break isLinkUp
break Display_Init
set disassemble-next-line on
