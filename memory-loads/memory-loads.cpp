/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2019 Andriy Berestovskyy <berestovskyy@gmail.com>
 *
 * Applied Benchmarks: Memory Loads
 */

#include <iostream>
#include "benchmark/benchmark.h"

auto constexpr operator"" _B(uint64_t n) { return n; }
auto constexpr operator"" _KB(uint64_t n) { return n * 1024; }
auto constexpr operator"" _M(uint64_t n) { return n * 1000 * 1000; }

/** Cache line len: 64 bytes for x86, 128 bytes for ARM */
const auto kCachelineSize = 64_B;
/** Memory page len. Default page len is 4KB. */
const auto kPageSize = 4_KB;

/**
 * Place list nodes in memory with the specified stride and offset.
 *
 * The function a bit complicated, so here is an example placement:
 * 1. Place the first object at memory[start_offset]
 * 2. Continue placing objects at memory[start_offset + stride]
 * 3. Once the maximum list size is reached,
 *    try to place the next object at memory[start_offset + sizeof(ListNode)]
 * 4. Repeat until there is a room for a new list node.
 *
 * @tparam ListNode
 *   List node type.
 *
 * @param memory
 *   A memory block to place nodes in.
 * @param memory_size
 *   Total memory block size in bytes.
 * @param max_nodes
 *   Maximum number of nodes to place and benchmark in the memory block.
 * @param stride
 *   Distance in bytes between adjacent list nodes.
 * @param start_offset
 *   Initial offset in bytes to place the first list node.
 *
 * @return
 *   Pointer to the list head.
 */
template <class ListNode>
static auto place_list_nodes(std::byte *memory, const size_t memory_size,
                             size_t max_nodes, const size_t stride,
                             const size_t start_offset) {
  /* Check if there is enough space for at least one node */
  assert(start_offset + sizeof(ListNode) <= memory_size);
  /* Make sure ListNode size fits into stride */
  assert(sizeof(ListNode) <= stride);
  /* Check if the start offset is less than stride */
  assert(start_offset < stride);
  /* Check if the stride is less than total list size */
  assert(stride <= memory_size);
  assert(max_nodes > 0);

  auto cur_offset = start_offset;
  ListNode *list_head = nullptr;
  ListNode **cur_node_ptr = &list_head;
  while (max_nodes--) {
    /* Place a new list node at the current offset */
    *cur_node_ptr = reinterpret_cast<ListNode *>(&memory[cur_offset]);
    cur_node_ptr = &(*cur_node_ptr)->next;
    /* Check bounds */
    if (cur_offset + stride + sizeof(ListNode) <= memory_size) {
      cur_offset += stride;
    } else {
      /* Getting back to the start of memory block */
      cur_offset %= stride;
      cur_offset += sizeof(ListNode);
      /* Check if there is a room for another loop */
      if (cur_offset >= start_offset + stride) break;
    }
  }
  /* Make a cycle */
  *cur_node_ptr = list_head;

  return list_head;
}

/**
 * Traverse the list and apply an operation on each node.
 *
 * @tparam ListNode
 *   List node type.
 * @tparam Operation
 *   An operation to perform on each list node.
 *
 * @param list_head
 *   Pointer to the list head.
 * @param num_ops
 *   Number of operations to perform.
 * @param op
 *   An operation to perform on each node.
 */
template <class ListNode, class Operation>
static auto traverse_list(ListNode *list_head, size_t num_ops, Operation op) {
  auto cur_node = list_head;
  while (num_ops--) {
    op(cur_node);
    cur_node = cur_node->next;
  }
  return cur_node;
}

/**
 * Place array elements in memory with the specified stride and offset.
 *
 * @tparam ArrayElement
 *   Array element type.
 *
 * @param memory
 *   A memory block to place elements in.
 * @param memory_size
 *   Total memory block size in bytes.
 * @param max_elements
 *   Maximum number of elements to place in the memory block.
 * @param stride
 *   Distance in bytes between adjacent array elements.
 * @param start_offset
 *   Initial offset in bytes to place the first array element.
 */
