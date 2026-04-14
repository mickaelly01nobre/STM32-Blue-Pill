# STM32 Blue Pill Bare-Metal Drivers (C)

## 📌 Overview
This repository provides a **complete bare-metal implementation in C** for the **STM32 Blue Pill (STM32F103C8T6)** microcontroller.

The project focuses on low-level development without using high-level frameworks such as HAL, offering direct control over hardware registers.

It includes implementations of several peripherals, from **GPIO to I2C**, as well as **ADC and interrupt handling**, serving as a comprehensive reference for embedded systems development.

---

## 🎯 Objectives
- Develop embedded systems using **bare-metal programming**  
- Understand **register-level hardware control**  
- Implement STM32 peripherals from scratch  
- Serve as a **learning and reference platform** for firmware development  

---

## ⚙️ Features
- 🧠 Pure **bare-metal C implementation**
- 🔧 Direct manipulation of hardware registers
- ⚡ Real-time behavior using interrupts
- 📚 Modular and educational code structure

---

## 🧠 Peripherals Covered

### 🔹 GPIO (General Purpose Input/Output)
- Pin configuration (input/output modes)
- Digital read/write
- Speed and configuration registers (CRL, CRH)

---

### 🔹 RCC (Reset and Clock Control)
- Peripheral clock enable
- System clock configuration basics

---

### 🔹 I2C (Inter-Integrated Circuit)
- Master mode communication
- Data transmission and reception
- Peripheral initialization

---

### 🔹 ADC (Analog-to-Digital Converter)
- Analog signal acquisition
- Channel configuration
- Continuous and single conversion modes
- Reading sensor data

---

### 🔹 Interrupt System (NVIC & EXTI)
- External interrupts (EXTI)
- NVIC configuration
- Interrupt handling routines (ISR)
- Event-driven programming

---

### 🔹 Timers (if included)
- Delay generation
- Time-based operations

---

---

## 🖥️ How It Works
Each peripheral is implemented from scratch by:

1. Defining memory-mapped registers  
2. Creating macros and structures  
3. Implementing initialization functions  
4. Developing control and read/write functions  
5. Testing with real hardware  

Interrupts are used to enable **asynchronous and real-time behavior**, improving system responsiveness.

---

## ⚡ Interrupt Handling
The project demonstrates:
- External interrupts via EXTI  
- NVIC configuration  
- Interrupt Service Routines (ISR)  
- Event-driven design  

---

## 🛠️ Technologies & Skills
- Embedded Systems  
- Bare-Metal Programming  
- C Language  
- STM32 (ARM Cortex-M3)  
- Register-Level Programming  
- Interrupt Systems  
- Analog Signal Processing (ADC)  

---
