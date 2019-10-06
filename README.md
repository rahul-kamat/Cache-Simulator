# Cache-Simulator
One Level (L1) Cache Simulator with Least Recently Used (LRU) Replacement Algo

Usage Interface:
  ./cacheSimulator <cache size> 
  
  Where:
    • <cache size> is the total size of the cache in bytes. This number should be a power of 2.
    • <associativity> is one of:
      – direct – simulate a direct mapped cache
      – assoc – simulate a fully associative cache
      – assoc:n – simulate an n-way associative cache. n will be a power of 2
    • <cache policy> is:
      - lru - evict the block accessed least recently in the set
    • <block size> is a power of 2 integer that specifies the size of the cache block in bytes.
    • <trace file> is the name of the trace file.


Cache Prefetcher:
  - Technique to increase the spatial locality of the caches beyond the cache line.
  - Brings in the block corresponding to the access but also prefetches one adjacent block, which will result in one extra memory read.
  - The prefetcher is activated only on misses. If the prefetched block is already in the cache, it does not issue a memory read.


Output:
- Program prints the number of memory reads (per cache block), memory writes (per cache block), cache hits, and cache misses for the    
  normal cache and the cache with prefetcher.
  
Example Run:
$ ./cacheSimulator 32 assoc:2 lru 4 smallTraceFile.txt

no-prefetch
Memory reads: 336
Memory writes: 334
Cache hits: 664
Cache misses: 336
with-prefetch
Memory reads: 336
Memory writes: 334
Cache hits: 832
Cache misses: 168
