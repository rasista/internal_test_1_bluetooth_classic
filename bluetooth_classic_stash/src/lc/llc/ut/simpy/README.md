#   README  #

### Simulation Framework    ###
&emsp;&emsp;The simulation framework(Hereinafter referred to as just Framework/framework ) utilizes the Simpy Python package to simulate various real-time models(developed by individual developers) in a unit testing environment. It employs the UDP communication protocol to establish a connection between the simulation and firmware environments. Serving as an intermediary layer on firmware modules, the framework creates testing environments. Commands originating from the simulation layer are transmitted to firmware modules via UDP protocol. Responses to these commands are then relayed back to the simulation layer using the same UDP port. To run a test case, the framework requires input in the form of simulation model files (written in Python), testcase configuration files (in YAML format), and test case files (in text format). Examples of the contents of these files can be found in the "[firmware layer folder](#firmware-layer-folder)" section.  

*For design related docs, please refer to [Simulation Framework](https://confluence.silabs.com/display/EN/Simulation+Framework)*  
>*__pre-requisite:__* The framework expects a basic level of understanding yaml file, Writing a testcase file that is framework compatible and basic level of understanding python from the developer.  

###    Build Folder  ###

Build Folder referred in this README file is present in the   
    
    BUILD_FOLDER_PATH = ~/src/{protocol}/{path_specific_to_test_folder}/ut/simpy/build

###    Folder Structure    ###
Before jumping on to running the framework, developer has to understand the folder structure of each file that is required.  
Following sections discuss in detail on the folder structure for [simulation layer](#simulation-framework-folder) and [firmware layer](#firmware-layer-folder).

####    Simulation Framework Folder ####
* Simulation framework is placed under **/home/src/ut_frameworks/simpy/**  
* But invoking the framework could be done from respective protocol build folders (*refer*: [Invoking Framework](#how-to-run-the-framework))
```sh
<protocol-name>/ut_frameworks/
└── simpy_framework
    ├── c_src                       #Directory for simulation interface on the firmware side would be present.
    ├── models                      #Example models.
    ├── tc                          #Example test case files.
    ├── testcase_configs            #Example test case configuration files.
    └── util_libraries              #Utility libraries that could be utilized for Simulation testing.
        ├── btc_pkt                 #BTC Packet types utility
        ├── sbc
        ├── sbc_frame
        ├── sbc_pkt
        └── udp_framework           #UDP protcol Utility files.
```
> **__Note:__** *simulation_run.sh* or the *pytest* framework will automatically invoke framework specific files required to run

####    Firmware Layer Folder  ####
* Models required for testing of this folder is present under **$BUILD_FOLDER_PATH/../models/**  
*Refer example model files located in the [simpy_framework](#simulation-framework-folder) folder*  

* Configuration files required for the simulation is present under **$BUILD_FOLDER_PATH/../testcase_configs/**    
    This file specifies models that need to be imported into framework.  

```YAML
define: &NUM_BTC_DEVS 3 #Global variable(Optional). Here we used it to specify number of instances for a specific model

Models:
  btc_dev: #Model name
    instances: *NUM_BTC_DEVS #Usage of global variable(Optional)
    secondary_init_function : secondary_init_function #Secondary init function(Optional) to initialze additional variables at t=0
    python_handlers: #List of commands that this model can take as a trigger and corressponding python handlers to those commands
      #command_name : command_handler
      init_device : init_device_handler
      init_page : init_page_handler
      init_page_scan : init_page_scan_handler
      start_page : start_page_handler
      start_page_scan : start_page_scan_handler
      init_inquiry : init_inquiry_handler
      start_inquiry : start_inquiry_handler
      init_inquiry_scan : init_inquiry_scan_handler
      start_inquiry_scan : start_inquiry_scan_handler
      compelete_event : complete_event_handler
      payload_buffer: payload_buffer_handler
  rail:
    instances: *NUM_BTC_DEVS
    secondary_init_function : secondary_init_function
    python_handlers:
      expect: expect_handler
      retry_tx : retry_tx_handler
      schedule_rx  : schedule_rx_handler
      schedule_tx  : schedule_tx_handler
      tx_done : tx_done_handler
      rx_done : rx_done_handler
      rx_timeout : rx_timeout_handler
      schedule_backoff : schedule_backoff_handler
      do_not_expect: do_not_expect_handler
      set_var : set_var_handler
      rx_header_received: rx_header_received_handler
```  
> **_Note:_** *Refer example test case configuration files located in the [simpy_framework](#simulation-framework-folder) folder*  

* Testcase files required are present under **$BUILD_FOLDER_PATH/../tc/**  

This file contains the test cases to test the functionality of firmware modules, triggers specified module with specified arguments. 

    #invoke <timestamp> <model_name> <command_name> <command_args>

* timestamp : Specifies at what time this line from text file has to be invoked.
* model_name : Specifies to which model this test case triggers.
* command_name : specifies the trigger to the model(Translation of command_name to command handler is taken care in testcase configuration file as discussed in "[firmware modules folder](#firmware-modules-folder)" section).
* command_args : specifies arguments that are passed on to the model for the corresponding command.

> **_e.g:_** #invoke 1100 btc_dev_1 init_page 1 0 66:55:44:33:22:11 0

* While sending parameters to simulation_run.sh or pytest pass configuration files and testcase files as arguments with the paths as mentioned above.  

###    How to run the Framework?  ###

This file provides you with a detailed procedure on how to run framework.  
Place required testcase configuration files in yaml format under $BUILD_FOLDER_PATH/../testcase_configs/ folder.  
Place required testcase files in text format under $BUILD_FOLDER_PATH/../tc/ folder.  
Models required for Simulation has to be placed under $BUILD_FOLDER_PATH/../models/ folder. Once required files have been placed under respective folders.  
Simulation could be started using two methods.
* Using shell script.
* Using pytest framework.

#####   Using Shell Script  #####
Under build folder, shell script "simulation_run.sh" will be placed. simulation_run.sh takes following parameters as inputs:  
&emsp;&ensp;-c configuration file required for simulation(required)  
&emsp;&ensp;-t testcase file(required)  
&emsp;&ensp;-s time until which simulation has to run(required)  
&emsp;&ensp;-f file to redirect the output generated by framework(optional)  
&ensp;e.g simulation_run.sh -c testcase_configs/s3_btc_dev.yaml -t tc/s3_btc_cmds.txt -s 100000
#####   Using PyTest Framework  #####
* Write various case files with all the corner cases you want to cover for a firmware module.   
* In test_sim.py write test functions with the test case files([](#firmware-layer-folder)) as a parameter, pass the desired yaml file, testcase file and simulation time (desired firmware can be passed as well). 
* Compile in build folder and run "pytest" as a command.  
* Firmware reports are generated in FW_logs.txt and for simulation in pytest_stdout.log, testcase report and code coverage is generated in pytest_test_report.log.

####    ####

