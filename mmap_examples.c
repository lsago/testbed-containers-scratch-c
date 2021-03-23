#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#define STACK_SIZE (1024 * 1024)


/* ~from: https://linuxhint.com/using_mmap_function_linux/ */
#define N 10
int test_mmap_no_file()
{
	int *array;

	array = mmap(NULL, N * sizeof(int),
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		0, 0);
	/* Writing PROT_READ alone for example, will result in a SIGSEGV */


	if (array == MAP_FAILED) {
		fprintf(stderr, "Something went wrong when mmap'ing()\n");
		return 1;
	}

	printf("About to write a '2' on the third position of the array\n");
	array[2] = 2;
	printf("Here: %d\n", array[2]);

	int err = munmap(array, N * sizeof(int));

	if (err != 0) {
		fprintf(stderr, "Something went wrong when munmap'ing()\n");
		return 1;
	}

	return 0;
}


/* File-backed mapping */
int test_mmap_with_file(const char *file_path)
{
	char *file_map;
	int fd = -1;

	if ((fd = open(file_path, O_RDWR, 0)) == -1) {
		fprintf(stderr, "Could no open file %s\n", file_path);
		return 1;
	}

	file_map = mmap(NULL, 10,
		PROT_READ | PROT_WRITE,
		MAP_SHARED,  /* MAP_PRIVATE */
		fd, 0);
	/* "If MAP_SHARED is specified, write references shall change the
	 * underlying  object.  If  MAP_PRIVATE  is specified, modifications to
	 * the mapped data by the calling process shall be visible only to the
	 * calling process and shall not change the  under‐ lying  object."
	 * -- man mmap(3p)
	 */

	if (file_map == MAP_FAILED) {
		fprintf(stderr, "Something went wrong when mmap'ing\n", file_path);
		return 1;
	}

	strncpy(file_map, "Try this", 8);

	printf("Here's the array (file-backed mapping): \"%s\"\n", file_map);

	return 0;
}


int main(int argc, char* argv[])
{

	if (argc < 2) {
		fprintf(stderr, "Insufficient arguments.\n");
		return 1;
	}

	const char *file_path = argv[1];

	test_mmap_no_file();

	test_mmap_with_file(file_path);
}





