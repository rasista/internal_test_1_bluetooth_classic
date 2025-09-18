# README #

## Getting started ##

### Get Repository ###

#### Obtain access rights ####

Requesting access rights to repositories might take a long time! If you already have access rights, skip this step. Perhaps access rights are nowadays already fine based on some group membership, but here's the details on the relevant repo and place in ipcatalog:

At ipcatalog.silabs.com go to the stash tab and find repository gsdk:
        ***ssh://stash.silabs.com/gsdk/gsdk.git***
In the bottom there is a "key" icon named access. Select a repository and request access from this icon.

#### Set SSH ####

- Run command `ssh keygen` on your linux console. This should generate a file `~/.ssh/id_rsa.pub`
- Go to [https://stash.silabs.com/account](https://stash.silabs.com/account)
- Navigate to SSH Keys → Add key → Add content of `~/.ssh/id_rsa.pub`

#### Pull Repository ####

```
git clone ssh://git@stash.silabs.com/gsdk/gsdk.git
cd gsdk
git submodule update --init --recursive
cd protocol/classic-bluetooth
```

### Set Up The Build Environement ###

It Is highly recommended to use [Dockerfile](./Dockerfile) for setting up the build environment for BT Classic code development. Docker can be initialised through [docker_init.sh](./docker_init.sh) file. Script executes

- *docker build* command to pull all required dependencies and create a *docker image*.
- *docker run* command to run the docker image

 Once succesfully executed, you should get into a docker terminal with your working gsdk directory mapped onto */home/src/gsdk*. Please not that *-v* command in *docker run* is used to mp your local path to docker. ie. you can access files in your local path from docker environement. Modify *docker_init.sh* to change paths.

### Folder Hierarchy ###

The BT-Classic repository is mjorly arranged in below folder heirarchy:

- cmake
- src
- tools
- ut_framworks

#### cmake ####

This folder comprise of generic cmake files common for all modules and submodules within BT-Classic

#### src ####

<details>
<summary>
This folder comprise of actual source code for BT-Classic protocol and organised into link controller([lc](./src/lc/)) and [profile stack](./src/protocol_stack/), [btcapi](./src/btcapi) and [utils](./src/utils/).
</summary>
folder which comprise of generic frameworks used across different layers like [State Machine Management](./src/utils/state_machine). The Link controller and Profile stacks are further organised in some modules. Each module is identified with its library, built through its own Cmake and shall have its own Unit test coverage.

##### Link Controller #####

<details>
<summary>
The Link control module is further organized into Lower Link Control([llc](./src/lc/llc) and Upper Link Control([ulc](./src/lc/ulc))).
</summary>

###### Lower Link Controller ########
<details>
<summary>
The Lower Link Control implements the lower most layer in BT CLassic protocol and is responsible for scheduling radio activities.
</summary>
The top level architecture for Lower Link Control is documented [here](https://confluence.silabs.com/display/PGEVEREST/Everest+SW+Architecture-BTC+Lower+Link+Layer).Rules that govern different radio activities are clubbed together in form of procedures and are implemented as independent state machines.
These procedures are:

- [Page Procedure](https://confluence.silabs.com/display/EN/BTC+HLD+Page)
- [Inquiry Procedure](!Todo LInk to design document)
- [Page Scan Procedure](!Todo LInk to design document)
- [Inquiry Scan Procedure](!Todo LInk to design document)

</details>

###### Upper Link Controller ########

<details>
<summary>

The Upper link control implements the rest of the Link Controler modules majorly(but not limited to):

- HCI Interface with Profile Stacks
- LMP Interface with peer link controller
- Scheduling between different Lower Link Controller Procedures

</summary>
</details>
</details>

#### Protocol Stacks ####

<details>
<summary>
Different use case sceanrios for Bluetooth classic are supported through dedicated profile stacks. [GAP](./src/protocol_stack/gap) and [L2CAP](./src/protocol_stack/l2cap/) can be seen as three basic profiles required to 1.) discover and establish connection with peers 3.) establish data path with remote peers respectively. They communicate to controller through HCI interface. Other profiles are built on top of L2CAP in heirarchial manner(with few exceptions, as explained below in order they occur). [SDP](./src/protocol_stack/l2cap/sdp) can be seen as a special profile on top of L2CAP as it is used to share information about other supported profiles between the peers. Each profile is maintained in a spearate subdirectory with same heirarchy. Each profile runs in its independent OS Task, is compiled through its own independent library and shall have own unit test coverage.
</summary>

##### GAP #####

!ToDo provide a write up on design of GAP

##### L2CAP #####

!ToDo provide a write up for L2CAP design

##### SDP #####

!ToDo provide a write up on design of SDP

!Todo Provide a write up for other profiles on top of L2CAP
</details>
</details>

#### tools ####

<details>
<summary>
This folder comprise of different scripts that are used at the time of pre-build, debug, analysis or any other purpose
</summary>

##### jinja_rendered.py #####

! ToDo add write up for jinja renderer and other scripts

</details>

#### ut_frameworks ####

<details>
<summary>
This folder comprise of unit test related librariries that might be shared across multiple modules and sub modules.
</summary>

##### simpy #####

!ToDo write a small write up on simpy
</details>

## Compilation And Unit Testing ##

<details>
<summary>
!ToDo add overall build environement architecture
</summary>

### Lower Link Layer ###

#### libbtc_llc.a ####

!ToDo Add steps to generate libbtc_llc.a here
#### btc_llc_ut_simpy ####

!ToDo Add steps to run llc unit test cases here
</details>

## Coding Guidelines ##

[Silabs Standard Coding Guidelines](https://stash.silabs.com/projects/WMN_DOCS/repos/docs_misc/browse/software/standards/coding-standard.md) should be strictly followed. Additionally, below mentioned guidelines should be followed.

- Modularity is a must, do not expect global state variables to be shared between different modules unless there is very compeling reason for same. There should be a single, simplified public header file for each module. Including a public header file for a module should not require path to private header files.
- Code should be platform agnostic. Endiannes and Word length should not break the interface functions. Message across interfaces should not be parsed through structures. Messages needs to be defined in form of Offset and length for individual fields and should be parsed/initalized the sme way. A frame work is created to generate appropriate defines to keep code readable. Refer appropraite section for same !ToDo add link to section @Manas
- Design to avoid using `#if` pre-processor directives. When absolute must, pre-processor defines should always be compared against a value. `#ifdef` is strictly prohibated. This is important to maintain uniformity and avoid unintended errors. `#if` will be used extensively in customer facing source codes to set certain configurations in customer build environement. Using both `#if` and `#ifdef` will cause confusion

## Code Commit Guidlines ##

!Todo Commit guidlines when available

## Jenkins Pipes ##

!ToDo add Jenkins pipe details 