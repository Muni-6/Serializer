# Synchronization with Serializers

## Overview
This project implements a **Serializer Library** to **synchronize multithreaded access** using **queues and crowds**. It ensures orderly execution of threads by controlling their entry and exit via mutex locks and condition variables.

## Features
- **Thread synchronization using serializers**
- **Mutex locks and condition variables for safe execution**
- **Queue-based and crowd-based thread management**
- **Efficient condition variable handling for wake-up signals**
- **Implementation of real-world synchronization problems**

## Design and Functionality
The serializer manages:
1. **Queues (Qs)** - Control thread execution order.
2. **Crowds (Cs)** - Allow parallel execution when conditions permit.
3. **Mutex Lock & Condition Variables** - Ensure safe access to shared resources.

### **Thread Execution Workflow**
#### **Gaining Possession**
- **Enter Serializer:** Mutex ensures exclusive entry.
- **Dequeue:** Threads wait on condition variables before execution.
- **Leave Crowd:** Decrement crowd count before exiting.

#### **Releasing Possession**
- **After Enqueue:** Signal is sent, releasing mutex.
- **Join Crowd:** Crowd count increases, allowing parallel execution.
- **Exit Serializer:** Final release of serializer resources.

## Why Condition Variable (CV) Per Queue Node?
Initially, a **single CV** for the serializer caused inefficiency due to unnecessary wake-ups. Using a **per-node CV** ensures:
- **Direct signaling to the correct thread**
- **No thread starvation or unnecessary wake-ups**
- **Efficient thread scheduling**

## Problems Implemented
### **Monkey Crossing Problem**
- Manages **eastbound and westbound threads** crossing a rope without collisions.
- Conditions ensure safe crossings with **fairness and starvation prevention**.

### **Child Care Problem**
- Manages **caregiver and child arrivals/departures**.
- Ensures at least one caregiver is present before children arrive.
- Synchronization prevents unsafe exits.

## Contributors
- **Muni Bhavana Konidala (mzk6126)**
- **Naga Sri Hita Veleti (nkv5154)**
