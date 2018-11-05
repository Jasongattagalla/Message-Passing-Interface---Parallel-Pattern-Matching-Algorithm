
### Prerequisites

What things you need to install the software and how to install them

 * C programming
 * MPI Library
 * SSH
     

### Installing

Install Message Passing Interface Library 

```
sudo apt install mpich
```

## Execution

Compile

```
mpicc myprojbackup.c -o myprojbackup
```

Run

```
mpiexec -iface eth1 -f hosts -n 4 ./myprojbackup 
```
You can add the different host's ipaddresses for processing it in distributed environment.


## Built With

* [MPI](https://computing.llnl.gov/tutorials/mpi/)


## Authors

* **Jason Gattagalla** - *Initial work* - [Github](https://github.com/Jasongattagalla) [Linkedin](https://www.linkedin.com/in/jasongattagalla/)


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
