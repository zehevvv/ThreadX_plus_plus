# ThreadX++
## Introduction

Hi all
This is my little project, it's a big wrapper for ThreadX (AZURE RTOS) to work as c++, here some important pointes about the project:

- The main target of the project is one: SIMPLE, it's meaning that i do it much tiny that it can be, the price of course is that in way we lost abilities of the ThreadX
- I not cover all the API of ThreadX,
- I build this project on STM32H743, So if you using other processor of STM32 family i guess it will work, but if you have other MCU some of the abilities of my project will not available (like a watchdog) but you still can get most of the core function (you will need to change the code).

## Abilities 
The abilities of ThreadX++

- **Events-oriented:** I added the events like delegate, so you can create event in one thread and register it from other thread.
- **Object-oriented:** most of the API of the ThreadX get warper of as class, hte list of the API
    * **Thread:**
    * **Event:** Event that one thread will create and other can register and wakeup one it invoke
    * **Mutex:**
    * **Queue:** Safe queue to send data between thread
    * **Semaphore:**
    * **TimeEvent:** Event that thread creating and the event will wake up the thread every amount time.
- **MemoryPool:** Easy way to create pool of memory (overriding "new" and "delete").
- **STM32 easy use:** I also do some wrapper to very useful peripherals of ST:
    * **Watchdog:** Simple watchdog 
    * **Measure US:** I used the "DWT" for create debug class that can easily measure time in microsecond
    * **Internal registry :** Registry (like dictionary) that use to save data in the internal flash on the ST.
    * **Printf :**  Use the SWT port for printing much more effective.

## The full documentaion
Go to https://zehevvv.github.io/ThreadX_plus_plus/
