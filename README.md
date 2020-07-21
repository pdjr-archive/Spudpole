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
                    
This basic state model is supplemented by a counter which records the extent of
winch line deployment in terms of rotations of the winch spool.

__Spudpole__ functionality can be extended by configuring mechanisms to support external
control, winch line length calculations and the recording of winch motor runtime.
  
## Creating and configuring a Spudpole instance
 
A __Spudpole__ instance is created in the usual C++ way by simply declaring a
variable of the Spudpole type and supplying some arguments which characterise
the new object.
```
#define manufacturerName "Ankreo"
#define modelCode "Type 32"
#define serialCode" "0001"
Spudpole mySpudpole(manufacturerName, modelCode, serialCode);
```
There following configuration methods set-up and enable optional features.

The _setControlCallback()_ method forces __Spudpole__ call an external function
each time the state model enters DEPLOYING, RETRIEVING or STOPPED states.  The
supplied function will be called with an argument expressing the required
external operating state: 0 = stop, 1 = deploy, 2 = retrieve.
```
void callback(int action) {
  switch (action) {
    case 0: break; // Switch motor off
    case 1: break; // Deploy rode
    case 2: break; // Retrieve rode
};

Spudpole mySpudpole(callback);
```
By default, __Spudpole__ models the extent of deployment of its pole by
counting rotations of the winch spool and there are a number of methods
which allow this counter to be updated. The _configureLineMeasurement()_
method enables support for calculating the lenth of line deployed by
the winch from the primitive rotation counter
```
// Diameter of the windlass spool/drum.
double spoolDiameter = 0.06;
// Diameter of the anchor rode.
double rodeDiameter = 0.01;
// Nominal number of turns across the spool/drum.
unsigned int spoolCapacity = 12;
// Number of spool revolutions between docked and fully deployed.
unsigned int operatingCapacity = 60;

mySpudpole.configureLineMeasurement(spoolDiameter, rodeDiameter, spoolCapacity, operatingCapacity);
```
The _configureMotorRuntime()_ method enables support for recording the total
run time of the winch motor.
```
unsigned long runtimeStart = 0L;
unsigned long timer(int mode, totalRuntime) {
  static unsigned long retval = totalRuntime;
  static unsigned long timestamp;
  switch (mode) {
    case 0: timestamp = millis(); break;
    case 1: retval += (millis() - timestamp); break;
    default: break;
  }
  return(retval);
}

mySpudpole.configureMotorRuntime(runtimeStart, timer);
```
## Methods

### Primitive methods

The configured instance identifiers can be recovered using three eponymous
_get..._ methods.
```
char* n = mySpudpole.getManufacturerName();
char* m = mySpudpole.getModelCode();
char* s = mySpudpole.getSerialCode();
```
Machine state is directly modified by the _setDocked()_, _deploy()_,
_retrieve()_ and _stop()_ methods each of which immediately asserts the
implied state and, with the exception of _setDocked()_, will call any
defined control callback function.  A side-effect of _setDocked()_ is
to zero the winch rotation counter.
```
mySpudpole.setDocked()
mySpudpole.deploy();
mySpudpole.retrieve();
mySpudpole.stop();
```
There are a number of methods which allow interrogation of the current
state.
```
SpudpoleState s = mySpudpole.getState();  // returns an enum value
bool b = mySpudpole.isWorking();          // true if DEPLOYING or RETRIEVING
bool b = mySpudpole.isDocked();           // true if DOCKED
bool b = mySpudpole.isDeployed();         // true if STOPPED
```

The winch rotation counter can be accessed using a number of functions.  The
counter is a rotation counter and should be updated once per winch revolution
by a strategy that is left to the host application.  Note that the counter is
reset to zero by a call to _setDocked()_.

The _getCounter()_ method returns the current counter value. The counter can be
updated using one of _counterIncr()_, _counterDecr()_ or _counterBump()_. Calls
to _counterIncr()_ or _counterDecr()_ will only be honoured if the machine state
is DEPLOYING or RETRIEVING respectively. A call to _counterBump()_ will increment
or decrement the counter value dependent upon the machine state.
```
unsigned long now = mySpudpole.getCounter();
mySpudpole.counterIncr();
mySpudpole.counterDecr();
mySpudpole.counterBump();
```

### Retrieving deployed line length

If line measurement has been configureded then the currently deployed length of the
winch line in metres can be computed from the current counter value.
```
double getDeployedLineLength();
```

### Retrieving total motor run time 

If motor runtime accounting has been configured then the currently computed total
motor run time in milliseconds can be easily recovered.
```
unsigned long getMotorRuntime();
```

 
