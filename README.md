---

# **MAHAM OS Simulation**

A miniature, **Unix-style operating system simulation** built in C. This project demonstrates foundational OS concepts—process creation, round-robin scheduling, user/kernel mode switching, resource management, system logging, and graceful shutdown—by launching actual helper programs in separate `xterm` windows.

---

## ✨ Key Features

| Feature                   | Description                                                                                                                        |
| ------------------------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| **Round-Robin Scheduler** | A dedicated scheduler thread manages a single ready queue with preemptive time-slicing and task re-queuing.                        |
| **User ⇄ Kernel Mode**    | Toggle via menu (option 8). Kernel mode grants privileged access to queues, resources, and task control; user mode launches tasks. |
| **Resource Management**   | Tracks and allocates RAM, HDD, and CPU cores. Prevents overallocation and reclaims resources on task exit.                         |
| **Process Control**       | Each task runs as a real child process in a separate `xterm` window. States include READY, RUNNING, and TERMINATED.                |
| **Graceful Shutdown**     | Sends `SIGTERM` to all live children, releases resources, and logs final system state.                                             |
| **Colorful CLI**          | ANSI escape codes for an enhanced, retro-style user interface.                                                                     |
| **System Logging**        | `system_log.c` records launches, completions, errors, and shutdowns in `system.log`.                                               |

---

## 🗂️ Repository Structure

```
/                     – Project root
├─ main.c             – Main shell, mode switcher, task launcher
├─ scheduler.c/h      – Round-robin queue, scheduler thread
├─ resource_manager.c/h – Manages RAM, HDD, CPU resources
├─ system_log.c/h     – Timestamped logger for events
│
├─ tasks/             – Mini applications compiled as ELF binaries
│   ├─ calculator.c   – CLI calculator
│   ├─ notepad.c      – Basic text editor (stdout)
│   ├─ clock.c        – Live clock (hh:mm:ss)
│   ├─ file_manager.c – File operation demo
│   ├─ music.c        – Background beep melody
│   ├─ minigame.c     – Terminal-based snake game
│   └─ calendar.c     – Monthly calendar viewer
│
└─ Makefile           – Compiles core modules and all tasks
```

---

## 🔧 Building & Running

```bash
# Compile core system and tasks (requires gcc, pthreads, xterm)
$ make

# Launch the OS simulation
$ ./mahamOS
```

> 💡 *Makefile includes `-pthread`, `-Wall`, and `-g` flags for debugging.*

---

## 👤 User Mode Menu

Available actions in **User Mode**:

| Key | Action                |
| --- | --------------------- |
| 1   | Launch Calculator     |
| 2   | Open Notepad          |
| 3   | Start Clock           |
| 4   | File Manager          |
| 5   | Music Simulator       |
| 6   | Play Mini-Game        |
| 7   | View Calendar         |
| 8   | Switch to Kernel Mode |
| 0   | Graceful Shutdown     |

Each task forks an `xterm -e ./tasks/<task>` child process and requests resources.

---

## 🔐 Kernel Mode Menu

Activated via **option 8** in the user menu:

| Key | Privileged Action                                 |
| --- | ------------------------------------------------- |
| 1   | View ready queue and task states                  |
| 2   | Show remaining system resources                   |
| 3   | Kill task by PID (frees resources, updates queue) |
| 0   | Return to User Mode                               |

---

## 📜 System Log

All critical events are timestamped in `system.log`:

```
[2025-05-20 14:32:01] 🚀 Task 0: launched in new terminal (PID: 1234)
[2025-05-20 14:32:04] ✅ Task 0 with PID 1234 completed.
[2025-05-20 14:35:00] ⚠️  Task 2 failed to allocate RAM.
[2025-05-20 14:40:10] ⏻ System shutdown initiated.
```

---

## 🚀 Extending the OS

* Add a new task: drop a compiled ELF binary into `tasks/` and add a menu entry in `main.c`.
* Try alternative scheduling algorithms by modifying `scheduler.c`.
* Improve task control using `SIGSTOP`/`SIGCONT` instead of `sleep()` for preemption.

---

## 🧑‍💻 Author

**Maham Noor**

> *"Build your own OS—one fork at a time 👾"*

---
