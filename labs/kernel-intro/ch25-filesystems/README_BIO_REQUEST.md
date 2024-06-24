
# Block I/0 requests

Linux Kernel version `6.6.14`

## Structure of the `bio` requests

- sector
- block
- segments

The main data structures for block I/O requests are:

- [struct request]: Represents a block I/O requests.
- [struct bio]: The fundamental block I/O unit.
- [struct bio_vec]: Vector of segments.
- [struct bvec_iter]: Helper class to iterate through the vector of segments.
- [struct req_iterator]: Helper class to iterate through the list of `struct bio` objects.
- [struct page]: Every physical page is associated to a `struct page`.

### struct request

The `struct request` represents a block I/O requests.

```
struct request {
	struct request_queue *q;
	struct blk_mq_ctx *mq_ctx;
	struct blk_mq_hw_ctx *mq_hctx;

	unsigned int timeout;

	/* the following two fields are internal, NEVER access directly */
	unsigned int __data_len;	/* total data len */
	sector_t __sector;		/* sector cursor */

	struct bio *bio;
	struct bio *biotail;
	...
};
```

###  struct bio

The `struct bio` is the fundamental block I/O unit.

```
struct bio {
	struct bio		*bi_next;	/* request queue link */
	struct block_device	*bi_bdev;
	struct bvec_iter	bi_iter;

	blk_qc_t		bi_cookie;
	bio_end_io_t		*bi_end_io;
	void			*bi_private;

	unsigned short		bi_vcnt;	/* how many bio_vec's */
	unsigned short		bi_max_vecs;	/* max bvl_vecs we can hold */
	struct bio_vec		*bi_io_vec;	/* the actual vec list */
	...
	/*
	 * We can inline a number of vecs at the end of the bio, to avoid
	 * double allocations for a small number of bio_vecs. This member
	 * MUST obviously be kept at the very end of the bio.
	 */
	struct bio_vec		bi_inline_vecs[];
};
```

### struct bio_vec

Every `struct bio` contains a pointer to a vector of segments represented
by `struct bio_vec`.

```
struct bio_vec {
	struct page	*bv_page;
	unsigned int	bv_len;
	unsigned int	bv_offset;
};
```

### struct bvec_iter

The `struct bvec_iter` is a helper class to iterate through the vector
of segments.

```
struct bvec_iter {
	sector_t		bi_sector;	/* device address in 512 byte
						   sectors */
	unsigned int		bi_size;	/* residual I/O count */

	unsigned int		bi_idx;		/* current index into bvl_vec */

	unsigned int            bi_bvec_done;	/* number of bytes completed in
						   current bvec */
} __packed __aligned(4);
```

### struct req_iterator

The `struct req_iterator` is a helper class to iterate through the list of
`struct bio` objects.

```
struct req_iterator {
	struct bvec_iter iter;
	struct bio *bio;
};
```

### struct page

Every physical page in the system is associated to a `struct page`. 

```
struct page {
	unsigned long flags;		/* Atomic flags, some possibly
					 * updated asynchronously */
	union {
		pgoff_t index;		/* Our offset within mapping. */
		unsigned long share;	/* share count for fsdax */
	};
	union {		/* This union is 4 bytes in size. */
		/*
		 * If the page can be mapped to userspace, encodes the number
		 * of times this page is referenced by a page table.
		 */
		atomic_t _mapcount;

		/*
		 * If the page is neither PageSlab nor mappable to userspace,
		 * the value stored here may help determine what this page
		 * is used for.  See page-flags.h for a list of page types
		 * which are currently stored here.
		 */
		unsigned int page_type;
	};

	/* Usage count. *DO NOT USE DIRECTLY*. See page_ref.h */
	atomic_t _refcount;

	/*
	 * On machines where all RAM is mapped into kernel address space,
	 * we can simply calculate the virtual address. On machines with
	 * highmem some memory is mapped into kernel virtual memory
	 * dynamically, so we need a place to store that address.
	 * Note that this field could be 16 bits on x86 ... ;)
	 *
	 * Architectures with slow multiplication can define
	 * WANT_PAGE_VIRTUAL in asm/page.h
	 */
#if defined(WANT_PAGE_VIRTUAL)
	void *virtual;			/* Kernel virtual address (NULL if
					   not kmapped, ie. highmem) */
#endif /* WANT_PAGE_VIRTUAL */

} _struct_page_alignment;
```

## Structure of a block I/O request

