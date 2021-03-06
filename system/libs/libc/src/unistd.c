#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/proto.h>
#include <sys/proc.h>
#include <sys/vfs.h>
#include <sys/ipc.h>
#include <sys/kserv.h>
#include <string.h>
#include <mstr.h>
#include <sys/shm.h>
#include <errno.h>
//#include <rawdata.h>

int errno = ENONE;

int getuid(void) {
	return syscall0(SYS_PROC_GET_UID);
}

int setuid(int uid) {
	return syscall1(SYS_PROC_SET_UID, uid);
}

int getpid_raw(int pid) {
	return syscall1(SYS_GET_PID, pid);
}

int getpid(void) {
	return getpid_raw(-1);
}

int fork(void) {
	return syscall0(SYS_FORK);
}

void detach(void) {
	syscall0(SYS_DETACH);
}

int usleep(unsigned int usecs) {
	if(usecs == 0)
		syscall0(SYS_YIELD);
	else
		syscall1(SYS_USLEEP, usecs);
	return 0;
}

unsigned int sleep(unsigned int seconds) {
	usleep(seconds * 1000 * 1000);
	return 0;
}

static int read_pipe(fsinfo_t* info, void* buf, uint32_t size, int block) {
	int res = vfs_read_pipe(info, buf, size, block);
	if(res == 0) { // pipe empty, do retry
		errno = EAGAIN;
		return -1;
	}
	if(res > 0) {
		return res;
	}
	return 0; //res < 0 , pipe closed, return 0.
}

#define SHM_ON 32
static int read_raw(int fd, int ufid, fsinfo_t *info, void* buf, uint32_t size) {
	/*mount_t mount;
	if(vfs_get_mount(info, &mount) != 0)
		return -1;
		*/

	int offset = 0;
	if(info->type == FS_TYPE_FILE) {
		offset = vfs_tell(fd);
		if(offset < 0)
			offset = 0;
	}

	int32_t shm_id = -1;
	void* shm = NULL;
	if(size >= SHM_ON) {
		shm_id = shm_alloc(size, SHM_PUBLIC);
		if(shm_id < 0)
			return -1;

		shm = shm_map(shm_id);
		if(shm == NULL) 
			return -1;
	}

	proto_t in, out;
	PF->init(&out, NULL, 0);

	PF->init(&in, NULL, 0)->addi(&in, fd)->addi(&in, ufid)->add(&in, info, sizeof(fsinfo_t))->addi(&in, size)->addi(&in, offset)->addi(&in, shm_id);

	int res = -1;
	if(ipc_call(info->mount_pid, FS_CMD_READ, &in, &out) == 0) {
		int rd = proto_read_int(&out);
		res = rd;
		if(rd > 0) {
			if(shm != NULL)
				memcpy(buf, shm, rd);
			else
				proto_read_to(&out, buf, size);
			offset += rd;
			if(info->type == FS_TYPE_FILE)
				vfs_seek(fd, offset);
		}
		if(res == ERR_RETRY) {
			errno = EAGAIN;
			res = -1;
		}
	}
	PF->clear(&in);
	PF->clear(&out);
	if(shm != NULL)
		shm_unmap(shm_id);
	return res;
}

int read_nblock(int fd, void* buf, uint32_t size) {
	errno = ENONE;
	fsinfo_t info;
	uint32_t ufid = 0;
	if(vfs_get_by_fd(fd, &ufid, &info) != 0)
		return -1;
	if(info.type == FS_TYPE_PIPE)
		return read_pipe(&info, buf, size, 0);
	return read_raw(fd, ufid, &info, buf, size);
}

int read(int fd, void* buf, uint32_t size) {
	errno = ENONE;
	fsinfo_t info;
	uint32_t ufid = 0;
	if(vfs_get_by_fd(fd, &ufid, &info) != 0)
		return -1;

	int res = -1;
	if(info.type == FS_TYPE_PIPE) {
		while(1) {
			res = read_pipe(&info, buf, size, 1);
			if(res >= 0)
				break;
			if(errno != EAGAIN)
				break;
			sleep(0);
		}
		return res;
	}

	while(1) {
		res = read_raw(fd, ufid, &info, buf, size);
		if(res >= 0)
			break;
		if(errno != EAGAIN)
			break;
		sleep(0);
	}
	return res;
}

