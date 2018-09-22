/*
 *  linux/fs/fs421/bitmap.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * Modified for 680x0 by Hamish Macdonald
 * Fixed for 680x0 by Andreas Schwab
 *
 *  Modified from minix fs for use as exercise for UMBC CMSC421 OS course
 *      by John Y. Park
 */

/* bitmap.c contains the code that handles the inode and block bitmaps */

/*
 * Additional notes:
 * The inode bitmap starts from 0, i.e., inode x is represented by bit x.
 * However, the actual inode array starts from index 1, i.e., inode 1 is
 * inodes[0] in the first block of inodes.
 * Therefore, bit 0 in the inode bitmap is always set, because it doesn't
 * correspond to any actual inode structure.  --jyp
 */


#include "fs421.h"
#include <linux/buffer_head.h>
#include <linux/bitops.h>
#include <linux/sched.h>

static DEFINE_SPINLOCK(bitmap_lock);

/*
 * General comments:
 *   Bitmap consists of blocks filled with 32bit words (you can walk it as
 *   8-bit or 16-bit "words" if desired, but note that FS421 pads to 32-bit
 *   boundaries).  Note: bit set == busy, bit clear == free
 */

/*
 * HINT-STUB:
 *
 * fs421_init_zmap_offsets():  NEW FUNCTION
 * Supplemental code to make dealing with bitmaps faster, by calculating
 * the size of each level of the Binary Buddy bitmap, and using that to
 * generate a precomputed cache of the offsets for each level of the
 * bitmaps, accounting for the 32-bit word-aligned padding for each level.
 * Note that this generates one extra "offset" value, i.e., where the
 * "n+1-th level" would start--this lets us calculate the size of the
 * nth level as a general case.
 * This table of offsets is used to conveniently go directly to any
 * desired level of the bitmap.
 * NB: the table is in units of bits.
 * Return: total number of bits across all the levels of BB bitmap hierarchy--
 *   used by fs421_fill_super() at mount time to validate that the
 *   bitmap size indicated in the superblock is of sufficient size.
 */
unsigned long fs421_init_zmap_offsets(struct fs421_sb_info *sbi) {
  __u32 bits = sbi->s_nzones - sbi->s_firstdatazone;
  __u32 total_bits = 0;
  int level = 0;
  
  sbi->s_zmap_offset[0] = 0;
  while (bits) {
    total_bits += DIV_ROUND_UP(bits, 32) * 32;
    sbi->s_zmap_offset[++level] = total_bits;
    bits /= 2;
  }
  sbi->s_zmap_nlevels = level;
  return total_bits;
}

/*
 * get_bh_and_bit():  NEW FUNCTION
 */
static inline void get_bh_and_bit(struct super_block *sb, int level, __u32 zchunk,
				  struct buffer_head **bh, __u32 *bit) {
  struct fs421_sb_info *sbi = fs421_sb(sb);
  int k = sb->s_blocksize_bits + 3;
  
  /* Calculate the absolute position of the bit we are
   * interested in, from start of entire bitmap set */
  __u32 abs_bit = sbi->s_zmap_offset[level] + (zchunk >> level);
  *bh = sbi->s_zmap[abs_bit >> k];
  *bit = abs_bit & ((1<<k) - 1);
}

/*
 * Tallies the number of free (zero) bits at one level of the BB bitmap set.
 * Param "start" is the bit offset in bitmap hierarchy for requested level
 *
 * HINT-STUB:
 * You should study this function very carefully--it provides a wealth
 * of information that you can reuse in other bitmap functions,
 * particularly how you span multiple blocks, while dealing with the
 * Linux buffer cache.
 */
