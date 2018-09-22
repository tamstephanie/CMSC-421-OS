#include <linux/kernel.h>

#include <linux/list.h>
#include <linux/klist.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>
#include <linux/errno.h>

#define __user __attribute__((noderef, address_space(1)))
#define MAX_MSG_LEN 512

struct msgQueue {
  struct list_head list;  /* Kernel list structure */
  long msgLen;            /* Keep track of message length */
  char *m_message;        /* Store message */
};

DEFINE_SEMAPHORE(lock);  /* Use when item could potentially be accessed and/or changed out of order */
/*  lock: down(&lock); 
 *  unlock: up(&lock);
 */

static LIST_HEAD(myQueueList);
static int queue_size = 0;


asmlinkage long sys_enqueue421(const void __user *data, long len) {
  /*  Description: Add new item to tail of queue
   *  Parameters: *data (new pointer to add in)
   *              len (# bytes to copy starting at data pointer)
   *  Return: Success => 0
   *          Error => -ENOMEM (insufficient memory to add or store item)
   *                   -EFAULT (error accessing user-space pointer to copy data)
   *                   -EINVAL (len < 0)
   *                   -E2BIG (len > 512)
   *  Other Notes: Don't modify any data in queue on any error
   */
  printk("SYS_ENQUEUE421() called\n");

  if(len < 0) {  /* Check if number of bytes to copy is negative */
    printk("ERROR: 'len' is negative\n");
    return -EINVAL;
  }
  if (len > MAX_MSG_LEN) {  /* Check if # of bytes to copy is greater than the allowed message size */
    printk("ERROR: Length of message too long\n");
    return -E2BIG;
  }

  char *copied_space = kmalloc(len, GFP_KERNEL);  /* Copy bytes from user to kernel */
  if(copied_space == NULL) {  /* Check if there's enough memory to add item */
    printk("ERROR: Not enough memory\n");
    return -ENOMEM;
  }
  if(access_ok(VERIFY_READ, data, len)) {  /* Check if user-space is accessible */
    long from_userSpace = __copy_from_user(copied_space, data, sizeof(data));
    if(from_userSpace != 0) {  /* Extra checking to ensure the space actually is accessible */
      printk("ERROR: Can't access user-space\n");
      return -EFAULT;
    }
  }
    
  /* Create new struct w/ data from given user-space pointer */
  struct msgQueue *new_item = kmalloc(sizeof(struct msgQueue), GFP_KERNEL);
  new_item->msgLen = len;
  new_item->m_message = data;

  down(&lock);
  list_add_tail(&(new_item->list), &myQueueList);  /* Add item to end of queue */
  queue_size++;
  up(&lock);

  return 0;
}

asmlinkage long sys_dequeue421(void __user *data, long len) {
  /*  Description: Remove item from head of queue & store data back into user-space
   *  Parameters: *data (user's space to store dequeued entry)
   *              len (upper limit on # bytes allocated starting at pointer data)
   *  Return: Success => 0
   *          Error => -ENODATA (empty queue)
   *                   -EFAULT (error accessing user-space pointer to copy data)
   *                   -EINVAL (len < 0)
   *                   -E2BIG (space allocated not large enough to store entire message)
   *  Other Notes: Don't modify any data in queue on any error.
   *               If len > size of element at head of queue, copy # of bytes contained in head element & ignore any extra space
   */
  printk("SYS_DEQUEUE421() called\n");
  struct msgQueue *temp;

  if(len < 0) {  /* Check if bytes allocated is negative */
    printk("ERROR: 'len' is negative\n");
    return -EINVAL;
  }

  down(&lock);
  temp = list_entry(myQueueList.next, struct msgQueue, list);  /* Grab first item in queue */

  if(queue_size == 0) {  /* Check if queue is empty */
    printk("ERROR: Queue is empty\n");
    up(&lock);
    return -ENODATA;
  }
  up(&lock);

  if(len < temp->msgLen) {  /* Check if user tried to allocate too little space */
    printk("ERROR: Not enough space to store message\n");
    return -E2BIG;
  }
  if(len >= temp->msgLen) {  /* Sets allocated space to size of message if space is too large */
    len = temp->msgLen;
  }
  if(access_ok(VERIFY_READ, data, len)) {  /* Check if user-space is accessible */
    long to_userSpace = copy_to_user(data, (void*)(temp->m_message), len);  /* Copy data to user-space */
    if(to_userSpace != 0) {  /* Extra checking to ensure the space actually is accessible */
      printk("ERROR: Can't access user-space\n");
      return -EFAULT;
    }
  }

  down(&lock);
  list_del(myQueueList.next);  /* Delete entry and free allocated space */
  queue_size--;  
  up(&lock);

  kfree(temp);  /* Don't need to lock the free(), so not included in the lock */

  return 0;
}

asmlinkage long sys_peekLen421(void) {
  /*  Description: Retrieve length of element at head of queue
   *  Parameters: None
   *  Return: Success => size of data in element at head of queue (in bytes)
   *          Error => -ENODATA (empty queue)
   *  Other Notes:
   */
  printk("SYS_PEEKLEN421() called\n");
  struct msgQueue *temp;

  down(&lock);
  if(queue_size == 0) {  /* Check if queue is empty */
    printk("ERROR: Queue is empty\n");
    up(&lock);  /* Need to release before or else starvation can happen */
    return -ENODATA;
  }
  
  temp = list_entry(myQueueList.next, struct msgQueue, list);  /* Get first element's size, in bytes, and return it */
  up(&lock);

  return temp->msgLen;
}

asmlinkage long sys_queueLen421() {
  /*  Description: Retrive # of elements currently in queue
   *  Parameters: 
   *  Return: Success => # of elements in queue
   *  Other Notes:
   */
  printk("SYS_QUEUELEN421() called\n");
  int temp_size;

  down(&lock);
  temp_size = queue_size;
  up(&lock);

  return temp_size;
}

asmlinkage long sys_clearQueue421() {
  /*  Description: Deletes all messages currently in queue, freeing all memory & releasing stored elements
   *  Parameters: 
   *  Return: 0 => success
   *  Other Notes:
   */
  printk("SYS_CLEARQUEUE421() called\n");
  struct msgQueue *loop_cursor, *temp_stg;  /* Create variables to keep track of spot while iterating */

  down(&lock);
  /* Iterate through queue and delete then free each item */
  list_for_each_entry_safe(loop_cursor, temp_stg, &myQueueList, list) {
    list_del(&(loop_cursor->list));
    kfree(loop_cursor);
    queue_size--;  /* Be sure to keep track of queue's size */
  }
  up(&lock);

  return 0;
}
