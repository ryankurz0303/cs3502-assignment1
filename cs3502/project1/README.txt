CS 3502 Project 1
Multi-Threaded Banking System

Name: Ryan Kurz

About the Project

For this project I made a banking program in C using pthreads. Each phase adds something different involving threads, mutexes, race conditions, and deadlock.

Files

phase1.c
This phase has multiple teller threads changing the same balance without any protection. Because of that, the final balance and transaction count can come out wrong.

phase2.c
This is the fixed version of Phase 1. I added a mutex so only one thread can change the account data at a time. The final balance and transaction count should always be correct.

phase3.c
This phase intentionally creates a deadlock. One thread locks Account 0 and waits for Account 1, while the other thread locks Account 1 and waits for Account 0. A monitor thread waits five seconds and prints a warning if no transfers finish.

phase4.c
This phase fixes the deadlock by always locking the lower account number first. Both threads follow the same lock order, so they do not get stuck waiting on each other.

How to Compile

gcc -Wall -pthread phase1.c -o phase1
gcc -Wall -pthread phase2.c -o phase2
gcc -Wall -pthread phase3.c -o phase3
gcc -Wall -pthread phase4.c -o phase4

How to Run

./phase1
./phase2
./phase3
./phase4

Notes

Phase 1 can give different answers each time because the threads are not synchronized.

Phase 2 should finish with a balance of 5000.00 and a transaction count of 4000.

Phase 3 is supposed to freeze because it creates a deadlock. After five seconds it prints a warning. I used Ctrl+C to stop it.

Phase 4 should finish normally. Account 0 should end at 950.00, Account 1 should end at 1050.00, and the total should still be 2000.00.

For the Phase 4 solution, I used lock ordering. Both threads lock Account 0 before Account 1, even when the transfer is going in the opposite direction. This prevents the circular wait that caused the deadlock.
