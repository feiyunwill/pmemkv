// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2020, Intel Corporation */

/*
 * erase.cpp -- erase pmreorder test
 */

#include "unittest.hpp"

static constexpr int len_elements = 10;

static void check_exist(pmem::kv::db &kv, const std::string &element,
			pmem::kv::status exists)
{
	std::string value;
	UT_ASSERT(kv.get(element, &value) == exists);

	if (exists == pmem::kv::status::OK) {
		UT_ASSERT(element == value);
	}
}

static void test_init(pmem::kv::db &kv)
{
	for (int i = 0; i < len_elements; i++) {
		std::string element = std::to_string(i);
		kv.put(element, element);
		check_exist(kv, element, pmem::kv::status::OK);
	}
}

static void test_erase(pmem::kv::db &kv)
{
	std::size_t size;
	kv.count_all(size);
	UT_ASSERT(size == len_elements);

	std::string element = std::to_string(1); /* remove this element */
	check_exist(kv, element, pmem::kv::status::OK);

	kv.remove(element);
	check_exist(kv, element, pmem::kv::status::NOT_FOUND);
}

static void check_consistency(pmem::kv::db &kv)
{
	std::size_t size;
	kv.count_all(size);
	std::size_t count = 0;

	for (int i = 0; i < len_elements; i++) {
		std::string element = std::to_string(i);
		if (kv.exists(element) == pmem::kv::status::OK) {
			++count;
			check_exist(kv, element, pmem::kv::status::OK);
		} else {
			check_exist(kv, element, pmem::kv::status::NOT_FOUND);
		}
	}

	UT_ASSERTeq(count, size);
}

static void test(int argc, char *argv[])
{
	std::cout << "ARGC: " << argc << std::endl;
	for (int i = 0; i < argc; ++i) {
		std::cout << "ARGV " << i << " : " << argv[i] << std::endl;
	}
	if (argc < 4)
		UT_FATAL("usage: %s engine json_config <create|open|erase>", argv[0]);

	std::string mode = argv[3];
	if (mode != "create" && mode != "open" && mode != "erase")
		UT_FATAL("usage: %s engine json_config <create|open|erase>", argv[0]);

	auto kv = INITIALIZE_KV(argv[1], CONFIG_FROM_JSON(argv[2]));

	if (mode == "create") {
		test_init(kv);
	} else if (mode == "open") {
		check_consistency(kv);
	} else if (mode == "erase") {
		test_erase(kv);
	}

	kv.close();
}

int main(int argc, char *argv[])
{
	return run_test([&] { test(argc, argv); });
}
