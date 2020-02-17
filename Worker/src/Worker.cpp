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
 * @name         Worker.cpp
 * @description  This program 'wakes' the threads created by the
 *               factory program using an named semaphore.
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

sem_t *factory_sem_2; /* Pointer to semaphore */

int main()
{
	factory_sem_2 = sem_open("factorySem", 0);

	std::cout << "Worked PID: " << getppid() << std::endl;

	int num_wakeup = -1;

	do {
		std::cout << "How many threads do you want to wake up?" << std::endl;
		std::cin >> num_wakeup;

		for(int i = 0; i < num_wakeup; i++)
		{
			sem_post(factory_sem_2); /* Increment the count of the semaphore */
		}

	} while(num_wakeup != 0);

		/* Cleanup */
	sem_close(factory_sem_2); /* Close the semaphore */
	exit(EXIT_SUCCESS);
}
