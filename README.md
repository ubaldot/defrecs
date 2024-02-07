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
That means that in Software Engineering language the anatomy of a component is the following:
1. Bunch of *static variables* representing the component state `x`,
2. *Init* function: to initialize the internal state, that is, our `x0`,
3. *Step* function: a function that update the internal state and produces the outputs, which is our `f`,
4. *publish_/subscribe_* methods to publish/subscribe the outputs and inputs `y` and `u`.
More precisely, the inputs `u` are **subscribed** signals whereas the outputs `y` are **published** signals.

Components' state and outputs `x` and `y` can be updated periodically or in an event-based fashion.

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


This implies that the header files of each component only contain the
declaration of the init
function, the step function, and the publish_/subscribe_ functions. Stop!

In this way it is easy to resemble "block diagrams" that are widely used in tools like Simulink.

Finally, each component shall have an associated prefix to help the navigation in the codebase.
In-fact, if we know the prefix of a subscribed signal, then we also know the
publisher. Handy!

### Components execution:
Components can be called:
1. Periodically,
2. On events.

Periodic execution is performed through periodic tasks, whereas event-based execution is achieved by interrupts.

To allow a bit of flexibility, the step functions take an argument to keep track of the component caller.
This because when running in periodic more we may want the component to behave in a certain way, but when called from
an interrupt "A" we may wish a slightly different behavior.

Once you have connected all the components, it is time to schedule them. That
is the topic of the next section.

 That is the topic of the next section.

Q: Among all the possible way of connecting blocks, is there any specific
guidelines? YES! See below.


### Application setup
Once you have connected your components, then you want to schedule them.
That happens here since it is here where the tasks are defined.

Each task has a list of component that will be executed and an execution
period.
But before starting to execute the components, they must be first initialized, and this also
happens here.

## Operating system
The chosen operating system is FreeRTOS.
Once

## Platform

### Interrupts
They are always deferred and they publish their own signals.
The ISR are defined in Core/Src/stm32fxx_it.c and, as you can see from the example, they have all the same structure:
1. Release a semaphore to wake up a task
2. Ask for a context switch in case the woken up task has higher priority than the current task.

## Hardware

## Some rules
1. components header files shall only have:
2. A component can only belong to one task
3. Order the components calls
4. Interrupts must always be deferred to tasks.

ISR are always the same
subscribe methods return copies of signals' realizations.

At the bottom level we have the hardware. That can be whatever.
