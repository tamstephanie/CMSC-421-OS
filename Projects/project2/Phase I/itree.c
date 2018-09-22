/*
 *  Modified from minix fs for use as exercise for UMBC CMSC421 OS course
 *      by John Y. Park
 * THIS FILE WAS PRETTY MUCH TOTALLY REWRITTEN!
 *
 *
 * HINT-STUB: ONE OF THE TWO MAJOR FILES THAT NEED UPDATING
 */

#include <linux/buffer_head.h>
#include "fs421.h"

enum {DIRECT = 10, DEPTH = 1};	/* No longer have triple indirect --jyp */

typedef u32 block_t;	/* 32 bit, host order */

static inline unsigned long block_to_cpu(block_t n)
{
	return n;
}

static inline block_t cpu_to_block(unsigned long n)
{
	return n;
}

static inline block_t *i_data(struct inode *inode)
{
	return (block_t *)fs421_i(inode)->i2_data;
}

/*
 * block_to_path():
 *   Maps a file-relative block number to a inode chunk level,
 *   and the offset within that chunk.
 *   Kept name of original, since what the new function does is somewhat
 *   similar to the old function, albeit simpler.
 *
 * Inputs:
 *   inode -- inode to operate on; just used to get sb info.
 *   block -- the file-relative block number to compute the path for
 *
 * Outputs:
 *   offset -- block offset relative to chunk at returned level
 * Return: level of chunk ptr, -1 on error
 */
static int block_to_path(struct inode * inode, long block, int *offset)
{
	char b[BDEVNAME_SIZE];
	struct super_block *sb = inode->i_sb;
	int level;

	if (block < 0) {
		printk("FS421-fs: block_to_path: block %ld < 0 on dev %s\n",
			block, bdevname(sb->s_bdev, b));
		return -1;
	} else if ((u64)block * (u64)sb->s_blocksize >=
			fs421_sb(sb)->s_max_size) {
		if (printk_ratelimit())
			printk("FS421-fs: block_to_path: "
			       "block %ld too big on dev %s\n",
				block, bdevname(sb->s_bdev, b));
		return -1;
	}

	/*
	 * HINT-STUB:
	 * NEED TO ADD CODE HERE TO COMPUTE level AND offset, BASED
	 * ON PARAM block
	 * E.g.s:
	 *   if request is for block# 0, return level=0, offset=0
	 *   if request is for block# 1, return level=1, offset=0
	 *   if request is for block# 2, return level=1, offset=1
	 *   if request is for block# 3, return level=2, offset=0
	 *    ... and so forth.
	 *
	 * THIS SHOULD BE ABOUT 4-5 LINES OF CODE
	 */
	int block_num = block + 1;
	int count = 0;
	while (block_num > 2) {
	  block_num = block_num / 2;
	  count += 1;
	}
	level = count;

	*offset = block_num - ((2 << level) - 1);  /* Return by reference */
	return level;
}

/*
 * fs421_get_block():
 *   Maps a file block offset to a disk zone# and fetches it. Works w/chunks.
 *   Substantially simpler because we don't use indirect blocks.
 * Inputs:
 *   inode -- inode to fetch a data block for
 *   block -- block offset in file to fetch
 *   create -- flag to indicate we should grow file if request is beyond EOF
 * Outputs:
 *   bh -- buffer_head that is configured to point to data block in buffer
 *     cache.
 * Return:
 *   0 if all ok; else -errno
 */

int fs421_get_block(struct inode * inode, sector_t block,
		       struct buffer_head *bh, int create)
{
	int err = 0;
	int offset;
	int l;
	int new_block = 0;
	block_t *idata = i_data(inode);
	int level = block_to_path(inode, block, &offset);

	if (level < 0)
		return -EIO;
	else if (idata[level] == 0) {
		if (!create)
			return 0;
		/*
		printk("FS421-fs: get_block(create==1): l=%d, off=%d\n",
		       level, offset);
		*/
		/* set flag to indicate we had to allocate a new block: */
		new_block = 1;

		/*
		 * HINT-STUB:
		 * NEED TO ADD CODE HERE TO EXTEND FILE IF NECESSARY:
		 * You need to:
		 * 1) See what level you have chunks allocated up to;
		 *    (look for first zero idata[] entry--cannot use inode's
		 *    size field, as it was already modified).
		 * 2) Allocate chunks for all empty levels up to requested
		 *    level (in "level", computed above)
		 * 3) For each chunk, put starting block address into
		 *    appropriate idata[] slot
		 * 4) IMPORTANT: keep track of what you are allocating:
		 *    on failure, MUST FREE UP WHAT YOU ADDED HERE SO FAR
		 *    DURING THIS CALL (but not what was already there before)
		 *
		 * You will need to call:
		 *   fs421_new_block(inode, size)
		 * and possibly:
		 *   fs421_free_block(inode, blocknum, size)
		 *
		 * (you will be writing these, too, in bitmap.c)
		 *
		 * Don't forget to set "err" if there was a problem
		 *
		 * THIS SHOULD BE ABOUT 8-10 LINES OF CODE
		 */
		


		err = -ENOSPC; // set to a negative errno (-ENOSPC makes most sense)
	}

