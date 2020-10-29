#include "core/util_string.h"

#include "tests.h"

/*static*/ std::int64_t T::cnt = 0;
/*static*/ std::int64_t T::inst_cnt = 0;
/*static*/ std::int64_t T::comp_cnt = 0;

void dump_and_destroy_global_pools() {
    for (auto item = util::pool_base::global_pool_list(); item; item = item->next) {
        auto desc = item->pool_desc();

        size_t free_count = 0;
        auto node = desc->free.next;
        while (node != &desc->free) {
            ++free_count;
            node = node->next;
        }

        size_t node_count_per_partition = desc->node_count_per_partition;
        size_t partition_count = 0;
        size_t total_use_count = 0;
        node = desc->partitions.next;
        while (node != &desc->partitions) {
            ++partition_count;
            total_use_count += static_cast<util::pool_base::part_hdr_t*>(node)->use_count;
            node = node->next;
        }

        std::cout << std::endl;
        std::cout << "------------------------ global pool for size " << (desc->size_and_alignment & 0xffff)
                  << " and alignment " << (desc->size_and_alignment >> 16) << std::endl;
        std::cout << "free_count = " << free_count << std::endl;
        std::cout << "partition_count = " << partition_count << std::endl;
        std::cout << "total_use_count = " << total_use_count << std::endl;
        std::cout << "node_count_per_partition = " << node_count_per_partition << " (" << free_count + total_use_count
                  << ")" << std::endl;

        item->unref();
    }
}

int perform_tests(std::pair<std::pair<size_t, void (*)()>*, size_t> tests) {
    try {
        for (size_t n = 0; n < tests.second; ++n) {
            std::cout << "Test #" << tests.first[n].first << "..." << std::flush;
            tests.first[n].second();
            std::cout << "OK!   " << std::endl;
        }
    } catch (const std::logic_error& er) {
        std::cout << "FAILURE! (" << er.what() << ")" << std::endl;
        return -1;
    }

    return 0;
}

std::pair<std::pair<size_t, void (*)()>*, size_t> get_util_tests();
std::pair<std::pair<size_t, void (*)()>*, size_t> get_string_tests();
std::pair<std::pair<size_t, void (*)()>*, size_t> get_vector_tests();
std::pair<std::pair<size_t, void (*)()>*, size_t> get_list_tests();
std::pair<std::pair<size_t, void (*)()>*, size_t> get_rbtree_tests();

int main(int argc, char* argv[]) {
#if _ITERATOR_DEBUG_LEVEL != 0
    std::cout << "Iterator debuging enabled!" << std::endl;
#endif

    std::cout << std::endl << "--------------- Util tests ---------------" << std::endl;
    if (perform_tests(get_util_tests()) != 0) { return -1; }
    std::cout << std::endl << "--------------- String tests ---------------" << std::endl;
    if (perform_tests(get_string_tests()) != 0) { return -1; }
    std::cout << std::endl << "--------------- Vector tests ---------------" << std::endl;
    if (perform_tests(get_vector_tests()) != 0) { return -1; }
    std::cout << std::endl << "--------------- List tests ---------------" << std::endl;
    if (perform_tests(get_list_tests()) != 0) { return -1; }
    std::cout << std::endl << "--------------- Red-black tree tests ---------------" << std::endl;
    if (perform_tests(get_rbtree_tests()) != 0) { return -1; }

    std::cout << std::endl;
    std::cout << "T::cnt = " << T::cnt << std::endl;
    std::cout << "T::inst_cnt = " << T::inst_cnt << std::endl;
    std::cout << "T::comp_cnt = " << T::comp_cnt << std::endl;
    dump_and_destroy_global_pools();

#ifdef _MSC_VER
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