static __u32 count_free(struct buffer_head *map[], unsigned blocksize,
			__u32 start, __u32 num_bits)
{
  struct buffer_head **curr_map;
  __u32 bits_per_zone = 8 * blocksize;
  __u32 bits_left = num_bits;
  unsigned start_in_blk, bits_to_test;
  unsigned words;
  __u32 sum = 0;
  __u16 *p;
  
  /* Should following also test if num_bits % 32? */
  if (start % 32) {
    printk("Non-word-aligned bitmap start or len: %u, %u\n",
	   start, num_bits);
    return 0;
  }
  
  curr_map = map + start / bits_per_zone;
  start_in_blk = start % bits_per_zone;
  
  do {
    bits_to_test = bits_per_zone - start_in_blk;
    if (bits_to_test > bits_left) {
      bits_to_test = bits_left;
    }
    /* Now, convert to words */
    words = bits_to_test / 16;
    p = (__u16 *)(*curr_map++)->b_data + start_in_blk / 16;
    while (words--)
      sum += 16 - hweight16(*p++);
    bits_left -= bits_to_test;
    start_in_blk = 0;	/* for all subsequent blocks */
  } while (bits_left);
  
  return sum;
}

/*
 * HINT-STUB:
 * This is a required function, which you must write most of.
 *
 * It receives a request to free up a chunk of the specified size
 * (units is log2(num_blocks_to_free)) starting at the specified disk
 * block number, and puts it back into the binary buddy bitmap.
 * Note it must handle mapping disk block# to bitmap level and position,
 * and then also handle coalescing.
 *
 * IMPORTANT: be very consistent in the units you are passing around: is it
 * bits, bytes, words, or blocks??
 */
void fs421_free_block(struct inode *inode, unsigned long block, unsigned size_log2)
{
  struct super_block *sb = inode->i_sb;
  struct fs421_sb_info *sbi = fs421_sb(sb);
  struct buffer_head *bh;
  __u32 zchunk, bit, bbit;
  int level;
  
#ifdef NEVERDEF
  CHECK THAT BLOCK IS IN LEGAL RANGE FOR DISK;
  
  CHECK THAT BLOCK IS PROPERLY ALIGNED FOR ITS BINARY SIZE;
  
  
  spin_lock(&bitmap_lock);  /* lEAVE THIS ALONE! */
  
  CHECK THAT BIT IS NOT ALREADY FREE AT LEVEL;
  
  FREE UP BIT AT LEVEL;
  
  IMPORTANT--EVERY TIME YOU CHANGE A BIT, MUST USE FOLLOWING
    ON CONTAINING BUFFER''S buffer_head:
    mark_buffer_dirty(bh);
  
  CHECK BUDDY FOR COALESCING;
  
  COALESCE IF NECESSARY;
  
  THIS MIGHT CAUSE CASCADE AT UPPER LEVELS;
  
  spin_unlock(&bitmap_lock);  /* lEAVE THIS ALONE! */
#endif

}

/*
 * HINT-STUB:
 * This is a required function, which you must write most of.
 *
 * It implements the allocation side of the binary buddy allocator.
 * It receives a request for a chunk of a specified size
 * (units is log2(num_blocks_to_free)), and tries to allocate it from
 * the binary buddy bitmap.
 * Note it must handle mapping bitmap level and position to  disk block#.
 *
 * Return: disk-relative block# of first disk block of contiguous chunk
 *   of size 2**size_log2,
 *   0 on error (e.g., no more space big enough)
 *
 * IMPORTANT: be very consistent in the units you are working with at any
 * given time: is it bits, bytes, words, or blocks??
 */
