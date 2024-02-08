*Disclaimer: this is a work in progress. Everything may change at any time. Also, consider that I am not a software engineering in a strict sense, but I am a control system engineer.*

# Motivation
Concurrency program is tough. Bugs are just behind the corner and having a bugs free software is practically impossible.
However, one may try to prevent the occurrence of such bugs by adhering to some coding rules and/or by following some standards.

Here, we aim at defining a software architecture that can help in preventing bugs when concurrency programming is employed.

# Architecture
The architecture is depicted below.
The idea is to abstract the application to allow its re-utilization on different platforms with as less pain as possible.
The OS used is FreeRTOS.

![Architecture](Architecture.svg)

The description of each layer is top-down.
## Application
The application is made by interconnected components through a
*publisher/subscriber* model to resemble as much as possible a Simulink model.
Each component has inputs, outputs and an internal state, hence it resemble the classic Control Theory state space form, as it follows:
```
x[k+1] = f(x[k], u[k])
y[k] = g(x[k], u[k])
```
By traducing this boring math in Software Engineering language, then we see
that the anatomy of a component is the following:
1. Bunch of *static variables* representing the component state `x`,
2. *Init* function: to initialize the internal state, that is, our `x0`,
3. *Step* function: a function that update the internal state and produces the outputs, which is our `f`,
4. *publish_/subscribe_* methods to publish/subscribe the outputs and inputs `y` and `u`.
More precisely, the inputs `u` are **subscribed** signals whereas the outputs `y` are **published** signals.

As you may have understood, a component is *enclosed in a file and the only
functions that will call and that are defined in another component file are
*only** the subscribe functions.
That is all.

> Example:
> Say that our application wants component A to send some characters over the
> serial port, which is modeled in another component.
> In a classic scenario, component A should *#include* some header file that allows writing over the serial port.
> Instead here, Component A limits to publish the message it wants to send and nothing more.
> It is then from the serial port component that we shall subscribe to the outputs of component A and then perform internally all the operations to send data over the serial port.

A corollary of the above is that the header files of each component must only contain the
declaration of the init
function, the step function, and the publish_/subscribe_ functions. Stop!


Finally, each component shall have an associated prefix to help the navigation in the codebase.
In-fact, if we know the prefix of a subscribed signal, then we also know its
publisher. Handy!

### Components execution:
Components' input, state and output `u` `x` and `y` can be updated periodically or in an event-based fashion.
Periodic execution is performed through periodic tasks, whereas event-based
execution is achieved by interrupts that unlock some aperiodic task.

To allow a bit of flexibility, the step functions take an argument to keep track of the component caller.
This because when running in periodic more we may want the component to behave in a certain way, but when called from
an interrupt "A" we may wish a slightly different behavior.

Once you have connected all the components, it is time to schedule them. That
is the topic of the next section.

Q: Among all the possible way of connecting blocks, is there any specific
guidelines? YES! See below.


### Application setup
Once you have connected your components, then you want to schedule them.
That happens here.

Here you define your periodically executed tasks.
Each tasks has a list of component that will be executed with the same period.
But before starting to execute the components, we must first initialize them.
This also happens here.

## Operating system
The chosen operating system is FreeRTOS.
We use the one shipped with CubeMX and ArduinoFreeRTOS.
It takes a bit of application because components actually use freertos API.

## Platform

### Interrupts
Interrupts are used to perform event-based execution of the components.
However, they don't just call some arbitrary code, but instead they always
wake up some task that will execute some code code.
In this way, we only have to deal with tasks concurrency and not with a mixture of
interrupts/tasks concurrency that can be very nasty!

Hence, ISR always have the same structure:

1. Release a semaphore to wake up a task
2. Ask for a context switch in case the woken up task has higher priority than the current task.

The ISR for STM32 are defined in Core/Src/stm32fxx_it.c (so you must modify
that file), whereas for Arduino I don't know... yet.
To keep things separated, the deferred tasks are defined in the
interrupts\_to\_task.c file.

## Hardware


1. components header files shall only have:
2. A component can only belong to one task
3. Order the components calls
4. Interrupts must always be deferred to tasks.

ISR are always the same
subscribe methods return copies of signals' realizations.

At the bottom level we have the hardware. That can be whatever.
