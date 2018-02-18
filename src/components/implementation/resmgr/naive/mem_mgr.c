#include <cos_component.h>
#include <cos_kernel_api.h>
#include <cos_defkernel_api.h>
#include <sl.h>
#include <memmgr.h>
#include "res_info.h"

vaddr_t
memmgr_heap_page_allocn_intern(spdid_t cur, unsigned int num, int u1, int u2, int *u3, int *u4)
{
	struct cos_compinfo *res_ci = cos_compinfo_get(cos_defcompinfo_curr_get());
	struct res_comp_info *cur_rci = res_info_comp_find(cur);
	struct cos_compinfo *cur_ci = res_info_ci(cur_rci);
	vaddr_t src_pg, dst_pg, first_pg;
	unsigned int off = 0;

	assert(cur_rci && res_info_init_check(cur_rci));
	assert(cur_ci);

	src_pg = (vaddr_t)cos_page_bump_allocn(res_ci, num * PAGE_SIZE);
	assert(src_pg);

	while (off < num) {
		dst_pg = cos_mem_alias(cur_ci, res_ci, src_pg + (off * PAGE_SIZE));
		assert(dst_pg);

		if (!off) first_pg = dst_pg;
		off++;
	}
	
	return first_pg;
}

int
memmgr_shared_page_allocn_intern(spdid_t cur, int num, int u1, int u2, vaddr_t *pgaddr, int *u3)
{
	struct res_comp_info *cur_rci = res_info_comp_find(cur);
	struct res_shmem_info *cur_shi  = res_info_shmem_info(cur_rci);
	int shmidx = -1;

	assert(cur_rci && res_info_init_check(cur_rci));
	assert(cur_shi);
	/*
	 * TODO: perhaps allocator should tell how can access this shared memory region
	 *       so that the mapping phase can check if a component is allowed to access that region!
	 */
	shmidx = res_shmem_region_alloc(cur_shi, num);
	if (shmidx < 0) goto done;

	*pgaddr = res_shmem_region_comp_vaddr(cur_shi, shmidx);
done:
	return shmidx;
}

int
memmgr_shared_page_map_range_intern(spdid_t cur, spdid_t src, int src_idx, u32_t range, vaddr_t *pgaddr, int *u1)
{
	struct res_comp_info *cur_rci = res_info_comp_find(cur);
	struct res_shmem_info *cur_shi  = res_info_shmem_info(cur_rci);
	struct res_comp_info *src_rci = res_info_comp_find(src);
	struct res_shmem_info *src_shi  = res_info_shmem_info(src_rci);
	int shmidx = -1;
	int off = (range >> 16), num_pages = (range << 16) >> 16;

	assert(cur != src);
	assert(cur_rci && res_info_init_check(cur_rci));
	assert(src_rci && res_info_init_check(src_rci));
	assert(cur_shi);
	assert(src_shi);
	/* TODO: can cur access src shared mem region? */

	shmidx = res_shmem_region_map(cur_shi, src_shi, src_idx, off, num_pages);
	if (shmidx < 0) goto done;

	*pgaddr = res_shmem_region_comp_vaddr(cur_shi, shmidx);
done:
	return shmidx;
}

vaddr_t
memmgr_shared_page_vaddr_intern(spdid_t cur, int cur_idx, int u1, int u2, int *u3, int *u4)
{
	struct res_comp_info *cur_rci = res_info_comp_find(cur);
	struct res_shmem_info *cur_shi  = res_info_shmem_info(cur_rci);

	assert(cur_rci && res_info_init_check(cur_rci));
	assert(cur_shi);

	return res_shmem_region_comp_vaddr(cur_shi, cur_idx);
}
