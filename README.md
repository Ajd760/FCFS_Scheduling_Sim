# FCFS_Scheduling_Sim
A First Come First Served process scheduling algorithm simulation

This program simulates a FCFS scheduling algorithm over a span of 5 minutes (simulated) while
	collecting statistics to evaluate the algorithm. The user will specify the number of processes to 
	simulate executing on the virtual system via a command line argument when executing the program.
		i.e. in a unix environment: "./a.out 20"
	The program will display a promt before beginning the simulation, showing the number of processes
	that were entered and telling the user to press the enter key to begin. Once the simulation is complete,
	the program will print out statistics for each individual process followed by a block of statistics on the
	overall performance of the simulation.

	*Note that while the number of processes to simulate can be specified, this program (in its current state) 
	does not allow the user to specify the details and times for each process - these are assigned randomly when 
	each simulated process is created.