int read_block(int pid, void* buf, uint32_t size, int32_t index) {
	int32_t shm_id = -1;
	void* shm = NULL;
	shm_id = shm_alloc(size, SHM_PUBLIC);
	if(shm_id < 0)
		return -1;

	shm = shm_map(shm_id);
	if(shm == NULL) 
		return -1;

	proto_t in, out;
	PF->init(&out, NULL, 0);

	PF->init(&in, NULL, 0)->
		addi(&in, size)->
		addi(&in, index)->
		addi(&in, shm_id);

	int res = -1;
	if(ipc_call(pid, FS_CMD_READ_BLOCK, &in, &out) == 0) {
		int rd = proto_read_int(&out);
		res = rd;
		if(rd > 0) {
			memcpy(buf, shm, rd);
		}
		if(res == ERR_RETRY) {
			errno = EAGAIN;
			res = -1;
		}
	}
	PF->clear(&in);
	PF->clear(&out);
	shm_unmap(shm_id);
	return res;
}

static int write_pipe(fsinfo_t* info, const void* buf, uint32_t size, int block) {
	int res = vfs_write_pipe(info, buf, size, block);
	if(res == 0) { // pipe not empty, do retry
		errno = EAGAIN;
		return -1;
	}
	if(res > 0)
		return res;
	return 0; //res < 0 , pipe closed, return 0.
}

int write_raw(int fd, int ufid, fsinfo_t* info, const void* buf, uint32_t size) {
	if(info->type == FS_TYPE_DIR) 
		return -1;

	/*mount_t mount;
	if(vfs_get_mount(info, &mount) != 0)
		return -1;
		*/

	int offset = 0;
	if(info->type == FS_TYPE_FILE) {
		offset = vfs_tell(fd);
		if(offset < 0)
			offset = 0;
	}
		
	int32_t shm_id = -1;
	void* shm = NULL;
	if(size >= SHM_ON) {
		shm_id = shm_alloc(size, SHM_PUBLIC);
		if(shm_id < 0)
			return -1;

		shm = shm_map(shm_id);
		if(shm == NULL) 
			return -1;
		memcpy(shm, buf, size);
	}

	proto_t in, out;
	PF->init(&out, NULL, 0);

	PF->init(&in, NULL, 0)->
		addi(&in, fd)->
		addi(&in, ufid)->
		add(&in, info, sizeof(fsinfo_t))->
		addi(&in, offset)->
		addi(&in, shm_id);
	if(shm_id < 0)
		PF->add(&in, buf, size);
	else
		PF->addi(&in, size);

	int res = -1;
	if(ipc_call(info->mount_pid, FS_CMD_WRITE, &in, &out) == 0) {
		int r = proto_read_int(&out);
		res = r;
		if(r > 0) {
			offset += r;
			if(info->type == FS_TYPE_FILE)
				vfs_seek(fd, offset);
		}
		if(res == -2) {
			errno = EAGAIN;
			res = -1;
		}
	}
	PF->clear(&in);
	PF->clear(&out);
	if(shm != NULL)
		shm_unmap(shm_id);
	return res;
}

int write_nblock(int fd, const void* buf, uint32_t size) {
	errno = ENONE;
	fsinfo_t info;
	uint32_t ufid = 0;
	if(vfs_get_by_fd(fd, &ufid, &info) != 0)
		return -1;
	if(info.type == FS_TYPE_PIPE) 
		return write_pipe(&info, buf, size, 0);
	return write_raw(fd, ufid, &info, buf, size);
}