```
   struct request                       struct req_iterator
   ______________                   __________________________
   |            |                   | struct bvec_iter iter; |
   |    *bio    |                   | struct bio *bio;       |
   |____________|                   |________________________|
          |
          |
          |
      struct bio          struct bio         struct bio          struct bio
    ______________      ______________      ______________      ______________
    |            |      |            |      |            |      |            |
    |  *bi_next  |______|  *bi_next  |______|  *bi_next  |______|  *bi_next  |______ NULL
    |   bi_iter  |      |   bi_iter  |      |   bi_iter  |      |   bi_iter  |
    | *bi_io_vec |      | *bi_io_vec |      | *bi_io_vec |      | *bi_io_vec |
    |____________|      |____________|      |____________|      |____________|
           |                   |                   |                   |
           |                   |                   |                   |
           |                .....                .....                 |
   struct bio_vec[]                                            struct bio_vec[]
    ______________     ---------                               ______________     ---------
    |            |_____| index | struct page                   |            |_____| index | struct page
    |  *bv_page  |     ---------                               |  *bv_page  |     ---------
    |____________|     ---------                               |____________|     ---------    
    |            |_____| index | struct page                   |            |_____| index | struct page
    |  *bv_page  |     ---------                               |  *bv_page  |     ---------
    |____________|     ---------                               |____________|     ---------
    |            |_____| index | struct page                   |            |_____| index | struct page
    |  *bv_page  |     ---------                               |  *bv_page  |     ---------
    |____________|                                             |____________|     ---------
                                                               |            |_____| index | struct page
                                                               |  *bv_page  |     ---------
                                                               |____________|     ---------
                                                               |            |_____| index | struct page
                                                               |  *bv_page  |     ---------
                                                               |____________|     ---------
                                                               |            |_____| index | struct page
                                                               |  *bv_page  |     ---------
                                                               |____________|

```

## How to iterate segments

There are several ways to iterate segments, two of them are:

- `for` loop to iterate segments.
- macro [rq_for_each_segment](https://elixir.bootlin.com/linux/v6.9.6/source/include/linux/blk-mq.h#L1016) to iterate segments.

### `for` loop to iterate segments

```
#include <linux/bio.h>
#include <linux/blk-mq.h>
#include <linux/blk_types.h>
#include <linux/bvec.h>
#include <linux/highmem.h>
#include <linux/highmem-internal.h>

int process_blk_request(struct request *rq) {
	struct bio_vec bvl;
	struct req_iterator iter;

	if (rq->bio)
		for (iter.bio = rq->bio; iter.bio; iter.bio = iter.bio->bi_next)
			for (iter.iter = ((iter.bio)->bi_iter);
	     			(iter.iter).bi_size &&
					((bvl = ((struct bio_vec) {						
						.bv_page = bvec_iter_page(((iter.bio)->bi_io_vec), (iter.iter)),
						.bv_len	= bvec_iter_len(((iter.bio)->bi_io_vec), (iter.iter)),
						.bv_offset	= bvec_iter_offset(((iter.bio)->bi_io_vec), (iter.iter)),
					}), 1);
					// Advaces iter->bi_idx++ if bv_len equals to copy bytes;
					bio_advance_iter_single((iter.bio), &(iter.iter), (bvl).bv_len)) {

				sector_t sector = iter.iter.bi_sector;
				unsigned long disk_offset = (sector * SECTOR_SIZE);
				unsigned long buf_offset = bvl.bv_offset;
				size_t len_in_bytes = bvl.bv_len;
				char *buffer = kmap_local_page(bvl.bv_page);
				int dir = bio_data_dir(iter.bio);

				// Lock here

				switch(dir) {
				case READ;
					// Read from block device.
					break;
				case WRITE:
					// Write to block device.
					break;
				}; 

				kunmap_local(buffer);

				// Unlock here
			}

	return BLK_STS_OK;
}
```

### Macro [rq_for_each_segment](https://elixir.bootlin.com/linux/v6.9.6/source/include/linux/blk-mq.h#L1016) to iterate segments

```
#include <linux/bio.h>
#include <linux/blk-mq.h>
#include <linux/blk_types.h>
#include <linux/bvec.h>
#include <linux/highmem.h>
#include <linux/highmem-internal.h>

int process_blk_request(struct request *rq) {
	struct bio_vec bvl;
	struct req_iterator iter;

	rq_for_each_segment(bvl, rq, iter) {
		sector_t sector = iter.iter.bi_sector;
		unsigned long disk_offset = (sector * SECTOR_SIZE);
		unsigned long buf_offset = bvl.bv_offset;
		size_t len_in_bytes = bvl.bv_len;
		char *buffer = kmap_local_page(bvl.bv_page);
		int dir = bio_data_dir(iter.bio);

		// Lock here

		switch(dir) {
		case READ;
		
		break;
		case WRITE:

		break;
		}; 

		kunmap_local(buffer);
		
		// Unlock here
	}

	return BLK_STS_OK;
}

```

[struct request]: https://elixir.bootlin.com/linux/v6.9.6/source/include/linux/blk-mq.h#L79
[struct bio]: https://elixir.bootlin.com/linux/v6.9.6/source/include/linux/blk_types.h#L223
[struct bio_vec]: https://elixir.bootlin.com/linux/v6.9.6/source/include/linux/bvec.h#L31
[struct bvec_iter]: https://elixir.bootlin.com/linux/v6.9.6/source/include/linux/bvec.h#L77
[struct page]: https://elixir.bootlin.com/linux/v6.9.6/source/include/linux/mm_types.h#L74
[struct req_iterator]: https://elixir.bootlin.com/linux/v6.9.6/source/include/linux/blk-mq.h#L1007