int fs421_new_block(struct inode * inode, unsigned size_log2)
{
  struct super_block *sb = inode->i_sb;
  struct fs421_sb_info *sbi = fs421_sb(sb);
  struct buffer_head *bh;
  int level, i, emptyBit_index, level_bits;
  __u32 bit, zchunk;
  __u32 *level_word, *higher_level_bit;
  
  spin_lock(&bitmap_lock);
  
  /*CALCULATE LEVEL BASED ON SIZE: STORE IN size_log2;*/
  level = size_log2;
  zchunk = 0;  /* will eventually change */

  printk("bitmap.c::217: level = %d\n", level);

  /* Scan requested level; if empty bit not found in that level, escalate & keep searching */
  for(i = level; i < sbi->s_zmap_nlevels; i++) {
    level_bits = sbi->s_zmap_offset[i+1] - sbi->s_zmap_offset[i];
    printk("bitmap.c::224: level = %d; level_bits = %d\n", level, level_bits);

    get_bh_and_bit(sb, level, zchunk, &bh, &bit);  /* Scan level */
    level_word = (__u32*) bh->b_data + bit / 32;  /* Get the word within the level and shift by each word */
    printk("bitmap.c::227-228: bit = %d; word = %d\n", bit, *level_word);
    
    emptyBit_index = find_first_zero_bit_le(level_word, level_bits);  /* Find the first empty bit */
    printk("bitmap.c::229: emptyBit_index = %d\n", emptyBit_index);

    if(emptyBit_index < level_bits) {
      break;  /* Break out of loop when the first zero bit is found */
    }
  }
  
  /* Check if request level is greater than complete levels
   * Return 0 on error--no space/no empty bit found
   */
  if(level >= sbi->s_zmap_nlevels) {
    printk("bitmap.c::fs421_new_block():ERROR: NO SPACE BIG ENOUGH\n");
    return 0;
  }
  
  /* If empty bit found at higher level, traverse down to requested level & split accordingly */
  for( ; i > level; i--) {  /* i starts at level where empty bit was found */
    higher_level_bit = (__u32*) bh->b_data + sbi->s_zmap_offset[i] / 32;
    printk("bitmap.c::249: higher_level_bit = %d\n", *higher_level_bit);

    /* Set and clear bits accordingly, and mark buffer as dirty, since it's been changed */
    set_bit_le(emptyBit_index, higher_level_bit);  /* Set empty bit to 1 */
    mark_buffer_dirty(bh);

    /* Move down and change the other bits accordingly */
    emptyBit_index *= 2;
    clear_bit_le(emptyBit_index + 1, higher_level_bit);  /* Set 2nd item in pair to 0 */
    mark_buffer_dirty(bh);
  }
  
  spin_unlock(&bitmap_lock);
  
  return (emptyBit_index >> level);  /*RETURN DISK-RELATIVE OFFSET OF CHUNK, OR 0 FOR ERROR;*/
}


unsigned long fs421_count_free_blocks(struct super_block *sb)
{
  __u32 sum = 0;
  struct fs421_sb_info *sbi = fs421_sb(sb);
  __u32 *offset = sbi->s_zmap_offset;
  int i;
  
  
  for (i = 0; i < sbi->s_zmap_nlevels; i++) {
    /* count_free() uses a bitcount for use on inode bitmap,
     * so keep it that way
     */
    sum += count_free(sbi->s_zmap, sb->s_blocksize,
		      offset[i], offset[i + 1] - offset[i])
      << i;
  }
  
  return (sum << sbi->s_log_zone_size);
}

/*
 * Was originally "struct minix2_inode minix_V2_raw_inode()" --jyp
 */

struct fs421_inode *
fs421_raw_inode(struct super_block *sb, ino_t ino, struct buffer_head **bh)
{
  int block;
  struct fs421_sb_info *sbi = fs421_sb(sb);
  struct fs421_inode *p;
  int fs421_inodes_per_block = sb->s_blocksize / sizeof(struct fs421_inode);
  
  *bh = NULL;
  if (!ino || ino > sbi->s_ninodes) {
    printk("Bad inode number on dev %s: %ld is out of range\n",
	   sb->s_id, (long)ino);
    return NULL;
  }
  ino--;
  block = 2 + sbi->s_imap_blocks + sbi->s_zmap_blocks +
    ino / fs421_inodes_per_block;
  *bh = sb_bread(sb, block);
  if (!*bh) {
    printk("Unable to read inode block\n");
    return NULL;
  }
  p = (void *)(*bh)->b_data;
  return p + ino % fs421_inodes_per_block;
}

