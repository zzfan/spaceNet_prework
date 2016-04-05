# spaceNet_prework
1. $ make . 生成netlinkKernel.ko.  
2. $ gcc netlinkUser.c -o netlinkUser  
3. $ sudo insmod netlinkKernel.ko      
4. Run ./netlinkUser   
5. Remove module: $ sudo rmmod netlinkKernel          
6. make clean.  
