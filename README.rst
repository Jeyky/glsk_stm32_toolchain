#################################
Distance security system
#################################

About the project
************
A distance security system designed for scanning closed rooms. 
System alarms in case someone crossed the measurement axis and the measured distance is below the threshold. The system based on the GLobalLogic starter kit. User menus and lock/unlock menus implemented with LCD and 5 buttons from the board. In the menu, the user can see measured distance, change threshold distance, rotate the distance sensor, and enable/disable output of measured distance to the speedometer of the BMW E46 dashboard. Default threshold value saved in flash memory. Ambient temperature affects the speed of sound, so to make distance measurement more precise device uses dht11 and lm335 temperature sensors. DHT11 is the main temperature sensor, lm335 uses in case DHT11 doesn't respond. Alarm imitated by speaker controlled with PWM 1kHz signal. For this project was used stm32f407 but it can be implemented on stm32f1 series.
List of used sensors, devices:

* STM32F407
* LCD hd44780
* HC-SR04 distance sensor
* SG90 servo motor
* Speaker
* DHT11 temperature sensor
* LM335  temperature sensor
* BMW E46 dashboard

Build
************
.. code-block:: shell-session

   make PROFILE=relesae target flash

All needed files in the PROJECT folder, so if you need them you can move src, inc, and makefile to the root of the repository and build

Requirements
************

.. note::
   Arch Linux means access to the most recent software versions. Package names and installation
   commands provided here are given for the Arch Linux and its derivatives (i.e. Manjaro).
   
   If you are using another distro, you need to figure the package names yourself or use something
   like `Archlinux Docker image <https://hub.docker.com/_/archlinux>`_.

- `OpenOCD <http://openocd.org>`_.
  
  | Stable version: ``openocd``
    (Not recommended as it is outdated and incompatible with our openocd config.
     If you prefer stable version, use ``openocd -f board/stm32f4discovery.cfg``
     instead of our ``openocd -f openocd_glstarterkit.cfg``)
  | Latest Git version: ``openocd-git`` (through AUR)
- `arm-none-eabi Toolchain <https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm>`_
  
  Official release verison:
     
     | `GCC <https://gcc.gnu.org/>`_: ``arm-none-eabi-gcc``
     | `Binutils <https://www.gnu.org/software/binutils/>`_: ``arm-none-eabi-binutils``
     | `GDB <https://www.gnu.org/software/gdb/>`_: ``arm-none-eabi-gdb``
     | `Newlib <https://sourceware.org/newlib/>`_: ``arm-none-eabi-newlib``
- `Doxygen <https://doxygen.nl>`_ and `GraphViz <https://graphviz.org/>`_ for building libopencm3 documentation
  
  ``doxygen`` and ``graphviz`` packages

Add user to plugdev group:
~~~~~~~~~~~~~~~~~~~~~~~~~
This step is required to allow working with debuggers OpenOCD supports as a user, without a need
for having root privileges.

OpenOCD package on Arch comes with udev rules file (``/usr/lib/udev/rules.d/60-openocd.rules``).
It gives access rights to users in plugdev group, which exists on Debian, but is not present
on Arch Linux. So we need to create the group and add our user to it:

.. code-block:: shell-session
   
   sudo groupadd -r -g 46 plugdev
   sudo useradd -G plugdev $USER

And log out (or reboot)

Install all packages:
~~~~~~~~~~~~~~~~~~~~~
.. code-block:: shell-session
   
   yay -S openocd-git
   sudo pacman -S arm-none-eabi-{gcc,binutils,gdb,newlib} doxygen graphviz

.. note::
   You need to either run ``sudo udevadm control --reload-rules`` and ``sudo udevadm trigger``
   or to reboot after installing OpenOCD for udev rules to start working

How to start
************
#. Make sure you have completed the installation steps described in Requirements_ first.
#. Recursively clone the repository:
   
   .. code-block:: shell-session
      
      git clone --recursive https://github.com/kpi-keoa/glsk_stm32_toolchain
   
   or clone first and then initialize all submodules
   
   .. code-block:: shell-session
      
      git clone https://github.com/kpi-keoa/glsk_stm32_toolchain
      git submodule update --init --recursive
#. Study the `<Makefile>`_. It is crucial to understand how tools work for properly using them.
#. Build example projects
#. Start your own project using this repository as a template.
   
   For that, you will probably need to delete the `<documentation>`_ and example directories.
   And at least change ``TARGET`` to the name of your project top-level file

License
*******
| Everything in this repository, except the STMicroelectronics documentation is licensed
  under the MIT License.
| See `<LICENSE>`_ for details.
| 
| For more on STMicroelectronics documentation licensing consider their official website
  (`<https://st.com>`_)

