/**
 * Copyright 2019 - Gerardo Alvarenga
 * Version 1, 29 August 2019
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 *
 * @name         Factory.cpp
 * @description  This program creates the worker threads and creates the named
 *               semaphore that will be used to synchronized the threads.
 * @author       Gerardo Enrique Alvarenga
 * @version      1.0
 *
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>
#include <csignal>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <fcntl.h>

volatile sig_atomic_t usr1_flag = 0; /* Indicates whether or not the handler function has been called */
sem_t *factory_sem; 				 /* Pointer to semaphore */

/**
 * This function sets usr1_flag to 1 to indicate that the signal has happened or to inform the parent that one of the child processes died.
 *
 * @param int signal 	Code of the signal that was received
 */
void handler(int signal)
{
	usr1_flag = 1; /* Trigger the usr1_flag */
}

/**
 * This function defines the behavior of the child threads
 *
 * Note: If a Thread is blocked and receives a Signal it will
 * not be blocked once it leaves the Signal Handler.
 */
void* child_thread(void *arg)
{
	printf("Worker thread %d created\n", pthread_self());

	int sem_val = 0;

	while(1)
	{
		sem_val = sem_wait(factory_sem); /* Wait on the semaphore */
		printf("Worker thread %d unblocked. \t\tStatus: %d\n", pthread_self(), sem_val);
		std::this_thread::sleep_for(std::chrono::milliseconds(5000)); /* Sleep for 5 seconds before looping */
	}
	return NULL;
}

int main()
{
		/* Interrupt handler - Applies to child threads */
	std::signal(SIGUSR1, handler); /* Register signal SIGUSR1 and signal handler */
		/* Ignore the following signals */
	std::signal(SIGUSR2, SIG_IGN);
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0; // or SA_RESTART
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		std::cerr << "Sigaction failed" << std::endl;
		exit(EXIT_FAILURE);
	}

		/* Prompt the user for the numbers of threads to create */
	int n_threads = 0; 										   /* Holds the number of worker threads */
	std::cout << "Enter the number of threads :" << std::endl; /* Get the number of threads to create from the user */
	std::cin >> n_threads;

	/*
	 * Initialize and open a named semaphore
	 *
	 * @params
	 * Name   - Name of the semaphore
	 * oflag  - Create a new named semaphore. If you set this bit, you must provide the mode and value arguments to sem_open().
	 * mode   - Specifies the permissions to be placed on the new semaphore.
	 * value  - The initial value of the semaphore. A positive value (i.e. greater than zero)
	 *          indicates an unlocked semaphore, and a value of 0 (zero) indicates a locked semaphore
	 *
	 * For other modes: http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_lib_ref%2Fs%2Fsem_open.html
	 */
	factory_sem = sem_open("factorySem", O_CREAT, S_IWOTH, 0);

	for(int i = 0; i < n_threads; i++)
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr); /* Initialize attr with all default thread attributes */

		/*
		 * @params
		 *
		 * threads - Is the location where the ID of the newly created thread should be stored, or NULL if the thread ID is not required.
		 * attr	   - Is the thread attribute object specifying the attributes for the thread that is being created. If attr is NULL, the thread is created with default attributes.
		 * task    - Is the main function for the thread; the thread begins executing user code at this address.
		 * arg     - Is the argument passed to start.
		 *
		 */
		int err = pthread_create(NULL, &attr, &child_thread, NULL); /* Create a new thread */
		if (err != 0)
		{
			std::cerr << "Failed to create thread" << std::endl;
		}
		pthread_attr_destroy(&attr); /* Destroy the attr */
	}

	while(usr1_flag != 1) /* Loop until flag is set to 1 */
	{
		/* Do nothing */
	}

		/* Cleanup */
	sem_close(factory_sem);   /* Close the semaphore */
	sem_unlink("factorySem"); /* Unlink the semaphore */

	exit(EXIT_SUCCESS);
}
