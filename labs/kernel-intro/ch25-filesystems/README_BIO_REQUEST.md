
# Block I/0 requests

Linux Kernel version `6.6.14`

## Structure of the `bio` requests

- sector
- block
- segments


## How to iterate segments

There are several ways to iterate segments, two of them are:

- `for` loop to iterate segments
- Macro `rq_for_each_segment` to iterate segments

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

### Macro `rq_for_each_segment` to iterate segments

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



