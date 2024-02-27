*Disclaimer: this is a work in progress. Everything may change at any time.
Also, consider that I am not a software engineering in a strict sense, but I
am a control system engineer.*

# Requirements

1. The toolchain of the platform that you are using,
2.  pyserial.

*Optional*:

2. compiledb - for creating compile\_commands.json files.


# Motivation

Concurrency program is tough. Bugs are just behind the corner and
having a bugs free software is practically impossible.  However, one may try
to prevent the occurrence of such bugs by adhering to some coding rules and/or
by following some standards.

Here, we aim at defining a software architecture that can help in preventing
bugs when concurrency programming is employed.

# Architecture

The architecture is depicted below.  The idea is to abstract
the application to allow its re-utilization on different platforms with as
less pain as possible.  The OS used is FreeRTOS.

![Architecture](Architecture.svg)

The description of each layer is top-down.

## Application

The application is made by interconnected components that shall
be platform independent. That is, you can use the same components no matter is
you are using a STM32 or a Arduino board.  The same applies for the *utils*
library.

Components communicates through a *publisher/subscriber* model to resemble as
much as possible Simulink models.  Each component has inputs `u`, outputs `y`,
an internal state `x`, a state transition function `f` and an output function
`h`, hence it resemble the classic Control Theory state space form, as it
follows:

```
x[k+1] = f(x[k], u[k])
y[k] = h(x[k], u[k])
```

By traducing this
boring math in Software Engineering language, and by considering C as language
reference, a component is a .c file that contains the following: 1. Bunch of
global *static variables* representing the component state `x`, 2. *Init*
function: to initialize the internal state, that is, our `x0`, 3. *Step*
function: a function that update the internal state and produces the outputs,
which is our `f`, 4. An optional output function that maps the state `x` to
the output `y`.  5. *publish_/subscribe_* methods to publish/subscribe the
outputs and inputs `y` and `u`.

That is, a component is *encapsulated* in a file and communicate with the
external world with its publish and subscribe functions.

> Example: Say that our application wants component A to send some characters
> over the serial port, which is modeled in another component.  In a classic
> scenario, component A should *#include* some header file that allows writing
> over the serial port.  Instead here, Component A limits to publish the
> message it wants to send and nothing more.  It is then from the serial port
> component that we shall subscribe to the outputs of component A and then
> perform internally all the operations to send data over the serial port.

A corollary of the above is that the header files of each component must only
contain the declaration of the init function, the step function, and the
publish_/subscribe_ functions. Stop!


Finally, each component shall have an associated prefix to help the navigation
in the code-base.  In-fact, if we know the prefix of a subscribed signal, then
we also know its publisher. Handy!

### Components execution

Components' input, state and output `u` `x` and `y`
can be updated periodically or in an event-based fashion.  Periodic execution
is performed through periodic tasks, whereas event-based execution is achieved
by interrupts that unlock some aperiodic task.

To allow a bit of flexibility, the step functions take an argument to keep
track of the component caller.  This because when running in periodic mode we
may want the component to behave in a certain way, but when called from an
interrupt "A" we may wish a slightly different behavior.

Once you have connected all the components, it is time to schedule them. That
is the topic of the next section.

Q: Among all the possible way of connecting blocks, is there any specific
guidelines? YES! See below.


### Application setup

Once you have connected your components, then you want
to schedule them.  That happens here.

Here you define your periodically executed tasks.  Each tasks has a list of
component that will be executed with the same period.  But before starting to
execute the components, we must first initialize them.  This also happens
here.

## Operating system

The chosen operating system is FreeRTOS.  We use the one
shipped with CubeMX and ArduinoFreeRTOS.  It takes a bit of application
because components actually use freertos API.

## Platform

In the `platform` folder there are stored all the components that make calls to the HAL layer.
If you are changing platform you have to only have to adjust the files here,
leaving the application untouched.

As in the application layer, the platform components have states, inputs,
outputs, an init function, a step function, etc.

### Interrupts

Interrupts are used to handle unpredictable events.  However,
they don't just execute arbitrary code by preempting OS operations and by
executing some arbitrary code, but
instead they unlock some task by releasing a semaphore and then they
ask for a context switch between the current and the unlocked task. This means
that interrupts are always deferred to tasks. In this way, we only have to
deal with tasks concurrency and not with a mixture of interrupts/tasks
concurrency that can be very nasty!


The identified use-cases for interrupts are two:

  1. Unpredictable event. Such an event could be a button connected to a GPIO
  pin that is pressed. In that case, the callback function immediately calls a
  component with a caller argument. The component is immediately executed.
  2. Predictable event. A periodic task is blocked waiting for an event that
  is happening soon. Such an event could be the end-of-conversion (EOC) of
  an ADC. The interrupt callback function just release a
  semaphore, as a task connected to it is already running.


ISR and Callbacks for STM32 are defined in Core/Src/stm32fxx_it.c (so you must
modify that file), whereas for Arduino I don't know... yet.
The functions associated to the deferred task are defined in the interrupts\_to\_task.c file.

## Hardware


1. components header files shall only have: 2. A component can only belong to
one task 3. Order the components calls 4. Interrupts must always be deferred
to tasks.

ISR are always the same subscribe methods return copies of signals'
realizations.

At the bottom level we have the hardware. That can be whatever.
