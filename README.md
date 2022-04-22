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
``` make all ```  
and run the code with  
``` ./wzip <somefile> ```  
