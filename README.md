<!--
     Copyright 2021 Aalto University & University of Waterloo

     SPDX-License-Identifier: CC-BY-SA-4.0
-->

# seL4 CAmkES TEE

This repository contains the trusted execution environment (TEE) proof-of-concept
implementation for the project researching integrity protection of remote
attestation procedures in the case of confidentiality compromise of the TEE.

## Getting started

This repository does not run on its own. A comprehensive project is a collection of
repository/project dependencies. For the TEE, this includes `seL4`, `camkes-tool`,
`global-components`, and `sel4-tee-mapping`. The dependencies are specified
in a [manifest](https://github.com/ssg-research/sel4-tee-manifest).
The mapping scripts are also available on [GitHub](https://github.com/ssg-research/sel4-tee-mapping).

### Prerequisites

- [repo](https://android.googlesource.com/tools/repo) (see also [seL4's documentation on its buildsystem](https://docs.sel4.systems/projects/buildsystem/repo-cheatsheet.html))
- [seL4 / CAmkES dockerfiles](https://github.com/seL4/seL4-CAmkES-L4v-dockerfiles)

### Build

1. Initialize project from a manifest using `repo` 
  ```
  repo init -u {manifest-git-repository}
  repo sync
  ```
2. Start build container in working directory
  ```
  make -C path/to/dockerfiles user HOST_DIR=$(pwd)
  ```
3. Build for `qemu-arm-virt` platform
  ```
  mkdir build && cd build
  ../init-build.sh -DPLATFORM=qemu-arm-virt -DAARCH64=TRUE -DSIMULATION=1 -DRELEASE=0 -DCAMKES_APP=tee -DKernelArmExportPCNTUser=On -DKernelArmExportPTMRUser=On
  ./compile.sh
  ```

The `compile.sh` script comes from the `sel4-tee-mapping` repository and
is initialized as a symlink in the project root directory. It automatically
executes the TA mapping scripts that make the remote attestation possible.
These are interleaved in multiple rounds of compilation.

### Run

To run the seL4 CAmkES TEE with named pipes for communication with the co-processor:

```
# Generate named pipes
mkfifo /tmp/guest.in /tmp/guest.out
# Run qemu but redirect serial I/O to named pipe
 qemu-system-aarch64 -machine virt -cpu cortex-a53 -nographic  -m size=1024  -kernel images/capdl-loader-image-arm-qemu-arm-virt -serial pipe:/tmp/guest
```

## Abstract

In recent years, trusted execution environments (TEEs) have seen increasing
deployment in computing devices to protect security-critical software from
run-time attacks and provide isolation from an untrustworthy operating system (OS).
A trusted party verifies the software that runs in a TEE using remote attestation
procedures. However, the publication of transient execution attacks such as
Spectre and Meltdown revealed fundamental weaknesses in many TEE architectures,
including Intel SGX and Arm TrustZone. These attacks can extract cryptographic
secrets, thereby compromising the integrity of the remote attestation procedure.

In this work, we design and develop a TEE architecture that provides remote
attestation integrity protection even when confidentiality of the TEE is
compromised. We use the formally verified seL4 microkernel to build the TEE,
which ensures strong isolation and integrity. We offload cryptographic operations
to a secure co-processor that does not share any vulnerable microarchitectural
hardware units with the main processor, to protect against transient execution
attacks. Our design guarantees integrity of the remote attestation procedure.
It can be extended to leverage co-processors from Google and Apple, for
wide-scale deployment on mobile devices.