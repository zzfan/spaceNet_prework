# netlink_test
```
1. $ make . 生成netlinkKernel.ko.  
2. $ gcc netlinkUser.c -o netlinkUser  
3. $ sudo insmod netlinkKernel.ko      
4. Run ./netlinkUser   
5. Remove module: $ sudo rmmod netlinkKernel          
6. make clean.  
```

一直发的，没有考虑发送缓冲区是否满了，但是一直没有返回-1，所以默认发的很快 (没有写一个接收线程) 
这样判断有点武断，其实我在内核中可以printf<0>等级的打出来，但是可能会不好，所以等级设置是默认的
需要自己去内核的<code>/var/log</code>去看下:  
<code>$grep -rn "Netlink" ./</code>   
看下在哪个文件里面，然后自己用awk和sed处理下
