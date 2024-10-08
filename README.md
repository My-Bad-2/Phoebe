<div align="center">
  <img src="docs/assets/logo.png" alt="logo" width="200" height="auto" />
  <h1>Phoebe</h1>
  
  <p>
    A Hobby Kernel. 
  </p>
  
  
<!-- Badges -->
<p>
  <a href="https://github.com/My-Bad-2/Phoebe/graphs/contributors">
    <img src="https://img.shields.io/github/contributors/My-Bad-2/Phoebe" alt="contributors" />
  </a>
  <a href="">
    <img src="https://img.shields.io/github/last-commit/My-Bad-2/Phoebe" alt="last update" />
  </a>
  <a href="https://github.com/My-Bad-2/Phoebe/network/members">
    <img src="https://img.shields.io/github/forks/My-Bad-2/Phoebe" alt="forks" />
  </a>
  <a href="https://github.com/My-Bad-2/Phoebe/stargazers">
    <img src="https://img.shields.io/github/stars/My-Bad-2/Phoebe" alt="stars" />
  </a>
  <a href="https://github.com/My-Bad-2/Phoebe/issues/">
    <img src="https://img.shields.io/github/issues/My-Bad-2/Phoebe" alt="open issues" />
  </a>
  <a href="https://github.com/My-Bad-2/Phoebe/blob/master/LICENSE">
    <img src="https://img.shields.io/github/license/My-Bad-2/Phoebe.svg" alt="license" />
  </a>
</p>
   
<h4>
    <a href="docs/">Documentation</a>
  <span> · </span>
    <a href="https://github.com/My-Bad-2/Phoebe/issues/">Report Bug</a>
  <span> · </span>
    <a href="https://github.com/My-Bad-2/Phoebe/issues/">Request Feature</a>
  </h4>
</div>

<br />

<!-- Table of Contents -->
# Table of Contents

- [Table of Contents](#table-of-contents)
  - [About the Project](#about-the-project)
    - [Features](#features)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
      - [Fedora](#fedora)
      - [Debian](#debian)
      - [Ubuntu](#ubuntu)
    - [Building](#building)
  - [Roadmap](#roadmap)
    - [Code of Conduct](#code-of-conduct)
  - [License](#license)
  - [Contact](#contact)
  - [Acknowledgements](#acknowledgements)
  

<!-- About the Project -->
## About the Project

<!-- Features -->
### Features

- Universal Asynchronous Receiver-Transmitter (UART)
- Memory Allocation

<!-- Getting Started -->
## Getting Started

<!-- Prerequisites -->
### Prerequisites

This project uses Clang for building, Meson build for project configuration, QEMU for system emulation, and xorriso for packaging.

#### Fedora
```bash
dnf install qemu xorriso meson clang clang-analyzer clang-libs clang-tools-extra llvm llvm-libs
```

#### Debian
```bash
    deb http://apt.llvm.org/bookworm/ llvm-toolchain-bookworm-19 main
    deb-src http://apt.llvm.org/bookworm/ llvm-toolchain-bookworm-19 main

    apt-get install clang-format clang-tidy clang-tools clang clangd libc++1 libc++abi1 libclang-dev libclang1 liblldb-dev libomp5 lld lldb llvm-dev llvm-runtime llvm python3-clang qemu-kvm qemu-system qemu-utils xorriso meson
```

#### Ubuntu
```bash
    deb http://apt.llvm.org/noble/ llvm-toolchain-noble-19 main
    deb-src http://apt.llvm.org/noble/ llvm-toolchain-noble-19 main

    apt-get install clang-format clang-tidy clang-tools clang clangd libc++1 libc++abi1 libclang-dev libclang1 liblldb-dev libomp5 lld lldb llvm-dev llvm-runtime llvm python3-clang qemu-system qemu-utils qemu-kvm xorriso meson
```

<!-- Building -->
### Building

Configure the [scripts/clang.amd64.cross-file](scripts/clang.amd64.cross-file) file to your system requirements.

```bash
    meson setup build --cross-file scripts/clang.amd64.cross-file
    cd build
    ninja run_uefi
```

<!-- Roadmap -->
## Roadmap

* [x] Universal Asynchronous Receiver-Transmitter (UART)
* [x] Logging
* [x] Global Descriptor Table
* [x] Interrupt Descriptor Table
* [x] Physical Memory Manager
* [x] Paging
* [x] Virtual Memory Manager
* [x] Heap
* [ ] Interrupt Handlers
* [ ] Advanced Configuration and Power Interface
* [ ] Timer
* [ ] Multithreading
* [ ] Symmetric Multiprocessing

<!-- Code of Conduct -->
### Code of Conduct

Please read the [Code of Conduct](https://github.com/My-Bad-2/Phoebe/blob/master/CODE_OF_CONDUCT.md)

<!-- License -->
## License

Distributed under the MIT License. See [LICENSE.md](LICENSE.md) for more information.


<!-- Contact -->
## Contact

Project Link: [https://github.com/My-Bad-2/Phoebe](https://github.com/My-Bad-2/Phoebe)

<!-- Acknowledgments -->
## Acknowledgements

 - [Buddy Alloc Contributors](https://github.com/spaskalev/buddy_alloc)
 - [OSDEV wikipedia](https://wiki.osdev.org)
 - [OSDEV Discord Server](https://discord.gg/RnCtsqD)
 - [Embedded Artistery](https://github.com/embeddedartistry)