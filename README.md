*Disclaimer: this is a work in progress. Everything may change at any time.
Also, consider that I am not a software engineering in a strict sense, but I am a control system engineer.
Finally, this is a perpetual on-going work and at the moment Arduino work is
left a bit behind, so don't use it.  *

# Motivation
Concurrency program is tough. Bugs are just behind the corner and having a bugs free software is practically impossible.
However, one may try to prevent the occurrence of such bugs by adhering to some coding rules and/or by following some standards.

Here, we aim at defining a software architecture that can help in preventing bugs when concurrency programming is employed.
We target STM32 F4 and Arduino platforms. For the Arduino platform we use
Platformio.

# Requirements

1. The toolchain of the platform that you are using,
2. pyserial.

*Optional*:

2. compiledb - for creating compile\_commands.json files.


# Architecture
The architecture is depicted below.
The idea is to abstract the application to allow its re-utilization on different platforms with as less pain as possible.
The OS used is FreeRTOS.

<!-- ![Architecture](Architecture.png) -->
<!-- *Cazzo figa culo deretano* -->

<div align="left">
  <img src="./Architecture.png" alt="Image Alt Text" style="width: 70%;"/>
</div>
<div align="center">
  <p><em>The proposed architecture.
<br>
  The OS layer span a bit over the application layer because the latter makes direct calls to FreeRTOS API.
  This means that you cannot replace such layers independently.</em></p>
</div>


## Application Layer
The application is made by interconnected components.
The aim is to make such a layer platform-independent and scalable.
That is, you should be able to connect the components as lego bricks and
to use them no matter is you are using a STM32 or a Arduino board.
<!-- The same applies for the *utils* library. -->

Components communicates one each other through a
*publisher/subscriber* model to resemble as much as possible Simulink models
and with the platform layer with the *pinin* and *pinout* modules.

Each component has inputs `u`, outputs `y`, an internal state `x`, a state
transition function `f` and
an output function `h`, hence each component can be seen in classic Control Theory state
space form (or the Moore/Melay machines if you like it more), as it follows:

```
x[k+1] = f(x[k], u[k]),
y[k] = h(x[k], u[k]).
```

By traducing this boring math in Software Engineering language, and by
considering C as language reference, a component is nothing more than a .h/.c pair that contains
the following:
1. Bunch of  global *static variables* representing the component state `x`,
2. *Init* function: to initialize the internal state, that is, our `x0`,
3. *Step* function: a function that update the internal state and produces the outputs, which is our `f`,
4. An optional output function that maps the state `x` to the output `y`.
5. *publish_/subscribe_* methods to publish/subscribe the outputs and inputs `y` and `u`.

That is, a component is *encapsulated* in a file and communicate with the
external world with its *publish* and *subscribe* functions.

<!-- ![Component](component.png) -->

<div align="center">
  <img src="./component.png" alt="Image Alt Text" style="width: 60%;" />
  <p><em>A component.
  <br> It has an internal state x, input u and output y, it is
  initialized with initial condition x0 and it can be scheduled periodically
  or in an event-based fashion. The step function (aka state-transition
  function) and the output function are f and h, respectively.
  A component is encapsulated in a .h,.c pair. </em></p>
</div>

