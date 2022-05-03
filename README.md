# Zip-Project
## By: Logan Cole
## CMPE-320 Operating Systems
The code takes in a file, and using Run Length Encoding will compress the file and output the compressed version to stdin  
For example:  
``` aaaa ```  
will become  
``` 4a ```  
for files bigger than 4096 bytes, the program creates 3 threads to handle the file, if its less than 4096
bytes it will compress on the main thread
### Compiling and Running
We can compile the code with  
``` make wzip ```  
and run the code with  
``` ./wzip <somefile> ``` or ```./wzip <multiple files> ```  
### For Files Bigger Than 4096 bytes
To speed up the encoding for large files, we create 3 threads. The first will read from start->some_offset, some_offset++, then from some_offset++ to the end of the file.  
In my implementation I went with a Queue style linked list following FIFO. Its not the most optimal solution, as it still takes around 2 seconds to read in a super large file.
### Problems and What I learned
To have a working solution, it was a lot of work. I probably wrote 8 different solutions until one actualy made sense and worked (being this version). The code runs successfully under my schools CS/SE linux environment, but will segfault on my 2021 Macbook pro M1 and I've yet to be able to figure out why. Just that segfault issue took me forever to figure out that its just my pc. It taught me a lot about working with queues and linked lists, especially with threads in the mix. Since I wasn't overlapping data with the threads and each had their own queue to work with, I didn't use any locks in my implementation. If I had to do this again, I probably would have went with some sort of arraylist or used one queue and would use locks to make sure the queue was in the correct order.

