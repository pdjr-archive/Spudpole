# ankreo-spudpole

NMEA 2000 (N2K) interface module for Ankreo spud poles.

This project implements a hardware and firmware solution for interfacing
Ankreo spudpoles onto an N2K bus using the Anchor Windlass Interface described
in ...

## Hardware

The prototype hardware platform consists of:

* Teensy 3.5 MCU board
* CAN adapter
* Opto isolator

## Firmware

Firmware is implemented using the Anduino IDE and consists of two principle
components:

* __Spudpole__      is a library class modelling an Ankreo spudpole as a simple state.
* __Module__        is an Arduino sketch providing hardware and N2K interfaces for a
                    __Spudpole__ instance.

### Spudpole

The __Spudpole__ class implements a state machine representing the associated spudpole
hardware through the following states.

* __UNKNOWN__.      The state of the hardware is unknown. This is generally a temporary
                    and transient condition which will be updated as soon as an input is
                    received from the host hardware interface.
* __DOCKED__        The spudpole is docked. This state is forced when a call is made
                    to the _setDocked()_ method. Typically this will be triggered when
                    the host hardware interface detects the triggering of an external pole
                    sensor activated by a retrieved pole. 
* __DEPLOYING__     The spudpole is being deployed. This state is forced when a
                    call is made to the _deploy()_ method. Typically, this will be a
                    consequence of a command for deployment being received in a
                    PGN128766 Anchor Windlass Control message being received over the
                    N2K bus.
* __RETRIEVING__    The spudpole is being retrieved. This state is forced when a
                    call is made to the _retrieve()_ method. The transition to
                    RETRIEVING cannot be made from the DOCKED state. 
* __STOPPED__       The spudpole is stopped, but is not DOCKED. This state is forced when
                    a call is made to the _setStopped()_ method. Typically this will be
                    triggered when the host hardware interface detects the triggering of 
                    the spudpole hardware's deployment sensor and will occur when either
                    spudpole is at its fully deployed extent or when the pole has embedded
                    in the bottom.
                    
 This basic state model is supplemented by an anchor rode counter and log of motor runtime.
 
 #### Creating and initialising a Spudpole instance
 
 A __Spudpole__ instance is created in the usual C++ way by simply declaring a
 variable of the Spudpole type.
 ```
 Spudpole mySpudpole;
 ```
 
 Before use, a new instance must be initialised by calling at least the _initialise()_
 method and, optionally, the _initialiseMotorTimer()_ method.
 ```
 mySpudpole.initialise(instance, config, actionCallback);
 ```
 _instance_ is an integer value in the range 0..253 specifying the NMEA instance number
 of this anchor windlass interface - if the host vessel has more than one anchor windlass
 then each must be identified by a different instance number. Typically, the __Spudpole__
 host application will harvest this value from hardware - frequently a switch is used to
 allow an installer to easily set or change the instance address of an installation.
 
 _config_ is a structure containing important configuration parameters which describe the
 physical characteristics of the spudpole hardware and the logical characteristics of the
 interface module and its firmware. 
 
 The _initialise()_ method loads 
 * method and supplying some critical configuration data.
 *
 * mySpudpole.initialise(instance, configuration, action);
 *
 * The <instance> value should be an integer in the range 0..252 which gives
 * the unique N2K identifier for <mySpudpole>.
 *
 * The <configuration> value must be a structure of type <SpudpoleConfiguration>
 * whose properties define the unique characteristics of <mySpudpole>, including
 * values which characterise associated hardware.
 *
 * Finally, the <action> value should be a pointer to a callback function with
 * the signature void action(int doThis) which will be called by a 
 *POSITION represents the length of anchor rode deployed from the windlass
 * drum in metres. The setDocked() function sets position to 0.0m which
 * indicates that the spudpole is fully retrieved and docked and 
