# ThreadX++
## Introduction

Hi all
This is my little project, it's a big wrapper for ThreadX (AZURE RTOS) to work as C++, here are some important points about the project:

- The main target of the project is one: SIMPLE, it's meaning that I do it much tiny as it can be, the price, of course, is that we lost some of the abilities of the ThreadX
- I create a lot of tutorials and explain everything about how to work with
- I did not cover all the API of ThreadX,
- I build this project on STM32H743, So if you using other processors of the STM32 family I guess it will work, but if you have other MCU some of the abilities of my project will not be available (like a watchdog) but you still can get most of the core function (you will need to change the code).

## Abilities 
The abilities of ThreadX++

- **Events-oriented:** I added the events like a delegate, so you can create an event in one thread and register it from another thread.
- **Object-oriented:** Most of the API of the ThreadX get warper as a class and the list of the API:
    * **Thread:**
    * **Event:** Event that one thread will create and another can register and wakeup one it invokes
    * **Mutex:**
    * **Queue:** Safe queue to send data between thread
    * **Semaphore:**
    * **TimeEvent:** The event will wake up the thread every amount of time and call to callback.
- **MemoryPool:** Easy way to create a pool of memory (also overriding "new" and "delete").
- **STM32 easy use:** I also do some wrapper to very useful peripherals of ST:
    * **Watchdog:** Simple watchdog 
    * **Measure US:** I used the "DWT" to create debug class that can easily measure time in microseconds
    * **Internal registry:** Registry (like a dictionary) that use to save data in the internal flash on the ST.
    * **Printf:**  Use the SWT port for printing much more effectively.

## The tutorials and documentation
Go to https://zehevvv.github.io/ThreadX_plus_plus/

