**Disclaimer**: *This is a perpetual work in progress. Everything may change
at any time.
Also, consider that I am not a software engineering in a strict sense, but I
am a control system engineer.
Finally, at the moment I am 100% focused on STM32 and therefore the Arduino
framework is
left a bit behind, so don't use it.*

Welcome to DEFRECS!
==================

DEFRECS stands for *DEvelopment Framework for Real-time Embedded Control
Systems* and
it is nothing more than a software architecture and a bunch of coding guides
to
simplify and scale the development of real-time control systems.
The architecture is an application of *component-based software engineering*
where the components communicate with a *publish/subscribe* model.
The chosen language is C.


# Motivation

Concurrency program is tough. Bugs are just behind the corner and in many
cases it is hard to detect them.
This issue is even more emphasized for those who are not strict software
developers,
like for example control systems engineers.
However, one may try to prevent the occurrence of bugs by adhering to some
coding rules and/or by following some standards.

Here, we aim at defining a software architecture and some guidelines that can
help in preventing bugs when concurrency programming is employed.
Furthermore, the proposed architecture allows different people working on
different components without the risk of interfering one each other.
For example, one person may work in designing a PID controller, another person
or team in developing a Kalman filter for estimating some quantities, etc.



# Requirements

1. The toolchain of the platform that you are using,
2. [pyserial](https://github.com/pyserial/pyserial)

*Optional*:

3. [compiledb](https://github.com/nickdiego/compiledb)- for creating
   `compile_commands.json` files in case you use
   some LSP like [clangd](https://clangd.llvm.org/).
4. [State Smith](https://github.com/statesmith/statesmith) for generating
   code for finite state-machines.

And of course a board to flash.
Here we used an STM32F446RE Nucleo board and an Arduino UNO. For the Arduino
platform we use
Platformio but at the moment the Arduino platform is left a bit behind. PR are
welcome of course.

# Repository structure

On the top level you have each folder representing each
platform. For every platform folder (e.g. `stm32f446re_platform`) each
sub-folder
represent an example (e.g. `Hello_World`, `ADC_Readings`, etc.). You also have
some additional files that you could ignore as they are using for the
debugging framework used by the author.

    defrecs
        ├── Platform1 (e.g Arduino)
        │   ├── Example1
        │   │   ├── application
        │   │   ├── platform
        │   │   ├── utils
        │   │   ├── contrib
        │   │   └── ... other platform specific folders
        │   └─── Example2
        │       ├── application
        │       ├── platform
        │       ├── utils
        │       ├── contrib
        │       └── ... other platform specific folders
        └── Platform2 (e.g. STM32446RE)
            ├── Example1
            │   ├── application
            │   ├── platform
            │   ├── utils
            │   ├── contrib
            │   └── ... other platform specific folders
            └── Example2
                ├── application
                ├── platform
                ├── utils
                ├── contrib
                └── ... other platform specific folders

The structure of each example is always the same: you have an `application`
folder that contains a selection of application components, and a `platform`
folder that contains platform components. Among other things, the platform
components are in
charge of wrapping the HAL of a specific vendor.

Then, you have a `utils` folder that contains a number of utilities (like for
example the `ftoa` used to convert floats into ASCII in embedded systems,
etc.).

You have a `contrib` folder that you can ignore as it contains
helpers from third-parties, for example for accessing
FreeRTOS tasks through *openocd* when you are debugging and so on.

Finally, you have a bunch of folders which are platform/framework specific.

## STM32

In the STM32 case, you have a bunch of folders (`Core`, `Drivers` and
`Middleware`) that are automatically
generated by CubeMX. They contain the board firmware and FreeRTOS.

You also have a `Makefile` which is originally generated by CubeMX and then it
has been slightly edited.

## Arduino

TBD.


# Architecture

The proposed architecture is depicted below.
The idea is to abstract the application to allow its re-utilization on
different platforms with as less pain as possible.
The OS used is FreeRTOS.


<div align="left">
  <img src="./Architecture.png" alt="Image Alt Text" style="width: 70%;"/>
</div>
<div align="center">
  <p><em>The proposed architecture.
<br>
  The OS layer span a bit over the application layer because the latter makes
  direct calls to FreeRTOS API.
  This means that you cannot replace such layers independently.</em></p>
</div>


## Application Layer

The application is made by interconnected components stored in the
`application`
folder.
A component could be a PI controller, a Kalman filter, a sensor reader, a
finite-state machine and so on and so forth.
The aim is to make components platform-independent and to allow users
to connect them in an easy manner.
To achieve such a goal, components communicates one each other through a
*publisher/subscriber* model to resemble as much as possible Simulink models.


<div align="center">
  <img src="./application_example.png" alt="Image Alt Text" style="width:
  60%;" />
  <p><em>Application example.
  <br>
  Components are connected through subscribe (inputs) and publish (outputs)
  functions.
   </em></p>
</div>

Each component has inputs `u`, outputs `y`, an internal state `x`, a state
transition function `f` and
an output function `h`.
Hence each component can be seen in classic Control Theory state
space form (or the Moore/Melay machines if you like it more), as it follows:

```
x[k+1] = f(x[k], u[k]),
y[k] = h(x[k], u[k]).
```

By traducing this boring math in Software Engineering language, and by
considering C as language reference, a component is nothing more than a
`.h,.c` pair that contains
the following:
1. *Static variables* representing the component state `x`,
2. *Init* function: to initialize the internal state, that is, our `x0`,
3. *Step* function: a function that update the internal state and produces the
   outputs, which is our `f`,
4. *Output function* that maps the state `x` to the output `y`. This is useful
   when the output is not the same as the state.
5. *publish_/subscribe_* functions to publish/subscribe the outputs and inputs
   `y` and `u`.

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
> In a classic scenario, component A should *#include* some header file that
> allows writing over the serial port.
> Instead here, Component A limits to publish the message it wants to send and
> nothing more.
> It is then from the serial port component (let's call it component B) that
> shall subscribe
> to the outputs of component A and then perform internally all the operations
> to send data over the serial port.

A corollary of the above is that the header files of each component must only
contain the
declaration of the init
function, the step function, and the publish_/subscribe_ functions. Stop!


Finally, each component shall have an associated prefix to help the navigation
in the code-base.
In-fact, if we know the prefix of a subscribed signal, then we also know its
publisher. Handy!

### Components execution:

Components' input, state and output `u` `x` and `y` can be updated
periodically or in an event-based fashion.
Periodic execution is performed through periodic tasks, whereas event-based
execution is achieved by interrupts.
More precisely, interrupts service routines (ISR) wake up dedicated, sleeping
tasks
that will carry out the actual work needed.

To allow a bit of flexibility, the step functions take an argument to keep
track of the component caller.
This because when running in periodic mode we may want the component to behave
in a certain way,
but we may want it to behave differently in response to a sudden event,
e.g. when a button connected to some GPIO is pressed.

However, given that the component can be called periodically and in response
to an event,
both the subscribe and publish functions are always protected by a mutex to
avoid race
conditions.

Once we have connected your selected components through their publish and
subscribe functions, it is time to run them.
That is the topic of the next section.

Q: Among all the possible way of connecting blocks, is there any specific
guidelines? YES! See below.


### application\_setup.c

We have connected our components. The data flow is clear. Next, we have to
schedule our components.
In the `application_setup.c` file you do the following:

1. Initalize the platform and the components,
2. Define the periodic tasks (i.e. you set sampling period, allocated stack
   memory, etc),
3. For each periodic task you list the components that shall be executed.


## Operating System Layer

The chosen operating system is FreeRTOS.
We use the one shipped with CubeMX and ArduinoFreeRTOS.
It takes a bit of application because components actually use FreeRTOS API.

## Platform Layer

In the `platform` folder there are stored all the components that make calls
to the HAL layer.
If you are changing platform you have to only have to adjust the files here,
leaving the application untouched.

As in the application layer, the platform components have states, inputs,
outputs, an init function, a step function, etc.

### Interrupts

Interrupts are used to perform some action in response to some event. Events
can be
somehow "predictable" or "unpredictable".
An example of "predictable" event
is the end-of-conversion (EOC) of an ADC when ADC readings are requested by a
periodically scheduled task. In nominal conditions, we know more or less when
the EOC is going to happen, which is about every *T* seconds, being *T* the
period of
the task. Another example of predictable event is the event corresponding to a
Timer that fires.
An example of "unpredictable" event is the pressure of a button connected to a
GPIO pin. We have absolutely no idea when such a button is going to be
pressed.

In both cases, interrupt service routines (ISRs) don't preempt the OS by
executing some
arbitrary code, but they are gentle in the sense that they
wake up some task that will in turn execute some code code.
We say that interrupts *defer* functionality to tasks.
In this way, we only have to deal with tasks concurrency and not with a
mixture of
interrupts/tasks concurrency that can be very nasty!

For this reason, ISR:s always have pretty much always the same structure
regardless
of the nature of the event as well as the associated callbacks functions that
only
perform the following operations:

1. Release a semaphore to unlock a task,
2. Ask for a context switch if the unlocked task has higher priority than the
current task.

However, the difference relies in the following:

1. *Predictable* events assume that a periodic task is already running and it
gets
   blocked while waiting for an event that will happen soon (an ADC reading
   to be
   completed, a Timer that fires, etc.). In this case the callback just
   release a
   semaphore to unlock the periodic task waiting for that event to happen and
   nothing more.
2. *Unpredictable* events. In this case the callback wakes up a specific task
   that calls the components with a specific caller argument.

ISR and Callbacks for STM32 are defined in `Core/Src/stm32f4xx_it.c` (so you
must modify
that file), whereas for Arduino I don't know... yet.
The functions implementing the deferred tasks for
unpredictable events are defined in the `interrupts_to_task.c` file.
When dealing with HAL functions that trigger interrupts, check these three
files: the function that calls the HAL function, the `Core/Src/stm32f4xx_it.c`
and the `interrupts_to_tasks.c`.

## Hardware Layer

At the bottom level we have the hardware that is what you plan to deploy your
code.

# Nice but... how to use it?

Good question. Copy any example and modify it at your will.

Here are some general guidelines:

1. Start your project by adjusting the platform components. Don't forget to
   initialize them and to schedule them from the `application_setup.c` file.
2. You only have two types of tasks: *periodic* tasks defined in
`application_setup.c` and *deferring*
   tasks called by ISR:s which are defined in
   `interrupts_to_tasks.c`. Deferring tasks wake up sporadically, i.e. upon
   Interrupt-Service-Routine (ISR) call, so they are not periodic.
3. ISR shall always be deferred to tasks.
4. A component should be scheduled only in one periodic task. Avoid calling
the same
   component from different periodic task. However, and already scheduled
   components can be called by
   a deferring tasks.
5. Never use HAL functions in the application layer!
6. Components shall communicate only through their publish/subscribe
   interfaces.
7. Pay attention to your components scheduling. Some outputs may be
   sampled faster/slower than some inputs and that could lead to a number of
   problems. Be sure that the scheduling makes sense to your application.

Here are some guidelines on how to write components:

1. Each component must have a prefix, to be easily searched,
2. Published signals must start with the prefix, e.g. `blink_led_state`
3. Mutex for publishing/subscribing must contain the same name of the
   associated output, e.g. `blink_led_state` -> `mutex_blink_led_state`
4. Publish and subscribing functions is the same for all the components and
   must have the form `publish_<output_name>(...)`,
   e.g. `publish_blink_led_state(&led_state)`,
5. The function to be placed in the scheduling must have the form
`<prefix>_step`,
   e.g. `blink_step(PERIODIC_TASK)`,
6. Outputs shall be initialized in the `<prefix>_init()` function,
7. In a component header file you must only include the prototypes for the
   init, the step and the subscribe functions.


Once done, edit the `Makefile` and you are set.
For STM32 you have to modify the *Programmer and compiler paths* and the
*Custom targets* sections.

<div align="center">
  <img src="./comp_scheduling.png" alt="Image Alt Text" style="width: 60%;" />
  <p><em>Scheduling example. Check that the execution periods won't mess up
  your application.
   </em></p>
</div>


# Application Software Architecture

TBD

## TODO:

1. cmake and/or function pointers to deal with cross-platform.
2. HAL and OS Error handling.
3. Doxygen
4. Components vs functions
