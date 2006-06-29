################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AngularMeter.cpp \
../LCDWindow.cpp 

OBJS += \
./AngularMeter.o \
./LCDWindow.o 

CPP_DEPS += \
./AngularMeter.d \
./LCDWindow.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo Building file: $<
	@echo Invoking: GCC C++ Compiler
	@echo g++ -DWX_PRECOMP -D__WXDEBUG__ -D__WXMSW__ -DNO_GCC_PRAGMA -IC:/wxWidgets-2.6.2/msw-debug/lib/wx/include/msw-ansi-debug-static-2.6 -IC:/wxWidgets-2.6.2/include -O0 -g3 -Wall -c -fmessage-length=0 -mthreads -MMD -MP -MF\"$(@:%.o=%.d)\" -MT\"$(@:%.o=%.d)\" -o\'$@\' \'$<\'
	@g++ -DWX_PRECOMP -D__WXDEBUG__ -D__WXMSW__ -DNO_GCC_PRAGMA -IC:/wxWidgets-2.6.2/msw-debug/lib/wx/include/msw-ansi-debug-static-2.6 -IC:/wxWidgets-2.6.2/include -O0 -g3 -Wall -c -fmessage-length=0 -mthreads -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o'$@' '$<'
	@echo Finished building: $<
	@echo ' '