template <class ArrayElement>
static void place_array_elements(std::byte *memory, const size_t memory_size,
                                 size_t max_elements, const size_t stride,
                                 const size_t start_offset) {
  /* Check if there is enough space for at least one element */
  assert(start_offset + sizeof(ArrayElement) <= memory_size);
  /* Make sure ArrayElement size fits into stride */
  assert(sizeof(ArrayElement) <= stride);
  /* Check if the start offset is less than stride */
  assert(start_offset < stride);
  /* Check if the stride is less than total list size */
  assert(stride <= memory_size);

  for (size_t stride_offset = start_offset;
       stride_offset <= stride - sizeof(ArrayElement);
       stride_offset += sizeof(ArrayElement)) {
    for (size_t memory_offset = stride_offset;
         memory_offset <= memory_size - sizeof(ArrayElement);
         memory_offset += stride) {
      auto cur_offset = memory_offset;
      if (max_elements-- == 0) return;
      /* Place an array element at the current offset */
      ArrayElement *cur_element =
          reinterpret_cast<ArrayElement *>(&memory[cur_offset]);
      cur_element->offset = cur_offset;
    }
  }
}

/**
 * Traverse the array and apply an operation on each element.
 *
 * @tparam ArrayElement
 *   Array element type.
 * @tparam Operation
 *   An operation to perform on each array element.
 *
 * @param memory
 *   A memory block of elements.
 * @param memory_size
 *   Total memory block size in bytes.
 * @param max_elements
 *   Maximum number of elements in the memory block.
 * @param num_ops
 *   Number of operations to perform.
 * @param stride
 *   Distance in bytes between adjacent array elements.
 * @param start_offset
 *   Initial offset in bytes to place the first array element.
 * @param op
 *   An operation to perform on each element.
 */
template <class ArrayElement, class Operation>
static auto traverse_array(std::byte *memory, const size_t memory_size,
                           size_t max_elements, size_t num_ops,
                           const size_t stride, const size_t start_offset,
                           Operation op) {
  ArrayElement *cur_element;
  size_t sum = 0;
  for (;;) {
  start_over:
    for (size_t stride_offset = start_offset;
         stride_offset <= stride + start_offset - sizeof(ArrayElement);
         stride_offset += sizeof(ArrayElement)) {
      for (size_t memory_offset = stride_offset;
           memory_offset <= memory_size - sizeof(ArrayElement);
           memory_offset += stride) {
        if (max_elements-- == 0) goto start_over;
        if (num_ops-- == 0) goto end;
        cur_element = reinterpret_cast<ArrayElement *>(&memory[memory_offset]);
        sum += cur_element->offset;
        op(cur_element);
      }
    }
  }
end:
  benchmark::DoNotOptimize(cur_element);
  return sum;
}

/**
 * Create and benchmark a list of nodes.
 *
 * @tparam ListNode
 *   List node type.
 * @tparam Operation
 *   An operation to perform on each list node.
 *
 * @param state
 *   Benchmark state object.
 * @param memory_size
 *   Total memory block size in bytes.
 * @param max_nodes
 *   Maximum number of nodes to place in the memory block.
 * @param num_ops
 *   Number of operations to perform per benchmark.
 * @param stride
 *   Distance in bytes between adjacent list nodes.
 * @param start_offset
 *   Initial offset in bytes to place the first list node.
 * @param op
 *   An operation to perform on each node.
 */
template <class ListNode, class Operation>
void benchmark_list(benchmark::State &state, const size_t memory_size,
                    const size_t max_nodes, const size_t num_ops,
                    const size_t stride, const size_t start_offset,
                    Operation op) {
  /* Allocate an aligned chunk of memory */
  auto memory = static_cast<std::byte *>(operator new(
      memory_size, std::align_val_t(kPageSize)));
  assert(reinterpret_cast<uintptr_t>(memory) % kPageSize == 0);

  const auto list_head = place_list_nodes<ListNode>(
      memory, memory_size, max_nodes, stride, start_offset);

  while (state.KeepRunningBatch(num_ops)) {
    benchmark::DoNotOptimize(traverse_list(list_head, num_ops, op));
  }

  operator delete(memory, std::align_val_t(kPageSize));
}

