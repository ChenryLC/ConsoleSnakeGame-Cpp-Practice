# Console Snake Game - with Server-Client Architecture 

> A Slither.io-inspired multiplayer console snake game.

![Status](https://img.shields.io/badge/status-active_development-yellow)
![Language](https://img.shields.io/badge/language-C++-blue)

## Current Status

**⚠️Not runnable yet, still under construction**

| Module | Status |
| :--- | :--- |
| **Server** | 🔨 Working |
| World Engine | ✅ Done |
| Player Join&Leave | ✅ Done |
| Direction Control | ✅ Done |
| Room Management | ❌ Pending |
| Event Callbacks | ❌ Pending |
| Thread Safety | ❌ Pending |
| Server top-level scheduler | ❌ Pending |
| Command-line control | ❌ Pending |
| Networking | ❌ Pending |
| **Client** | ❌ Pending |

## Features

- Chunk-based dynamic world loading (inspired by Minecraft)
- Multi-player world with safe spawn search
- Snake movement, turning, and collision detection (boundary + self + others)
- Food generation and consumption with full-chunk fallback

## Next Steps

>from bottom to top.

- Refactor World logic
- Room system
- Thread locks
- Server top-level scheduler
- Log system
- Networking(on server)

## Structure
```text
├─src/ #source code
│  ├─client/ #client code(empty yet)
│  ├─common/ #shared code
│  └─server/ #server code
│      ├─error/ #error definition
│      ├─event/ #event callbacks
│      ├─game/ #game core code
│      ├─network/ #network layer code(Pending)
│      └─room/ #room management
```
## Run

**⚠️Not runnable yet:** client and network layer are still pending.