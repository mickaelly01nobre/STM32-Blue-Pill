# STM32 Blue Pill Bare-Metal Drivers (C)

## 📌 Visão Geral
Este repositório fornece uma **implementação completa em bare-metal em C** para o microcontrolador **STM32 Blue Pill (STM32F103C8T6)**.

O projeto foca no desenvolvimento de baixo nível sem o uso de frameworks de alto nível, como HAL, oferecendo controle direto sobre os registradores de hardware.

Ele inclui implementações de diversos periféricos, desde **GPIO até I2C**, bem como **ADC e tratamento de interrupções**, servindo como uma referência abrangente para o desenvolvimento de sistemas embarcados.

---

## 🎯 Objetivos
- Desenvolver sistemas embarcados utilizando **programação bare-metal**  
- Compreender o **controle de hardware em nível de registradores**  
- Implementar periféricos do STM32 do zero  
- Servir como uma **plataforma de aprendizado e referência** para desenvolvimento de firmware  

---

## ⚙️ Funcionalidades
- 🧠 Implementação em **C bare-metal puro**
- 🔧 Manipulação direta dos registradores de hardware
- ⚡ Comportamento em tempo real utilizando interrupções
- 📚 Estrutura de código modular e didática

---

## 🧠 Periféricos Abordados

### 🔹 GPIO (Entrada/Saída de Propósito Geral)
- Configuração de pinos (modos de entrada/saída)
- Leitura e escrita digital
- Registradores de velocidade e configuração (CRL, CRH)

---

### 🔹 RCC (Reset e Controle de Clock)
- Habilitação do clock dos periféricos
- Configuração básica do clock do sistema

---

### 🔹 I2C (Inter-Integrated Circuit)
- Comunicação no modo mestre
- Transmissão e recepção de dados
- Inicialização do periférico

---

### 🔹 ADC (Conversor Analógico-Digital)
- Aquisição de sinais analógicos
- Configuração de canais
- Modos de conversão contínua e única
- Leitura de dados de sensores

---

### 🔹 Sistema de Interrupções (NVIC & EXTI)
- Interrupções externas (EXTI)
- Configuração do NVIC
- Rotinas de tratamento de interrupção (ISR)
- Programação orientada a eventos

---

### 🔹 Timers (se incluídos)
- Geração de delays
- Operações baseadas em tempo

---

## 🖥️ Como Funciona
Cada periférico é implementado do zero através de:

1. Definição dos registradores mapeados em memória  
2. Criação de macros e estruturas  
3. Implementação de funções de inicialização  
4. Desenvolvimento de funções de controle e leitura/escrita  
5. Testes com hardware real  

As interrupções são utilizadas para permitir **comportamento assíncrono e em tempo real**, melhorando a resposta do sistema.

---

## ⚡ Tratamento de Interrupções
O projeto demonstra:
- Interrupções externas via EXTI  
- Configuração do NVIC  
- ISR (Rotinas de Serviço de Interrupção)  
- Projeto orientado a eventos  

---

## 🛠️ Tecnologias & Habilidades
- Sistemas Embarcados  
- Programação Bare-Metal  
- Linguagem C  
- STM32 (ARM Cortex-M3)  
- Programação em Nível de Registradores  
- Sistemas de Interrupção  
- Processamento de Sinais Analógicos (ADC)  

---



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
