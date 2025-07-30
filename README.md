# I2C Command Application 

## I2C Command 
### 1. scan  
```
    syntax: 
        scan  

    return:
        success: OK [return hex string]
        fail: NG
```
### 2. detect  
```
    syntax: 
        detect [addr]  

    parameters:  
        - addr: hex string. (like 3C) 

    return: 
        success: OK 
        fail: NG 
```   
### 3. read 
```
    syntax: 
        read [addr] [len]

    parameters:
        - addr: hex string. 
        - len: hex string. (like AA)

    return:
        success: OK [return hex string]
        fail: NG
```
### 4. write 
```
    syntax: 
        write [addr] [len] [data] 

    parameters:
        - addr: hex string. 
        - len: hex string. (like AA)
        - data: hex string with comma. (like AA:BB:CC:DD) 
       
    return:
        success: OK 
        fail: NG
```