int write(int fd, const void* buf, uint32_t size) {
	errno = ENONE;
	fsinfo_t info;
	uint32_t ufid = 0;
	if(vfs_get_by_fd(fd, &ufid, &info) != 0)
		return -1;

	int res = -1;
	if(info.type == FS_TYPE_PIPE) {
		while(1) {
			res = write_pipe(&info, buf, size, 1);
			if(res >= 0)
				break;
			if(errno != EAGAIN)
				break;
			sleep(0);
		}
		return res;
	}

	while(1) {
		res = write_raw(fd, ufid, &info, buf, size);
		if(res >= 0)
			break;
		if(errno != EAGAIN)
			break;
		sleep(0);
	}
	return res;
}

int write_block(int pid, const void* buf, uint32_t size, int32_t index) {
	proto_t in, out;
	PF->init(&out, NULL, 0);

	PF->init(&in, NULL, 0)->
		add(&in, buf, size)->
		addi(&in, index);

	int res = -1;
	if(ipc_call(pid, FS_CMD_WRITE_BLOCK, &in, &out) == 0) {
		int r = proto_read_int(&out);
		res = r;
		if(res == -2) {
			errno = EAGAIN;
			res = -1;
		}
	}
	PF->clear(&in);
	PF->clear(&out);
	return res;
}

int lseek(int fd, uint32_t offset, int whence) {
	if(whence == SEEK_CUR) {
		int cur = vfs_tell(fd);
		if(cur < 0)
			cur = 0;
		offset += cur;
	}	
	else if(whence == SEEK_END) {
		fsinfo_t info;
		int cur = 0;
		if(vfs_get_by_fd(fd, NULL, &info) == 0)
			cur = info.size;
		offset += cur;
	}
	return vfs_seek(fd, offset);
}

void exec_elf(const char* cmd_line, const char* elf, int32_t size) {
	syscall3(SYS_EXEC_ELF, (int32_t)cmd_line, (int32_t)elf, size);
}

int exec(const char* cmd_line) {
	str_t* cmd = str_new("");
	const char *p = cmd_line;
	while(*p != 0 && *p != ' ') {
		str_addc(cmd, *p);
		p++;
	}
	str_addc(cmd, 0);
	int sz;
	void* buf = vfs_readfile(cmd->cstr, &sz);
	if(buf == NULL) {
		str_free(cmd);
		return -1;
	}
	exec_elf(cmd_line, buf, sz);
	free(buf);
	return 0;
}

inline static int get_procd_pid(void) {
	return kserv_get(KSERV_PROC);
}

char* getcwd(char* buf, uint32_t size) {
	proto_t out;
	PF->init(&out, NULL, 0);
	if(ipc_call(get_procd_pid(), PROC_CMD_GET_CWD, NULL, &out) == 0) {
		if(proto_read_int(&out) == 0) {
			strncpy(buf, proto_read_str(&out), size-1);
		}
	}
	PF->clear(&out);
	return buf;
}

int chdir(const char* path) {
	proto_t in, out;
	PF->init(&out, NULL, 0);
	PF->init(&in, NULL, 0)->adds(&in, path);
	int res = ipc_call(get_procd_pid(), PROC_CMD_SET_CWD, &in, &out);
	PF->clear(&in);
	if(res == 0) {
		res = proto_read_int(&out);
	}
	PF->clear(&out);
	return res;
}

int dup2(int from, int to) {
	return vfs_dup2(from, to);
}

int dup(int from) {
	return vfs_dup(from);
}

int pipe(int fds[2]) {
	return vfs_open_pipe(fds);
}

int unlink(const char* fname) {
	fsinfo_t info;
	if(vfs_get(fname, &info) != 0)
		return -1;
	if(info.type != FS_TYPE_FILE) 
		return -1;
	
	/*mount_t mount;
	if(vfs_get_mount(&info, &mount) != 0)
		return -1;
		*/
	
	proto_t in, out;
	PF->init(&out, NULL, 0);

	PF->init(&in, NULL, 0)->
		add(&in, &info, sizeof(fsinfo_t))->
		adds(&in, fname);

	ipc_call(info.mount_pid, FS_CMD_UNLINK, &in, &out);
	PF->clear(&in);
	int res = proto_read_int(&out);
	PF->clear(&out);
	if(res == 0)
		return vfs_del(&info);
	return -1;
}
