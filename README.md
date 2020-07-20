# ankreo-spudpole

Abstract data type modelling spudpoles from the manufacturer Ankreo.

The __Spudpole__ class implements an abstract data type which models the key
characteristics of a winch-operated spudpole. The ADT uses a state machine
to represent spudpole hardware and supports the following states.

* __UNKNOWN__.      The state of the hardware is unknown. This is generally a temporary
                    and transient condition which will be updated as soon as the machine
                    begins to operate.
* __DOCKED__        The spudpole is fully retracted and docked. This state is forced
                    when a call is made to the _setDocked()_ method. Typically this call
                    will be triggered when the host hardware interface detects through a
                    sensor that the pole has been shipped and secured. 
* __DEPLOYING__     The spudpole is being deployed. This state is forced when a call is
                    made to the _deploy()_ method. Typically, this call will be triggered
                    when a command for deployment of the pole is received over some
                    external control mechanism.
* __RETRIEVING__    The spudpole is being retrieved. This state is forced when a call is
                    made to the _retrieve()_ method (the transition to RETRIEVING cannot
                    be made from the DOCKED state).  Typically, this call will be triggered
                    when a command for retrieval of the pole is received over some
                    external control mechanism.
* __STOPPED__       The spudpole is stopped. This state is forced when a call is made to
                    the _Stop()_ method and the transition will only occur from the
                    DEPLOYING state. Typically, this call will be triggered when either
                    (i) a command to stop pole deployment or retrieval is received over
                    some external control mechanism, or (ii) the host hardware interface
                    detects that the spudpole is fully deployed or that the pole has
                    embedded in _terra-firma_.                  
                    
This basic state model is supplemented by an anchor rode counter and mechanisms
supporting logging of winch motor runtime and measuremnt of deployed rode.
  
#### Creating and configuring a Spudpole instance
 
A __Spudpole__ instance is created in the usual C++ way by simply declaring a
variable of the Spudpole type.
```
void callback(int action) {
  switch (action) {
    case 0: break; // Switch motor off
    case 1: break; // Deploy rode
    case 2: break; // Retrieve rode
};

Spudpole mySpudpole(callback);
```
_actionCallback_ is a callback function which will be called by the spudpole with
an argument expressing the required external operating state: 0 = stop, 1 = deploy,
2 = retrieve.

There are a number of configuration methods which set-up and select some optional
features.

The _configureRodeMeasurement()_ method allows the introduction of configuration
values which support calculation of the length of anchor rode deployed at any
given time.  If this configuration is not done, then the only deployment data
available will be a counter indication revolutions of the windlass spool.
```
// Diameter of the windlass spool/drum.
double spoolDiameter = 0.06;
// Diameter of the anchor rode.
double rodeDiameter = 0.01;
// Nominal number of turns across the spool/drum.
unsigned char spoolCapacity = 12;
// Number of spool revolutions between docked and fully deployed.
unsigned char operatingCapacity = 60;

mySpudpole.configureRodeMeasurement(spoolDiameter, rodeDiameter, spoolCapacity, operatingCapacity);
```

The _configureMotorRuntime()_ method allows the introduction of configuration
values which support recording of winch motor runtime.
```
mySpudpole.configureMotorRuntime(
 
 Before use, a new instance must be initialised by calling at least the _initialise()_
 method and, optionally, the _initialiseMotorTimer()_ method.
 ```
 mySpudpole.initialise(actionCallback);
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
