#include "OriginalDecryptor.h"

char *original_getcwd(char *buf, int size)
{
#ifdef _MSC_VER
	return _getcwd(buf, size);
#else
	return getcwd(buf, size);
#endif
}

int original_chdir(char *path)
{
#ifdef _MSC_VER
	return _chdir(path);
#else
	return chdir(path);
#endif
}

int original_getstat(char *pathname, original_stat *buf)
{
#ifdef _MSC_VER
#ifdef _WIN32
	return _stat32(pathname, buf);
#else
	return _stat64(pathname, buf);
#endif
#endif
#ifndef _MSC_VER
	return stat(pathname, buf);
#endif
}

/* Search pattern must end with * in Visual C */
original_dir *original_opendir(char *name)
{
#ifdef _MSC_VER
	original_dir *dir;

	if (!name || strlen(name) > original_MAX_LENGTH - 2) return NULL;
	dir = malloc(sizeof(original_dir));
	dir->ent.d_name = NULL;
	strcpy(dir->name, name);
	dir->fhandle = (ptrdiff_t) _findfirst(dir->name, &dir->fdata);
	if ((int) dir->fhandle == -1) {
		free (dir);
		return NULL;
	}
	return dir;
#else
	return opendir(name);
#endif
}

original_dirent *original_readdir(original_dir *dirp)
{
#ifdef _MSC_VER
	if(dirp->ent.d_name &&
		(int) _findnext(dirp->fhandle, &dirp->fdata) == -1)
		return NULL;
	dirp->ent.d_name = dirp->fdata.name;
	return &dirp->ent;
#else
	return readdir(dirp);
#endif
}

int original_closedir(original_dir *dirp)
{
#ifdef _MSC_VER
	int n;

	if (!dirp || dirp->fhandle == -1) return -1;
	n = (int) _findclose(dirp->fhandle);
	free(dirp);
	return n;
#else
	return closedir(dirp);
#endif
}

int original_isdir(original_stat s)
{
#ifdef _MSC_VER
	return s.st_mode & _S_IFDIR;
#else
	return S_ISDIR(s.st_mode);
#endif
}

int original_isreg(original_stat s)
{
#ifdef _MSC_VER
	return s.st_mode & _S_IFREG;
#else
	return S_ISREG(s.st_mode);
#endif
}
 
