#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// MUSL  

static char* twobyte_strstr(const unsigned char* h, const unsigned char* n)
{
	uint16_t nw = n[0] << 8 | n[1], hw = h[0] << 8 | h[1];
	for (h++; *h && hw != nw; hw = hw << 8 | *++h);
	return *h ? (char*)h - 1 : 0;
}

static char* threebyte_strstr(const unsigned char* h, const unsigned char* n)
{
	uint32_t nw = (uint32_t)n[0] << 24 | n[1] << 16 | n[2] << 8;
	uint32_t hw = (uint32_t)h[0] << 24 | h[1] << 16 | h[2] << 8;
	for (h += 2; *h && hw != nw; hw = (hw | *++h) << 8);
	return *h ? (char*)h - 2 : 0;
}

static char* fourbyte_strstr(const unsigned char* h, const unsigned char* n)
{
	uint32_t nw = (uint32_t)n[0] << 24 | n[1] << 16 | n[2] << 8 | n[3];
	uint32_t hw = (uint32_t)h[0] << 24 | h[1] << 16 | h[2] << 8 | h[3];
	for (h += 3; *h && hw != nw; hw = hw << 8 | *++h);
	return *h ? (char*)h - 3 : 0;
}

static char* eightbyte_strstr(const char* h, const char* n) {
	uint64_t nq = (uint64_t)n[0] << 56 | (uint64_t)n[1] << 48 | (uint64_t)n[2] << 40 | (uint64_t)n[3] << 32 | (uint64_t)n[4] << 24 | (uint64_t)n[5] << 16 | (uint64_t)n[6] << 8 | n[7];
	uint64_t hq = (uint64_t)h[0] << 56 | (uint64_t)h[1] << 48 | (uint64_t)h[2] << 40 | (uint64_t)h[3] << 32 | (uint64_t)h[4] << 24 | (uint64_t)h[5] << 16 | (uint64_t)h[6] << 8 | h[7];
	for (h += 7; *h && hq != nq; hq = hq << 8 | *++h);
	return *h ? (char*)h - 7 : 0;
}

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define BITOP(a,b,op) \
 ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))

static char* twoway_strstr(const unsigned char* h, const unsigned char* n)
{
	const unsigned char* z;
	size_t l, ip, jp, k, p, ms, p0, mem, mem0;
	size_t byteset[32 / sizeof(size_t)] = { 0 };
	size_t shift[256];

	/* Computing length of needle and fill shift table */
	for (l = 0; n[l] && h[l]; l++)
		BITOP(byteset, n[l], |=), shift[n[l]] = l + 1;
	if (n[l]) return 0; /* hit the end of h */

	/* Compute maximal suffix */
	ip = -1; jp = 0; k = p = 1;
	while (jp + k < l) {
		if (n[ip + k] == n[jp + k]) {
			if (k == p) {
				jp += p;
				k = 1;
			}
			else k++;
		}
		else if (n[ip + k] > n[jp + k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		}
		else {
			ip = jp++;
			k = p = 1;
		}
	}
	ms = ip;
	p0 = p;

	/* And with the opposite comparison */
	ip = -1; jp = 0; k = p = 1;
	while (jp + k < l) {
		if (n[ip + k] == n[jp + k]) {
			if (k == p) {
				jp += p;
				k = 1;
			}
			else k++;
		}
		else if (n[ip + k] < n[jp + k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		}
		else {
			ip = jp++;
			k = p = 1;
		}
	}
	if (ip + 1 > ms + 1) ms = ip;
	else p = p0;

	/* Periodic needle? */
	if (memcmp(n, n + p, ms + 1)) {
		mem0 = 0;
		p = MAX(ms, l - ms - 1) + 1;
	}
	else mem0 = l - p;
	mem = 0;

	/* Initialize incremental end-of-haystack pointer */
	z = h;

	/* Search loop */
	for (;;) {
		/* Update incremental end-of-haystack pointer */
		if (z - h < l) {
			/* Fast estimate for MAX(l,63) */
			size_t grow = l | 63;
			const unsigned char* z2 = memchr(z, 0, grow);
			if (z2) {
				z = z2;
				if (z - h < l) return 0;
			}
			else z += grow;
		}

		/* Check last byte first; advance by shift on mismatch */
		if (BITOP(byteset, h[l - 1], &)) {
			k = l - shift[h[l - 1]];
			if (k) {
				if (k < mem) k = mem;
				h += k;
				mem = 0;
				continue;
			}
		}
		else {
			h += l;
			mem = 0;
			continue;
		}

		/* Compare right half */
		for (k = MAX(ms + 1, mem); n[k] && n[k] == h[k]; k++);
		if (n[k]) {
			h += k - ms;
			mem = 0;
			continue;
		}
		/* Compare left half */
		for (k = ms + 1; k > mem && n[k - 1] == h[k - 1]; k--);
		if (k <= mem) return (char*)h;
		h += p;
		mem = mem0;
	}
}

char* musl_strstr(const char* h, const char* n)
{
	/* Return immediately on empty needle */
	if (!n[0]) return (char*)h;

	/* Use faster algorithms for short needles */
	h = strchr(h, *n);
	if (!h || !n[1]) return (char*)h;
	if (!h[1]) return 0;
	if (!n[2]) return twobyte_strstr((void*)h, (void*)n);
	if (!h[2]) return 0;
	if (!n[3]) return threebyte_strstr((void*)h, (void*)n);
	if (!h[3]) return 0;
	if (!n[4]) return fourbyte_strstr((void*)h, (void*)n);
	if (!h[4]) return 0;
	if (!n[8]) return eightbyte_strstr(h, n);

	return twoway_strstr((void*)h, (void*)n);
}
