//
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 Andriy Berestovskyy <berestovskyy@gmail.com>
//
// Applied Benchmarks: Memory Latency
// Benchmarking Kaby Lake and Haswell memory latency using lists
//

#include <iostream>
#include "benchmark/benchmark.h"

// User-defined literals
auto constexpr operator""_B(unsigned long long int n) { return n; }
auto constexpr operator""_KB(unsigned long long int n) { return n * 1024; }
auto constexpr operator""_M(unsigned long long int n) { return n * 1000 * 1000; }

// Cache line size: 64 bytes for x86-64, 128 bytes for A64 ARMs
const auto kCachelineSize = 64_B;
// Memory page size. Default page size is 4 KB
const auto kPageSize = 4_KB;

// Singly linked list node with padding
struct ListNode {
  ListNode *next;
  std::byte padding[kPageSize];
};

//
// Traverse the list of nodes.
//
// @param head
//   Pointer to the list head.
// @param num_ops
//   Number of operations to perform.
//
static auto traverse_list(ListNode *head, size_t num_ops) {
  while (num_ops--) head = head->next;
  return head;
}

//
// Benchmark memory latency using a list.
//
// @param state.range(0)
//   Memory block size in KB to benchmark.
//
static void memory_latency_list(benchmark::State &state) {
  const auto mem_block_size = operator""_KB(state.range(0));
  // Each memory access fetches a cache line
  const auto num_nodes = mem_block_size / kCachelineSize;
  assert(num_nodes > 0);

  // Allocate a contiguous list of nodes for an iteration
  std::vector<ListNode> list(num_nodes);
  // Make a cycle of the list nodes
  for (size_t i = 0; i < list.size() - 1; i++) list[i].next = &list[i + 1];
  list[list.size() - 1].next = &list[0];

  const auto num_ops = 1_M;
  while (state.KeepRunningBatch(num_ops)) {
    auto last_node = traverse_list(&list[0], num_ops);
    benchmark::DoNotOptimize(last_node);
  }

  state.counters["Nodes"] =
      benchmark::Counter(num_nodes, benchmark::Counter::kDefaults,
                         benchmark::Counter::OneK::kIs1024);
  state.counters["Read Rate"] = benchmark::Counter(
      state.iterations() * kCachelineSize, benchmark::Counter::kIsRate,
      benchmark::Counter::OneK::kIs1024);
}
BENCHMARK(memory_latency_list)
    ->ArgName("size KB")
    // Overview
    ->RangeMultiplier(2)
    ->Range(1, 2048)
    // Custom point for 3 MB L3 caches
    // ->Arg(3072)
    ->Range(4096, 16384)
    // L1 Cache
    ->DenseRange(1, 8, 1)
    // L2 Cache
    ->DenseRange(48, 160, 16)
    // L3 Cache
    ->DenseRange(512, 4096, 512)
    ;

BENCHMARK_MAIN();