/* Clear the link count and mode of a deleted inode on disk. */

static void fs421_clear_inode(struct inode *inode)
{
  struct buffer_head *bh = NULL;
  
  /* Following was formerly V1|V2-specific */
  {
    struct fs421_inode *raw_inode;
    raw_inode = fs421_raw_inode(inode->i_sb, inode->i_ino, &bh);
    if (raw_inode) {
      raw_inode->i_nlinks = 0;
      raw_inode->i_mode = 0;
    }
  }
  if (bh) {
    mark_buffer_dirty(bh);
    brelse (bh);
  }
}

void fs421_free_inode(struct inode * inode)
{
  struct super_block *sb = inode->i_sb;
  struct fs421_sb_info *sbi = fs421_sb(inode->i_sb);
  struct buffer_head *bh;
  int k = sb->s_blocksize_bits + 3;
  unsigned long ino, bit;
  
  ino = inode->i_ino;
  if (ino < 1 || ino > sbi->s_ninodes) {
    printk("fs421_free_inode: inode 0 or nonexistent inode\n");
    return;
  }
  bit = ino & ((1<<k) - 1);
  ino >>= k;
  if (ino >= sbi->s_imap_blocks) {
    printk("fs421_free_inode: nonexistent imap in superblock\n");
    return;
  }
  
  fs421_clear_inode(inode);	/* clear on-disk copy */
  
  bh = sbi->s_imap[ino];
  spin_lock(&bitmap_lock);
  if (!__test_and_clear_bit_le(bit, bh->b_data))
    printk("fs421_free_inode: bit %lu already cleared\n", bit);
  spin_unlock(&bitmap_lock);
  mark_buffer_dirty(bh);
}

struct inode *fs421_new_inode(const struct inode *dir, umode_t mode, int *error)
{
  struct super_block *sb = dir->i_sb;
  struct fs421_sb_info *sbi = fs421_sb(sb);
  struct inode *inode = new_inode(sb);
  struct buffer_head * bh;
  int bits_per_zone = 8 * sb->s_blocksize;
  unsigned long j;
  int i;
  
  if (!inode) {
    *error = -ENOMEM;
    return NULL;
  }
  j = bits_per_zone;
  bh = NULL;
  *error = -ENOSPC;
  spin_lock(&bitmap_lock);
  for (i = 0; i < sbi->s_imap_blocks; i++) {
    bh = sbi->s_imap[i];
    j = find_first_zero_bit_le(bh->b_data, bits_per_zone);
    if (j < bits_per_zone)
      break;
  }
  if (!bh || j >= bits_per_zone) {
    spin_unlock(&bitmap_lock);
    iput(inode);
    return NULL;
  }
  if (__test_and_set_bit_le(j, bh->b_data)) {	/* shouldn't happen */
    spin_unlock(&bitmap_lock);
    printk("fs421_new_inode: bit already set\n");
    iput(inode);
    return NULL;
  }
  spin_unlock(&bitmap_lock);
  mark_buffer_dirty(bh);
  j += i * bits_per_zone;
  if (!j || j > sbi->s_ninodes) {
    iput(inode);
    return NULL;
  }
  inode_init_owner(inode, dir, mode);
  inode->i_ino = j;
  inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME_SEC;
  inode->i_blocks = 0;
  memset(&fs421_i(inode)->i2_data, 0, sizeof(fs421_i(inode)->i2_data));
  insert_inode_hash(inode);
  mark_inode_dirty(inode);
  
  *error = 0;
  return inode;
}

unsigned long fs421_count_free_inodes(struct super_block *sb)
{
  struct fs421_sb_info *sbi = fs421_sb(sb);
  u32 bits = sbi->s_ninodes + 1;
  
  return count_free(sbi->s_imap, sb->s_blocksize, 0, bits);
}