/**
 * Create and benchmark an array of elements.
 *
 * @tparam ArrayElement
 *   Array element type.
 * @tparam Operation
 *   An operation to perform on each array element.
 *
 * @param state
 *   Benchmark state object.
 * @param memory_size
 *   Total memory block size in bytes.
 * @param max_elements
 *   Maximum number of elements to place in the memory block.
 * @param num_ops
 *   Maximum number of elements to place and number of operations to perform.
 * @param stride
 *   Distance in bytes between adjacent array elements.
 * @param start_offset
 *   Initial offset in bytes to place the first array element.
 * @param op
 *   An operation to perform on each element.
 */
template <class ArrayElement, class Operation>
void benchmark_array(benchmark::State &state, const size_t memory_size,
                     const size_t max_elements, const size_t num_ops,
                     const size_t stride, const size_t start_offset,
                     Operation op) {
  /* Allocate an aligned chunk of memory */
  auto memory = static_cast<std::byte *>(operator new(
      memory_size, std::align_val_t(kPageSize)));
  assert(reinterpret_cast<uintptr_t>(memory) % kPageSize == 0);

  place_array_elements<ArrayElement>(memory, memory_size, max_elements, stride,
                                     start_offset);

  while (state.KeepRunningBatch(num_ops)) {
    auto sum = traverse_array<ArrayElement>(memory, memory_size, max_elements,
                                            num_ops, stride, start_offset, op);
    benchmark::DoNotOptimize(sum);
  }

  operator delete(memory, std::align_val_t(kPageSize));
}

////////////////////////////////////////////////////////////////////////
// Misaligned Loads
////////////////////////////////////////////////////////////////////////

