# README.md
Two scheduling algorithms: Deficit Round Robin (DRR) and Strict Priority Queue (SPQ), which are used to prioritize network traffic based on different QoS requirements are implemented in the ns-3 network simulation framework. We evaluate the performance of the framework using Wireshark IO Graphs and compare the pre and post QoS scenarios for each algorithm.

## Developers
1. Tanya Jain
2. Parisa Abbasi
3. Arash Ansari

## Set Up

Clone the repository in the scratch directory of your ns3 installation:

```bash
cd scratch
git clone git@github.com:TanyaaCJain/QoS.git
cd QoS
```

You need to have the configuration files for every Differentiated Service established. You can find them in the `config` directory. The files are named according to the service they represent. For example, the configuration file for the
- `SPQ` service is `spq.txt`
- `DRR` service is `drr.txt`

Note: The lab 3.2 is an implementation of a simple queue and has its own configuration file `simple.txt`.

## Run

To run the simulation, you need to specify the configuration file as an argument to the program via the command line using the `--i` flag. For example, to run the simulation for the `SPQ` service, you would run the following command:

```bash
./ns3 run "qos --i=scratch/qos/config/spq.txt"
```

The name of the configuration file helps in identifying the service that is being simulated with the help of the `factory.h` file. No code needs to be altered to run the simulation for a different service.

## Results

The results of the simulation are stored in the `output` directory. The IO Graphs are stored in the form of `.png` files.