	if (!err) {
		/* STUB: really should zero out all of the blocks in all
		 * the chunks we allocated in this call */
		if (new_block) {
			set_buffer_new(bh);
		}
		map_bh(bh, inode->i_sb, idata[level] + offset);
	}
	return err;
}

/*
 * free_data():
 *   This function frees up chunks at levels beyond the end of a file.
 *
 * Inputs:
 *   inode -- inode to free up data blocks on
 *   trunc_start -- the level to start freeing up chunks from
 */
static inline void free_data(struct inode *inode, int trunc_start)
{
	block_t *idata = i_data(inode);
	unsigned long nr;

	/*
	 * This helper function is called by fs421_truncate to free
	 * up soon-to-be-unused chunks.  The parameter "trunc_start"
	 * specifies the level at which to start freeing up chunks,
	 * which are stored in idata[] (fetched above).
	 * We have no indication of how many we were using previously,
	 * so we will just have to loop over all idata[] entries
	 * IMPORTANT: you MUST set the idata[] entries to 0 as you free
	 * them up, since other functions (and this one, too) depend on
	 * that!
	 *
	 * To free up the chunks, you should call:
	 *   fs421_free_block(inode, blocknum, size)
	 *
	 * (you will be writing this, too, in bitmap.c)
	 */
	while (trunc_start < DIRECT) {
		nr = block_to_cpu(idata[trunc_start]);
		if (nr) {
			idata[trunc_start] = 0;
			fs421_free_block(inode, nr, trunc_start);
		}
		trunc_start++;
	}
}

/*
 * fs421_truncate():
 *   This function is called for doing file truncation, either by file
 *   "re-creation", or truncation syscall.  Rework of existing function.
 * Inputs:
 *   inode -- inode to free up data blocks on
 *     note that inode->i_size has already been updated to indicate desired
 *     new endpoint (so this is our actual implicit arg)
 *
 */
void fs421_truncate(struct inode * inode)
{
	struct super_block *sb = inode->i_sb;
	int offset;
	int n, trunc_start;
	long iblock;

	if (!(S_ISREG(inode->i_mode) || S_ISDIR(inode->i_mode) || S_ISLNK(inode->i_mode)))
		return;

	/* Following gets addr of last block used, possibly -1 */
	iblock = ((inode->i_size + sb->s_blocksize -1) >> sb->s_blocksize_bits)
		- 1;

	block_truncate_page(inode->i_mapping, inode->i_size, fs421_get_block);

	if (iblock < 0)
		trunc_start = 0;
	else {
		n = block_to_path(inode, iblock, &offset);
		if (n < 0) /* block_to_path() already did kprint() */
			return;
		trunc_start = n + 1;
	}
	free_data(inode, trunc_start);
	inode->i_mtime = inode->i_ctime = CURRENT_TIME_SEC;
	mark_inode_dirty(inode);
}

/*
 * fs421_release():  NEW FUNCTION
 *   Hook for being called on last close of a file (this is best time
 *   for truncating any excess allocations).
 * Removed because this was made optional
 */
int fs421_release(struct inode *inode, struct file *file)
{
	printk("FS421-fs: release: called on i=%u, f=%u\n", inode, file);
	/* Just for yucks, check that file->f_count is actually 0 */
	return 0; /* doc says this value is ignored by VFS */
}

/*
 * fs421_blocks():
 *   Computes the actual number of blocks that would be used for a file
 *   of a given size.  This takes overhead into account; in original MINIX,
 *   we had to account for indirect blocks-of-pointers; for FS421, we have
 *   to count any overallocations due to the binary buddy system.
 *   Only used by the stat syscall, through fs421_getattr()
 * Inputs:
 *   size -- size, in BYTES, of our hypothetical file
 *   sd -- superblock w/fs info to use in our calculations
 */
unsigned fs421_blocks(loff_t size, struct super_block *sb)
{
	int bs_diff_bits = BLOCK_SIZE_BITS - sb->s_blocksize_bits;
	unsigned blocks, res, nlevels;

	if (size == 0)
		return 0;
	blocks = (size + sb->s_blocksize - 1) >> (sb->s_blocksize_bits);
	nlevels = fls(blocks);
	res = (1 << nlevels) - 1;
	/* Students might not do following, to change fs421 blocks to Linux
	 * blocks--warn, but no deduction */
	if (bs_diff_bits > 0)
		res = DIV_ROUND_UP(res, 1 << bs_diff_bits);
	else
		res <<= -bs_diff_bits;
	return res;
}
