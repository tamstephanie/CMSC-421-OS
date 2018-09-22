# Project 2
**Due Dates:
<br>Phase I: 2017 Nov. 29, 11:59PM
<br>Phase II: 2017 Dec. 6, 11:59PM (Extension: allowed to be graded w/ Phase III)
<br>Phase III: 2017 Dec. ?, 11:59PM**

Author: Stephanie Tam<br>
Email: ru14618@umbc.edu

## Introduction/Objective
The goal is to build our own file system (FS). We aren't expected to build the FS from scratch; the professor will provide a design for the FS as well as a skeletal Linux FS driver implementation, some of the parts already done.

## Approach
**Install Provided Code**
1. Create fresh copy of kernel into project2/ & cd into it
2. Download "proj2-fall17.tar.bz2" into the directory
3. Untar the file (tar -xf proj2-fall17.tar.bz2)
4. Delete the .bz2 file
5. Edit top-level Makefile to change kernel name
6. DO NOT RUN "make xconfig"
7. Begin editing files in project2/fs/fs421/
8. Only run "make" and "make modules_install install"

**Phase I:**
<br>Add code to the itree.c::block_to_path() function to calculate the level and offset based on the block number given. This function will then be used in the fs421_get_block() function.

**Phase II:**
<br>

**Note:** Taking extension


**Phase III:**
<br>To be updated


**Resources:**
1. https://elixir.free-electrons.com/linux/v3.0/source/include/asm-generic/bitops/le.h
2. http://time.to.pullthepl.ug/annotated-xiafs/xiafs/module/bitmap.c.html
3. http://opensourceforu.com/2011/01/understanding-a-kernel-oops/

## Driver Programs/Testing
**Mount & Unmount:**
  - Mount:
    - _sudo losetup /dev/loop0 [test image]_
    - _sudo mount -t fs421 /dev/loop0 /tmp/mntpt_ (NOTE: run this command in project2/fs/)
  - Unmount & Check for Corruption:
    - _sudo umount /dev/loop0_
    - _sudo losetup -d /dev/loop0_
    - _./fsck.minix -v -s -l -f fs421smwithfiles.img_ (Make sure the image isn't corrupted)

## Submission Instructions:
1. Run typescript
    - Run _uname -a_
    - Show that filesystem access is working (Call "ls", "cat", "more")
2. Create & verify patchfile
    - Create patchfile: _diff -rcP -X ignorelist.txt linux-3.12.9 project2 > project2-submission/[phaseI,phaseII,phaseIII]/patchprj2.diff_
    - Verify:
    <br> _rm -rf verify-linux
    <br> cp -ra linux-3.12.9 verify-linux
    <br> cd verify-linux
    <br> patch -p1 < ../project2-submission/patchprj2.diff
    <br> cd ..
    <br> diff -r -X ignorelist.txt project2 verify-linux_
3. Submit tarfile
    - Files included:
      - Makefile (in project2/)
      - typescript
      - patchfile
      - README
    - Tar: _tar -cjvf ru14618-project2.tar.bz2 [files included]_
