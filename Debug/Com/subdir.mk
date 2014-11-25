################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Com/Com.cpp \
../Com/Thread.cpp 

OBJS += \
./Com/Com.o \
./Com/Thread.o 

CPP_DEPS += \
./Com/Com.d \
./Com/Thread.d 


# Each subdirectory must supply rules for building sources it contributes
Com/%.o: ../Com/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/OGRE -I/usr/include/OIS -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


