# ADC reading example

In this example you can see the power of multitasking shining!
The "system" behaves as it follows:

1. The builtin led blinks every second,
2. Two sensors are connected to the ADC1, but:
    a. The values of the first sensor are sent periodically over the USART2,
    b. The values of the second sensor are sent over the USART2 only when the
    builtin button is pressed.
3. User can type anything on its computer keyboard. Once `Enter` is pressed,
   the message is sent through the USART2.

We used two small photo-voltaic panels connected in
series and a LM335 temperature sensor as sensors.
Feel free to replace these sensors with whatever you want, but mind that
you must have to create two new application components and adjust the
publish/subscribe links, see picture below.

# Hardware

The first sensor is connected to `PC0` whereas the second is connected to
`PC1`. The Nucleo board is connected to the PC.


# Software

The component view is the following:

## Platform
We only use a `usart2`, `adc1` and `digital_out` components. These components
calls HAL function and publish signals that the application components can
subscribe to or they are subscribed to application components.

### Interrupts

We have:

- Predictable event:
    1. "ADC1 end-of-conversion",

- Unpredictable events:
    1. "Builtin button is pushed",
    2. "A character is received over the USART2".

In the first case, the Callback function associated to the ISR
notify a blocked periodic task, whereas in the latter case the work is
deferred to two deferring tasks. Such deferring tasks are defined in
`interrupts_to_task.c`

## Application
The `blink_` does not subscribe anything but it publishes its state. The
`digital_out_` subscribe that signal.

The `pv_` and `tempsems_` components interpret the pin voltages encapsulated in
the published signals from `adc1_` and publish a value. The `serial_` subscribe
these signals and send them to `usart2_`.

## Scheduling
