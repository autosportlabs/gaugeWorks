################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AngularMeter.cpp \
../src/LCDWindow.cpp 

OBJS += \
./src/AngularMeter.o \
./src/LCDWindow.o 

CPP_DEPS += \
./src/AngularMeter.d \
./src/LCDWindow.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo Building file: $<
	@echo Invoking: GCC C++ Compiler
	@echo g++ -DWX_PRECOMP -D__WXMSW__ -DNO_GCC_PRAGMA -IC:/wxWidgets-2.6.2/msw-release/lib/wx/include/msw-ansi-release-static-2.6 -I../include -IC:/wxWidgets-2.6.2/include -O3 -Wall -c -fmessage-length=0 -mthreads -MMD -MP -MF\"$(@:%.o=%.d)\" -MT\"$(@:%.o=%.d)\" -o\'$@\' \'$<\'
	@g++ -DWX_PRECOMP -D__WXMSW__ -DNO_GCC_PRAGMA -IC:/wxWidgets-2.6.2/msw-release/lib/wx/include/msw-ansi-release-static-2.6 -I../include -IC:/wxWidgets-2.6.2/include -O3 -Wall -c -fmessage-length=0 -mthreads -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o'$@' '$<'
	@echo Finished building: $<
	@echo ' '


