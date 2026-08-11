/* this can be moved in memory.c and linked to interp code? */

void interpreter_open_shmem()
{
	int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

	ftruncate(fd, sizeof(shared_state_t));

	shared_state_t *st = mmap(NULL, sizeof(*st), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
}
