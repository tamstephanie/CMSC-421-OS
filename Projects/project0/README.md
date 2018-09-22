# Project 0
### Due Mon., Sept. 25, 2017, 11:59 PM

## Introduction/Objectives
All of the programming projects in this course will require making changes to the Linux kernel. In addition, all projects will be performed inside of a Virtual Machine. In order to do the later projects in this course, you must become familiar with the prospects of compiling the kernel inside the VM and creating update patches to submit. Project 0 is a very simple project that is designed to familiarize you with the procedures of installing and updating the kernel inside a VM.

This project is divided into several parts that will help you perform the following objectives:
- Becoming familiar with the tools used to build a custom Linux kernel image.
- Installing Linux and building a custom kernel.
- Modifying the Linux kernel.
- Creating a Linux kernel patch file.
- Formatting and submitting projects for this course.

## Hardware and Software Requirements
For the purposes of projects in this course, we will be using the x86 (32-bit) version of the Fedora 18 Linux distribution. In addition, the custom kernels that will be built in this course will be based on the Linux Kernel version 3.12.9. (Some of the accompanying documentation might refer to an older version of Fedora or Linux; ignore that, and use what is specified here.)

### Installing Linux
Perform the following tasks to create the environment that will be used to complete the projects in this course:
1. Install [VirtualBox](http://www.virtualbox.org/) on your computer.
2. Create a Virtual Machine and install Fedora 18 into it.<br>
    - Download the [Fedora 18 x86 LiveCD (Fedora-18-i686-Live-Desktop.iso)](http://archives.fedoraproject.org/pub/archive/fedora/linux/releases/18/Live/i386/Fedora-18-i686-Live-Desktop.iso)
    - Browse through the [Fedora 18 installation guide](http://docs.fedoraproject.org/en-US/Fedora/18/html/Installation_Guide/index.html).
    - Create your VM, **421VM**, and install Fedora into it. Follow the pdf guide.
    - After installation, shut down your VM. Make sure you follow the directions for doing this gracefully, or you might corrupt your virtual filesystem.
    - Remove the LiveCD from the virtual CD drive in the VM, as instructed in the install guide.
    - Boot the VM, and ensure that it boots properly.
    - Ensure that you can connect to the Internet with your VM.
    - Update the packages installed on the VM by executing the following command in a Terminal:
      `sudo yum update`
3. After updating, reboot the VM, to ensure that all updates complete before proceeding (the kernel will be updated in the yum update, and you must reboot to have the kernel update be applied).
4. Make sure that every time you shut down your VM, you do it using the orderly procedure mentioned in the guide. **DO NOT JUST CLICK THE "X" button!**

Many of the commands that you will be running on the Linux installation within the VM will require root privileges. There are a variety of methods to elevate your user privileges on Linux. You can use any of the following methods to do so:
```
sudo -s
(enter your user password when prompted)
(perform any commands to execute as root)
exit
```
**OR**<br>
```
sudo sh
(enter your user password when prompted)
(perform any commands to execute as root)
exit
```
**OR**

`sudo (command to execute as root)`

### Building a Custom Linux Kernel
The next part of this project is to obtain the Linux kernel sources and build a customized kernel. Follow this set of steps to do that:
1. Install the required software packages used for kernel development.<br>
   In a terminal, as root, install the required packages with the following command:<br>
   `yum install gcc gcc-c++ qt-devel emacs perl nano patch`
2. Obtain the Linux Kernel sources and unpack them into the appropriate directory.<br>
   In a terminal, as root, run the following commands:<br>
   ```
   cd /usr/src
   chmod 775 .
   chown root:wheel .
   curl -O https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.12.9.tar.xz
   sha256sum linux-3.12.9.tar.xz
   ```
   The kernel sources should be 76,397,560 bytes long. If you are getting a file that is not that length, then the address you are using is either wrong or down at the moment. Please try to re-download the file. (Note that the URL starts with "https:" instead of the usual "http:")<br>
    - Verify that the SHA256 displayed matches the following: 6a3e9f1abbaeaad34cddf0ddd69d60877765003faccf151b99d33e073566f5cb
    - Unpack the kernel sources, and make a working copy to use for the modifications for this project by running the following commands in a terminal as root (in the /usr/src directory):<br>
      ```
      tar -xvf linux-3.12.9.tar.xz
      cp -ra linux-3.12.9 project0
      ```
    - For the rest of this project, the sources in the directory /usr/src/project0 will be referred to as the **working copy** of the kernel. The sources in the directory /usr/src/linux-3.12.9 will be referred to as the **vanilla copy** of the kernel. **Never make any modifications to the vanilla copy of the kernel!**
3. Build and install your customized kernel.
    - Read the [Kernel Rebuild Guide](http://web.archive.org/web/20110720224829/http://www.digitalhermit.com/linux/Kernel-Build-HOWTO.html). We will be using Linux 3.x kernel for this project, which is handled very similarly to Linux 2.6.x.
    - You may want to review [Linux Kernel in a Nutshell](http://www.kroah.com/lkn/) as well.
    - Change the version string of your working copy of the kernel to 3.12.9-glusername-cs421project0 Where glusername is your UMBC GL username. If you have followed all of the instructions up to this point, you should have no trouble doing this. (Hint: You will have to change at least one file that is distributed with the kernel--we want you to make sure you can create/apply a patch. The file you must change is in the top directory of the kernel source tree.)
      <br>As an example, if I were doing the project, this would be what should be printed out by `uname -a`:
<br>_Linux linux421jp 3.12.9-park-cs421project0 #1 TODO SMP Mon Feb 4 14:59:49 EST 2013 i686 i686 i386 GNU/Linux_
<br>**In order to accomplish this project's goals, please make sure that you have changed at least one file that is included in the kernel's source tarball. There is an option to change the kernel's name in the "make xconfig" step below. DO NOT USE IT. You must change at least one file that is included in the Linux kernel source distribution, and you should make your change BEFORE running any of the commands in the next step.**
    - Configure and compile the custom kernel and its modules. This step will likely take quite a while. Run the following commands in a root terminal (you will have to hit a "save" button in the xconfig step):<br>
      ```
      cd /usr/src/project0
      make mrproper
      make xconfig
      make
      make modules_install install
      ```
    - Your working copy of the kernel should now be built and installed. In addition, the install process should have updated your bootloader configuration to add the new entry to it (and made it the default kernel option too).
    - Reboot your VM, and start your customized kernel.
    - Make sure your custom kernel boots properly in the VM. Run the following commands to check the customized kernel and prepare the first part of your project submission:<br>
      ```
      cd /usr/src
      mkdir project0-submission
      cd project0-submission
      script
      uname -a
      exit
      cat typescript
      ```
    - Verify that the output from the last command shows the correct version string, as specified earlier in the project. If it does not match the format shown above please go back and try again. For obvious reasons, your date string in the kernel's uname will be different, as will your machine's name. Also, the "#1" is a counter for how many times the kernel has been compiled out of the source tree in use, so if you have to try more than once, that will be different as well. If you do have to compile the kernel again, you do not have to run the "make mrproper" step again.

### Creating and applying Kernel updates/patches
Your next objective is to learn using `diff` to generate kernel source patches (updates) and applying the patches to the original kernel. Since many modifications to the kernel are small (compared to the total kernel source size), such updates are usually distributed as "patches". If you make minor modifications to the kernel, you will want to create a patch.

A good tutorial on how to create patchfiles using diff can be found at http://www.gnu.org/software/diffutils/diffutils.html. The typical call is:
`diff [<options>] <from-file> <to-file>`

You should have the vanilla kernel sources in the directory /usr/src/linux-3.12.9 as described earlier. You should have already built your first custom kernel as specified in the last part, by making appropriate changes to the experimental kernel sources.

1. Download ignorelist.txt. This will permit you to create patches without first running make mrproper in the kernel sources directory. Place the file in the /usr/src directory in your vm.
2. Create a patchfile for your kernel using the diff command:
   ```
   cd /usr/src/
   diff -rcP -X ignorelist.txt linux-3.12.9 project0 > project0-submission/patchprj0.diff
   ```
   The file patchfile patchprj0.diff contains all the information the patch command needs to transform the vanilla kernel sources to your working copy of kernel sources.
   <br>**IMPORTANT:** If the ignorelist does not work then you should execute `make mrproper` in the experimental kernel sources directory before creating the patchfile. If you forget to do this, you may get a very large patchfile.
3. Verify your patch using the patch command.
<br>You should always validate your patchfiles before submitting them. Make a new full copy of your vanilla kernel sources, apply your patch to it, and compare with the working kernel sources:
   ```
   rm -rf verify-linux
   cp -ra linux-3.12.9 verify-linux
   cd verify-linux
   patch -p1 < ../project0-submission/patchprj0.diff
   cd ..
   diff -r -X ignorelist.txt project0 verify-linux
   ```
   The sources in the verify-linux directory should now be transformed into your working sources. The diff command above should not show any differences. For Project 0, if you see several Only in project0/..., or Only in linux/... this is also OK.
4. Examine your patchfile, note the format and that your changes are included in the file.
5. Note that all patches you submit must work with the "-p1" option. You can guarantee this by always using diff as shown above. Also, do not edit patch files once you have created them! This will break the patch command.

## Submission Instructions
You should submit a single .tar.bz2 file glusername-project0.tar.bz2 (once again, replacing glusername as appropriate) that contains the following files (in the root directory):
- patchprj0.diff
- Your /usr/src/project0/.config file (may be renamed to config with no leading dot--**NOTE:** This is not the Kconfig file!)
- The typescript file containing the output from `uname -a`

You can create the glusername-project0.tar.bz2 file as follows (replacing glusername as appropriate):
```
cd /usr/src/project0-submission
... copy the files requested into this directory if they are not already there ...
tar -cjvf glusername-project0.tar.bz2 .
```
Once you create the glusername-project0.tar.bz2 file, upload it as an attachment on the Project 0 assignment on the course blackboard.

### What to do if you want to get a 0 on this project
**Any of the following will cause a significant point loss on this project. Items marked with a * will result in a 0.**
- Missing patchprj0.diff file. * 
- patchprj0.diff file is in excess of 2KiB (2048 bytes).
- Submission tarball file is in excess of 1MB (1,048,576 bytes). * 
- Any other file listed in the Submission Instructions is missing.
- Patch file is not named patchprj0.diff.
- Project submissions that do not extract properly with the following command: `tar -xjvf glusername-project0.tar.bz2`
- Submissions that create a directory and put all their files in it (i.e, the patchprj0.diff file is not in the base directory).
- Submission tarball is not named correctly.

Please do not make us take off points for any of these things!