> Example:
> Say that our application wants component A performs some sensor readings and
> wants to send the values over the serial port.
> In a classic scenario, component A should *#include* some header file that allows writing over the serial port.
> Instead here, Component A limits to publish the message it wants to send and nothing more.
> It is then from the serial port component (let's call it component B) that shall subscribe
> to the outputs of component A and then perform internally all the operations to send data over the serial port.

A corollary of the above is that the header files of each component must only contain the
declaration of the init
function, the step function, and the publish_/subscribe_ functions. Stop!


Finally, each component shall have an associated prefix to help the navigation
in the code-base.
In-fact, if we know the prefix of a subscribed signal, then we also know its
publisher. Handy!

### Components execution:
Components' input, state and output `u` `x` and `y` can be updated periodically or in an event-based fashion.
Periodic execution is performed through periodic tasks, whereas event-based
execution is achieved by interrupts.
More precisely, interrupts service routines (ISR) wake up dedicated, sleeping tasks
that will carry out the actual work needed.

To allow a bit of flexibility, the step functions take an argument to keep track of the component caller.
This because when running in periodic mode we may want the component to behave in a certain way,
but we may want it to behave differently in response to a sudden event, e.g. when a button connected to some GPIO is pressed.

However, given that the component can be called periodically and in response to an event,
both the subscribe and publish functions are always protected by a mutex to avoid race
conditions

Once we have connected your selected components through their publish and subscribe functions, it is time to run them.
That is the topic of the next section.

Q: Among all the possible way of connecting blocks, is there any specific
guidelines? YES! See below.


### Application setup

We have connected our components. The data flow is clear. Next, we have to
schedule our components.
In the *application_setup.c* file you do the following:

1. Initalize the platform and the components,
2. Define the periodic tasks (i.e. you set sampling period, allocated stack
   memory, etc),
3. For each periodic task you list the components that shall be executed.


## Operating System Layer
The chosen operating system is FreeRTOS.
We use the one shipped with CubeMX and ArduinoFreeRTOS.
It takes a bit of application because components actually use freertos API.

## Platform Layer

### pinin and pinout

To keep a minimum degree of flexibility among different hardware, the application software won't
directly call HAL functions of a specific platform, but instead it uses this
abstraction. For example, *pinin(&led\_state)* calls
e.g. *HAL_GPIO_ReadPin(...)* in STM32 and *digitalRead(...)* in Arduino.
However, the application shall not know about the underlying platform. A
component just calls *pinin(&led\_state)*.
The same for the pinout.

A more elegant solution would be to use function pointers depending on the
platform used. That would be nice to investigate as future work.
At the moment we use just different files.

Note that although the pinin and pinout may be modeled as components themselves, at the
moment we don't see any major benefit is doing so as they model the
physical boundary of the MCU.

However, if more than one component may wish to read/write from/to some data
connected to the same pin, then race conditions may occur.
That shall be handled at the platform level by using e.g. HAL blocking
functions or by blocking the task who scheduled the
ADC reading until some flag is set to high.
An example is the ADC in the considered STM32 framework.
In this case either you use a blocking function (polling from ADC) or you
block the task who scheduled the ADC reading until a EOC flag is released.
Note that ADC reading cannot be preempted.

<!-- ![Pinin and pinout](pinin_pinout.png) -->

<div align="center">
  <img src="./pinin_pinout.png" alt="Image Alt Text" style="width: 80%;"/>
  <p><em>Communication with the platform layer.
<br>
  The components make calls to
  generic functions to read/write data from/to the hardware layer.
  In this picture such generic functions are mapped to the STM32 HAL.
  In this way, any change in the underlying hardware should not affect the application.
  </em></p>
</div>

### Interrupts
Interrupts are used to perform some action in response to some event. Events can be
somehow "predictable" or "unpredictable".
An example of "predictable" event
is the end-of-conversion (EOC) of an ADC when ADC readings are requested by a
periodically scheduled task. In nominal conditions, we know more or less when
the EOC is going to happen, which is about every T seconds, being T the period of
the task. Another example is the event corresponding to a Timer that fires.
An example of unpredictable event is the pressure of a button connected to a
GPIO pin. We have absolutely no idea when such a button is going to be pressed.

In both cases, interrupt service routines (ISR) don't preempt the OS by executing some
arbitrary code, but they are gentle in the sense that they
wake up some task that will in turn execute some code code.
We say that interrupts *defer* functionality to tasks.
In this way, we only have to deal with tasks concurrency and not with a mixture of
interrupts/tasks concurrency that can be very nasty!

For this reason, ISR:s always have pretty much always the same structure regardless
of the nature of the event as well as the associated callbacks functions that only
perform the following operations:

1. Release a semaphore to unlock a task,
2. Ask for a context switch if the unlocked task has higher priority than the current task.

However, the difference relies in the following:

1. Predictable events assume that a periodic task is already running and it gets
   blocked while waiting for an event that will happen soon (an ADC reading
   to be
   completed, a Timer that fires, etc.). In this case the callback just release a
   semaphore to unlock the periodic task waiting for that event to happen and
   nothing more.
2. Unpredictable events. In this case the callback wakes up a specific task
   that performs the following actions:
    1. Publish few signals,
    2. Call all the components subscribed to such signals.

ISR and Callbacks for STM32 are defined in Core/Src/stm32fxx_it.c (so you must modify
that file), whereas for Arduino I don't know... yet.
To keep things separated, the functions implementing the deferred tasks for
unpredictable events are defined in the interrupts\_to\_task.c file.

Differently from pinin and pinout functions that we discussed above, interrupt
are tightly coupled to hardware so it appears very difficult if not impossible
to have a flexible solution for them. Hence, when switching platform, more
work is required to accommodate interrupts.


## Hardware Layer


1. components header files shall only have:
2. A component can only belong to one task
3. Order the components calls
4. Interrupts must always be deferred to tasks.

ISR are always the same
subscribe methods return copies of signals' realizations.

At the bottom level we have the hardware. That can be whatever.
