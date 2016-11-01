@setlocal
@PATH=..\mspgcc-20120406-p20120911\bin\;..\MSP430Flasher_1.2.1\

@REM Compile the C code
msp430-gcc -mmcu=msp430g2553 main.c
@if errorlevel 1 goto end

@REM Convert compiled program to hex format
msp430-objcopy -O ihex a.out a.hex
@if errorlevel 1 goto end

@REM Copy hex file onto microcontroller
msp430flasher -n MSP430G2553 -w a.hex

:end
@pause