static void misaligned_list(benchmark::State &state) {
  const auto list_size = operator""_KB(state.range(0));
  const auto offset = operator""_B(state.range(1));

  /* Cacheline aligned singly linked list node */
  struct alignas(kCachelineSize) CachelineAlignedListNode {
    CachelineAlignedListNode *next;
  };
  const auto list_nodes = list_size / sizeof(CachelineAlignedListNode);

  const auto stride = offset < kCachelineSize ? kCachelineSize : kPageSize;
  benchmark_list<CachelineAlignedListNode>(state, list_nodes * kPageSize,
                                           list_nodes, 1_M, stride, offset,
                                           [](CachelineAlignedListNode *) {});

  state.counters["Stride"] = benchmark::Counter(
      stride, benchmark::Counter::kDefaults, benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(misaligned_list)
    ->ArgNames({"size KB", "offset"})
    ->Args({32, 0})
    ->Args({32, 1})
    ->Args({32, kCachelineSize - 1})
    ->Args({32, kPageSize - 1})

    ->Args({128, 0})
    ->Args({128, 1})
    ->Args({128, kCachelineSize - 1})
    ->Args({128, kPageSize - 1})

    ->Args({1_KB, 0})
    ->Args({1_KB, 1})
    ->Args({1_KB, kCachelineSize - 1})
    ->Args({1_KB, kPageSize - 1})

    ->Args({8_KB, 0})
    ->Args({8_KB, 1})
    ->Args({8_KB, kCachelineSize - 1})
    ->Args({8_KB, kPageSize - 1})
    //
    ;

static void misaligned_array(benchmark::State &state) {
  const auto array_size = operator""_KB(state.range(0));
  const auto offset = operator""_B(state.range(1));

  /* Cachelien aligned array element */
  struct alignas(kCachelineSize) CachelineAlignedArrayElement {
    volatile uint64_t offset;
  };
  const auto array_elements = array_size / sizeof(CachelineAlignedArrayElement);

  const auto stride = offset < kCachelineSize ? kCachelineSize : kPageSize;
  benchmark_array<CachelineAlignedArrayElement>(
      state, array_elements * kPageSize, array_elements, 1_M, stride, offset,
      [](CachelineAlignedArrayElement *) {});

  state.counters[" Stride"] = benchmark::Counter(
      stride, benchmark::Counter::kDefaults, benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(misaligned_array)
    ->ArgNames({"size KB", "offset"})
    ->Args({32, 0})
    ->Args({32, 1})
    ->Args({32, kCachelineSize - 1})
    ->Args({32, kPageSize - 1})

    ->Args({128, 0})
    ->Args({128, 1})
    ->Args({128, kCachelineSize - 1})
    ->Args({128, kPageSize - 1})

    ->Args({1_KB, 0})
    ->Args({1_KB, 1})
    ->Args({1_KB, kCachelineSize - 1})
    ->Args({1_KB, kPageSize - 1})

    ->Args({8_KB, 0})
    ->Args({8_KB, 1})
    ->Args({8_KB, kCachelineSize - 1})
    ->Args({8_KB, kPageSize - 1})
    //
    ;

////////////////////////////////////////////////////////////////////////
// Cache Associativity
////////////////////////////////////////////////////////////////////////

static void cache_associativity_list(benchmark::State &state) {
  const auto ways = state.range(0);

  /* Cacheline aligned singly linked list node */
  struct alignas(kCachelineSize) CachelineAlignedListNode {
    CachelineAlignedListNode *next;
  };

  benchmark_list<CachelineAlignedListNode>(state, ways * kPageSize, ways, 1_M,
                                           kPageSize, 0,
                                           [](CachelineAlignedListNode *) {});
  state.counters["List Size"] =
      benchmark::Counter(ways * kCachelineSize, benchmark::Counter::kDefaults,
                         benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(cache_associativity_list)
    ->ArgName("ways")
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Arg(32)
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1_KB)
    ->Arg(2_KB)
    ->Arg(8_KB)
    //
    ;

// static void cache_associativity_array(benchmark::State &state) {
//   const auto ways = state.range(0);

//   /* Cachelien aligned array element */
//   struct alignas(kCachelineSize) CachelineAlignedArrayElement {
//     volatile uint64_t offset;
//   };

//   benchmark_array<CachelineAlignedArrayElement>(
//       state, ways * kPageSize, ways, 1_M, kPageSize, 0,
//       [](CachelineAlignedArrayElement *) {});
// }

////////////////////////////////////////////////////////////////////////
// Hardware Prefetch
////////////////////////////////////////////////////////////////////////

static void hardware_prefetch_list(benchmark::State &state) {
  const auto list_size = operator""_KB(state.range(0));
  const auto stride = operator""_B(state.range(1));

  /* Cacheline aligned singly linked list node */
  struct alignas(kCachelineSize) CachelineAlignedListNode {
    CachelineAlignedListNode *next;
  };
  const auto list_nodes = list_size / sizeof(CachelineAlignedListNode);

  benchmark_list<CachelineAlignedListNode>(state, list_nodes * stride,
                                           list_nodes, list_nodes, stride, 0,
                                           [](CachelineAlignedListNode *) {});

  state.counters["Read Rate"] = benchmark::Counter(
      state.iterations() * kCachelineSize, benchmark::Counter::kIsRate,
      benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(hardware_prefetch_list)
    ->ArgNames({"size KB", "stride"})
    ->Args({32, kCachelineSize})
    ->Args({32, kCachelineSize * 2})
    ->Args({32, kCachelineSize * 3})
    ->Args({32, kCachelineSize * 5})
    ->Args({32, kCachelineSize * 7})
    ->Args({32, kCachelineSize * 11})
    ->Args({32, kCachelineSize * 13})
    ->Args({32, kPageSize})
    ->Args({32, kPageSize + kCachelineSize})
    ->Args({32, kPageSize * 2})

    ->Args({128, kPageSize / 64})
    ->Args({128, kPageSize / 32})
    ->Args({128, kPageSize / 16})
    ->Args({128, kPageSize / 8})
    ->Args({128, kPageSize / 4})
    ->Args({128, kPageSize / 2})
    ->Args({128, kPageSize})
    ->Args({128, kPageSize + kCachelineSize})
    ->Args({128, kPageSize * 2})

    ->Args({1_KB, kPageSize / 64})
    ->Args({1_KB, kPageSize / 32})
    ->Args({1_KB, kPageSize / 16})
    ->Args({1_KB, kPageSize / 8})
    ->Args({1_KB, kPageSize / 4})
    ->Args({1_KB, kPageSize / 2})
    ->Args({1_KB, kPageSize})
    ->Args({1_KB, kPageSize + kCachelineSize})
    ->Args({1_KB, kPageSize * 2})

    ->Args({8_KB, kPageSize / 64})
    ->Args({8_KB, kPageSize / 32})
    ->Args({8_KB, kPageSize / 16})
    ->Args({8_KB, kPageSize / 8})
    ->Args({8_KB, kPageSize / 4})
    ->Args({8_KB, kPageSize / 2})
    ->Args({8_KB, kPageSize})
    ->Args({8_KB, kPageSize + kCachelineSize})
    ->Args({8_KB, kPageSize * 2})
    //
    ;

// static void hardware_prefetch_array(benchmark::State &state) {
//   const auto array_size = operator""_KB(state.range(0));
//   const auto stride = operator""_B(state.range(1));

//   /* Cachelien aligned array element */
//   struct alignas(kCachelineSize) CachelineAlignedArrayElement {
//     volatile uint64_t offset;
//   };
//   const auto array_elements = array_size /
//   sizeof(CachelineAlignedArrayElement);

//   benchmark_array<CachelineAlignedArrayElement>(
//       state, array_elements * stride, array_elements, array_elements, stride,
//       0,
//       [](CachelineAlignedArrayElement *) {});

//   state.counters["Read Rate"] = benchmark::Counter(
//       state.iterations() * kCachelineSize, benchmark::Counter::kIsRate,
//       benchmark::Counter::OneK::kIs1024);
// }

////////////////////////////////////////////////////////////////////////
// Cache Hierarchy
////////////////////////////////////////////////////////////////////////

static void cache_hierarchy_list(benchmark::State &state) {
  const auto list_size = operator""_KB(state.range(0));

  /* Cacheline aligned singly linked list node */
  struct alignas(kCachelineSize) CachelineAlignedListNode {
    CachelineAlignedListNode *next;
  };
  benchmark_list<CachelineAlignedListNode>(state, list_size, 1_M, 1_M,
                                           kPageSize + kCachelineSize, 0,
                                           [](CachelineAlignedListNode *) {});

  state.counters[" Read Rate"] = benchmark::Counter(
      state.iterations() * kCachelineSize, benchmark::Counter::kIsRate,
      benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(cache_hierarchy_list)
    ->ArgName("size KB")
    ->Arg(8)
    ->Arg(16)
    ->Arg(32)
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1_KB)
    ->Arg(2_KB)
    ->Arg(4_KB)
    ->Arg(8_KB)
    ->Arg(16_KB)
    //
    ;

static void cache_hierarchy_array(benchmark::State &state) {
  const auto array_size = operator""_KB(state.range(0));

  /* Cachelien aligned array element */
  struct alignas(kCachelineSize) CachelineAlignedArrayElement {
    volatile uint64_t offset;
  };

  benchmark_array<CachelineAlignedArrayElement>(
      state, array_size, 1_M, 1_M, kPageSize + kCachelineSize, 0,
      [](CachelineAlignedArrayElement *) {});

  state.counters[" Read Rate"] = benchmark::Counter(
      state.iterations() * kCachelineSize, benchmark::Counter::kIsRate,
      benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(cache_hierarchy_array)
    ->ArgName("size KB")
    ->Arg(8)
    ->Arg(16)
    ->Arg(32)
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1_KB)
    ->Arg(2_KB)
    ->Arg(4_KB)
    ->Arg(8_KB)
    ->Arg(16_KB)
    //
    ;

////////////////////////////////////////////////////////////////////////
// TLB Hierarchy
////////////////////////////////////////////////////////////////////////

static void tlb_cache_list(benchmark::State &state) {
  const auto pages = state.range(0);

  /* Cacheline aligned singly linked list node */
  struct alignas(kCachelineSize) CachelineAlignedListNode {
    CachelineAlignedListNode *next;
  };

  benchmark_list<CachelineAlignedListNode>(state, pages * kPageSize, pages,
                                           pages, kPageSize + kCachelineSize, 0,
                                           [](CachelineAlignedListNode *) {});
  state.counters["List Size"] =
      benchmark::Counter(pages * kCachelineSize, benchmark::Counter::kDefaults,
                         benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(tlb_cache_list)
    ->ArgName("pages")
    ->Arg(16)
    ->Arg(32)
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1_KB)
    ->Arg(2_KB)
    ->Arg(4_KB)
    ->Arg(8_KB)
    //
    ;

// static void tlb_cache_array(benchmark::State &state) {
//   const auto pages = state.range(0);

//   /* Cachelien aligned array element */
//   struct alignas(kCachelineSize) CachelineAlignedArrayElement {
//     volatile uint64_t offset;
//   };

//   benchmark_array<CachelineAlignedArrayElement>(
//       state, pages * kPageSize, pages, pages, kPageSize + kCachelineSize, 0,
//       [](CachelineAlignedArrayElement *) {});

//   state.counters["Array Size"] =
//       benchmark::Counter(pages * kCachelineSize,
//       benchmark::Counter::kDefaults,
//                          benchmark::Counter::OneK::kIs1024);
// }

////////////////////////////////////////////////////////////////////////
// Cache Conflicts
////////////////////////////////////////////////////////////////////////

static void cache_conflicts_list(benchmark::State &state) {
  const auto list_size = operator""_KB(state.range(0));
  const auto stride = operator""_B(state.range(1));

  /* Cacheline aligned singly linked list node */
  struct alignas(kCachelineSize) CachelineAlignedListNode {
    CachelineAlignedListNode *next;
  };
  const auto list_nodes = list_size / sizeof(CachelineAlignedListNode);

  benchmark_list<CachelineAlignedListNode>(state, list_nodes * stride,
                                           list_nodes, list_nodes, stride, 0,
                                           [](CachelineAlignedListNode *) {});

  state.counters["Read Rate"] = benchmark::Counter(
      state.iterations() * kCachelineSize, benchmark::Counter::kIsRate,
      benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(cache_conflicts_list)
    ->ArgNames({"size KB", "stride"})
    ->Args({32, kPageSize - kCachelineSize})
    ->Args({32, kPageSize})
    ->Args({32, kPageSize + kCachelineSize})

    ->Args({128, kPageSize - kCachelineSize})
    ->Args({128, kPageSize})
    ->Args({128, kPageSize + kCachelineSize})

    ->Args({1_KB, kPageSize - kCachelineSize})
    ->Args({1_KB, kPageSize})
    ->Args({1_KB, kPageSize + kCachelineSize})
    //
    ;

static void cache_conflicts_array(benchmark::State &state) {
  const auto array_size = operator""_KB(state.range(0));
  const auto stride = operator""_B(state.range(1));

  /* Cachelien aligned array element */
  struct alignas(kCachelineSize) CachelineAlignedArrayElement {
    volatile uint64_t offset;
  };
  const auto array_elements = array_size / sizeof(CachelineAlignedArrayElement);

  benchmark_array<CachelineAlignedArrayElement>(
      state, array_elements * stride, array_elements, array_elements, stride, 0,
      [](CachelineAlignedArrayElement *) {});

  state.counters["Read Rate"] = benchmark::Counter(
      state.iterations() * kCachelineSize, benchmark::Counter::kIsRate,
      benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(cache_conflicts_array)
    ->ArgNames({"size KB", "stride"})
    ->Args({32, kPageSize - kCachelineSize})
    ->Args({32, kPageSize})
    ->Args({32, kPageSize + kCachelineSize})

    ->Args({128, kPageSize - kCachelineSize})
    ->Args({128, kPageSize})
    ->Args({128, kPageSize + kCachelineSize})

    ->Args({1_KB, kPageSize - kCachelineSize})
    ->Args({1_KB, kPageSize})
    ->Args({1_KB, kPageSize + kCachelineSize})
    //
    ;

BENCHMARK_MAIN();
