# Hello World!

This is a very simple example.

# Requirements:

The "system" behaves as it follows:

1. The builtin led blinks every second,
2. The led state is sent over the USART2,
3. Every time the builtin button is pressed, the string `Hello World"` is sent
   over the USART2.

# Implementation

## Hardware

Nothing is really done since in Nucleo boards both the LED and the button are builtin.
However, if you decide to port this example on another platform, you may need
to connect an external button and LED to your board.

## Software

The component view is the following:

<div align="center">
  <img src="./drawing.png" alt="Image Alt Text" style="width: 70%;"/>
</div>
<div align="center">
  <p><em>Hello world example software architecture.
  We can see how the components are connected, how they are
  scheduled and we can also see the unpredictable events.
  </em></p>
</div>

### Platform

We only use the `usart2_` and the `digital_out_` components. These components
calls HAL function and subscribe to signals published by application
components.

We have the following *interrupts*:

- Predictable event:
    1. None

- Unpredictable events:
    1. "Builtin button is pushed",

The deferring task associated to the unpredictable event
is defined in `interrupts_to_task.c`.

Note how `usart2_` is called from both periodically and from an ISR and how
the case is simply handled by a switch condition on the caller.

Finally note that only platform components are allowed to make HAL API calls
and how they are protected with a mutex in case of potential concurrency,
as done in `usart2_`

### Application

The `blink_` does not subscribe anything but it publishes its state. The
`digital_out_` subscribe that signal.

We further have a `debug` component that does not publish nor subscribe any
signal but it is handy for visualizing the system state when debugging
with e.g. `gdb`.


### Scheduling

There are only two periodic tasks: one running at 1000ms and the other at
200ms. However, there are also two deferring tasks associated to the
unpredictable events. Such deferring tasks call the `usart2_` component.
