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
The application is made by interconnected components and it follows a
*publisher/subscriber* model to resemble as much as possible a Simulink model.
Each component has inputs, outputs and an internal state, hence it resemble the classic Control Theory state space form, as it follows:
```
x[k+1] = f(x[k], u[k])
y[k] = g(x[k], u[k])
```
Components' state and outputs `x` and `y` can be updated periodically or in an event-based fashion.

The inputs `u` are **subscribed** signals whereas the outputs `y` are **published** signals.
The state `x` is represented by **static** variables within the file scope.

Hence, a component is *enclosed in a file and it won't call any function of any other component.
Instead, it communicates with other components through a publish/subscribe
mechanism. That is all.

> Example:
> Say that our application wants component A to send some characters over the
> serial port, which is modeled in another component.
> In a classic scenario, component A should *#include* some header file that allows writing over the serial port.
> Instead here, Component A limits to publish the message it wants to send and nothing more.
> It is then from the serial port component that we shall subscribe to the outputs of component A and then perform internally all the operations to send data over the serial port.

The anatomy of a component is the following:
1. Bunch of *static variables* representing the component state `x`,
2. *Init* function: to initialize the internal state, that is, our `x0`,
3. *Step* function: a function that update the internal state and produces the outputs, which is our `f`,
4. *publish_/subscribe_* methods to publish/subscribe the outputs and inputs `y` and `u`.

This implies that the header files only contain the declaration of init
function, step function, and the publish_/subscribe_ functions. It shall not contain anything more than that!

In this way it is easy to resemble "block diagrams" that are widely used in tools like Simulink.
Finally, it is a good idea if each component has an associated prefix. That would help the navigation in the codebase.

### Components execution:
Components can be called:
1. Periodically,
2. On events.

Periodic execution is performed through periodic tasks, whereas event-based execution is achieved by interrupts.

To allow a bit of flexibility, the step function takes an argument to keep track of the component caller.
This because when running in periodic more we may want the component to behave in a certain way, but when called from
an interrupt "A" we may wish a slightly different behavior.

Once you have connected all the components, it is time to schedule them.

### Application setup
This is where you shape your application, namely you decide how you want to schedule your
components.
Each task has a list of component that will be executed.
But before that, the components must be first initialized, and this also
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
