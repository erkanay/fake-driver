# Minimal Fake Device Driver

This is a minimalistic kernel module which written for learning purposes using C language. 
It corperates relation between non-exist [device driver](https://en.wikipedia.org/wiki/Device_driver) and user space.

## Build Route

First of all create a [character device file](http://www.tldp.org/LDP/lkmpg/2.4/html/c577.htm) in ```/dev```.
```{r, engine='bash', count_lines}
mknod /dev/testDeviceFile c 900 1
```
Clone the repository
```{r, engine='bash', count_lines}
git clone https://github.com/erkanay/minimal-fake-driver.git
cd minimal-fake-driver
```
Then compile it with
```
make
```
Check it out with
```
dmesg
```

### TODO 
 * Write a simple user application for it

