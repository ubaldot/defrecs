*Disclaimer: this is a work in progress. Everything may change at any time.*

# Motivation
Concurrency program is tough. Bugs are just behind the corner and having a bugs free software is practically impossible. 
However, one may try to prevent the occurrence of such bugs by adhering to some coding rules and/or by following some standards. 

Here, we aim at defining a software architecture that can help in preventing bugs when concurrency programming is employed. 

# Architecture
The architecture is depcited below. 
The idea is to abstract the application to allow its re-utilization on different platforms with as less pain as possible. 
The OS used is FreeRTOS. 

![Architecture](Architecture.svg)

The description of each layer is top-down.
## Components
The application follows a *publisher/subscriber* model to resemble as much as possible the classic Simulink model of connecting blocks.
Each component has inputs, outputs and an internal state, hence it resemble the classic Control Theory state space form, as it follows:
```
x[k+1] = f(x[k], u[k])
y[k] = g(x[k], u[k])
```
The inputs are the *subscribed* signals whereas the outputs are the *published* signals. 
A component is enclosed in a file and it won't call any function of any other component. 
It limits to read the subscribed signals, make some internal computations, and publish the output signals. 
That is all. 

> Example:
> Say that our application wants component A to send some characters over the serial port, which can easily be another component.
> In a classic scenario, component A should *#include* some header file that allows writing over the serial port.
> Instead here, Component A limits to publish the message it wants to send and nothing more.
> It is then from the serial port component that we shall subscribe to the outputs of component A and then perform internally all the operations to send data over the serial port.

The anatomy of a component is the following:
1. Bunch of *static variables* representing the component state `x`,
2. *Init* function: to initialize the internal state, that is, our `x0`,
3. *Step* function: a function that update the internal state and produces the outputs, which is our `f`,
4. *publish_/subscribe_* methods to publish/subscribe the outputs and inputs `y` and `u`.

This implies that the header files only contain the declaration of init function, main function, and the publish_/subscribe_ functions. And shall not contain anything more than that!

In this way it is easy to resemble "block diagrams" that are widely used in tools like Simulink.

### Components instantiation:
Components can be called:
1. Periodically,
2. On events.

Periodic execution is performed through periodic tasks, whereas event-based execution is achieved by interrupts. 

To allow a bit of flexibility, the step function takes an argument to keep track of the component caller. 
This because when running in periodic more we may want the component to behave in a certain way, but when called from 
an interrupt "A" we may wish a slightly different behavior. 

## Operating system
The chosen operating system is FreeRTOS. 
Once 




At the bottom level we have the hardware. That can be whatever.
## 
## Operating System
