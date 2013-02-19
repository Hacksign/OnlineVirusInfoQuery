################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sources/Curl.cpp \
../src/sources/Entry.cpp \
../src/sources/JHelper.cpp \
../src/sources/VSHelper.cpp \
../src/sources/VTHelper.cpp 

OBJS += \
./src/sources/Curl.o \
./src/sources/Entry.o \
./src/sources/JHelper.o \
./src/sources/VSHelper.o \
./src/sources/VTHelper.o 

CPP_DEPS += \
./src/sources/Curl.d \
./src/sources/Entry.d \
./src/sources/JHelper.d \
./src/sources/VSHelper.d \
./src/sources/VTHelper.d 


# Each subdirectory must supply rules for building sources it contributes
src/sources/%.o: ../src/sources/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/hacksign/Code/Cpp/VInfo/src/headers" -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


