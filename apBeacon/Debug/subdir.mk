################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../BeaconSender.cc \
../BeaconSniffer.cc \
../main.cc \
../myID.cc 

CC_DEPS += \
./BeaconSender.d \
./BeaconSniffer.d \
./main.d \
./myID.d 

OBJS += \
./BeaconSender.o \
./BeaconSniffer.o \
./main.o \
./myID.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.o: ../main.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/tins -I/usr/include/tins/dot11 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"main.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


